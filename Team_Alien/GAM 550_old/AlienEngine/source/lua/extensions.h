#include "ext_array.h"
#include "ext_chunk.h"
#include "ext_string.h"
#include "ext_math.h"

#define EXTRALIBS {"array",luaopen_array },\
				  {"chunk",luaopen_chunk },\
				  {"mstring",luaopen_mstring },\
				  {"mmath",luaopen_mmath },