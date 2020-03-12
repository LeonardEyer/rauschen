/**
 * Created on 2019-06-04.
 * 
 * Taken from the CMSIS DSP Library (arm_lms_norm_q15.c)
 * and adapted by Leonard Eyer
 * 
**/

#include "Filter.h"

Filter::Filter(uint16_t tap_count, q15_t learing_rate, uint32_t blockSize)
{

    /* Assign filter taps */
    numTaps = tap_count;

    /* Assign coefficient pointer */
    coefficients = (q15_t *) calloc(sizeof(q15_t), numTaps);

    /* Clear state buffer and size is always blockSize + numTaps - 1 */
    /* Assign state pointer */
    buffer = (q15_t *) calloc(sizeof(q15_t), numTaps + blockSize - 1U);

    /* Assign Step size value */
    mu = learing_rate;

    /* Initialize reciprocal pointer table */
    recipTable = (q15_t *)armRecipTableQ15;

    /* Initialise Energy to zero */
    energy = 0;

    /* Initialise x0 to zero */
    x0 = 0;

    /* Initialize nonfreezing */
    freezeCoeffs = false;
}

void Filter::block(q15_t *pSrc, q15_t *pRef, q15_t *pOut, q15_t *pErr, uint32_t blockSize)
{
    q15_t *pState = buffer;                        /* State pointer */
    q15_t *pCoeffs = coefficients;                 /* Coefficient pointer */
    q15_t *pStateCurnt;                            /* Points to the current sample of the state */
    q15_t *px, *pb;                                /* Temporary pointers for state and coefficient buffers */                    
    uint32_t tapCnt, blkCnt;                       /* Loop counters */
    q63_t acc;                                     /* Accumulator */
    q15_t e = 0, d = 0;                            /* Error, reference data sample */
    q15_t w = 0, in;                               /* Weight factor and state */
    q15_t errorXmu, oneByEnergy;                   /* Temporary variables to store error and mu product and reciprocal of energy */
    q15_t postShift;                               /* Post shift to be applied to weight after reciprocal calculation */
    q31_t coef;                                    /* Temporary variable for coefficient */
    q31_t acc_l, acc_h;                            /* Temporary input */
    int32_t lShift = (15 - 0); /*  Post shift  */
    int32_t uShift = (32 - lShift);

    /* S->pState points to buffer which contains previous frame (numTaps - 1) samples */
    /* pStateCurnt points to the location where the new input data should be written */
    pStateCurnt = &(buffer[(numTaps - 1U)]);

    /* initialise loop count */
    blkCnt = blockSize;

    while (blkCnt > 0U)
    {
        /* Copy the new input sample into the state buffer */
        *pStateCurnt++ = *pSrc;

        /* Initialize pState pointer */
        px = pState;

        /* Initialize coefficient pointer */
        pb = pCoeffs;

        /* Read the sample from input buffer */
        in = *pSrc++;

        /* Update the energy calculation */
        energy -= (((q31_t)x0 * (x0)) >> 15);
        energy += (((q31_t)in * (in)) >> 15);

        /* Set the accumulator to zero */
        acc = 0;

        /* Initialize tapCnt with number of samples */
        tapCnt = numTaps;

        while (tapCnt > 0U)
        {
            /* Perform the multiply-accumulate */
            acc += (q63_t)(((q31_t)(*px++) * (*pb++)));

            /* Decrement the loop counter */
            tapCnt--;
        }

        /* Calc lower part of acc */
        acc_l = acc & 0xffffffff;

        /* Calc upper part of acc */
        acc_h = (acc >> 32) & 0xffffffff;

        /* Apply shift for lower part of acc and upper part of acc */
        acc = (uint32_t)acc_l >> lShift | acc_h << uShift;

        /* Converting the result to 1.15 format and saturate the output */
        acc = __SSAT(acc, 16U);

        /* Store the result from accumulator into the destination buffer. */
        *pOut++ = (q15_t)acc;

        /* Compute and store error */
        d = *pRef++;
        e = d - (q15_t)acc;
        *pErr++ = e;

        /* Calculation of 1/energy */
        postShift = arm_recip_q15((q15_t)energy + DELTA_Q15, &oneByEnergy, recipTable);

        /* Calculation of e * mu value */
        errorXmu = (q15_t)(((q31_t)e * mu) >> 15);

        /* Calculation of (e * mu) * (1/energy) value */
        acc = (((q31_t)errorXmu * oneByEnergy) >> (15 - postShift));

        /* Weighting factor for the normalized version */
        w = (q15_t)__SSAT((q31_t)acc, 16);

        /* Initialize pState pointer */
        px = pState;

        /* Initialize coefficient pointer */
        pb = pCoeffs;

        //if(freezeCoeffs == false) {
        /* Initialize tapCnt with number of samples */
        tapCnt = numTaps;

        while (tapCnt > 0U)
        {
            /* Perform the multiply-accumulate */
            coef = (q31_t)*pb + (((q31_t)w * (*px++)) >> 15);
            *pb++ = (q15_t)__SSAT(coef, 16);

            /* Decrement loop counter */
            tapCnt--;
        }
        //}

        x0 = *pState;

        /* Advance state pointer by 1 for the next sample */
        pState = pState + 1;

        /* Decrement loop counter */
        blkCnt--;
    }

    /* Processing is complete.
     Now copy the last numTaps - 1 samples to the start of the state buffer.
     This prepares the state buffer for the next function call. */

    /* Points to the start of the pState buffer */
    pStateCurnt = buffer;

    /* Initialize tapCnt with number of samples */
    tapCnt = (numTaps - 1U);

    while (tapCnt > 0U)
    {
        *pStateCurnt++ = *pState++;

        /* Decrement loop counter */
        tapCnt--;
    }
}

void Filter::reset(uint32_t blockSize)
{
    memset(buffer, 0, sizeof(q15_t) * numTaps + (blockSize - 1U));
}

void Filter::zeroCoeff()
{
    memset(coefficients, 0, sizeof(q15_t) * numTaps);
}

void Filter::set_mu(q15_t newMu)
{
    mu = newMu;
}

void Filter::set_taps(uint16_t taps, uint32_t blockSize) {

    if(taps == numTaps) return;
    
    /* Assign coefficient pointer */
    q15_t * coefficients_new = (q15_t *) calloc(sizeof(q15_t), taps);

    /* Clear state buffer and size is always blockSize + taps - 1 */
    /* Assign state pointer */
    q15_t * buffer_new = (q15_t *) calloc(sizeof(q15_t), taps + blockSize - 1U);

    // if the new size is bigger we need to increase memory space
    if(taps > numTaps) {
        memcpy(coefficients_new, coefficients, numTaps);
        memcpy(buffer_new,buffer, numTaps);
    } else {
        // otherwise just cut off the values that don't fit into new space
        memcpy(coefficients_new, coefficients, taps);
        memcpy(buffer_new,buffer, taps);
    }

    // free state
    free(coefficients);
    free(buffer);

    // assign new state
    coefficients = coefficients_new;
    buffer = buffer_new;

    // update taps variable
    numTaps = taps;
}

void Filter::freeze() {
    freezeCoeffs = !freezeCoeffs;
}