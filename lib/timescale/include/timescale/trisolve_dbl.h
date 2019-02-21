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
 * @file trisolve_bdl.h
 * @author Paul Kettle
 * @date July 24 2017
 * @brief This function solves the linear, unit triangular system of equations Y = T * X 
 * using backsubstitution, and returns X such that X = T^(-1) * Y;
 * \cite moore1979optimal \cite bierman2006factorization
 *
 */

#ifndef TRISOLVE_DBL_INCLUDE
#define TRISOLVE_DBL_INCLUDE

#include <stdint.h>
#include <stdlib.h>

void trisolve_dbl(double x[], double U[], const double y[], uint16_t n, uint16_t m);

#endif


