#include "TextureArrayApplication.h"
#include <fw/Common.h>

#ifdef _WIN32
#undef main
#endif

int main(int /*argc*/, char* /*argv[]*/)
{
    return fw::executeGenericMain<TextureArrayApplication>();
}