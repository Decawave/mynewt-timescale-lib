<!--
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#
-->

# Decawave wireless clock sync lib

## Overview

Precompiled library for Wireless Clock Calibration. This library for precompiled for Cortex class M3 devices. The Library work in conjunction with Clock Calibration Packets (CCP) sent by the Clock-Master. The library supports multiple instances for the multi-clock-master application. The clkcal class binds to the post-processing callback for the CCP frame type handler and calculates the skew of the clock. 

The estimated skew is of type double and is accessible through the CCP instance ccp->clkcal->skew.

cfgsys:
*    DW1000_CLOCK_CALIBRATION: = 1   // Enable CCP backed handler
*    CLOCK_CALIBRATION: = 1          // Enable clkcal pkg
*    TIMESCALE: = 1                  // Time scale processing algorithm, required for TDOA class applcaitons
*    TIMESCALE: = 0                  // Defaults to linear interpolation algorithm, required for TWR class applications

dependencies:
*    CCP_PERIOD:                     // sample period, defined within core drivers. 
