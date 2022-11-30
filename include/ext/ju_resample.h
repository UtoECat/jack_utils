// REsampler function
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!
//
// internal function code : Copyright (c) 2019 Zhihan Gao
// see more info in ju_resample.c

#pragma once

#include <stdint.h>
#include <ju_defs.h>

/*
 * Resamples float audio data
 * @arg input
 * @arg output
 * @arg input data samplerate
 * @arg output data samplerate
 * @arg input size (to get output size, use jr_outsize())
 */
size_t jr_resample (const float *in, float *out, int inr, int outr, size_t ins);
size_t jr_outsize (int inr, int outr, size_t ins);
//(inputSize * (double) outSampleRate / (double) inSampleRate)
