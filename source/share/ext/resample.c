// code from : https://github.com/cpuimage/resampler/blob/master/main.c
/*
Copyright (c) 2019 Zhihan Gao

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdint.h>
#include <stddef.h>

uint64_t jr_resample_impl(const float *input, float *output, int inSampleRate, int outSampleRate, uint64_t inputSize, uint32_t channels) {
    if (input == NULL)
        return 0;
		if (inSampleRate == outSampleRate) {
			for (size_t i = 0; i < inputSize; i++) {
				output[i] = input[i];
			}
			return inputSize;
		}
    uint64_t outputSize = (uint64_t) (inputSize * (double) outSampleRate / (double) inSampleRate);
    outputSize -= outputSize % channels;
    if (output == NULL)
        return outputSize;
    double stepDist = ((double) inSampleRate / (double) outSampleRate);
    const uint64_t fixedFraction = (1LL << 32);
    const double normFixed = (1.0 / (1LL << 32));
    uint64_t step = ((uint64_t) (stepDist * fixedFraction + 0.5));
    uint64_t curOffset = 0;
    for (uint32_t i = 0; i < outputSize; i += 1) {
        for (uint32_t c = 0; c < channels; c += 1) {
            *output++ = (float) (input[c] + (input[c + channels] - input[c]) * (
                    (double) (curOffset >> 32) + ((curOffset & (fixedFraction - 1)) * normFixed)
            )
            );
        }
        curOffset += step;
        input += (curOffset >> 32) * channels;
        curOffset &= (fixedFraction - 1);
    }
    return outputSize;
}
