#
#  Copyright (c) 2006, Peter Kümmel, <syntheticpp@gmx.net>
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#  1. Redistributions of source code must retain the copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. The name of the author may not be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
#  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
#  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
#  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
#  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
#  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
#  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

if (ICONV_INCLUDE_DIR)
  # Already in cache, be silent
  set(ICONV_FIND_QUIETLY TRUE)
endif()

find_path(ICONV_INCLUDE_DIR iconv.h
 /usr/include
 /usr/local/include)

set(POTENTIAL_ICONV_LIBS iconv libiconv libiconv2)

find_library(ICONV_LIBRARY NAMES ${POTENTIAL_ICONV_LIBS}
	PATHS /usr/lib /usr/local/lib)

if(WIN32)
	set(ICONV_DLL_NAMES iconv.dll  libiconv.dll libiconv2.dll)
	find_file(ICONV_DLL
					NAMES ${ICONV_DLL_NAMES}
					PATHS ENV PATH
					NO_DEFAULT_PATH)
	find_file(ICONV_DLL_HELP
					NAMES ${ICONV_DLL_NAMES}
					PATHS ENV PATH
					${ICONV_INCLUDE_DIR}/../bin)
	if(ICONV_FIND_REQUIRED)
		if(NOT ICONV_DLL AND NOT ICONV_DLL_HELP)
			message(FATAL_ERROR "Could not find iconv.dll, please add correct your PATH environment variable")
		endif()
		if(NOT ICONV_DLL AND ICONV_DLL_HELP)
			get_filename_component(ICONV_DLL_HELP ${ICONV_DLL_HELP} PATH)
			message(STATUS)
			message(STATUS "Could not find iconv.dll in standard search path, please add ")
			message(STATUS "${ICONV_DLL_HELP}")
			message(STATUS "to your PATH environment variable.")
			message(STATUS)
			message(FATAL_ERROR "exit cmake")
		endif()
	endif()
	if(ICONV_INCLUDE_DIR AND ICONV_LIBRARY AND ICONV_DLL)
   		set(ICONV_FOUND TRUE)
	endif()
else()
	include(CheckFunctionExists)
	check_function_exists(iconv HAVE_ICONV_IN_LIBC)
	if(ICONV_INCLUDE_DIR AND HAVE_ICONV_IN_LIBC)
   		set(ICONV_FOUND TRUE)
		set(ICONV_LIBRARY  CACHE TYPE STRING FORCE)
	endif()
	if(ICONV_INCLUDE_DIR AND ICONV_LIBRARY)
   		set(ICONV_FOUND TRUE)
	endif()
endif()



if(ICONV_FOUND)
   if(NOT ICONV_FIND_QUIETLY)
      message(STATUS "Found iconv library: ${ICONV_LIBRARY}")
      #message(STATUS "Found iconv   dll  : ${ICONV_DLL}")
   endif()
else()
   if(ICONV_FIND_REQUIRED)
      message(STATUS "Looked for iconv library named ${POTENTIAL_ICONV_LIBS}.")
      message(STATUS "Found no acceptable iconv library. This is fatal.")
      message(STATUS "iconv header: ${ICONV_INCLUDE_DIR}")
      message(STATUS "iconv lib   : ${ICONV_LIBRARY}")
      message(FATAL_ERROR "Could NOT find iconv library")
   endif()
endif()

mark_as_advanced(ICONV_LIBRARY ICONV_INCLUDE_DIR)
