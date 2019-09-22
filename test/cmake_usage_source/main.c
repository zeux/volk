#include "volk.h"

#include "stdio.h"
#include "assert.h"

/* In this example, we assume a Windows build and that VK_USE_PLATFORM_WIN32_KHR
was successfully defined in the CMakeLists. */
int main()
{
    VkResult r;
    uint32_t version;
    r = volkInitialize();
    if (r != VK_SUCCESS) {
        printf("volkInitialize failed!\n");
        return -1;
    }
    version = volkGetInstanceVersion();
    assert(version != 0);
    printf("Vulkan version %d.%d.%d initialized.\n",
            VK_VERSION_MAJOR(version),
            VK_VERSION_MINOR(version),
            VK_VERSION_PATCH(version));
    return 0;
}

