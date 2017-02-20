#include "ExampleApplication.h"
#include <Framework/Framework.h>
#include <iostream>

#ifdef _WIN32
#undef main
#endif

int main(int /*argc*/, char* /*argv[]*/)
{	
	fw::Framework framework;
	if (!framework.initialize()) {
		return 1;
	}

	ExampleApplication app;
	if (!framework.setApplication(&app)) {
		return 1;
	}

	framework.execute();
	framework.uninitialize();
	return 0;
}