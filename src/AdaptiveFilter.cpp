//
// Created by Leonard Eyer on 03-12-2019.
//

#include <Arduino.h>
#include "AdaptiveFilter.h"

void AdaptiveFilter::update(void)
{
	audio_block_t *x, *y, *out, *err;

	x = receiveReadOnly(0);
    y = receiveReadOnly(1);
	if (!(y && x)) return;


	// get a block for the FIR output
	out = allocate();
    err = allocate();
	if (out && err) {

		//arm_lms_norm_q15(&arm_nlms_inst, (q15_t*) x->data,(q15_t*) y->data,(q15_t*) out->data,(q15_t*) err->data, AUDIO_BLOCK_SAMPLES);
		nlms->block((q15_t*) x->data,(q15_t*) y->data,(q15_t*) out->data,(q15_t*) err->data, AUDIO_BLOCK_SAMPLES);
		
		transmit(out, 0); // send the FIR output
        transmit(y, 1);
		release(out);
        release(err);
	}
	release(x);
    release(y);
;}