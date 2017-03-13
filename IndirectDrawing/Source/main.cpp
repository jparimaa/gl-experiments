#include "IndirectDrawingApplication.h"
#include <Framework/Framework.h>
#include <iostream>

#ifdef _WIN32
#undef main
#endif

int main(int /*argc*/, char* /*argv[]*/)
{	
	fw::Framework framework;
	bool success = framework.initialize();
	if (success) {
		IndirectDrawingApplication app;
		success = framework.setApplication(&app);
		if (success) {
			framework.execute();
			framework.uninitialize();
		}
	}

	if (!success) {
		std::cout << "Press any key to exit\n";
		std::cin.get();
		return 1;
	}
	return 0;
}