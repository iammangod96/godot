/*************************************************************************/
/*  memory_pool_static_nedmalloc.cpp                                     */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2016 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#ifdef NEDMALLOC_ENABLED

//
// C++ Implementation: memory_static_malloc
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "memory_pool_static_nedmalloc.h"
#include "error_macros.h"
#include "os/memory.h"
#include <stdlib.h>
#include <stdio.h>
#include "os/copymem.h"
#include "os/os.h"

#include "nedmalloc.h"

/**
 * NOTE NOTE NOTE NOTE
 * in debug mode, this appends the memory size before the allocated, returned pointer
 * so BE CAREFUL!
 */

void* MemoryPoolStaticNedMalloc::alloc(size_t p_bytes,const char *p_description) {

 	ERR_FAIL_COND_V(p_bytes==0,0);

	MutexLock lock(mutex);

	void *mem=nedalloc::nedmalloc(p_bytes);

	ERR_FAIL_COND_V(!mem,0); //out of memory, or unreasonable request
	return mem;			
}


void* MemoryPoolStaticNedMalloc::realloc(void *p_memory,size_t p_bytes) {
	
	if (p_memory==NULL) {
		
		return alloc( p_bytes );
	}
		
	if (p_bytes<=0) {
		
		this->free(p_memory);
		ERR_FAIL_COND_V( p_bytes < 0 , NULL );
		return NULL;
	}
	
	MutexLock lock(mutex);

	return nedalloc::nedrealloc( p_memory, p_bytes );
}

void MemoryPoolStaticNedMalloc::free(void *p_ptr) {
	
	MutexLock lock(mutex);
	
	ERR_FAIL_COND(p_ptr==0);

	nedalloc::nedfree(p_ptr);
}


size_t MemoryPoolStaticNedMalloc::get_available_mem() const {

	return 0xffffffff;
}

size_t MemoryPoolStaticNedMalloc::get_total_usage() {

	return nedalloc::nedmalloc_footprint();
}
			
/* Most likely available only if memory debugger was compiled in */
int MemoryPoolStaticNedMalloc::get_alloc_count() {
	
	return 0;
}
void * MemoryPoolStaticNedMalloc::get_alloc_ptr(int p_alloc_idx) {
	
	return 0;
}
const char* MemoryPoolStaticNedMalloc::get_alloc_description(int p_alloc_idx) {
	
	
	return "";
}
size_t MemoryPoolStaticNedMalloc::get_alloc_size(int p_alloc_idx) {
	
	return 0;
}


void MemoryPoolStaticNedMalloc::debug_print_all_memory() {

	nedalloc::nedmalloc_stats();
}

MemoryPoolStaticNedMalloc::MemoryPoolStaticNedMalloc() {
	
	mutex=NULL;
#ifndef NO_THREADS

	mutex=Mutex::create(); // at this point, this should work
#endif

}


MemoryPoolStaticNedMalloc::~MemoryPoolStaticNedMalloc() {
	
	Mutex *old_mutex=mutex;
	mutex=NULL;
	if (old_mutex)
		memdelete(old_mutex);
	
#ifdef DEBUG_ENABLED

	if (OS::get_singleton()->is_stdout_verbose())
		nedalloc::nedmalloc_stats();

#endif
}


#endif

