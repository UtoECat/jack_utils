// JackUtils Library - provides more simplificated JACK API for clients
// This header implements ATOMIC reference counter with FREED/NOT READY/READY states.
// Copyright (C) UtoECat 2022. All rights Reserved!
// This Program is free software. Licensed Under GNU GPL 3.0.
// No any warrianty!

#pragma once
#include <stdatomic.h>

// This header REQUIRES GCC11 EXTENSIONS.
// (but it's possible to rewrite this macros as inline functions, btw)

/*
 * Atomic reference counter definition.
 * Usage rules : 
 * 1. do NOT READ AND MODIFY REFCOUNTER DIRECTLY! Use special macros for this.
 * 2. do NOT READ AND MODIFY OBJECT, that has this refcnt without CHECKS!
 * 3. do NOT CHANGE default atomic operation memory syncronization for this refcounter!
 * 4. Understand how it works BEFORE USAGE!
 *
 * This refcounter has three stages with values :
 * 0  => object is RELEASED and unused and MUST NOT BE MODIFIED AND READED without aqquiring!
 * 1  => object is AQQUIRED. But STILL INITIALIZES/DESTRUCTS!
 *     > ONLY thread, that OWNS this object can use it!
 *     > DO NOT READ AND MODIFY OBJECT with this refcnt, if you are not owner!
 * 2+ => object is AQQUIRED AND READY. You can use it with some syncronization.
 *
 * This idea is fully mine. But maybe it was already implemented somewhere. IDK :)
 * This maybe best thing, what i've ever done lol.
 */
#define REFCNT refcnt
typedef _Atomic unsigned int J_REF_T;
#define DEFINE_REF J_REF_T REFCNT;

// preinitialize atomrefcnt IN OBJECT. probably not important
#define PREINITREF(A) atomic_init(&A->REFCNT, 0);

// increment and decrement AQQUIRED reference IN OBJECT.
// WARNING: be careful with it! It can't make additional checks (and you too lol)
#define INCREF(A) atomic_fetch_add(&A->REFCNT, 1);
#define DECREF(A) atomic_fetch_sub(&A->REFCNT, 1);

// tries aqquire reference -> if object was unused, refincs and returns true.
// if you're stupid and trying to aqquire already aqquired and used object, don't worry. :)
// It checks is object released, so nothing will be corrupted!
#define TRYAQREF(A) ({\\
		signed int except = 0;\\
		atomic_compare_exchange_strong(&A->REFCNT, &except, 2);\\
})

// tries release reference -> if object was used, but without reference, makes unused and returns true. OBJECT MUST NOT BE USED AFTER THIS!
// It has checks too, so it returns false if object still has reference, or already released!
// Calling this function means, that you're DONE YOUR FINNALIZATION! See function below to know how to check is object should be destroyed!
#define TRYREREF(A) ({\\
		signed int except = 1;\\
		atomic_compare_exchange_strong(&A->REFCNT, &except, 0);\\
})

// this functions checks is object AQQUIRED and has no references. 
//
#define HASREF(A) (atomic_load(&A->REFCNT) != 1)

// no used anymore
#undef REFCNT
