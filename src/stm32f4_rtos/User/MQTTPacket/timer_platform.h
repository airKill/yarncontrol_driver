/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef __TIMER_PLATFORM_H__
#define __TIMER_PLATFORM_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include "timer_interface.h"

/**
 * definition of the Timer struct. Platform specific
 */
struct Timer {
	unsigned long end_time;
};

/**
 * @brief Timer Type
 *
 * Forward declaration of a timer struct.  The definition of this struct is
 * platform dependent.  When porting to a new platform add this definition
 * in "timer_<platform>.h" and include that file above.
 *
 */
typedef struct Timer Timer;
extern Timer last_sent, last_received,last_ping;

#ifdef __cplusplus
}
#endif

#endif /* SRC_PROTOCOL_MQTT_AWS_IOT_EMBEDDED_CLIENT_WRAPPER_PLATFORM_LINUX_COMMON_TIMER_PLATFORM_H_ */
