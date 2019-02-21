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
 * @file srkf_dbl.h
 * @author Paul Kettle
 * @date Jul 25 2017
 * @brief Square Root Extended Kalman Estimator Core
 *
 */

#ifndef SRKF_DBL_INCLUDE
#define SRKF_DBL_INCLUDE

#include <stdint.h>
#include "thornton_dbl.h"
#include "bierman_dbl.h"
#include "trisolve_dbl.h"
#include "ud_dbl.h"

typedef struct _srkf_dbl_status_t{
    uint16_t selfmalloc:1;      /**< Internal flag for garbage collection  */
    uint16_t initialized:1;     /**< Resources allocated and instance is valid     */
    uint16_t divergence:1;      /**< NAN or INF present error covarience matrix, behaviour undefined */
    uint16_t inhibit:1;         /**< postprior updates inhibited by external event. Behaviour is defined but will result in divergence if persistent.*/
    uint16_t illconditioned:1;  /**< Error covarience matrix not full rank, behaviour undefined */
    uint16_t NotPositiveDefinitive:1;
}srkf_dbl_status_t;

typedef struct _srkf_dbl_ctrl_t{
    uint16_t diagonal:1;      /**< Indicates a diagonal measurement model, as such ud-factorization not required.  */
}srkf_dbl_ctrl_t;

typedef struct _srkf_dbl_instance_t{
    uint16_t n, m, _m;   /**< Estimator dimensions */
	double * x;		/**< States             */
    double * _x;	/**< States             */
	double * y;		/**< Input Estimate     */
    double * z;		/**< Observations     */
    double * e;		/**< error              */
	double * H;     /**< Measurement matrix */
	double * P;     /**< Error Covarience   */
    double * UP;    /**< Error Covarience factorized Upper triangle post */
    double * DP;    /**< Error Covarience factorized Diagonal post */
    double * Q;     /**< Model variances    */
    double * UQ;    /**< Variances factorized Upper triangle    */
    double * DQ;    /**< Variances factorized Diagonal          */
   	double * R;     /**< Measurement variances */
    double * UR;    /**< Measurement variances Upper triangle   */
    double * DR;    /**< Measurement variances Diagonal   */
	double * S;     /**< Scratch pad matrix, used for intermidiate P */
	double * _S;    /**< Scratch pad matrix, used for intermidiate P */
	double * A;     /**< State Transition */
	double * K;     /**< Gain Matrix */
	void (*Aofx)(struct _srkf_dbl_instance_t * inst, const double u[], double T);  /**< Callback function for Nonlinear state transition */
	void (*JofAx)(struct _srkf_dbl_instance_t * inst, const double u[], double T); /**< Callback function for Jocobian of state transition */
	void (*Hofx)(struct _srkf_dbl_instance_t * inst, double T);  /**< Callback function for Nonlinear measurement equation */
	void (*JofHx)(struct _srkf_dbl_instance_t * inst, double T); /**< Callback function for Jocobian of measurement equation */
    void (*constraints)(struct _srkf_dbl_instance_t * inst, double T);    /**< Callback function constraints [optional] */
    void (*innovation)(struct _srkf_dbl_instance_t * inst, double T);    /**< Callback function innovation sequence [optional] */
    void *parent;              /**< Reference pointer for parent instance, use this to access derrived class variables */
    srkf_dbl_status_t status;        /**< Internal status and error flags*/
    srkf_dbl_ctrl_t ctrl;
}srkf_dbl_instance_t;

srkf_dbl_instance_t * srkf_dbl_init(srkf_dbl_instance_t * eke, const double x0[], uint16_t n, uint16_t m);
void srkf_dbl_free(srkf_dbl_instance_t * eke);
srkf_dbl_status_t srkf_dbl_main(srkf_dbl_instance_t * eke, const double z[], const double u[], double T, uint16_t flag);


#endif
