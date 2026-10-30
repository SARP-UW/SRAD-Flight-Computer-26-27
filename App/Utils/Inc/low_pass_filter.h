/**
 * @file App/Utils/Inc/low_pass_filter.h
 * @authors Michael Zheng
 * @brief First-order low-pass filter implementation
 */

#pragma once

#include <stdint.h>

typedef struct {
	float alpha;
	float cutoff_frequency;
	float prev_output;
} low_pass_filter_t;

/**
 * @brief Initializes a low-pass filter instance.
 *
 * @param filter pointer to the filter instance to initialize.
 * @param cutoff_frequency_hz cutoff frequency of the filter in Hz.
 * @param initial_value value to seed the filter output with.
 */
void low_pass_filter_init(low_pass_filter_t *filter, float cutoff_frequency_hz, float initial_value);

/**
 * @brief Feeds a new sample through the filter and returns the filtered value.
 *
 * @param filter pointer to an initialized filter instance.
 * @param input the new raw sample.
 * @param sample_interval time since the previous sample in milliseconds.
 * @return float the filtered output.
 */
float low_pass_filter_update(low_pass_filter_t *filter, float input, uint32_t sample_interval);
