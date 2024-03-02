#include "GameFuncs.h"

void MonoAddInternalCall(const char* name, LPVOID funcPtr) {
	gameExeFuncs["mono_add_internal_call"].CdeclCall<void>(name, funcPtr);
}