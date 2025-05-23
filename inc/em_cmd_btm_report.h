/**
 * Copyright 2023 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EM_CMD_STA_STEER_H
#define EM_CMD_STA_STEER_H

#include "em_cmd.h"

class em_cmd_btm_report_t : public em_cmd_t {

public:
    
	/**!
	 * @brief 
	 *
	 * This function processes the BTM report based on the provided parameters.
	 *
	 * @param[in] params The parameters required to generate the BTM report.
	 *
	 * @returns em_cmd_btm_report_t The generated BTM report.
	 *
	 * @note Ensure that the parameters are correctly initialized before calling this function.
	 */
	em_cmd_btm_report_t(em_cmd_btm_report_params_t params);
};

#endif
