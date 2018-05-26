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

#ifndef _CLKCAL_H_
#define _CLKCAL_H_

#include <stdlib.h>
#include <stdint.h>
#include <os/os.h>
#include <dw1000/dw1000_dev.h>
#include <dw1000/dw1000_ccp.h>

#ifdef __cplusplus
extern "C" {
#endif

#if MYNEWT_VAL(TIMESCALE)
#include <timescale/timescale.h>        
#endif


typedef struct _clkcal_status_t{
    uint16_t selfmalloc:1;
    uint16_t initialized:1;
    uint16_t valid:1;
}clkcal_status_t;

typedef struct _clkcal_config_t{
    uint16_t postprocess:1;
}clkcal_config_t;

typedef struct _clkcal_control_t{
    uint16_t restart:1;
}clkcal_control_t;

typedef struct _clkcal_instance_t{
    clkcal_status_t status;
    clkcal_control_t control;
    clkcal_config_t config;
    struct _dw1000_cpp_instance_t * ccp;
#if MYNEWT_VAL(TIMESCALE)
    struct _timescale_instance_t * timescale;
    double x0[TIMESCALE_N];
    double q[TIMESCALE_N];
    double r[TIMESCALE_M];
#endif
    int16_t nT;
    double skew;
    uint32_t period;
    struct os_callout callout_postprocess;
}clkcal_instance_t; 


clkcal_instance_t * clkcal_init(clkcal_instance_t * inst, struct _dw1000_ccp_instance_t * ccp);
void clkcal_free(clkcal_instance_t * inst);
void clkcal_set_postprocess(clkcal_instance_t * inst, os_event_fn * postprocess); 

#ifdef __cplusplus
}
#endif

#endif /* _CLK_H_ */
