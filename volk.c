// This file is part of volk library; see volk.h for version/license details
#include "volk.h"

#ifdef __cplusplus
extern "C" {
#endif

static void volkGenLoadLoader(void* context, PFN_vkVoidFunction (*load)(void*, const char*));
static void volkGenLoadInstance(void* context, PFN_vkVoidFunction (*load)(void*, const char*));
static void volkGenLoadDevice(void* context, PFN_vkVoidFunction (*load)(void*, const char*));
static void volkGenLoadDeviceTable(struct VolkDeviceTable* table, void* context, PFN_vkVoidFunction (*load)(void*, const char*));

struct VolkDeviceTable;

VkResult volkInitialize()
{
	return VK_SUCCESS;
}

void volkLoadInstance(VkInstance* instance)
{
}

void volkLoadDevice(VkDevice* device)
{
}

void volkLoadDeviceTable(struct VolkDeviceTable* table, VkDevice* device)
{
}

/* VOLK_GENERATE_LOADERS */
static void volkGenLoadLoader(void* context, PFN_vkVoidFunction (*load)(void*, const char*))
{
}

static void volkGenLoadInstance(void* context, PFN_vkVoidFunction (*load)(void*, const char*))
{
}

static void volkGenLoadDevice(void* context, PFN_vkVoidFunction (*load)(void*, const char*))
{
}

static void volkGenLoadDeviceTable(struct VolkDeviceTable* table, void* context, PFN_vkVoidFunction (*load)(void*, const char*))
{
}
/* VOLK_GENERATE_LOADERS */

/* VOLK_GENERATE_PROTOTYPES_C */
/* VOLK_GENERATE_PROTOTYPES_C */

#ifdef __cplusplus
}
#endif
