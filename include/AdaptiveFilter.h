//
// Created by Leonard Eyer on 03-12-2019.
//

#ifndef filter_adaptive_h_
#define filter_adaptive_h_

#define FIR_MAX_COEFFS 200

#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"
#include "Filter.h"

/**
 * @brief Adaptive filter wrapper class for managing state and initialization
 * 
 */
class AdaptiveFilter : public AudioStream
{
public:
	AdaptiveFilter(void): AudioStream(2,inputQueueArray), nlms(NULL){}

	/**
	 * @brief Initialization of parameters
	 * 
	 * @param n_coeffs Number of filter taps/coefficients
	 * @param learning_rate Learning rate
	 */
	void begin(int n_coeffs, float32_t learning_rate) {

		// Conversion to q15
		q15_t mu;
		arm_float_to_q15(&learning_rate, &mu, 1);

		nlms = new Filter(n_coeffs, mu, AUDIO_BLOCK_SAMPLES);
		arm_lms_norm_init_q15(&lms_norm_inst, n_coeffs, &pCoeffs[0], &pState[0], mu, AUDIO_BLOCK_SAMPLES, 0);
		enabled = true;
	}
	void end(void) {
		//free(lms_norm_inst);
	}

	/**
	 * @brief Set the mu object
	 * 
	 * @param learning_rate 
	 */
	void set_mu(float32_t learning_rate) {
		q15_t mu;
		arm_float_to_q15(&learning_rate, &mu, 1);
		nlms->set_mu(mu);
	}

	/**
	 * @brief Set the taps count
	 * 
	 * @param tapCount 
	 */
	void set_taps(uint16_t tapCount) {
		nlms->set_taps(tapCount, AUDIO_BLOCK_SAMPLES);
	}

	/**
	 * @brief Freeze coefficient state
	 * 
	 */
	void freeze() {
		nlms->freeze();
	}

	/**
	 * @brief Update coefficients calculate new audio block
	 * 
	 */
	virtual void update(void);
private:
	// 2 channel input
	audio_block_t *inputQueueArray[2];

	// Filter is disabled until initialized
	bool enabled = false;
	// Pointer to actual filtering instance
	Filter* nlms;
	arm_lms_norm_instance_q15 lms_norm_inst;

	// State and coefficients
	q15_t pState[AUDIO_BLOCK_SAMPLES + FIR_MAX_COEFFS - 1];
	q15_t pCoeffs[AUDIO_BLOCK_SAMPLES];
};

#endif
