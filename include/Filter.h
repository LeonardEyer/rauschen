//
// Created by Leonard Eyer on 2019-06-04.
//

#ifndef AKSYNTH_PROJECT_FILTER_H
#define AKSYNTH_PROJECT_FILTER_H

#include <string.h>
#include <stdlib.h>
#include <arm_math.h>
#include "arm_common_tables.h"

/**
 * Adaptive finite impulse response filter
 * Using NLMS Algorithm for weight updates
 */
class Filter
{

public:
    /**
     * Constructor
     * @param tap_count Amount of taps/coefficients
     * @param learning_rate Learning rate
     * @param blockSize how big are the audio blocks
     */
    Filter(uint16_t tap_count, q15_t learning_rate, uint32_t blockSize);

    /**
     * Procces a block 
     * @param pSrc source signal pointer
     * @param pRef reference signal pointer
     * @param pOut output pointer
     * @param pErr error pointer
     * @param blockSize size of this block
     */
    void block(q15_t *pSrc, q15_t *pRef, q15_t *pOut, q15_t *pErr, uint32_t blockSize);

    /**
     * Sets new mu
     * @param newMu
     */
    void set_mu(q15_t newMu);

    /**
     * sets new tap count
     * @param taps new tap count
     * @param blockSize size of this block
     */
    void set_taps(uint16_t taps, uint32_t blockSize);

    /**
     * @brief Reset the filter state
     * 
     * @param blockSize 
     */
    void reset(uint32_t blockSize);
    /**
     * @brief Reset the coefficient state
     * 
     */
    void zeroCoeff();
    /**
     * @brief Freeze coefficient state
     * 
     */
    void freeze();

private:
    bool freezeCoeffs;                             /* Make filter non adaptive */ 
    uint16_t numTaps;                              /* Number of filter coefficients in the filter */
    q15_t *coefficients;                           /* Pointer to coefficient array */
    q15_t *buffer;                                 /* Pointer to state array */
    q15_t mu;                                      /* Adaptive factor */
    q15_t energy;                                  /* Energy of the input */
    q15_t x0;                                      /* Temporary variable to hold input sample */
    q15_t * recipTable;
    const float eps = 1.0;
};

#endif //AKSYNTH_PROJECT_FILTER_H
