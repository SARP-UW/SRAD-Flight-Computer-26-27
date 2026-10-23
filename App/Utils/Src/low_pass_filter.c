/**
 * @file App/Utils/Inc/low_pass_filter.c
 * @authors Michael Zheng
 * @brief First-order low-pass filter implementation
 */

/**
 * Notes:
 * - Should make alpha dynamic
 */

#include "low_pass_filter.h"
#include <math.h>

float low_pass_filter(float input, float prev_output, float alpha) {
	return alpha * input + (1.0f - alpha) * prev_output;
}
