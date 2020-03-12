//
// Created by Leonard Eyer on 03-12-2019.
//

#include <Arduino.h>
#include "AdaptiveFilter.h"
#include <SerialFlash.h>

void AdaptiveFilter::update(void)
{
	if (!enabled)
		return;

	audio_block_t *x, *y, *out, *err;

	x = receiveReadOnly(0);
	y = receiveReadOnly(1);

	if (!(y && x))
	{
		release(x);
		release(y);
		return;
	}

	// get a block for the FIR output
	out = allocate();
	err = allocate();
	if (out && err)
	{
		// Adaptive filtering of this block
		arm_lms_norm_q15(&lms_norm_inst, (q15_t*) x->data,(q15_t*) y->data,(q15_t*) out->data,(q15_t*) err->data, AUDIO_BLOCK_SAMPLES);
		// nlms->block((q15_t*) x->data,(q15_t*) y->data,(q15_t*) out->data,(q15_t*) err->data, AUDIO_BLOCK_SAMPLES);

		transmit(out, 0); // send the FIR output
		transmit(err, 1);
	}
	release(out);
	release(err);
	release(x);
	release(y);
}