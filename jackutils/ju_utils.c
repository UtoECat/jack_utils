// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#include <ju_resample.h>

uint64_t jr_resample_impl(const float *input, float *output, int inSampleRate, int outSampleRate, uint64_t inputSize, uint32_t channels);

size_t jr_resample (const float *in, float *out, int inr, int outr, size_t ins) {
	return jr_resample_impl(in, out, inr, outr, ins, 1);
}
size_t jr_outsize (int inr, int outr, size_t ins) {
	return (ins * (double) outr / (double) inr); 
}
//(inputSize * (double) outSampleRate / (double) inSampleRate)
