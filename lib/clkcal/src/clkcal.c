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

/**
 * @file clkcal.c
 * @author Paul.Kettle@decawave.com
 * @date May 25 2018
 * @brief Clock Calibration toplevel
 *
 * @details This is a topleval package for managing Clock Calibration using Clock Calibration Packet (CCP). 
 * In an RTLS system the Clock Master send a periodic blink which is received by the anchor nodes. The device driver model on the node
 * handles the ccp frame and schedules a callback for post-processing of the event. The Clock Calibration herein is an example 
 * of this post-processing. In TDOA-base RTLS system clock synchronization is essential, this can be either wired or wireless depending on the requirements.
 * In the case of wireless clock synchronization clock skew is estimated from the CCP packets. Depending on the accuracy required and the 
 * available computational resources two model is available; timescale or linear interpolation.  
 *
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
#include <ccp/dw1000_ccp.h>
#include <clkcal/clkcal.h>
#if MYNEWT_VAL(TIMESCALE_PROCESSING_ENABLED)
#include <timescale/timescale.h>
#endif

//#define DIAGMSG(s,u) printf(s,u)
#ifndef DIAGMSG
#define DIAGMSG(s,u)
#endif

#if MYNEWT_VAL(CLOCK_CALIBRATION_ENABLED)

void clkcal_update_cb(struct os_event * ev);
static void clkcal_postprocess(struct os_event * ev);

/*! 
 * @fn clkcal_init(clkcal_instance_t * inst,  dw1000_ccp_instance_t * ccp)
 *
 * @brief Allocate resources for the clkcal calibration tasks. Binds resources 
 * to dw1000_ccp interface and instantiate timescale instance if in use.      
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

#if MYNEWT_VAL(TIMESCALE_PROCESSING_ENABLED)
    double x0[] = {0};
    inst->q[0] = MYNEWT_VAL(TIMESCALE_QVAR) * 1.0;
    inst->q[1] = MYNEWT_VAL(TIMESCALE_QVAR) * 0.1;
    inst->r[0] = MYNEWT_VAL(TIMESCALE_RVAR);
    double T = 1e-6l * inst->period;   // peroid in sec
    inst->timescale = timescale_init(NULL, x0, inst->q, T); 
    inst->timescale->status.initialized = 0; //Ignore X0 values, until we get first event
#endif

    inst->skew = 1.0f;

    dw1000_ccp_set_postprocess(ccp, clkcal_update_cb);
    clkcal_set_postprocess(inst, clkcal_postprocess);

    inst->status.initialized = 0;
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

    dw1000_ccp_set_postprocess((dw1000_ccp_instance_t *)inst->ccp, NULL);

#if MYNEWT_VAL(TIMESCALE_PROCESSING_ENABLED)
        timescale_free(inst->timescale);
#endif
    if (inst->status.selfmalloc)
        free(inst);
    else
        inst->status.initialized = 0;
}

/*! 
 * @fn clkcal_update_cb(struct os_event * ev)
 *
 * @brief This function serves as a placeholder for clkcal updates based on ccp observation. The clkcal thread uses timescale 
 * if available otherwise defaults to linear interpolation. Once complete a post-process event is invoked.     
 *
 * input parameters
 * @param inst - struct os_event * ev * 
 *
 * output parameters
 *
 * returns none 
 */
void clkcal_update_cb(struct os_event * ev){
    assert(ev != NULL);
    assert(ev->ev_arg != NULL);
    dw1000_ccp_instance_t * ccp = (dw1000_ccp_instance_t *)ev->ev_arg;
    clkcal_instance_t * inst = ccp->clkcal;

    DIAGMSG("{\"utime\": %lu,\"msg\": \"clkcal_update_cb\"}\n",os_cputime_ticks_to_usecs(os_cputime_get32()));

#if MYNEWT_VAL(DW1000_CCP_ENABLED)    
    if(ccp->status.valid){ 
        ccp_frame_t * previous_frame = ccp->frames[(ccp->idx-2)%ccp->nframes]; 
        ccp_frame_t * frame = ccp->frames[(ccp->idx-1)%ccp->nframes]; 
        inst->nT = (int16_t)frame->seq_num - (int16_t)previous_frame->seq_num;
        inst->nT = (inst->nT < 0)?0x100+inst->nT:inst->nT;
       
#if MYNEWT_VAL(TIMESCALE_PROCESSING_ENABLED) 
        timescale_instance_t * timescale = inst->timescale; 
        timescale_states_t * states = (timescale_states_t *) (inst->timescale->eke->x); 
        
        if (inst->status.initialized == 0 ){
            states->time = frame->reception_timestamp;
            states->skew = ((double) ((uint64_t)1 << 16) / 1e-6l); 
            inst->status.initialized = 1;
        }else{
            double T = 1e-6l * inst->period * inst->nT;   // peroid in sec
//          uint32_t tic = os_cputime_ticks_to_usecs(os_cputime_get32());
            inst->status.valid = timescale_main(timescale, ccp->epoch, inst->q, inst->r, T).valid;
//          uint32_t toc = os_cputime_ticks_to_usecs(os_cputime_get32());
//          printf("{\"utime\": %lu,\"timescale_main_tic_toc\": %lu}\n",toc,toc-tic);
        }
        inst->skew = states->skew * (1e-6l/((uint64_t)1 << 16));
#else 
        uint64_t interval = (uint64_t)((uint64_t)(frame->reception_timestamp) - (uint64_t)(previous_frame->reception_timestamp)) & 0xFFFFFFFFFULL;
        inst->skew = (double) interval / (double)(inst->nT * ((uint64_t)inst->period * ((uint64_t)1 <<16)));
#endif 
    }
#endif
    if (inst->config.postprocess) 
        os_eventq_put(os_eventq_dflt_get(), &inst->clkcal_postprocess_ev);
}

/*! 
 * @fn clkcal_set_postprocess(clkcal_instance_t * inst * inst, os_event_fn * ccp_postprocess)
 *
 * @brief Overrides the default post-processing behaviors, replacing the JSON stream with an alternative 
 * or an advanced timescale processing algorithm.
 * 
 * input parameters
 * @param inst - clkcal_instance_t *
 *
 * returns none
 */
void 
clkcal_set_postprocess(clkcal_instance_t * inst, os_event_fn * postprocess){
    inst->clkcal_postprocess_ev.ev_cb = postprocess;
    inst->clkcal_postprocess_ev.ev_arg = (void *)inst;
    inst->config.postprocess = true;
}

/*! 
 * @fn clkcal_postprocess(struct os_event * ev)
 *
 * @brief This function serves as a placeholder for timescale processing and by default creates json string for the event
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

#if MYNEWT_VAL(CLOCK_CALIBRATION_VERBOSE)
    clkcal_instance_t * inst = (clkcal_instance_t *)ev->ev_arg;
    dw1000_ccp_instance_t * ccp = (void *)inst->ccp; 
    ccp_frame_t * previous_frame = ccp->frames[(ccp->idx-2)%ccp->nframes]; 
    ccp_frame_t * frame = ccp->frames[(ccp->idx-1)%ccp->nframes]; 

    printf("{\"utime\": %lu,\"clkcal\": [%llu,%llu],\"skew\": %llu,\"nT\": [%d,%d,%d]}\n",
        os_cputime_ticks_to_usecs(os_cputime_get32()),
        frame->reception_timestamp,
        (uint64_t)((uint64_t)(frame->reception_timestamp) - (uint64_t)(previous_frame->reception_timestamp)) & 0xFFFFFFFFFFULL,
        *(uint64_t *)&inst->skew,
        inst->nT,
        frame->seq_num,
        previous_frame->seq_num
    );
#endif
}

#endif // MYNEWT_VAL(CLOCK_CALIBRATION_ENABLED)




