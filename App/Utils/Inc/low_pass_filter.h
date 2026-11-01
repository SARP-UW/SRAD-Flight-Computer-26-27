/**
 * @file App/Utils/Inc/low_pass_filter.h
 * @authors Michael Zheng
 * @brief First-order low-pass filter implementation
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	float alpha;
	float cutoff_frequency;
	float prev_output;
	bool has_initial_value;
} low_pass_filter_t;

/**
 * @brief Initializes a low-pass filter instance with an initial value.
 *
 * @param filter pointer to the filter instance to initialize.
 * @param cutoff_frequency_hz cutoff frequency of the filter in Hz.
 * @param initial_value value to seed the filter output with.
 */
void low_pass_filter_init(low_pass_filter_t *filter, float cutoff_frequency_hz, float initial_value);

/**
 * @brief Initializes a low-pass filter instance that does not
 *        require an initial value.
 *
 * @param filter pointer to the filter instance to initialize.
 * @param cutoff_frequency_hz cutoff frequency of the filter in Hz.
 */
void low_pass_filter_init_blank(low_pass_filter_t *filter, float cutoff_frequency_hz);

/**
 * @brief Feeds a new sample through the filter and returns the filtered value.
 *
 * @param filter pointer to an initialized filter instance.
 * @param input the new raw sample.
 * @param sample_interval time since the previous sample in milliseconds.
 * @return float the filtered output.
 */
float low_pass_filter_update(low_pass_filter_t *filter, float input, uint32_t sample_interval);
