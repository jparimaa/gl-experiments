#include "MultiDrawApplication.h"
#include <Framework/Common.h>

#define CHECK_MEMORY_LEAKS
#ifdef CHECK_MEMORY_LEAKS
#include <vld.h>
#endif

#ifdef _WIN32
#undef main
#endif

int main(int /*argc*/, char* /*argv[]*/)
{
	return fw::executeGenericMain<MultiDrawApplication>();
}