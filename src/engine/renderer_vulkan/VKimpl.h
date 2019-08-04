#ifndef VK_IMPL_H_
#define VK_IMPL_H_

/*
==========================================================

      IMPLEMENTATION/PLATFORM SPECIFIC FUNCTIONS

Different platform use different libs, therefore can have
different implementation, however the interface should be
consistant.
==========================================================
*/

#define VK_NO_PROTOTYPES

#if defined(_WIN32) || defined(_WIN64)

#define VK_NO_PROTOTYPES 
#define VK_USE_PLATFORM_WIN32_KHR 1

#pragma comment(linker, "/subsystem:windows")

#elif defined(__unix__) || defined(__linux) || defined(__linux__)

#define VK_USE_PLATFORM_XCB_KHR 1

#endif

#include "../vulkan/vulkan.h"
#include "../vulkan/vk_platform.h"


void vk_destroyWindowImpl(void);
void vk_minimizeWindowImpl(void);

void* vk_getInstanceProcAddrImpl(void);

void vk_createSurfaceImpl(VkInstance hInstance, void * pCtx, VkSurfaceKHR* const pSurface);

#endif
