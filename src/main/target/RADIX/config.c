/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>

#include <platform.h>

#ifdef USE_TARGET_CONFIG

#include "fc/config.h"

#include "flight/pid.h"
#include "sensors/gyro.h"
#include "blackbox/blackbox.h"

void targetConfiguration(void)
{
    gyroConfigMutable()->gyro_sync_denom = 1;
    pidConfigMutable()->pid_process_denom = 1;
    blackboxConfigMutable()->p_ratio = 128;
}
#endif
