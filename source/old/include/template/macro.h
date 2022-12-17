// Pure C templates! :D
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#pragma once

#define __MK_LIT_IMPL(A) #A

#define CONCAT(A, B) A##B
#define MAKELIT(A) __MK_LIT_IMPL(A)
