/* Jack Utils Variables API
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

/**
 * @file ju_variables.h
 * This file is included by @ref jackutils.h automaticly :)
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <jackgui.h>

/**
 * @file ju_vars.h
 * Defines variables manipulation functions : render, save, load from file, etc.
 * @see jackutilsvars for more detailed info.
 */

/**
 * @defgroup jackutilsvars Jack Utils Variables Module.
 *
 * This module allows you to wrap your variable in code to allow it's easy
 * rendering, saving and loading from/to file.
 *
 * @since jackutils 5.0
 *
 * @{
 */

/** simple wraper... */
typedef struct ju_var_s ju_var_t;

/** class of variable.
 * Specifies rendering, parsing, and writing functions.
 */
struct ju_varclass {
	/**
	 * function (method) to render this variable, and
	 * maybe even process and change it with the user input :)
	 *
	 * WARINIG: DON'T FORGET TO PROTECT VARIABLE CHANGING WITH MUTEX!
	 *
	 * @param gui JackGui Context
	 * @param v current variable
	 * @see ju_vars_lock()
	 * @see ju_vars_unlock()
	 */
	void (*render) (jg_ctx_t* gui, ju_var_t* v);
	/**
	 * Parses (reads) variable from a file.   
	 *
	 * WARINIG: DON'T FORGET TO PROTECT VARIABLE CHANGING WITH MUTEX!
	 * ALWAYS use '\n' or EOF character as break for reading.
	 *
	 * @param f stdlib FILE
	 * @param v current variable
	 * @return 0 in case of sucessful parsing. any other number otherwise.
	 * @see ju_vars_lock()
	 * @see ju_vars_unlock()
	 */
	int  (*parse ) (FILE* f,  ju_var_t* v);
	/**
	 * Serializes (writes) variable to the file.
	 *
	 * WARINIG: DON'T FORGET TO PROTECT VARIABLE CHANGING WITH MUTEX!
	 *
	 * WARNING: DO NOT WRITE characters '=' and '\n' INTO THE FILE!
	 *
	 * @param f stdlib FILE
	 * @param v current variable
	 * @return 0 in case of sucessful serialization. any other number otherwise.
	 * @see ju_vars_lock()
	 * @see ju_vars_unlock()
	 */
	int  (*write ) (FILE* f,  ju_var_t* v);
	/**
	 * Specifies ratio width/height (optional).
	 */
	float widthratio;
};

/**
 * Variable shared structure.
 * Any custom structure MUST NOT be bigger than this!
 *
 * Remember that name of variable and it's serialized value
 * MUST NOT contain special characters : '=' and '\n'!
 */
struct ju_var_s {
	const struct ju_varclass* _vp; /**< pointer to the class */
	union {
		void*       p;
		int         i;
		ju_sample_t f;
	} data[5]; /** more than enough... */
	ju_cstr_t name; /**< do NOT change variable name after creation! It used for file writing and reading!
DO NOT use special characters in variables name!!!	
*/
};

/**
 * Creates ZERO variable to show end of variables array.
 * @return ZERO or NULL or NIL (same meaning) variable.
 */
JU_API ju_var_t ju_var_nil();

/**
 * Creates modifyable float variable :)
 * @param name name of the variable
 * @param val POINTER on value of variable!
 * @param min minimal float value
 * @param max maximal float value
 */
JU_API ju_var_t ju_var_float_rw(ju_cstr_t name, float* val, float min, float max);

/**
 * Creates readonly float variable :)
 * It will not be saved/parsed and changed anyhow!
 *
 * @param name name of the variable
 * @param val POINTER on value of variable!
 */
JU_API ju_var_t ju_var_float_ro(ju_cstr_t name, float* val);

/**
 * A common way to protect your variables in multithread world :)
 * ONLY in space between calling ju_vars_lock() and ju_vars_unlock() 
 * you may read and modify your variables. 
 *
 * Direct acess in one thread without locking, but locking in other, or
 * direct acess everywhere is UNDEFINED BEHAVIOUR.
 *
 * A cost of lock is much smaller than cost of hours of debugging :) believe me.
 */
JU_API void ju_vars_lock();

/**
 * See @ref ju_vars_lock()! It is important!
 */
JU_API void ju_vars_unlock();

/**
 * Saves your variables into the file.
 * @param filename self-describing
 * @param vararr array of variables. MUST termitates with ju_var_nil()!
 * @return 0 at sucess.
 */
JU_API int ju_vars_save(ju_cstr_t filename, ju_var_t* vararr);

/**
 * Loads your variables from the file.
 * @param filename self-describing
 * @param vararr array of variables. MUST termitates with ju_var_nil()!
 * @return 0 at sucess.
 */
JU_API int ju_vars_load(ju_cstr_t filename, ju_var_t* vararr);

/**
 * Renders (and probably processes) your variables.
 * @param u JackGUI context
 * @param vararr array of variables. MUST termitates with ju_var_nil()!
 * @param eh height oh each element!
 * @return 0 at sucess.
 */
JU_API int ju_vars_draw(jg_ctx_t* u, ju_var_t* vararr, int eh);

/**
 * @}
 */
