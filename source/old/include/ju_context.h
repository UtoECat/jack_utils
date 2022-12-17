// JackUtils Context API
// Copyright (C) UtoECat 2022
// GNU GPL License. No Warrianty!

#pragma once

/* 
 * Allocates and Initializes jackutils context structure. 
 *
 * @arg client name (may be changed by server)
 * @arg (optional) server name. Pass NULL to autoselect.
 * @ret !=NULL on sucess
 */
JU_API ju_ctx_t* (ju_ctx_init)  (ju_cstr_t, ju_cstr_t);

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
 * This function must not call functions that might block for long time,
 * like malloc(), free(), pthread_join(), sleep(), wait() and etc.
 * This function should work as fast as possible! 
 *
 * @arg context
 * @arg process function
 * @ret 0 on sucess
 */
JU_API int  (ju_start) (ju_ctx_t*, ju_process_func_t);

/*
 * Stops client processing. ju_is_online returns false after this
 */
JU_API void (ju_stop)  (ju_ctx_t*);

/*
 * @arg context
 * @ret sample array length
 */
JU_PROC size_t (ju_length) (ju_ctx_t*);

/* 
 * @arg context
 * @ret sample rate
 */
JU_API  jack_nframes_t (ju_samplerate) (ju_ctx_t*);

/*
 * Checks is server online, and we are start procssing (ju_start).
 *
 * @arg context
 * @arg timeout in MILLISeconds to wait (-1 forever, 0 - no sleep)
 */
JU_API int (ju_is_online) (ju_ctx_t* x, int sec); 

// DEPRECATED FUNCTIONS!
// JU_API void (ju_loop) (ju_ctx_t* x, void (*) (ju_ctx_t*), int ms); 
// JU_API void (ju_wait) (void); 

