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
 * @file matrix_print.h
 * @author Paul Kettle
 * @date May 28 2016
 * @brief Matlab friendly matrix output
 *
 */

#ifndef MATRIXPRINT_INCLUDE
#define MATRIXPRINT_INCLUDE
#include <stdio.h>

#ifdef MEX
static inline void matrix_print(double x[], const char * name, int m, int n){
    mexPrintf("%s = [\n\t",name);
    for (int j=0;j<m;j++){
        for (int i=0;i<n;i++){
            mexPrintf("%f",x[j*n+i]);
            if (i!=n-1) printf(",");
        }
        if (j!=m-1) mexPrintf("\n\t");
    }
    mexPrintf("\n]\n");
}
#else
    static inline void matrix_print(double x[], const char * name, int m, int n){
    printf("%s = [\n\t",name);
    for (int j=0;j<m;j++){
        for (int i=0;i<n;i++){
            printf("% f",x[j*n+i]);
            if (i!=n-1) printf(",");
        }
        if (j!=m-1) printf("\n\t");
    }
    printf("\n]\n");
}
#endif

#endif
