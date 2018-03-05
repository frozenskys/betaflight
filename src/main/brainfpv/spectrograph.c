#include <stdint.h>
#include "common/maths.h"

#include "ch.h"
#include "osd_utils.h"
#include "arm_math.h"
#include "platform.h"
#include "drivers/light_led.h"

#include "common/maths.h"
#include "common/axis.h"
#include "common/color.h"
#include "common/utils.h"
#include "common/printf.h"
#include "common/typeconversion.h"

#include "drivers/sensor.h"
#include "drivers/system.h"
#include "drivers/serial.h"
#include "drivers/timer.h"
#include "drivers/light_led.h"
#include "drivers/light_ws2811strip.h"
#include "drivers/sound_beeper.h"
#include "pg/vcd.h"
#include "drivers/max7456.h"
#include "drivers/max7456_symbols.h"

#include "sensors/sensors.h"
#include "sensors/boardalignment.h"
#include "sensors/compass.h"
#include "sensors/acceleration.h"
#include "sensors/barometer.h"
#include "sensors/gyro.h"
#include "sensors/battery.h"

#include "io/flashfs.h"
#include "io/gimbal.h"
#include "io/gps.h"
#include "io/ledstrip.h"
#include "io/serial.h"
#include "io/beeper.h"
#include "io/osd.h"

#include "telemetry/telemetry.h"

#include "flight/mixer.h"
#include "flight/failsafe.h"
#include "flight/imu.h"
#include "flight/navigation.h"

#include "brainfpv/spectrograph.h"

#if defined(USE_BRAINFPV_SPECTROGRAPH)

#define SAMPLING_FREQ 3200
#define FFT_BIN(freq) ((SPEC_FFT_LENGTH / 2 - 1) * freq) / (SAMPLING_FREQ / 2)

#define SPEC_MAX_FREQ 800
#define SPEC_N_SAMPLES (FFT_BIN(SPEC_MAX_FREQ) + 1)

enum GyroAxis {
    AXIS_ROLL,
    AXIS_PITCH,
    AXIS_YAW,
    AXIS_ALL
};



// Hanning window
const float FFT_WINDOW[SPEC_FFT_LENGTH] = {
#if (SPEC_FFT_LENGTH == 512)
    0.00000, 0.00004, 0.00015, 0.00034, 0.00060, 0.00094, 0.00136, 0.00185, 0.00242, 0.00306, 0.00377, 0.00457, 0.00543, 0.00637, 0.00739, 0.00848,
    0.00964, 0.01088, 0.01220, 0.01358, 0.01504, 0.01658, 0.01818, 0.01986, 0.02161, 0.02344, 0.02533, 0.02730, 0.02934, 0.03145, 0.03363, 0.03589,
    0.03821, 0.04060, 0.04306, 0.04559, 0.04819, 0.05086, 0.05359, 0.05640, 0.05927, 0.06220, 0.06521, 0.06827, 0.07141, 0.07461, 0.07787, 0.08120,
    0.08459, 0.08804, 0.09155, 0.09513, 0.09877, 0.10247, 0.10623, 0.11004, 0.11392, 0.11786, 0.12185, 0.12590, 0.13001, 0.13417, 0.13839, 0.14266,
    0.14699, 0.15137, 0.15580, 0.16029, 0.16483, 0.16941, 0.17405, 0.17874, 0.18347, 0.18826, 0.19309, 0.19796, 0.20288, 0.20785, 0.21286, 0.21792,
    0.22301, 0.22815, 0.23333, 0.23855, 0.24381, 0.24911, 0.25445, 0.25982, 0.26523, 0.27068, 0.27616, 0.28167, 0.28722, 0.29280, 0.29841, 0.30405,
    0.30972, 0.31542, 0.32115, 0.32691, 0.33269, 0.33849, 0.34432, 0.35018, 0.35605, 0.36195, 0.36787, 0.37381, 0.37977, 0.38574, 0.39174, 0.39775,
    0.40377, 0.40981, 0.41587, 0.42193, 0.42801, 0.43410, 0.44020, 0.44631, 0.45243, 0.45855, 0.46468, 0.47081, 0.47695, 0.48310, 0.48924, 0.49539,
    0.50154, 0.50768, 0.51383, 0.51998, 0.52612, 0.53225, 0.53839, 0.54451, 0.55063, 0.55675, 0.56285, 0.56894, 0.57503, 0.58110, 0.58716, 0.59321,
    0.59924, 0.60526, 0.61126, 0.61725, 0.62321, 0.62916, 0.63509, 0.64100, 0.64689, 0.65275, 0.65860, 0.66441, 0.67021, 0.67598, 0.68172, 0.68743,
    0.69312, 0.69877, 0.70440, 0.70999, 0.71556, 0.72109, 0.72658, 0.73205, 0.73748, 0.74287, 0.74822, 0.75354, 0.75882, 0.76406, 0.76926, 0.77442,
    0.77954, 0.78462, 0.78965, 0.79464, 0.79958, 0.80448, 0.80934, 0.81414, 0.81890, 0.82361, 0.82827, 0.83289, 0.83745, 0.84196, 0.84642, 0.85083,
    0.85518, 0.85948, 0.86373, 0.86792, 0.87205, 0.87613, 0.88015, 0.88412, 0.88802, 0.89187, 0.89566, 0.89939, 0.90306, 0.90667, 0.91021, 0.91370,
    0.91712, 0.92048, 0.92377, 0.92700, 0.93017, 0.93327, 0.93630, 0.93927, 0.94218, 0.94501, 0.94778, 0.95048, 0.95312, 0.95568, 0.95818, 0.96061,
    0.96296, 0.96525, 0.96747, 0.96961, 0.97169, 0.97369, 0.97562, 0.97748, 0.97927, 0.98099, 0.98263, 0.98420, 0.98570, 0.98712, 0.98847, 0.98975,
    0.99095, 0.99207, 0.99313, 0.99411, 0.99501, 0.99584, 0.99659, 0.99727, 0.99788, 0.99840, 0.99886, 0.99923, 0.99954, 0.99976, 0.99991, 0.99999,
    0.99999, 0.99991, 0.99976, 0.99954, 0.99923, 0.99886, 0.99840, 0.99788, 0.99727, 0.99659, 0.99584, 0.99501, 0.99411, 0.99313, 0.99207, 0.99095,
    0.98975, 0.98847, 0.98712, 0.98570, 0.98420, 0.98263, 0.98099, 0.97927, 0.97748, 0.97562, 0.97369, 0.97169, 0.96961, 0.96747, 0.96525, 0.96296,
    0.96061, 0.95818, 0.95568, 0.95312, 0.95048, 0.94778, 0.94501, 0.94218, 0.93927, 0.93630, 0.93327, 0.93017, 0.92700, 0.92377, 0.92048, 0.91712,
    0.91370, 0.91021, 0.90667, 0.90306, 0.89939, 0.89566, 0.89187, 0.88802, 0.88412, 0.88015, 0.87613, 0.87205, 0.86792, 0.86373, 0.85948, 0.85518,
    0.85083, 0.84642, 0.84196, 0.83745, 0.83289, 0.82827, 0.82361, 0.81890, 0.81414, 0.80934, 0.80448, 0.79958, 0.79464, 0.78965, 0.78462, 0.77954,
    0.77442, 0.76926, 0.76406, 0.75882, 0.75354, 0.74822, 0.74287, 0.73748, 0.73205, 0.72658, 0.72109, 0.71556, 0.70999, 0.70440, 0.69877, 0.69312,
    0.68743, 0.68172, 0.67598, 0.67021, 0.66441, 0.65860, 0.65275, 0.64689, 0.64100, 0.63509, 0.62916, 0.62321, 0.61725, 0.61126, 0.60526, 0.59924,
    0.59321, 0.58716, 0.58110, 0.57503, 0.56894, 0.56285, 0.55675, 0.55063, 0.54451, 0.53839, 0.53225, 0.52612, 0.51998, 0.51383, 0.50768, 0.50154,
    0.49539, 0.48924, 0.48310, 0.47695, 0.47081, 0.46468, 0.45855, 0.45243, 0.44631, 0.44020, 0.43410, 0.42801, 0.42193, 0.41587, 0.40981, 0.40377,
    0.39775, 0.39174, 0.38574, 0.37977, 0.37381, 0.36787, 0.36195, 0.35605, 0.35018, 0.34432, 0.33849, 0.33269, 0.32691, 0.32115, 0.31542, 0.30972,
    0.30405, 0.29841, 0.29280, 0.28722, 0.28167, 0.27616, 0.27068, 0.26523, 0.25982, 0.25445, 0.24911, 0.24381, 0.23855, 0.23333, 0.22815, 0.22301,
    0.21792, 0.21286, 0.20785, 0.20288, 0.19796, 0.19309, 0.18826, 0.18347, 0.17874, 0.17405, 0.16941, 0.16483, 0.16029, 0.15580, 0.15137, 0.14699,
    0.14266, 0.13839, 0.13417, 0.13001, 0.12590, 0.12185, 0.11786, 0.11392, 0.11004, 0.10623, 0.10247, 0.09877, 0.09513, 0.09155, 0.08804, 0.08459,
    0.08120, 0.07787, 0.07461, 0.07141, 0.06827, 0.06521, 0.06220, 0.05927, 0.05640, 0.05359, 0.05086, 0.04819, 0.04559, 0.04306, 0.04060, 0.03821,
    0.03589, 0.03363, 0.03145, 0.02934, 0.02730, 0.02533, 0.02344, 0.02161, 0.01986, 0.01818, 0.01658, 0.01504, 0.01358, 0.01220, 0.01088, 0.00964,
    0.00848, 0.00739, 0.00637, 0.00543, 0.00457, 0.00377, 0.00306, 0.00242, 0.00185, 0.00136, 0.00094, 0.00060, 0.00034, 0.00015, 0.00004, 0.00000
#elif (SPEC_FFT_LENGTH == 1024)
    0.00000, 0.00001, 0.00004, 0.00008, 0.00015, 0.00024, 0.00034, 0.00046, 0.00060, 0.00076, 0.00094, 0.00114, 0.00136, 0.00159, 0.00185, 0.00212,
    0.00241, 0.00272, 0.00305, 0.00340, 0.00377, 0.00415, 0.00456, 0.00498, 0.00542, 0.00588, 0.00636, 0.00686, 0.00738, 0.00791, 0.00846, 0.00904,
    0.00963, 0.01024, 0.01086, 0.01151, 0.01217, 0.01286, 0.01356, 0.01428, 0.01501, 0.01577, 0.01654, 0.01734, 0.01815, 0.01898, 0.01982, 0.02069,
    0.02157, 0.02247, 0.02339, 0.02433, 0.02528, 0.02626, 0.02725, 0.02826, 0.02928, 0.03033, 0.03139, 0.03247, 0.03357, 0.03468, 0.03582, 0.03697,
    0.03813, 0.03932, 0.04052, 0.04174, 0.04298, 0.04423, 0.04550, 0.04679, 0.04810, 0.04942, 0.05076, 0.05212, 0.05349, 0.05488, 0.05629, 0.05771,
    0.05915, 0.06061, 0.06208, 0.06357, 0.06508, 0.06660, 0.06814, 0.06970, 0.07127, 0.07286, 0.07446, 0.07608, 0.07772, 0.07937, 0.08104, 0.08273,
    0.08443, 0.08614, 0.08787, 0.08962, 0.09138, 0.09316, 0.09495, 0.09676, 0.09858, 0.10042, 0.10227, 0.10414, 0.10603, 0.10792, 0.10984, 0.11177,
    0.11371, 0.11567, 0.11764, 0.11962, 0.12162, 0.12364, 0.12567, 0.12771, 0.12977, 0.13184, 0.13392, 0.13602, 0.13813, 0.14026, 0.14240, 0.14455,
    0.14672, 0.14890, 0.15109, 0.15330, 0.15552, 0.15775, 0.15999, 0.16225, 0.16452, 0.16681, 0.16910, 0.17141, 0.17373, 0.17606, 0.17841, 0.18077,
    0.18314, 0.18552, 0.18791, 0.19032, 0.19273, 0.19516, 0.19760, 0.20005, 0.20252, 0.20499, 0.20748, 0.20997, 0.21248, 0.21500, 0.21752, 0.22006,
    0.22261, 0.22517, 0.22774, 0.23033, 0.23292, 0.23552, 0.23813, 0.24075, 0.24338, 0.24602, 0.24867, 0.25133, 0.25400, 0.25668, 0.25937, 0.26206,
    0.26477, 0.26748, 0.27020, 0.27294, 0.27568, 0.27843, 0.28118, 0.28395, 0.28672, 0.28950, 0.29229, 0.29509, 0.29789, 0.30071, 0.30353, 0.30636,
    0.30919, 0.31203, 0.31488, 0.31774, 0.32060, 0.32347, 0.32635, 0.32923, 0.33212, 0.33502, 0.33792, 0.34083, 0.34374, 0.34666, 0.34959, 0.35252,
    0.35545, 0.35840, 0.36135, 0.36430, 0.36726, 0.37022, 0.37319, 0.37616, 0.37914, 0.38212, 0.38511, 0.38810, 0.39109, 0.39409, 0.39710, 0.40010,
    0.40311, 0.40613, 0.40915, 0.41217, 0.41519, 0.41822, 0.42125, 0.42429, 0.42732, 0.43036, 0.43341, 0.43645, 0.43950, 0.44255, 0.44560, 0.44865,
    0.45171, 0.45477, 0.45782, 0.46089, 0.46395, 0.46701, 0.47008, 0.47314, 0.47621, 0.47928, 0.48235, 0.48542, 0.48848, 0.49156, 0.49463, 0.49770,
    0.50077, 0.50384, 0.50691, 0.50998, 0.51305, 0.51612, 0.51919, 0.52226, 0.52532, 0.52839, 0.53146, 0.53452, 0.53758, 0.54065, 0.54371, 0.54676,
    0.54982, 0.55288, 0.55593, 0.55898, 0.56203, 0.56507, 0.56812, 0.57116, 0.57420, 0.57723, 0.58026, 0.58329, 0.58632, 0.58934, 0.59236, 0.59538,
    0.59839, 0.60140, 0.60441, 0.60741, 0.61040, 0.61340, 0.61639, 0.61937, 0.62235, 0.62533, 0.62830, 0.63126, 0.63422, 0.63718, 0.64013, 0.64307,
    0.64601, 0.64895, 0.65188, 0.65480, 0.65772, 0.66063, 0.66353, 0.66643, 0.66933, 0.67221, 0.67509, 0.67796, 0.68083, 0.68369, 0.68654, 0.68939,
    0.69223, 0.69506, 0.69788, 0.70070, 0.70351, 0.70631, 0.70910, 0.71189, 0.71467, 0.71744, 0.72020, 0.72295, 0.72569, 0.72843, 0.73116, 0.73388,
    0.73659, 0.73929, 0.74198, 0.74466, 0.74734, 0.75000, 0.75265, 0.75530, 0.75794, 0.76056, 0.76318, 0.76578, 0.76838, 0.77097, 0.77354, 0.77611,
    0.77866, 0.78121, 0.78374, 0.78626, 0.78878, 0.79128, 0.79377, 0.79625, 0.79872, 0.80117, 0.80362, 0.80605, 0.80848, 0.81089, 0.81329, 0.81567,
    0.81805, 0.82041, 0.82276, 0.82510, 0.82743, 0.82975, 0.83205, 0.83434, 0.83661, 0.83888, 0.84113, 0.84337, 0.84559, 0.84781, 0.85001, 0.85219,
    0.85437, 0.85653, 0.85867, 0.86081, 0.86292, 0.86503, 0.86712, 0.86920, 0.87126, 0.87331, 0.87535, 0.87737, 0.87938, 0.88137, 0.88335, 0.88532,
    0.88727, 0.88920, 0.89112, 0.89303, 0.89492, 0.89679, 0.89865, 0.90050, 0.90233, 0.90415, 0.90595, 0.90773, 0.90950, 0.91126, 0.91300, 0.91472,
    0.91643, 0.91812, 0.91979, 0.92145, 0.92310, 0.92473, 0.92634, 0.92794, 0.92952, 0.93108, 0.93263, 0.93416, 0.93567, 0.93717, 0.93866, 0.94012,
    0.94157, 0.94300, 0.94442, 0.94582, 0.94720, 0.94856, 0.94991, 0.95124, 0.95256, 0.95385, 0.95513, 0.95640, 0.95764, 0.95887, 0.96008, 0.96128,
    0.96245, 0.96361, 0.96475, 0.96588, 0.96698, 0.96807, 0.96914, 0.97020, 0.97123, 0.97225, 0.97325, 0.97423, 0.97520, 0.97614, 0.97707, 0.97798,
    0.97887, 0.97975, 0.98060, 0.98144, 0.98226, 0.98306, 0.98385, 0.98461, 0.98536, 0.98609, 0.98680, 0.98749, 0.98816, 0.98882, 0.98945, 0.99007,
    0.99067, 0.99125, 0.99182, 0.99236, 0.99288, 0.99339, 0.99388, 0.99435, 0.99480, 0.99523, 0.99565, 0.99604, 0.99642, 0.99678, 0.99711, 0.99743,
    0.99774, 0.99802, 0.99828, 0.99853, 0.99875, 0.99896, 0.99915, 0.99932, 0.99947, 0.99960, 0.99971, 0.99981, 0.99988, 0.99994, 0.99998, 1.00000,
    1.00000, 0.99998, 0.99994, 0.99988, 0.99981, 0.99971, 0.99960, 0.99947, 0.99932, 0.99915, 0.99896, 0.99875, 0.99853, 0.99828, 0.99802, 0.99774,
    0.99743, 0.99711, 0.99678, 0.99642, 0.99604, 0.99565, 0.99523, 0.99480, 0.99435, 0.99388, 0.99339, 0.99288, 0.99236, 0.99182, 0.99125, 0.99067,
    0.99007, 0.98945, 0.98882, 0.98816, 0.98749, 0.98680, 0.98609, 0.98536, 0.98461, 0.98385, 0.98306, 0.98226, 0.98144, 0.98060, 0.97975, 0.97887,
    0.97798, 0.97707, 0.97614, 0.97520, 0.97423, 0.97325, 0.97225, 0.97123, 0.97020, 0.96914, 0.96807, 0.96698, 0.96588, 0.96475, 0.96361, 0.96245,
    0.96128, 0.96008, 0.95887, 0.95764, 0.95640, 0.95513, 0.95385, 0.95256, 0.95124, 0.94991, 0.94856, 0.94720, 0.94582, 0.94442, 0.94300, 0.94157,
    0.94012, 0.93866, 0.93717, 0.93567, 0.93416, 0.93263, 0.93108, 0.92952, 0.92794, 0.92634, 0.92473, 0.92310, 0.92145, 0.91979, 0.91812, 0.91643,
    0.91472, 0.91300, 0.91126, 0.90950, 0.90773, 0.90595, 0.90415, 0.90233, 0.90050, 0.89865, 0.89679, 0.89492, 0.89303, 0.89112, 0.88920, 0.88727,
    0.88532, 0.88335, 0.88137, 0.87938, 0.87737, 0.87535, 0.87331, 0.87126, 0.86920, 0.86712, 0.86503, 0.86292, 0.86081, 0.85867, 0.85653, 0.85437,
    0.85219, 0.85001, 0.84781, 0.84559, 0.84337, 0.84113, 0.83888, 0.83661, 0.83434, 0.83205, 0.82975, 0.82743, 0.82510, 0.82276, 0.82041, 0.81805,
    0.81567, 0.81329, 0.81089, 0.80848, 0.80605, 0.80362, 0.80117, 0.79872, 0.79625, 0.79377, 0.79128, 0.78878, 0.78626, 0.78374, 0.78121, 0.77866,
    0.77611, 0.77354, 0.77097, 0.76838, 0.76578, 0.76318, 0.76056, 0.75794, 0.75530, 0.75265, 0.75000, 0.74734, 0.74466, 0.74198, 0.73929, 0.73659,
    0.73388, 0.73116, 0.72843, 0.72569, 0.72295, 0.72020, 0.71744, 0.71467, 0.71189, 0.70910, 0.70631, 0.70351, 0.70070, 0.69788, 0.69506, 0.69223,
    0.68939, 0.68654, 0.68369, 0.68083, 0.67796, 0.67509, 0.67221, 0.66933, 0.66643, 0.66353, 0.66063, 0.65772, 0.65480, 0.65188, 0.64895, 0.64601,
    0.64307, 0.64013, 0.63718, 0.63422, 0.63126, 0.62830, 0.62533, 0.62235, 0.61937, 0.61639, 0.61340, 0.61040, 0.60741, 0.60441, 0.60140, 0.59839,
    0.59538, 0.59236, 0.58934, 0.58632, 0.58329, 0.58026, 0.57723, 0.57420, 0.57116, 0.56812, 0.56507, 0.56203, 0.55898, 0.55593, 0.55288, 0.54982,
    0.54676, 0.54371, 0.54065, 0.53758, 0.53452, 0.53146, 0.52839, 0.52532, 0.52226, 0.51919, 0.51612, 0.51305, 0.50998, 0.50691, 0.50384, 0.50077,
    0.49770, 0.49463, 0.49156, 0.48848, 0.48542, 0.48235, 0.47928, 0.47621, 0.47314, 0.47008, 0.46701, 0.46395, 0.46089, 0.45782, 0.45477, 0.45171,
    0.44865, 0.44560, 0.44255, 0.43950, 0.43645, 0.43341, 0.43036, 0.42732, 0.42429, 0.42125, 0.41822, 0.41519, 0.41217, 0.40915, 0.40613, 0.40311,
    0.40010, 0.39710, 0.39409, 0.39109, 0.38810, 0.38511, 0.38212, 0.37914, 0.37616, 0.37319, 0.37022, 0.36726, 0.36430, 0.36135, 0.35840, 0.35545,
    0.35252, 0.34959, 0.34666, 0.34374, 0.34083, 0.33792, 0.33502, 0.33212, 0.32923, 0.32635, 0.32347, 0.32060, 0.31774, 0.31488, 0.31203, 0.30919,
    0.30636, 0.30353, 0.30071, 0.29789, 0.29509, 0.29229, 0.28950, 0.28672, 0.28395, 0.28118, 0.27843, 0.27568, 0.27294, 0.27020, 0.26748, 0.26477,
    0.26206, 0.25937, 0.25668, 0.25400, 0.25133, 0.24867, 0.24602, 0.24338, 0.24075, 0.23813, 0.23552, 0.23292, 0.23033, 0.22774, 0.22517, 0.22261,
    0.22006, 0.21752, 0.21500, 0.21248, 0.20997, 0.20748, 0.20499, 0.20252, 0.20005, 0.19760, 0.19516, 0.19273, 0.19032, 0.18791, 0.18552, 0.18314,
    0.18077, 0.17841, 0.17606, 0.17373, 0.17141, 0.16910, 0.16681, 0.16452, 0.16225, 0.15999, 0.15775, 0.15552, 0.15330, 0.15109, 0.14890, 0.14672,
    0.14455, 0.14240, 0.14026, 0.13813, 0.13602, 0.13392, 0.13184, 0.12977, 0.12771, 0.12567, 0.12364, 0.12162, 0.11962, 0.11764, 0.11567, 0.11371,
    0.11177, 0.10984, 0.10792, 0.10603, 0.10414, 0.10227, 0.10042, 0.09858, 0.09676, 0.09495, 0.09316, 0.09138, 0.08962, 0.08787, 0.08614, 0.08443,
    0.08273, 0.08104, 0.07937, 0.07772, 0.07608, 0.07446, 0.07286, 0.07127, 0.06970, 0.06814, 0.06660, 0.06508, 0.06357, 0.06208, 0.06061, 0.05915,
    0.05771, 0.05629, 0.05488, 0.05349, 0.05212, 0.05076, 0.04942, 0.04810, 0.04679, 0.04550, 0.04423, 0.04298, 0.04174, 0.04052, 0.03932, 0.03813,
    0.03697, 0.03582, 0.03468, 0.03357, 0.03247, 0.03139, 0.03033, 0.02928, 0.02826, 0.02725, 0.02626, 0.02528, 0.02433, 0.02339, 0.02247, 0.02157,
    0.02069, 0.01982, 0.01898, 0.01815, 0.01734, 0.01654, 0.01577, 0.01501, 0.01428, 0.01356, 0.01286, 0.01217, 0.01151, 0.01086, 0.01024, 0.00963,
    0.00904, 0.00846, 0.00791, 0.00738, 0.00686, 0.00636, 0.00588, 0.00542, 0.00498, 0.00456, 0.00415, 0.00377, 0.00340, 0.00305, 0.00272, 0.00241,
    0.00212, 0.00185, 0.00159, 0.00136, 0.00114, 0.00094, 0.00076, 0.00060, 0.00046, 0.00034, 0.00024, 0.00015, 0.00008, 0.00004, 0.00001, 0.00000
#else
#error(No WINDOW for this FFT_LENGTH)
#endif
};

const char * AXIS_NAMES[] = {"ROLL", "PITCH", "YAW"};



float spec_gyro_data_roll[SPEC_FFT_LENGTH];
float spec_gyro_data_pitch[SPEC_FFT_LENGTH];
float spec_gyro_data_yaw[SPEC_FFT_LENGTH];
float fft_out[SPEC_FFT_LENGTH];

float * spec_gyro_data_rpy[3] = {spec_gyro_data_roll, spec_gyro_data_pitch, spec_gyro_data_yaw};

static float max_rpy[3] = {0, 0, 0};
static uint8_t spec_disp_buffer_rpy[3][SPEC_N_SAMPLES];
static uint8_t spec_disp_buffer_max_rpy[3][SPEC_N_SAMPLES];

volatile bool spec_data_processed = false;

static mutex_t fftOutputMtx;
binary_semaphore_t spectrographDataReadySemaphore;

static arm_rfft_fast_instance_f32 fft_inst;

void spectrographInit()
{
    arm_rfft_fast_init_f32(&fft_inst, SPEC_FFT_LENGTH);
    chMtxObjectInit(&fftOutputMtx);

    memset(spec_disp_buffer_max_rpy[0], 0, SPEC_N_SAMPLES);
    memset(spec_disp_buffer_max_rpy[1], 0, SPEC_N_SAMPLES);
    memset(spec_disp_buffer_max_rpy[2], 0, SPEC_N_SAMPLES);
}


void spectrographMain()
{
    float max_val;
    float old_max;
    float this_val;
    uint32_t max_idx;

    chMtxLock(&fftOutputMtx);
    for (int axis = 0; axis < 3; axis ++) {
        for (int i=0; i<SPEC_FFT_LENGTH; i++) {
            spec_gyro_data_rpy[axis][i] *= FFT_WINDOW[i];
        }
        arm_rfft_fast_f32(&fft_inst, spec_gyro_data_rpy[axis], fft_out, 0);
        arm_cmplx_mag_f32(fft_out, fft_out, SPEC_N_SAMPLES);

        #define MAX_START_FREQ 50
        arm_max_f32(&fft_out[FFT_BIN(MAX_START_FREQ)], SPEC_N_SAMPLES - FFT_BIN(MAX_START_FREQ) - 1, &max_val, &max_idx);

        old_max = max_rpy[axis];
        max_rpy[axis] = MAX(max_rpy[axis], max_val);

        for (int i=0; i<SPEC_N_SAMPLES; i++){
            this_val = MIN(fft_out[i + 1], max_rpy[axis]); // clamp values
            spec_disp_buffer_max_rpy[axis][i] = 255 * MAX(this_val, old_max * (float)spec_disp_buffer_max_rpy[axis][i] / 255) / max_rpy[axis];
            spec_disp_buffer_rpy[axis][i] = 255 * (this_val / max_rpy[axis]);
        }
    }
    chMtxUnlock(&fftOutputMtx);

    spec_data_processed = true;
}



void spectrographDraw(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, enum GyroAxis axis)
{
    char tmp_str[10];
    uint8_t this_height;
    uint16_t freq;

    if (axis == AXIS_ALL){
        return;
    }

    write_hline_lm(x0 ,x0 + width , y0, 1, 1);
    write_hline_lm(x0 ,x0 + width , y0 + 1, 0, 1);
    write_vline_lm(x0, y0, y0 - height + 10, 1, 1);
    write_vline_lm(x0 - 1, y0 + 1, y0 - height + 10, 0, 1);

    write_string(AXIS_NAMES[axis], x0 + width, y0 - height, 0, 0, TEXT_VA_TOP, TEXT_HA_RIGHT, FONT_OUTLINED8X8);

    for (int i=0; i<5; i++) {
        tfp_sprintf(tmp_str, "%d", i * SPEC_MAX_FREQ / 4);
        write_string(tmp_str, x0 + i * width / 4, y0 + 2, 0, 0, TEXT_VA_TOP, TEXT_HA_CENTER, FONT_OUTLINED8X8);
    }

    chMtxLock(&fftOutputMtx);
    for (int i=0; i < width; i++) {
        freq = (i * SPEC_MAX_FREQ) / width;
        this_height = (height * (uint16_t)spec_disp_buffer_rpy[axis][FFT_BIN(freq)]) >> 8;
        write_vline_lm(x0 + i, y0, y0 -this_height , 1, 1);
        write_pixel_lm(x0 + i, y0 -this_height , 1, 0);

        this_height = (height * (uint16_t)spec_disp_buffer_max_rpy[axis][FFT_BIN(freq)]) >> 8;
        write_pixel_lm(x0 + i, y0 -this_height, 1, 0);
        write_pixel_lm(x0 + i, y0 -this_height + 1, 1, 1);
        write_pixel_lm(x0 + i, y0 -this_height - 1, 1, 1);
    }
    tfp_sprintf(tmp_str, "%d", (int)max_rpy[axis] / SPEC_FFT_LENGTH);
    write_string(tmp_str, x0, y0 - height, 0, 0, TEXT_VA_TOP, TEXT_HA_CENTER, FONT_OUTLINED8X8);
    chMtxUnlock(&fftOutputMtx);

    if (gyroConfig()->gyro_soft_notch_hz_1) {
        uint16_t pos = x0 + (gyroConfig()->gyro_soft_notch_hz_1 * width) / SPEC_MAX_FREQ;
        write_vline_lm(pos, y0, y0 - height + 10, 0, 1);
        write_string("N1", pos, y0 - height, 0, 0, TEXT_VA_TOP, TEXT_HA_CENTER, FONT_OUTLINED8X8);
        pos = x0 + (gyroConfig()->gyro_soft_notch_cutoff_1 * width) / SPEC_MAX_FREQ;
        write_vline_lm(pos, y0, y0 - height + 10, 0, 1);
        write_string("N1", pos, y0 - height, 0, 0, TEXT_VA_TOP, TEXT_HA_CENTER, FONT_OUTLINED8X8);
    }

    if (gyroConfig()->gyro_soft_notch_hz_2) {
        uint16_t pos = x0 + (gyroConfig()->gyro_soft_notch_hz_2 * width) / SPEC_MAX_FREQ;
        write_vline_lm(pos, y0, y0 - height + 10, 0, 1);
        write_string("N2", pos, y0 - height, 0, 0, TEXT_VA_TOP, TEXT_HA_CENTER, FONT_OUTLINED8X8);
        pos = x0 + (gyroConfig()->gyro_soft_notch_cutoff_2 * width) / SPEC_MAX_FREQ;
        write_vline_lm(pos, y0, y0 - height + 10, 0, 1);
        write_string("N2", pos, y0 - height, 0, 0, TEXT_VA_TOP, TEXT_HA_CENTER, FONT_OUTLINED8X8);
    }
}

#define GRAPH_LEFT (GRAPHICS_LEFT + 20)
#define GRAPH_BOTTOM (GRAPHICS_BOTTOM - 30)
#define GRAPH_WIDTH (GRAPHICS_RIGHT - GRAPHICS_LEFT - 40)
#define GRAPH_HEIGHT_TOT 200
#define GRAPH_HEIGHT_SPACING 10
#define GRAPH_HEIGHT_SINGLE (GRAPH_HEIGHT_TOT / 3 - GRAPH_HEIGHT_SPACING)


void  spectrographOSD(enum SpecCommand command)
{
    static enum GyroAxis current_axis = AXIS_ALL;

    switch (current_axis) {
        case AXIS_ALL:
            spectrographDraw(GRAPH_LEFT, GRAPH_BOTTOM - 2 * (GRAPH_HEIGHT_SINGLE + GRAPH_HEIGHT_SPACING),
                             GRAPH_WIDTH, GRAPH_HEIGHT_SINGLE, AXIS_ROLL);
            spectrographDraw(GRAPH_LEFT, GRAPH_BOTTOM - (GRAPH_HEIGHT_SINGLE + GRAPH_HEIGHT_SPACING),
                             GRAPH_WIDTH, GRAPH_HEIGHT_SINGLE, AXIS_PITCH);
            spectrographDraw(GRAPH_LEFT, GRAPH_BOTTOM,
                             GRAPH_WIDTH, GRAPH_HEIGHT_SINGLE, AXIS_YAW);
            break;
        case AXIS_ROLL:
        case AXIS_PITCH:
        case AXIS_YAW:
            spectrographDraw(GRAPH_LEFT, GRAPH_BOTTOM,
                             GRAPH_WIDTH, GRAPH_HEIGHT_TOT - 20, current_axis);
            break;
    }

    switch (command) {
        case SPEC_COMMAND_SWAXIS:
            if (current_axis == AXIS_ALL)
                current_axis = AXIS_ROLL;
            else
                current_axis += 1;
            break;
        default:
            break;
    }
    write_string("GYRO SPECTRUM", GRAPHICS_X_MIDDLE, GRAPHICS_BOTTOM - GRAPH_HEIGHT_TOT - 30, 0, 0, TEXT_VA_TOP, TEXT_HA_CENTER, FONT8X10);
    write_string("ROLL RIGHT: SWITCH   ROLL LEFT: EXIT", GRAPHICS_X_MIDDLE, GRAPHICS_BOTTOM - 18, 0, 0, TEXT_VA_TOP, TEXT_HA_CENTER, FONT8X10);
}

#endif /* defined(USE_BRAINFPV_SPECTROGRAPH) */































