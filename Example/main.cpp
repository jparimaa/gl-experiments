#include "ExampleApplication.h"
#include <Framework.h>
#include <iostream>

#ifdef _WIN32
#undef main
#endif

int main(int argc, char *argv[])
{
	ExampleApplication app;
	fw::Framework framework(&app);
	framework.initialize();
	framework.execute();
	framework.uninitialize();
	return 0;
}