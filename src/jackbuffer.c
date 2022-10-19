// Extension for jackutils library.
// Sample buffer manipulation library
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#include <jackutils.h>
#include <stdlib.h>
#include <stdio.h>
#include <threads.h>

struct ju_buff_s {
	size_t len, pos;
	char overflow;
	mtx_t mutex;
	ju_sample_t arr[0];
};
