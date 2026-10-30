/**
 * @file App/Utils/Src/low_pass_filter.c
 * @authors Michael Zheng
 * @brief First-order low-pass filter implementation
 */

/**
 * Notes:
 * - Originally implemented for use in idle_descent.c
 */

#include "low_pass_filter.h"
#include <math.h>
#define PI 3.14159265f

void low_pass_filter_init(low_pass_filter_t *filter, float cutoff_frequency_hz, float initial_value) {
	filter->cutoff_frequency = cutoff_frequency_hz;
	filter->prev_output = initial_value;
	filter->has_initial_value = true;
}

void low_pass_filter_init_blank(low_pass_filter_t *filter, float cutoff_frequency_hz) {
	filter->cutoff_frequency = cutoff_frequency_hz;
	filter->prev_output = 0.0f;
	filter->has_initial_value = false;
}

float low_pass_filter_update(low_pass_filter_t *filter, float input, uint32_t sample_interval) {
	if (!filter->has_initial_value) {
		filter->prev_output = input;
		filter->has_initial_value = true;
		return input;
	}

	// ref: https://stoicescueric.github.io/control-lab/docs/signal-processing/low-pass
	filter->alpha = 1.0f - expf(-2.0f * PI * filter->cutoff_frequency * (sample_interval / 1000.0f));
	float curr_output = filter->alpha * input + (1.0f - filter->alpha) * filter->prev_output;
	filter->prev_output = curr_output;
	return curr_output;
}
