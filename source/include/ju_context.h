// JackUtils Context API
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#pragma once

/* 
 * Allocates and Initializes jackutils context structure. 
 * UPDATE: added NSM support. It will be loaded in this function too!
 *
 * @arg client name (may be changed by server)
 * @arg argv[0] value. 
 * @arg has gui?
 * @ret !=NULL on sucess
 */
JU_API ju_ctx_t* (ju_ctx_init)  (ju_cstr_t, ju_cstr_t, int has_gui);

/*
 * Destroy context and release internal resources
 *
 * @arg jackutils context
 */
JU_API void      (ju_ctx_uninit) (ju_ctx_t*);

/*
 * Returns real client name
 */
JU_API ju_cstr_t (ju_get_name) (ju_ctx_t*);

/*
 * Returns information about JACK Library.
 */
JU_API ju_cstr_t (ju_jack_info) (void);

/*
 * Starts client processing. Do this after client and port creation.
 * ju_process_func_t will be called in another thread every time, when
 * audio data is available.
 *
 * This procerss callback must not call functions that might block for long time,
 * like malloc(), free(), pthread_join(), sleep(), wait() and etc.
 * => process callback should work as fast as possible! 
 *
 * @arg context
 * @arg process callack
 * @ret 0 on sucess
 */
JU_API int  (ju_start) (ju_ctx_t*, ju_process_func_t);

/*
 * Stops client processing. ju_is_online returns false after this
 */
JU_API void (ju_stop)  (ju_ctx_t*);

/*
 * This function may be called only after starting the processing!
 * @arg context
 * @ret sample array length
 */
JU_PROC size_t (ju_length) (ju_ctx_t*);

/* 
 * Returns global JACK samplerate. It does not changes during server lifetime.
 * @arg context
 * @ret sample rate
 */
JU_API  jack_nframes_t (ju_samplerate) (ju_ctx_t*);

/*
 * Checks is server online, and we are started processing (ju_start).
 *
 * UPDATE: since new jackutils version NSM support is added.
 * If connection with session manager is exists, this function
 * will return 0 ONLY in case of session manager stops us
 * or JACK server is crashed.
 * Else if there is no connection with session manager, this
 * function will return 0 only if jack is crashed OR __close 
 * port is connected. __close port is NOT created if session 
 * manager is available!
 *
 * @arg context
 * @arg timeout in MILLISeconds to wait (-1 forever, 0 - no sleep)
 */
JU_API int (ju_is_online) (ju_ctx_t* x, int sec); 

