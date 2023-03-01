// REsampler function
/* Copyright (C) UtoECat 2022. All rights Reserved!

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
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
