/* Jackutils library.
 * Copyright (C) UtoECat 2022. All rights Reserved!

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

#include <ext/ju_resample.h>

uint64_t jr_resample_impl(const float *input, float *output, int inSampleRate, int outSampleRate, uint64_t inputSize, uint32_t channels);

size_t jr_resample (const float *in, float *out, int inr, int outr, size_t ins) {
	return jr_resample_impl(in, out, inr, outr, ins, 1);
}
size_t jr_outsize (int inr, int outr, size_t ins) {
	return (ins * (double) outr / (double) inr); 
}
//(inputSize * (double) outSampleRate / (double) inSampleRate)
