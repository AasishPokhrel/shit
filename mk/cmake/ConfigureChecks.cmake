include(CheckCXXSourceCompiles)
include(CheckTypeSize)

check_cxx_source_compiles(
	"
	#include <iconv.h>
	// this declaration will fail when there already exists a non const char** version which returns size_t
	double iconv(iconv_t cd,  char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
	int main() { return 0; }
	"
	HAVE_ICONV_CONST
)

if(NOT EMSCRIPTEN)
  CHECK_TYPE_SIZE("void*" SIZEOF_VOID_P)
  message(STATUS "Size of void* is ${SIZEOF_VOID_P}")
endif()
