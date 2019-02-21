/*
 * Copyright 2018 Paul Kettle
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file timescale.h
 * @author Paul Kettle
 * @date May 16 2018
 * @brief Extended Kalman Estimator timescale estimation
 *
 */


#ifndef TIMESCALE_INCLUDE
#define TIMESCALE_INCLUDE

#include <stdint.h>
#include "srkf_dbl.h"

#define TIMESCALE_N (sizeof(timescale_states_t)/sizeof(double))
#define TIMESCALE_M (sizeof(timescale_observations_t)/sizeof(double))
#define TIMESCALE_DYNAMICS 3
#undef TIMESCALE_SKEW_OBSERVATION

typedef struct  _timescale_states_t{
    double time;                            /**< Local time state */
    double skew;                            /**< Skew parameter  */
#if (TIMESCALE_DYNAMICS == 3)
    double drift;                           /**< Rate of change of Skew  */
#endif
}timescale_states_t;

typedef struct _timescale_observations_t{
    double time;                     /**< ccp timestamp */
#ifdef TIMESCALE_SKEW_OBSERVATION
    double skew;                     /**< Skew parameter  */
#endif
}timescale_observations_t;

typedef struct _timescale_status_t{
    uint16_t selfmalloc:1;     /**< Internal flag for memory garbage collection */
    uint16_t initialized:1;    /**< Instance allocated and valids */
    uint16_t illconditioned:1; /**< Generally an indication of an illposed problem, only coarse of action is tare down and reconstruct */
    uint16_t valid:1;
    uint16_t NotPositiveDefinitive:1;
    uint16_t rollover:1;
    uint16_t halfperiod:1;
}timescale_status_t;

typedef struct _timescale_instance_t{
    srkf_dbl_instance_t * eke;        /**< Extended Kalman base class ptr */
    timescale_status_t status; /**< Instance status */
}timescale_instance_t;

timescale_instance_t * timescale_init(timescale_instance_t * inst, const double x0[], const double q[], double T);
void timescale_free(timescale_instance_t * inst);
timescale_status_t timescale_main(timescale_instance_t * inst, const double z[], const double q[], const double r[], double T);
double timescale_forward(timescale_instance_t * inst, double T);
double timescale_inverse(timescale_instance_t * inst, double T);
    
#endif
