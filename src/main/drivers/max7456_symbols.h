/* @file max7456_symbols.h
 * @brief max7456 symbols for the mwosd font set
 *
 * @author Nathan Tsoi nathan@vertile.com
 *
 * Copyright (C) 2016 Nathan Tsoi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#pragma once

//Misc
#define SYM_END_OF_FONT             0xFF
#define SYM_BLANK                   0x00
#define SYM_COLON                   0x2D

// Heading Graphics
#define SYM_HEADING_N               0x01
#define SYM_HEADING_S               0x02
#define SYM_HEADING_E               0x03
#define SYM_HEADING_W               0x04
#define SYM_HEADING_DIVIDED_LINE    0x05
#define SYM_HEADING_LINE            0x06

// AH Center screen Graphics
#define SYM_AH_CENTER_LINE          0x07
#define SYM_AH_CENTER_LINE_RIGHT    0x08
#define SYM_AH_LEFT                 0x09
#define SYM_AH_CENTER               0x0A
#define SYM_AH_RIGHT                0x0B
#define SYM_AH_DECORATION           0x0C

// Satellite Graphics
#define SYM_SAT_L                   0x0D
#define SYM_SAT_R                   0x0E

//RSSI
#define SYM_RSSI                    0x0F

// Direction arrows
#define SYM_ARROW_SOUTH             0x10
#define SYM_ARROW_2                 0x11
#define SYM_ARROW_3                 0x12
#define SYM_ARROW_4                 0x13
#define SYM_ARROW_EAST              0x14
#define SYM_ARROW_6                 0x15
#define SYM_ARROW_7                 0x16
#define SYM_ARROW_8                 0x17
#define SYM_ARROW_NORTH             0x18
#define SYM_ARROW_10                0x19
#define SYM_ARROW_11                0x1A
#define SYM_ARROW_12                0x1B
#define SYM_ARROW_WEST              0x1C
#define SYM_ARROW_14                0x1D
#define SYM_ARROW_15                0x1E
#define SYM_ARROW_16                0x1F

// AH Bars
#define SYM_AH_BAR9_0               0x60
#define SYM_AH_BAR9_1               0x61
#define SYM_AH_BAR9_2               0x62
#define SYM_AH_BAR9_3               0x63
#define SYM_AH_BAR9_4               0x64
#define SYM_AH_BAR9_5               0x65
#define SYM_AH_BAR9_6               0x66
#define SYM_AH_BAR9_7               0x67
#define SYM_AH_BAR9_8               0x68

// Throttle Position (%)
#define SYM_THR                     0x69
#define SYM_THR1                    0x6A

// Map mode
#define SYM_HOME                    0x69
#define SYM_AIRCRAFT                0x6A

// Time
#define SYM_ON_M                    0x6B
#define SYM_FLY_M                   0x6C

// Unit 
#define SYM_M                       0x6D
#define SYM_FT                      0x6E

// Battery 
#define SYM_MAIN_BATT               0x6F
#define SYM_BATT_FULL               0x70
#define SYM_BATT_5                  0x71
#define SYM_BATT_4                  0x72
#define SYM_BATT_3                  0x73
#define SYM_BATT_2                  0x74
#define SYM_BATT_1                  0x75
#define SYM_BATT_EMPTY              0x76

// Progress bar
#define SYM_PB_START                0x77
#define SYM_PB_FULL                 0x78
#define SYM_PB_HALF                 0x79
#define SYM_PB_EMPTY                0x7A
#define SYM_PB_END                  0x7B
#define SYM_PB_CLOSE                0x7C

// Voltage and amperage
#define SYM_AMP                     0x7D
#define SYM_VOLT                    0x7E
#define SYM_MAH                     0x7F

// Menu cursor
#define SYM_CURSOR                  SYM_AH_LEFT

//New Features
#define SYM_PILOT_LOGO              0xB1