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

# Decawave Timescale lib

## Overview

Precompiled library for timescale processing. The Timescale algorithm estimates clock skew and drift of the local in the master clock reference frame. The library is precompiled for Cortex-m0/m3/m4f devices and works in conjunction with CCP and WCS packages. The library is included as a dependent repo as required. The 

The timescale library implements wireless clock synchronization, when used in conjunction with the mynewt-dw1000-core all timestamps are transformers into the master reference, this frame greatly simplifying the TDOA problem. The timescale_forward, timescale_inverse fuctions allow you convert between referece frames.

### Perforamnce:
| profile       | Description          | Benchmark  |
| ------------- |:-------------:| -----:|
| 1s CCP superframe  | In TDOA usercase  | 1-sigma 30 DTUs(~500ps) |

