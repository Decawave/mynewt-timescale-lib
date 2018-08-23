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
 * @file bierman_dbl.h
 * @author Paul Kettle
 * @date July 24 2017
 * @brief Berman Factorization, square root filtering without square roots
 *  \cite bierman2006factorization
 *
 */

#ifndef BIERMAN_DBL_INCLUDE
#define BIERMAN_DBL_INCLUDE

#include <stdint.h>
#include <stdlib.h>

void bierman_dbl(double z, double r, double H[], double x[], double UP[], double DP[], uint16_t n);

#endif


