// JackUtils Context API
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#pragma once

/**
 * @file ju_context.h
 * Contains functions for context creation and manipulation. Part of the JackUtils API.
 */

/** 
 * Allocates and Initializes jackutils context structure. 
 * Connection with the session manager will be establilshed in this function too!
 *
 * @param n client name (may be changed by server)
 * @param pn filename of current executable (argv[0] value) 
 * @param has_gui has GUI?
 * @return new context on sucess. NULL on failture.
 */
JU_API ju_ctx_t* (ju_ctx_init) (ju_cstr_t n, ju_cstr_t pn, int has_gui);

/**
 * Destroy context and release resources
 * @param x JackuUtils context
 */
JU_API void      (ju_ctx_uninit) (ju_ctx_t* x);

/**
 * Returns real JACK client name
 * @param x JackuUtils context
 */
JU_API ju_cstr_t (ju_get_name) (ju_ctx_t* x);

/**
 * Returns information about JACK library
 * @return constant string with general information
 */
JU_API ju_cstr_t (ju_jack_info) (void);

/**
 * Starts JACK Audio Processing. Do this after context and port creation.
 * @ref ju_process_func_t will be called in another thread each time when audio data needs to be processed
 *
 * This process callback **should not call functions that might block for long time**,
 * like malloc(), free(), pthread_join(), sleep(), wait() and etc.
 * => *process callback should work as fast as possible!* 
 *
 * @param x JackUtils context
 * @param c process callack
 * @return 0 on sucess
 *
 * @see ju_process_func_t
 * @ref ju_is_online() returns 1 after this :)
 */
JU_API int  (ju_start) (ju_ctx_t* x, ju_process_func_t c);

/**
 * Stops client processing. 
 * @param x JackUtils Context
 *
 * @ref ju_is_online() returns 0 after this
 */
JU_API void (ju_stop)  (ju_ctx_t* x);

/**
 * This function may be called only after starting Jack Audio Processing!
 * @param x context
 * @return length of sample arrays of ports
 */
JU_PROC size_t (ju_length) (ju_ctx_t* x);

/** 
 * Returns global JACK samplerate. It does not changes during JACK server lifetime.
 * @param x context
 * @return sample rate
 */
JU_API  jack_nframes_t (ju_samplerate) (ju_ctx_t* x);

/**
 * Checks is server online, and are we started Jack Audio Processing @ref ju_start().
 *
 * UPDATE: since Session manager support was added.
 * If connection with session manager is exists, this function
 * will returns 0 ONLY in case of session manager stops us
 * or JACK server is crashed.
 *
 * Else if there is no connection with session manager, this
 * function will return 0 only if jack is crashed OR __close 
 * port is connected. __close port is NOT created if session 
 * manager is available!
 *
 * @param x JackUtils context
 * @param t timeout in MILLISeconds to wait (-1 forever, 0 - no sleep)
 * @return 1 if Jack Audio Processing is still works
 */
JU_API int (ju_is_online) (ju_ctx_t* x, int t); 

