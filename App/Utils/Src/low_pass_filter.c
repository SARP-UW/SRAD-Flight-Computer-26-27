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

typedef struct {
	float alpha;
	float prev_output;
} low_pass_filter_t;

void low_pass_filter_init(low_pass_filter_t *filter, float alpha, float initial_value) {
	filter->alpha = alpha;
	filter->prev_output = initial_value;
}

float low_pass_filter_update(low_pass_filter_t *filter, float input) {
	float curr_output = filter->alpha * input + (1.0f - filter->alpha) * filter->prev_output;
	filter->prev_output = curr_output;
	return curr_output;
}