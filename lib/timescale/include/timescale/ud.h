/*
 * Copyright 2017 Paul Kettle
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
 * @file ud.h
 * @author Paul Kettle
 * @date July 24 2017
 * @brief UD decomposition
 *
 */


#ifndef UD_INCLUDE
#define UD_INCLUDE

#include <stdint.h>
#include <stdlib.h>

typedef struct _ud_status_t{
    uint16_t NotPositiveDefinitive:1;  /**< Matrix inverison, input not full rank    */
}ud_status_t;

ud_status_t ud(double A[],double U[], double D[], uint16_t n);

#endif


