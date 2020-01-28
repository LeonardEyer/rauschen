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

class AdaptiveFilter : public AudioStream
{
public:
	AdaptiveFilter(void): AudioStream(2,inputQueueArray), nlms(NULL){}

	void begin(int n_coeffs, float32_t learning_rate) {

		q15_t mu;
		arm_float_to_q15(&learning_rate, &mu, 1);

		nlms = new Filter(n_coeffs, mu, AUDIO_BLOCK_SAMPLES);
		arm_lms_norm_init_q15(&arm_nlms_inst, n_coeffs, &pCoeff[0], &pState[0], mu, AUDIO_BLOCK_SAMPLES, 0);

	}
	void end(void) {
		//free(lms);
	}

	void set_mu(float32_t learning_rate) {

		q15_t mu;
		arm_float_to_q15(&learning_rate, &mu, 1);
		nlms->set_mu(mu);
	}

	void set_taps(uint16_t tapCount) {
		nlms->set_taps(tapCount, AUDIO_BLOCK_SAMPLES);
	}
	virtual void update(void);
private:
	audio_block_t *inputQueueArray[2];

	Filter* nlms;
	arm_lms_norm_instance_q15 arm_nlms_inst;
	q15_t pCoeff[FIR_MAX_COEFFS];
	q15_t pState[AUDIO_BLOCK_SAMPLES + FIR_MAX_COEFFS - 1];
};

#endif
