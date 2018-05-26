/**
 * Copyright 2018, Decawave Limited, All Rights Reserved
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <os/os.h>
#include <hal/hal_spi.h>
#include <hal/hal_gpio.h>
#include "bsp/bsp.h"

#include <dw1000/dw1000_regs.h>
#include <dw1000/dw1000_dev.h>
#include <dw1000/dw1000_hal.h>
#include <dw1000/dw1000_mac.h>
#include <dw1000/dw1000_phy.h>
#include <dw1000/dw1000_ftypes.h>
#include <dw1000/dw1000_ccp.h>
#include <clkcal/clkcal.h>
#if MYNEWT_VAL(TIMESCALE)
#include <timescale/timescale.h>
#endif

#if MYNEWT_VAL(CLOCK_CALIBRATION)

static void ccp_complate_cb(struct os_event * ev);
static void clkcal_postprocess(struct os_event * ev);

/*! 
 * @fn clkcal_init(clkcal_instance_t * inst,  dw1000_ccp_instance_t * ccp)
 *
 * @brief Allocate resources for clkcal calibration tasks.   
 *
 * input parameters
 * @param inst - clkcal_instance_t *
 * @param ccp - dw1000_ccp_instance_t *,
 *
 * output parameters
 *
 * returns none
 */

clkcal_instance_t * 
clkcal_init(clkcal_instance_t * inst,  dw1000_ccp_instance_t * ccp){

    if (inst == NULL ) {
        inst = (clkcal_instance_t *) malloc(sizeof(clkcal_instance_t)); 
        assert(inst);
        memset(inst, 0, sizeof(clkcal_instance_t));
        inst->status.selfmalloc = 1;
    }

    inst->period = MYNEWT_VAL(CCP_PERIOD);
    inst->config = (clkcal_config_t){
        .postprocess = false
    };
    inst->ccp = (void *) ccp;

#if MYNEWT_VAL(TIMESCALE)
    double x0[] = {0,1.0};
    inst->q[0] = MYNEWT_VAL(TIMESCALE_QVAR);
    inst->q[1] = MYNEWT_VAL(TIMESCALE_QVAR) * 0.1;
    inst->r[0] = MYNEWT_VAL(TIMESCALE_RVAR);
    double T = 1e-6 * inst->period;   // peroid in sec
    inst->timescale = timescale_init(NULL, x0, inst->q, T); 
#endif

    dw1000_ccp_set_postprocess(ccp, ccp_complate_cb);
    clkcal_set_postprocess(inst,clkcal_postprocess);

    inst->status.initialized = 1;
    return inst;
}

/*! 
 * @fn clkcal_free(clkcal_instance_t * inst)
 *
 * @brief Free resources and restore default behaviour. 
 *
 * input parameters
 * @param inst - clkcal_instance_t * inst
 *
 * output parameters
 *
 * returns none
 */
void 
clkcal_free(clkcal_instance_t * inst){
    assert(inst);  
#if MYNEWT_VAL(TIMESCALE)
        timescale_free(inst->timescale);
#endif
    if (inst->status.selfmalloc)
        free(inst);
    else
        inst->status.initialized = 0;
}

/*! 
 * @fn ccp_complate_cb(struct os_event * ev)
 *
 * @brief This serves as a place holder for clkcal processing and by default is creates json string for the event
 *
 * input parameters
 * @param inst - struct os_event * ev * 
 *
 * output parameters
 *
 * returns none 
 */
static void ccp_complate_cb(struct os_event * ev){
    assert(ev != NULL);
    assert(ev->ev_arg != NULL);
    dw1000_ccp_instance_t * ccp = (dw1000_ccp_instance_t *)ev->ev_arg;
    clkcal_instance_t * inst = ccp->clkcal;

#if MYNEWT_VAL(DW1000_CLOCK_CALIBRATION)    
    if(ccp->status.valid){ 
        ccp_frame_t * previous_frame = ccp->frames[(ccp->idx-1)%ccp->nframes]; 
        ccp_frame_t * frame = ccp->frames[(ccp->idx)%ccp->nframes]; 
        inst->nT = (int16_t)frame->seq_num - (int16_t)previous_frame->seq_num;
        inst->nT = (inst->nT < 0)?0x100+inst->nT:inst->nT;

#if MYNEWT_VAL(TIMESCALE) 
        timescale_instance_t * timescale = inst->timescale; 
        timescale_states_t * states = (timescale_states_t *) (inst->timescale->eke->x); 
        double T = 1e-6 * inst->period * inst->nT;   // peroid in sec
        inst->status.valid = timescale_main(timescale, frame->reception_timestamp, inst->q, inst->r, T).initialized;
        inst->skew = states->skews * (1e-6/(1 << 16));
#endif
    }
#endif
    if (inst->config.postprocess) 
        os_eventq_put(os_eventq_dflt_get(), &inst->callout_postprocess.c_ev);
}


/*! 
 * @fn clkcal_set_postprocess(clkcal_instance_t * inst * inst, os_event_fn * ccp_postprocess)
 *
 * @briefOverrides the default post-processing behaviors, replacing the JSON stream with an alternative 
 * or an advanced timescale processing algorithm.
 * 
 * input parameters
 * @param inst - dw1000_dev_instance_t * 
 *
 * returns none
 */
void 
clkcal_set_postprocess(clkcal_instance_t * inst, os_event_fn * postprocess){
    os_callout_init(&inst->callout_postprocess, os_eventq_dflt_get(), postprocess, (void *) inst);
    inst->config.postprocess = true;
}

/*! 
 * @fn clkcal_postprocess(struct os_event * ev)
 *
 * @brief This serves as a place holder for timescale processing and by default is creates json string for the event
 *
 * input parameters
 * @param inst - struct os_event *  
 *
 * output parameters
 *
 * returns none 
 */
static void 
clkcal_postprocess(struct os_event * ev){
    assert(ev != NULL);
    assert(ev->ev_arg != NULL);

    clkcal_instance_t * inst = (clkcal_instance_t *)ev->ev_arg;
    dw1000_ccp_instance_t * ccp = (void *)inst->ccp; 
    ccp_frame_t * previous_frame = ccp->frames[(ccp->idx-1)%ccp->nframes]; 
    ccp_frame_t * frame = ccp->frames[(ccp->idx)%ccp->nframes]; 

     printf("{\"utime\":%lu,\"ccp\":[%llu,%llu],\"skew\":%llu,\"nT\":%d}\n", 
        os_cputime_ticks_to_usecs(os_cputime_get32()),
        frame->reception_timestamp,
        (uint64_t)((uint64_t)(frame->reception_timestamp) - (uint64_t)(previous_frame->reception_timestamp)) & 0xFFFFFFFFF,
        *(uint64_t *)&inst->skew,
        inst->nT
    );
}

#endif // MYNEWT_VAL(CLOCK_CALIBRATION)




