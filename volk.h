/**
 * volk
 *
 * Copyright (C) 2018-2025, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
 * Report bugs and download new versions at https://github.com/zeux/volk
 *
 * This library is distributed under the MIT License. See notice at the end of this file.
 */
/* clang-format off */
#ifndef VOLK_H_
#define VOLK_H_

#ifndef VOLK_API
#	if defined(_WIN32) || defined(_WIN64)
#		if defined(VOLK_API_FOR_DLL) // need GLOBAL definition!!!
#			if defined(VOLK_API_EXPORT)
#				define VOLK_API __declspec(dllexport)
#			else
#				define VOLK_API __declspec(dllimport)
#			endif
#		else
#			define VOLK_API
#		endif
#	else
#		if defined(VOLK_API_EXPORT) && defined(VOLK_API_FOR_DLL)
#			define VOLK_API __attribute__((visibility("default")))
#		else
#			define VOLK_API
#		endif
#	endif
#endif

#if defined(VOLK_NAMESPACE) && !defined(__cplusplus)
#error VOLK_NAMESPACE is only supported in C++
#endif

#if defined(VULKAN_H_) && !defined(VK_NO_PROTOTYPES)
#	error To use volk, you need to define VK_NO_PROTOTYPES before including vulkan.h
#endif

/* VOLK_GENERATE_VERSION_DEFINE */
#define VOLK_HEADER_VERSION 336
/* VOLK_GENERATE_VERSION_DEFINE */

#ifndef VK_NO_PROTOTYPES
#	define VK_NO_PROTOTYPES
#endif

#ifndef VULKAN_H_
#	ifdef VOLK_VULKAN_H_PATH
#		include VOLK_VULKAN_H_PATH
#	else /* Platform headers included below */
#		include <vulkan/vk_platform.h>
#		include <vulkan/vulkan_core.h>
#	endif
#endif

#ifdef __cplusplus
#ifdef VOLK_NAMESPACE
namespace volk {
#else
extern "C" {
#endif
#endif

struct VolkInstanceTable;
struct VolkDeviceTable;

/**
 * Initialize library by loading Vulkan loader; call this function before creating the Vulkan instance.
 *
 * Returns VK_SUCCESS on success and VK_ERROR_INITIALIZATION_FAILED otherwise.
 */
VkResult volkInitialize(void);

/**
 * Initialize library by providing a custom handler to load global symbols.
 *
 * This function can be used instead of volkInitialize.
 * The handler function pointer will be asked to load global Vulkan symbols which require no instance
 * (such as vkCreateInstance, vkEnumerateInstance* and vkEnumerateInstanceVersion if available).
 */
void volkInitializeCustom(PFN_vkGetInstanceProcAddr handler);

/**
 * Finalize library by unloading Vulkan loader and resetting global symbols to NULL.
 *
 * This function does not need to be called on process exit (as loader will be unloaded automatically) or if volkInitialize failed.
 * In general this function is optional to call but may be useful in rare cases eg if volk needs to be reinitialized multiple times.
 */
void volkFinalize(void);

/**
 * Get Vulkan instance version supported by the Vulkan loader, or 0 if Vulkan isn't supported
 *
 * Returns 0 if volkInitialize wasn't called or failed.
 */
uint32_t volkGetInstanceVersion(void);

/**
 * Load global function pointers using application-created VkInstance; call this function after creating the Vulkan instance.
 */
void volkLoadInstance(VkInstance instance);

/**
 * Load global function pointers using application-created VkInstance; call this function after creating the Vulkan instance.
 * Skips loading device-based function pointers, requires usage of volkLoadDevice afterwards.
 */
void volkLoadInstanceOnly(VkInstance instance);

/**
 * Load global function pointers using application-created VkDevice; call this function after creating the Vulkan device.
 *
 * Note: this is not suitable for applications that want to use multiple VkDevice objects concurrently.
 */
void volkLoadDevice(VkDevice device);

/**
 * Return last VkInstance for which global function pointers have been loaded via volkLoadInstance(),
 * or VK_NULL_HANDLE if volkLoadInstance() has not been called.
 */
VkInstance volkGetLoadedInstance(void);

/**
 * Return last VkDevice for which global function pointers have been loaded via volkLoadDevice(),
 * or VK_NULL_HANDLE if volkLoadDevice() has not been called.
 */
VkDevice volkGetLoadedDevice(void);

/**
 * Load function pointers using application-created VkInstance into a table.
 * Application should use function pointers from that table instead of using global function pointers.
 */
void volkLoadInstanceTable(struct VolkInstanceTable* table, VkInstance instance);

/**
 * Load function pointers using application-created VkDevice into a table.
 * Application should use function pointers from that table instead of using global function pointers.
 */
void volkLoadDeviceTable(struct VolkDeviceTable* table, VkDevice device);

#ifdef __cplusplus
} // extern "C" / namespace volk
#endif

/* Instead of directly including vulkan.h, we include platform-specific parts of the SDK manually
 * This is necessary to avoid including platform headers in some cases (which vulkan.h does unconditionally)
 * and replace them with forward declarations, which makes build times faster and avoids macro conflicts.
 *
 * Note that we only replace platform-specific headers when the headers are known to be problematic: very large
 * or slow to compile (Windows), or introducing unprefixed macros which can cause conflicts (Windows, Xlib).
 */
#if !defined(VULKAN_H_) && !defined(VOLK_VULKAN_H_PATH)

#ifdef VK_USE_PLATFORM_ANDROID_KHR
#include <vulkan/vulkan_android.h>
#endif

#ifdef VK_USE_PLATFORM_FUCHSIA
#include <zircon/types.h>
#include <vulkan/vulkan_fuchsia.h>
#endif

#ifdef VK_USE_PLATFORM_IOS_MVK
#include <vulkan/vulkan_ios.h>
#endif

#ifdef VK_USE_PLATFORM_MACOS_MVK
#include <vulkan/vulkan_macos.h>
#endif

#ifdef VK_USE_PLATFORM_METAL_EXT
#include <vulkan/vulkan_metal.h>
#endif

#ifdef VK_USE_PLATFORM_VI_NN
#include <vulkan/vulkan_vi.h>
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan_wayland.h>
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
typedef unsigned long DWORD;
typedef const wchar_t* LPCWSTR;
typedef void* HANDLE;
typedef struct HINSTANCE__* HINSTANCE;
typedef struct HWND__* HWND;
typedef struct HMONITOR__* HMONITOR;
typedef struct _SECURITY_ATTRIBUTES SECURITY_ATTRIBUTES;
#include <vulkan/vulkan_win32.h>
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
#include <xcb/xcb.h>
#include <vulkan/vulkan_xcb.h>
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
typedef struct _XDisplay Display;
typedef unsigned long Window;
typedef unsigned long VisualID;
#include <vulkan/vulkan_xlib.h>
#endif

#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
#include <directfb.h>
#include <vulkan/vulkan_directfb.h>
#endif

#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
typedef struct _XDisplay Display;
typedef unsigned long RROutput;
#include <vulkan/vulkan_xlib_xrandr.h>
#endif

#ifdef VK_USE_PLATFORM_GGP
#include <ggp_c/vulkan_types.h>
#include <vulkan/vulkan_ggp.h>
#endif

#ifdef VK_USE_PLATFORM_SCREEN_QNX
#include <screen/screen.h>
#include <vulkan/vulkan_screen.h>
#endif

#ifdef VK_USE_PLATFORM_SCI
#include <nvscisync.h>
#include <nvscibuf.h>
#include <vulkan/vulkan_sci.h>
#endif

#ifdef VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan_beta.h>
#endif

#endif

#ifdef __cplusplus
#ifdef VOLK_NAMESPACE
namespace volk {
#else
extern "C" {
#endif
#endif

/**
 * Instance-specific function pointer table
 */
struct VolkInstanceTable
{
	/* VOLK_GENERATE_INSTANCE_TABLE */
#if defined(VK_VERSION_1_0)
	PFN_vkCreateDevice vkCreateDevice;
	PFN_vkDestroyInstance vkDestroyInstance;
	PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
	PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties;
	PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
	PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
	PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
	PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties;
	PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties;
	PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
	PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties;
#else
	PFN_vkVoidFunction padding_f34b07f4[13];
#endif /* defined(VK_VERSION_1_0) */
#if defined(VK_VERSION_1_1)
	PFN_vkEnumeratePhysicalDeviceGroups vkEnumeratePhysicalDeviceGroups;
	PFN_vkGetPhysicalDeviceExternalBufferProperties vkGetPhysicalDeviceExternalBufferProperties;
	PFN_vkGetPhysicalDeviceExternalFenceProperties vkGetPhysicalDeviceExternalFenceProperties;
	PFN_vkGetPhysicalDeviceExternalSemaphoreProperties vkGetPhysicalDeviceExternalSemaphoreProperties;
	PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2;
	PFN_vkGetPhysicalDeviceFormatProperties2 vkGetPhysicalDeviceFormatProperties2;
	PFN_vkGetPhysicalDeviceImageFormatProperties2 vkGetPhysicalDeviceImageFormatProperties2;
	PFN_vkGetPhysicalDeviceMemoryProperties2 vkGetPhysicalDeviceMemoryProperties2;
	PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties2 vkGetPhysicalDeviceQueueFamilyProperties2;
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 vkGetPhysicalDeviceSparseImageFormatProperties2;
#else
	PFN_vkVoidFunction padding_73de037b[11];
#endif /* defined(VK_VERSION_1_1) */
#if defined(VK_VERSION_1_3)
	PFN_vkGetPhysicalDeviceToolProperties vkGetPhysicalDeviceToolProperties;
#else
	PFN_vkVoidFunction padding_60958868[1];
#endif /* defined(VK_VERSION_1_3) */
#if defined(VK_ARM_data_graph)
	PFN_vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM;
	PFN_vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM;
#else
	PFN_vkVoidFunction padding_15920a35[2];
#endif /* defined(VK_ARM_data_graph) */
#if defined(VK_ARM_performance_counters_by_region)
	PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM;
#else
	PFN_vkVoidFunction padding_4fd09193[1];
#endif /* defined(VK_ARM_performance_counters_by_region) */
#if defined(VK_ARM_tensors)
	PFN_vkGetPhysicalDeviceExternalTensorPropertiesARM vkGetPhysicalDeviceExternalTensorPropertiesARM;
#else
	PFN_vkVoidFunction padding_4622403f[1];
#endif /* defined(VK_ARM_tensors) */
#if defined(VK_EXT_acquire_drm_display)
	PFN_vkAcquireDrmDisplayEXT vkAcquireDrmDisplayEXT;
	PFN_vkGetDrmDisplayEXT vkGetDrmDisplayEXT;
#else
	PFN_vkVoidFunction padding_8e427d62[2];
#endif /* defined(VK_EXT_acquire_drm_display) */
#if defined(VK_EXT_acquire_xlib_display)
	PFN_vkAcquireXlibDisplayEXT vkAcquireXlibDisplayEXT;
	PFN_vkGetRandROutputDisplayEXT vkGetRandROutputDisplayEXT;
#else
	PFN_vkVoidFunction padding_6e6f0a05[2];
#endif /* defined(VK_EXT_acquire_xlib_display) */
#if defined(VK_EXT_calibrated_timestamps)
	PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT vkGetPhysicalDeviceCalibrateableTimeDomainsEXT;
#else
	PFN_vkVoidFunction padding_61710136[1];
#endif /* defined(VK_EXT_calibrated_timestamps) */
#if defined(VK_EXT_debug_report)
	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
	PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT;
	PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;
#else
	PFN_vkVoidFunction padding_250c28de[3];
#endif /* defined(VK_EXT_debug_report) */
#if defined(VK_EXT_debug_utils)
	PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
	PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;
	PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT;
	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
	PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT;
	PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT;
	PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT;
	PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
	PFN_vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT;
	PFN_vkSubmitDebugUtilsMessageEXT vkSubmitDebugUtilsMessageEXT;
#else
	PFN_vkVoidFunction padding_3e2e81f7[11];
#endif /* defined(VK_EXT_debug_utils) */
#if defined(VK_EXT_direct_mode_display)
	PFN_vkReleaseDisplayEXT vkReleaseDisplayEXT;
#else
	PFN_vkVoidFunction padding_899830c3[1];
#endif /* defined(VK_EXT_direct_mode_display) */
#if defined(VK_EXT_directfb_surface)
	PFN_vkCreateDirectFBSurfaceEXT vkCreateDirectFBSurfaceEXT;
	PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT vkGetPhysicalDeviceDirectFBPresentationSupportEXT;
#else
	PFN_vkVoidFunction padding_f7e0f7b1[2];
#endif /* defined(VK_EXT_directfb_surface) */
#if defined(VK_EXT_display_surface_counter)
	PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT vkGetPhysicalDeviceSurfaceCapabilities2EXT;
#else
	PFN_vkVoidFunction padding_8bff43f7[1];
#endif /* defined(VK_EXT_display_surface_counter) */
#if defined(VK_EXT_full_screen_exclusive)
	PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT vkGetPhysicalDeviceSurfacePresentModes2EXT;
#else
	PFN_vkVoidFunction padding_ff6b086[1];
#endif /* defined(VK_EXT_full_screen_exclusive) */
#if defined(VK_EXT_headless_surface)
	PFN_vkCreateHeadlessSurfaceEXT vkCreateHeadlessSurfaceEXT;
#else
	PFN_vkVoidFunction padding_f8cab9e0[1];
#endif /* defined(VK_EXT_headless_surface) */
#if defined(VK_EXT_metal_surface)
	PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT;
#else
	PFN_vkVoidFunction padding_1c6d079a[1];
#endif /* defined(VK_EXT_metal_surface) */
#if defined(VK_EXT_sample_locations)
	PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT vkGetPhysicalDeviceMultisamplePropertiesEXT;
#else
	PFN_vkVoidFunction padding_fd7ffce7[1];
#endif /* defined(VK_EXT_sample_locations) */
#if defined(VK_EXT_tooling_info)
	PFN_vkGetPhysicalDeviceToolPropertiesEXT vkGetPhysicalDeviceToolPropertiesEXT;
#else
	PFN_vkVoidFunction padding_99aa5ee9[1];
#endif /* defined(VK_EXT_tooling_info) */
#if defined(VK_FUCHSIA_imagepipe_surface)
	PFN_vkCreateImagePipeSurfaceFUCHSIA vkCreateImagePipeSurfaceFUCHSIA;
#else
	PFN_vkVoidFunction padding_6db35e8f[1];
#endif /* defined(VK_FUCHSIA_imagepipe_surface) */
#if defined(VK_GGP_stream_descriptor_surface)
	PFN_vkCreateStreamDescriptorSurfaceGGP vkCreateStreamDescriptorSurfaceGGP;
#else
	PFN_vkVoidFunction padding_cc96d0ec[1];
#endif /* defined(VK_GGP_stream_descriptor_surface) */
#if defined(VK_KHR_android_surface)
	PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
#else
	PFN_vkVoidFunction padding_ab4fe82c[1];
#endif /* defined(VK_KHR_android_surface) */
#if defined(VK_KHR_calibrated_timestamps)
	PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsKHR vkGetPhysicalDeviceCalibrateableTimeDomainsKHR;
#else
	PFN_vkVoidFunction padding_663b2fa0[1];
#endif /* defined(VK_KHR_calibrated_timestamps) */
#if defined(VK_KHR_cooperative_matrix)
	PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR;
#else
	PFN_vkVoidFunction padding_59e376cc[1];
#endif /* defined(VK_KHR_cooperative_matrix) */
#if defined(VK_KHR_device_group_creation)
	PFN_vkEnumeratePhysicalDeviceGroupsKHR vkEnumeratePhysicalDeviceGroupsKHR;
#else
	PFN_vkVoidFunction padding_6db81211[1];
#endif /* defined(VK_KHR_device_group_creation) */
#if defined(VK_KHR_display)
	PFN_vkCreateDisplayModeKHR vkCreateDisplayModeKHR;
	PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR;
	PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR;
	PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR;
	PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR;
	PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
	PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR;
#else
	PFN_vkVoidFunction padding_cce37eaf[7];
#endif /* defined(VK_KHR_display) */
#if defined(VK_KHR_external_fence_capabilities)
	PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR vkGetPhysicalDeviceExternalFencePropertiesKHR;
#else
	PFN_vkVoidFunction padding_b2076412[1];
#endif /* defined(VK_KHR_external_fence_capabilities) */
#if defined(VK_KHR_external_memory_capabilities)
	PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR vkGetPhysicalDeviceExternalBufferPropertiesKHR;
#else
	PFN_vkVoidFunction padding_f167e378[1];
#endif /* defined(VK_KHR_external_memory_capabilities) */
#if defined(VK_KHR_external_semaphore_capabilities)
	PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR vkGetPhysicalDeviceExternalSemaphorePropertiesKHR;
#else
	PFN_vkVoidFunction padding_acdaf099[1];
#endif /* defined(VK_KHR_external_semaphore_capabilities) */
#if defined(VK_KHR_fragment_shading_rate)
	PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR vkGetPhysicalDeviceFragmentShadingRatesKHR;
#else
	PFN_vkVoidFunction padding_d59cae82[1];
#endif /* defined(VK_KHR_fragment_shading_rate) */
#if defined(VK_KHR_get_display_properties2)
	PFN_vkGetDisplayModeProperties2KHR vkGetDisplayModeProperties2KHR;
	PFN_vkGetDisplayPlaneCapabilities2KHR vkGetDisplayPlaneCapabilities2KHR;
	PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR vkGetPhysicalDeviceDisplayPlaneProperties2KHR;
	PFN_vkGetPhysicalDeviceDisplayProperties2KHR vkGetPhysicalDeviceDisplayProperties2KHR;
#else
	PFN_vkVoidFunction padding_46c0938b[4];
#endif /* defined(VK_KHR_get_display_properties2) */
#if defined(VK_KHR_get_physical_device_properties2)
	PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR;
	PFN_vkGetPhysicalDeviceFormatProperties2KHR vkGetPhysicalDeviceFormatProperties2KHR;
	PFN_vkGetPhysicalDeviceImageFormatProperties2KHR vkGetPhysicalDeviceImageFormatProperties2KHR;
	PFN_vkGetPhysicalDeviceMemoryProperties2KHR vkGetPhysicalDeviceMemoryProperties2KHR;
	PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR vkGetPhysicalDeviceQueueFamilyProperties2KHR;
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR vkGetPhysicalDeviceSparseImageFormatProperties2KHR;
#else
	PFN_vkVoidFunction padding_5fac460e[7];
#endif /* defined(VK_KHR_get_physical_device_properties2) */
#if defined(VK_KHR_get_surface_capabilities2)
	PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR;
	PFN_vkGetPhysicalDeviceSurfaceFormats2KHR vkGetPhysicalDeviceSurfaceFormats2KHR;
#else
	PFN_vkVoidFunction padding_3baff606[2];
#endif /* defined(VK_KHR_get_surface_capabilities2) */
#if defined(VK_KHR_performance_query)
	PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR;
	PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR;
#else
	PFN_vkVoidFunction padding_1b45ef8f[2];
#endif /* defined(VK_KHR_performance_query) */
#if defined(VK_KHR_surface)
	PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
#else
	PFN_vkVoidFunction padding_8f1ea665[5];
#endif /* defined(VK_KHR_surface) */
#if defined(VK_KHR_video_encode_queue)
	PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR;
#else
	PFN_vkVoidFunction padding_f0a3114[1];
#endif /* defined(VK_KHR_video_encode_queue) */
#if defined(VK_KHR_video_queue)
	PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR vkGetPhysicalDeviceVideoCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR vkGetPhysicalDeviceVideoFormatPropertiesKHR;
#else
	PFN_vkVoidFunction padding_12d937aa[2];
#endif /* defined(VK_KHR_video_queue) */
#if defined(VK_KHR_wayland_surface)
	PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR;
	PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR;
#else
	PFN_vkVoidFunction padding_92436324[2];
#endif /* defined(VK_KHR_wayland_surface) */
#if defined(VK_KHR_win32_surface)
	PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
	PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR;
#else
	PFN_vkVoidFunction padding_b8dcaf56[2];
#endif /* defined(VK_KHR_win32_surface) */
#if defined(VK_KHR_xcb_surface)
	PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR;
	PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR vkGetPhysicalDeviceXcbPresentationSupportKHR;
#else
	PFN_vkVoidFunction padding_b6b79326[2];
#endif /* defined(VK_KHR_xcb_surface) */
#if defined(VK_KHR_xlib_surface)
	PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;
	PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR vkGetPhysicalDeviceXlibPresentationSupportKHR;
#else
	PFN_vkVoidFunction padding_c5e2b5db[2];
#endif /* defined(VK_KHR_xlib_surface) */
#if defined(VK_MVK_ios_surface)
	PFN_vkCreateIOSSurfaceMVK vkCreateIOSSurfaceMVK;
#else
	PFN_vkVoidFunction padding_52f99096[1];
#endif /* defined(VK_MVK_ios_surface) */
#if defined(VK_MVK_macos_surface)
	PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK;
#else
	PFN_vkVoidFunction padding_1d7ced9a[1];
#endif /* defined(VK_MVK_macos_surface) */
#if defined(VK_NN_vi_surface)
	PFN_vkCreateViSurfaceNN vkCreateViSurfaceNN;
#else
	PFN_vkVoidFunction padding_d9ec3901[1];
#endif /* defined(VK_NN_vi_surface) */
#if defined(VK_NV_acquire_winrt_display)
	PFN_vkAcquireWinrtDisplayNV vkAcquireWinrtDisplayNV;
	PFN_vkGetWinrtDisplayNV vkGetWinrtDisplayNV;
#else
	PFN_vkVoidFunction padding_41c66e6[2];
#endif /* defined(VK_NV_acquire_winrt_display) */
#if defined(VK_NV_cooperative_matrix)
	PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV vkGetPhysicalDeviceCooperativeMatrixPropertiesNV;
#else
	PFN_vkVoidFunction padding_ee7fcfc8[1];
#endif /* defined(VK_NV_cooperative_matrix) */
#if defined(VK_NV_cooperative_matrix2)
	PFN_vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV;
#else
	PFN_vkVoidFunction padding_2ec091f4[1];
#endif /* defined(VK_NV_cooperative_matrix2) */
#if defined(VK_NV_cooperative_vector)
	PFN_vkGetPhysicalDeviceCooperativeVectorPropertiesNV vkGetPhysicalDeviceCooperativeVectorPropertiesNV;
#else
	PFN_vkVoidFunction padding_50d51145[1];
#endif /* defined(VK_NV_cooperative_vector) */
#if defined(VK_NV_coverage_reduction_mode)
	PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV;
#else
	PFN_vkVoidFunction padding_9a9f15ac[1];
#endif /* defined(VK_NV_coverage_reduction_mode) */
#if defined(VK_NV_external_memory_capabilities)
	PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV vkGetPhysicalDeviceExternalImageFormatPropertiesNV;
#else
	PFN_vkVoidFunction padding_988145[1];
#endif /* defined(VK_NV_external_memory_capabilities) */
#if defined(VK_NV_optical_flow)
	PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV vkGetPhysicalDeviceOpticalFlowImageFormatsNV;
#else
	PFN_vkVoidFunction padding_46a4b95[1];
#endif /* defined(VK_NV_optical_flow) */
#if defined(VK_OHOS_surface)
	PFN_vkCreateSurfaceOHOS vkCreateSurfaceOHOS;
#else
	PFN_vkVoidFunction padding_b94570ee[1];
#endif /* defined(VK_OHOS_surface) */
#if defined(VK_QNX_screen_surface)
	PFN_vkCreateScreenSurfaceQNX vkCreateScreenSurfaceQNX;
	PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX vkGetPhysicalDeviceScreenPresentationSupportQNX;
#else
	PFN_vkVoidFunction padding_9b43b57c[2];
#endif /* defined(VK_QNX_screen_surface) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	PFN_vkGetPhysicalDevicePresentRectanglesKHR vkGetPhysicalDevicePresentRectanglesKHR;
#else
	PFN_vkVoidFunction padding_a8092b55[1];
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
	/* VOLK_GENERATE_INSTANCE_TABLE */
};

/**
 * Device-specific function pointer table
 */
struct VolkDeviceTable
{
	/* VOLK_GENERATE_DEVICE_TABLE */
#if defined(VK_VERSION_1_0)
	PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
	PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
	PFN_vkAllocateMemory vkAllocateMemory;
	PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
	PFN_vkBindBufferMemory vkBindBufferMemory;
	PFN_vkBindImageMemory vkBindImageMemory;
	PFN_vkCmdBeginQuery vkCmdBeginQuery;
	PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
	PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
	PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
	PFN_vkCmdBindPipeline vkCmdBindPipeline;
	PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
	PFN_vkCmdBlitImage vkCmdBlitImage;
	PFN_vkCmdClearAttachments vkCmdClearAttachments;
	PFN_vkCmdClearColorImage vkCmdClearColorImage;
	PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage;
	PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
	PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
	PFN_vkCmdCopyImage vkCmdCopyImage;
	PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer;
	PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults;
	PFN_vkCmdDispatch vkCmdDispatch;
	PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect;
	PFN_vkCmdDraw vkCmdDraw;
	PFN_vkCmdDrawIndexed vkCmdDrawIndexed;
	PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect;
	PFN_vkCmdDrawIndirect vkCmdDrawIndirect;
	PFN_vkCmdEndQuery vkCmdEndQuery;
	PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
	PFN_vkCmdExecuteCommands vkCmdExecuteCommands;
	PFN_vkCmdFillBuffer vkCmdFillBuffer;
	PFN_vkCmdNextSubpass vkCmdNextSubpass;
	PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
	PFN_vkCmdPushConstants vkCmdPushConstants;
	PFN_vkCmdResetEvent vkCmdResetEvent;
	PFN_vkCmdResetQueryPool vkCmdResetQueryPool;
	PFN_vkCmdResolveImage vkCmdResolveImage;
	PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants;
	PFN_vkCmdSetDepthBias vkCmdSetDepthBias;
	PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds;
	PFN_vkCmdSetEvent vkCmdSetEvent;
	PFN_vkCmdSetLineWidth vkCmdSetLineWidth;
	PFN_vkCmdSetScissor vkCmdSetScissor;
	PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask;
	PFN_vkCmdSetStencilReference vkCmdSetStencilReference;
	PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask;
	PFN_vkCmdSetViewport vkCmdSetViewport;
	PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer;
	PFN_vkCmdWaitEvents vkCmdWaitEvents;
	PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp;
	PFN_vkCreateBuffer vkCreateBuffer;
	PFN_vkCreateBufferView vkCreateBufferView;
	PFN_vkCreateCommandPool vkCreateCommandPool;
	PFN_vkCreateComputePipelines vkCreateComputePipelines;
	PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
	PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
	PFN_vkCreateEvent vkCreateEvent;
	PFN_vkCreateFence vkCreateFence;
	PFN_vkCreateFramebuffer vkCreateFramebuffer;
	PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
	PFN_vkCreateImage vkCreateImage;
	PFN_vkCreateImageView vkCreateImageView;
	PFN_vkCreatePipelineCache vkCreatePipelineCache;
	PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
	PFN_vkCreateQueryPool vkCreateQueryPool;
	PFN_vkCreateRenderPass vkCreateRenderPass;
	PFN_vkCreateSampler vkCreateSampler;
	PFN_vkCreateSemaphore vkCreateSemaphore;
	PFN_vkCreateShaderModule vkCreateShaderModule;
	PFN_vkDestroyBuffer vkDestroyBuffer;
	PFN_vkDestroyBufferView vkDestroyBufferView;
	PFN_vkDestroyCommandPool vkDestroyCommandPool;
	PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
	PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
	PFN_vkDestroyDevice vkDestroyDevice;
	PFN_vkDestroyEvent vkDestroyEvent;
	PFN_vkDestroyFence vkDestroyFence;
	PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
	PFN_vkDestroyImage vkDestroyImage;
	PFN_vkDestroyImageView vkDestroyImageView;
	PFN_vkDestroyPipeline vkDestroyPipeline;
	PFN_vkDestroyPipelineCache vkDestroyPipelineCache;
	PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
	PFN_vkDestroyQueryPool vkDestroyQueryPool;
	PFN_vkDestroyRenderPass vkDestroyRenderPass;
	PFN_vkDestroySampler vkDestroySampler;
	PFN_vkDestroySemaphore vkDestroySemaphore;
	PFN_vkDestroyShaderModule vkDestroyShaderModule;
	PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
	PFN_vkEndCommandBuffer vkEndCommandBuffer;
	PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges;
	PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
	PFN_vkFreeDescriptorSets vkFreeDescriptorSets;
	PFN_vkFreeMemory vkFreeMemory;
	PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
	PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment;
	PFN_vkGetDeviceQueue vkGetDeviceQueue;
	PFN_vkGetEventStatus vkGetEventStatus;
	PFN_vkGetFenceStatus vkGetFenceStatus;
	PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
	PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements;
	PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout;
	PFN_vkGetPipelineCacheData vkGetPipelineCacheData;
	PFN_vkGetQueryPoolResults vkGetQueryPoolResults;
	PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity;
	PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges;
	PFN_vkMapMemory vkMapMemory;
	PFN_vkMergePipelineCaches vkMergePipelineCaches;
	PFN_vkQueueBindSparse vkQueueBindSparse;
	PFN_vkQueueSubmit vkQueueSubmit;
	PFN_vkQueueWaitIdle vkQueueWaitIdle;
	PFN_vkResetCommandBuffer vkResetCommandBuffer;
	PFN_vkResetCommandPool vkResetCommandPool;
	PFN_vkResetDescriptorPool vkResetDescriptorPool;
	PFN_vkResetEvent vkResetEvent;
	PFN_vkResetFences vkResetFences;
	PFN_vkSetEvent vkSetEvent;
	PFN_vkUnmapMemory vkUnmapMemory;
	PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
	PFN_vkWaitForFences vkWaitForFences;
#else
	PFN_vkVoidFunction padding_6ce80d51[120];
#endif /* defined(VK_VERSION_1_0) */
#if defined(VK_VERSION_1_1)
	PFN_vkBindBufferMemory2 vkBindBufferMemory2;
	PFN_vkBindImageMemory2 vkBindImageMemory2;
	PFN_vkCmdDispatchBase vkCmdDispatchBase;
	PFN_vkCmdSetDeviceMask vkCmdSetDeviceMask;
	PFN_vkCreateDescriptorUpdateTemplate vkCreateDescriptorUpdateTemplate;
	PFN_vkCreateSamplerYcbcrConversion vkCreateSamplerYcbcrConversion;
	PFN_vkDestroyDescriptorUpdateTemplate vkDestroyDescriptorUpdateTemplate;
	PFN_vkDestroySamplerYcbcrConversion vkDestroySamplerYcbcrConversion;
	PFN_vkGetBufferMemoryRequirements2 vkGetBufferMemoryRequirements2;
	PFN_vkGetDescriptorSetLayoutSupport vkGetDescriptorSetLayoutSupport;
	PFN_vkGetDeviceGroupPeerMemoryFeatures vkGetDeviceGroupPeerMemoryFeatures;
	PFN_vkGetDeviceQueue2 vkGetDeviceQueue2;
	PFN_vkGetImageMemoryRequirements2 vkGetImageMemoryRequirements2;
	PFN_vkGetImageSparseMemoryRequirements2 vkGetImageSparseMemoryRequirements2;
	PFN_vkTrimCommandPool vkTrimCommandPool;
	PFN_vkUpdateDescriptorSetWithTemplate vkUpdateDescriptorSetWithTemplate;
#else
	PFN_vkVoidFunction padding_1ec56847[16];
#endif /* defined(VK_VERSION_1_1) */
#if defined(VK_VERSION_1_2)
	PFN_vkCmdBeginRenderPass2 vkCmdBeginRenderPass2;
	PFN_vkCmdDrawIndexedIndirectCount vkCmdDrawIndexedIndirectCount;
	PFN_vkCmdDrawIndirectCount vkCmdDrawIndirectCount;
	PFN_vkCmdEndRenderPass2 vkCmdEndRenderPass2;
	PFN_vkCmdNextSubpass2 vkCmdNextSubpass2;
	PFN_vkCreateRenderPass2 vkCreateRenderPass2;
	PFN_vkGetBufferDeviceAddress vkGetBufferDeviceAddress;
	PFN_vkGetBufferOpaqueCaptureAddress vkGetBufferOpaqueCaptureAddress;
	PFN_vkGetDeviceMemoryOpaqueCaptureAddress vkGetDeviceMemoryOpaqueCaptureAddress;
	PFN_vkGetSemaphoreCounterValue vkGetSemaphoreCounterValue;
	PFN_vkResetQueryPool vkResetQueryPool;
	PFN_vkSignalSemaphore vkSignalSemaphore;
	PFN_vkWaitSemaphores vkWaitSemaphores;
#else
	PFN_vkVoidFunction padding_a3e00662[13];
#endif /* defined(VK_VERSION_1_2) */
#if defined(VK_VERSION_1_3)
	PFN_vkCmdBeginRendering vkCmdBeginRendering;
	PFN_vkCmdBindVertexBuffers2 vkCmdBindVertexBuffers2;
	PFN_vkCmdBlitImage2 vkCmdBlitImage2;
	PFN_vkCmdCopyBuffer2 vkCmdCopyBuffer2;
	PFN_vkCmdCopyBufferToImage2 vkCmdCopyBufferToImage2;
	PFN_vkCmdCopyImage2 vkCmdCopyImage2;
	PFN_vkCmdCopyImageToBuffer2 vkCmdCopyImageToBuffer2;
	PFN_vkCmdEndRendering vkCmdEndRendering;
	PFN_vkCmdPipelineBarrier2 vkCmdPipelineBarrier2;
	PFN_vkCmdResetEvent2 vkCmdResetEvent2;
	PFN_vkCmdResolveImage2 vkCmdResolveImage2;
	PFN_vkCmdSetCullMode vkCmdSetCullMode;
	PFN_vkCmdSetDepthBiasEnable vkCmdSetDepthBiasEnable;
	PFN_vkCmdSetDepthBoundsTestEnable vkCmdSetDepthBoundsTestEnable;
	PFN_vkCmdSetDepthCompareOp vkCmdSetDepthCompareOp;
	PFN_vkCmdSetDepthTestEnable vkCmdSetDepthTestEnable;
	PFN_vkCmdSetDepthWriteEnable vkCmdSetDepthWriteEnable;
	PFN_vkCmdSetEvent2 vkCmdSetEvent2;
	PFN_vkCmdSetFrontFace vkCmdSetFrontFace;
	PFN_vkCmdSetPrimitiveRestartEnable vkCmdSetPrimitiveRestartEnable;
	PFN_vkCmdSetPrimitiveTopology vkCmdSetPrimitiveTopology;
	PFN_vkCmdSetRasterizerDiscardEnable vkCmdSetRasterizerDiscardEnable;
	PFN_vkCmdSetScissorWithCount vkCmdSetScissorWithCount;
	PFN_vkCmdSetStencilOp vkCmdSetStencilOp;
	PFN_vkCmdSetStencilTestEnable vkCmdSetStencilTestEnable;
	PFN_vkCmdSetViewportWithCount vkCmdSetViewportWithCount;
	PFN_vkCmdWaitEvents2 vkCmdWaitEvents2;
	PFN_vkCmdWriteTimestamp2 vkCmdWriteTimestamp2;
	PFN_vkCreatePrivateDataSlot vkCreatePrivateDataSlot;
	PFN_vkDestroyPrivateDataSlot vkDestroyPrivateDataSlot;
	PFN_vkGetDeviceBufferMemoryRequirements vkGetDeviceBufferMemoryRequirements;
	PFN_vkGetDeviceImageMemoryRequirements vkGetDeviceImageMemoryRequirements;
	PFN_vkGetDeviceImageSparseMemoryRequirements vkGetDeviceImageSparseMemoryRequirements;
	PFN_vkGetPrivateData vkGetPrivateData;
	PFN_vkQueueSubmit2 vkQueueSubmit2;
	PFN_vkSetPrivateData vkSetPrivateData;
#else
	PFN_vkVoidFunction padding_ee798a88[36];
#endif /* defined(VK_VERSION_1_3) */
#if defined(VK_VERSION_1_4)
	PFN_vkCmdBindDescriptorSets2 vkCmdBindDescriptorSets2;
	PFN_vkCmdBindIndexBuffer2 vkCmdBindIndexBuffer2;
	PFN_vkCmdPushConstants2 vkCmdPushConstants2;
	PFN_vkCmdPushDescriptorSet vkCmdPushDescriptorSet;
	PFN_vkCmdPushDescriptorSet2 vkCmdPushDescriptorSet2;
	PFN_vkCmdPushDescriptorSetWithTemplate vkCmdPushDescriptorSetWithTemplate;
	PFN_vkCmdPushDescriptorSetWithTemplate2 vkCmdPushDescriptorSetWithTemplate2;
	PFN_vkCmdSetLineStipple vkCmdSetLineStipple;
	PFN_vkCmdSetRenderingAttachmentLocations vkCmdSetRenderingAttachmentLocations;
	PFN_vkCmdSetRenderingInputAttachmentIndices vkCmdSetRenderingInputAttachmentIndices;
	PFN_vkCopyImageToImage vkCopyImageToImage;
	PFN_vkCopyImageToMemory vkCopyImageToMemory;
	PFN_vkCopyMemoryToImage vkCopyMemoryToImage;
	PFN_vkGetDeviceImageSubresourceLayout vkGetDeviceImageSubresourceLayout;
	PFN_vkGetImageSubresourceLayout2 vkGetImageSubresourceLayout2;
	PFN_vkGetRenderingAreaGranularity vkGetRenderingAreaGranularity;
	PFN_vkMapMemory2 vkMapMemory2;
	PFN_vkTransitionImageLayout vkTransitionImageLayout;
	PFN_vkUnmapMemory2 vkUnmapMemory2;
#else
	PFN_vkVoidFunction padding_82585fa3[19];
#endif /* defined(VK_VERSION_1_4) */
#if defined(VK_AMDX_shader_enqueue)
	PFN_vkCmdDispatchGraphAMDX vkCmdDispatchGraphAMDX;
	PFN_vkCmdDispatchGraphIndirectAMDX vkCmdDispatchGraphIndirectAMDX;
	PFN_vkCmdDispatchGraphIndirectCountAMDX vkCmdDispatchGraphIndirectCountAMDX;
	PFN_vkCmdInitializeGraphScratchMemoryAMDX vkCmdInitializeGraphScratchMemoryAMDX;
	PFN_vkCreateExecutionGraphPipelinesAMDX vkCreateExecutionGraphPipelinesAMDX;
	PFN_vkGetExecutionGraphPipelineNodeIndexAMDX vkGetExecutionGraphPipelineNodeIndexAMDX;
	PFN_vkGetExecutionGraphPipelineScratchSizeAMDX vkGetExecutionGraphPipelineScratchSizeAMDX;
#else
	PFN_vkVoidFunction padding_9d3e2bba[7];
#endif /* defined(VK_AMDX_shader_enqueue) */
#if defined(VK_AMD_anti_lag)
	PFN_vkAntiLagUpdateAMD vkAntiLagUpdateAMD;
#else
	PFN_vkVoidFunction padding_cf792fb4[1];
#endif /* defined(VK_AMD_anti_lag) */
#if defined(VK_AMD_buffer_marker)
	PFN_vkCmdWriteBufferMarkerAMD vkCmdWriteBufferMarkerAMD;
#else
	PFN_vkVoidFunction padding_7836e92f[1];
#endif /* defined(VK_AMD_buffer_marker) */
#if defined(VK_AMD_buffer_marker) && (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
	PFN_vkCmdWriteBufferMarker2AMD vkCmdWriteBufferMarker2AMD;
#else
	PFN_vkVoidFunction padding_bbf9b7bb[1];
#endif /* defined(VK_AMD_buffer_marker) && (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2)) */
#if defined(VK_AMD_display_native_hdr)
	PFN_vkSetLocalDimmingAMD vkSetLocalDimmingAMD;
#else
	PFN_vkVoidFunction padding_6b81b2fb[1];
#endif /* defined(VK_AMD_display_native_hdr) */
#if defined(VK_AMD_draw_indirect_count)
	PFN_vkCmdDrawIndexedIndirectCountAMD vkCmdDrawIndexedIndirectCountAMD;
	PFN_vkCmdDrawIndirectCountAMD vkCmdDrawIndirectCountAMD;
#else
	PFN_vkVoidFunction padding_fbfa9964[2];
#endif /* defined(VK_AMD_draw_indirect_count) */
#if defined(VK_AMD_shader_info)
	PFN_vkGetShaderInfoAMD vkGetShaderInfoAMD;
#else
	PFN_vkVoidFunction padding_bfb754b[1];
#endif /* defined(VK_AMD_shader_info) */
#if defined(VK_ANDROID_external_memory_android_hardware_buffer)
	PFN_vkGetAndroidHardwareBufferPropertiesANDROID vkGetAndroidHardwareBufferPropertiesANDROID;
	PFN_vkGetMemoryAndroidHardwareBufferANDROID vkGetMemoryAndroidHardwareBufferANDROID;
#else
	PFN_vkVoidFunction padding_c67b1beb[2];
#endif /* defined(VK_ANDROID_external_memory_android_hardware_buffer) */
#if defined(VK_ARM_data_graph)
	PFN_vkBindDataGraphPipelineSessionMemoryARM vkBindDataGraphPipelineSessionMemoryARM;
	PFN_vkCmdDispatchDataGraphARM vkCmdDispatchDataGraphARM;
	PFN_vkCreateDataGraphPipelineSessionARM vkCreateDataGraphPipelineSessionARM;
	PFN_vkCreateDataGraphPipelinesARM vkCreateDataGraphPipelinesARM;
	PFN_vkDestroyDataGraphPipelineSessionARM vkDestroyDataGraphPipelineSessionARM;
	PFN_vkGetDataGraphPipelineAvailablePropertiesARM vkGetDataGraphPipelineAvailablePropertiesARM;
	PFN_vkGetDataGraphPipelinePropertiesARM vkGetDataGraphPipelinePropertiesARM;
	PFN_vkGetDataGraphPipelineSessionBindPointRequirementsARM vkGetDataGraphPipelineSessionBindPointRequirementsARM;
	PFN_vkGetDataGraphPipelineSessionMemoryRequirementsARM vkGetDataGraphPipelineSessionMemoryRequirementsARM;
#else
	PFN_vkVoidFunction padding_894d85d8[9];
#endif /* defined(VK_ARM_data_graph) */
#if defined(VK_ARM_tensors)
	PFN_vkBindTensorMemoryARM vkBindTensorMemoryARM;
	PFN_vkCmdCopyTensorARM vkCmdCopyTensorARM;
	PFN_vkCreateTensorARM vkCreateTensorARM;
	PFN_vkCreateTensorViewARM vkCreateTensorViewARM;
	PFN_vkDestroyTensorARM vkDestroyTensorARM;
	PFN_vkDestroyTensorViewARM vkDestroyTensorViewARM;
	PFN_vkGetDeviceTensorMemoryRequirementsARM vkGetDeviceTensorMemoryRequirementsARM;
	PFN_vkGetTensorMemoryRequirementsARM vkGetTensorMemoryRequirementsARM;
#else
	PFN_vkVoidFunction padding_df67a729[8];
#endif /* defined(VK_ARM_tensors) */
#if defined(VK_ARM_tensors) && defined(VK_EXT_descriptor_buffer)
	PFN_vkGetTensorOpaqueCaptureDescriptorDataARM vkGetTensorOpaqueCaptureDescriptorDataARM;
	PFN_vkGetTensorViewOpaqueCaptureDescriptorDataARM vkGetTensorViewOpaqueCaptureDescriptorDataARM;
#else
	PFN_vkVoidFunction padding_9483bf7e[2];
#endif /* defined(VK_ARM_tensors) && defined(VK_EXT_descriptor_buffer) */
#if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
	PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT vkCmdSetAttachmentFeedbackLoopEnableEXT;
#else
	PFN_vkVoidFunction padding_760a41f5[1];
#endif /* defined(VK_EXT_attachment_feedback_loop_dynamic_state) */
#if defined(VK_EXT_buffer_device_address)
	PFN_vkGetBufferDeviceAddressEXT vkGetBufferDeviceAddressEXT;
#else
	PFN_vkVoidFunction padding_3b69d885[1];
#endif /* defined(VK_EXT_buffer_device_address) */
#if defined(VK_EXT_calibrated_timestamps)
	PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT;
#else
	PFN_vkVoidFunction padding_d0981c89[1];
#endif /* defined(VK_EXT_calibrated_timestamps) */
#if defined(VK_EXT_color_write_enable)
	PFN_vkCmdSetColorWriteEnableEXT vkCmdSetColorWriteEnableEXT;
#else
	PFN_vkVoidFunction padding_d301ecc3[1];
#endif /* defined(VK_EXT_color_write_enable) */
#if defined(VK_EXT_conditional_rendering)
	PFN_vkCmdBeginConditionalRenderingEXT vkCmdBeginConditionalRenderingEXT;
	PFN_vkCmdEndConditionalRenderingEXT vkCmdEndConditionalRenderingEXT;
#else
	PFN_vkVoidFunction padding_ab532c18[2];
#endif /* defined(VK_EXT_conditional_rendering) */
#if defined(VK_EXT_custom_resolve) && (defined(VK_KHR_dynamic_rendering) || defined(VK_VERSION_1_3))
	PFN_vkCmdBeginCustomResolveEXT vkCmdBeginCustomResolveEXT;
#else
	PFN_vkVoidFunction padding_962e418a[1];
#endif /* defined(VK_EXT_custom_resolve) && (defined(VK_KHR_dynamic_rendering) || defined(VK_VERSION_1_3)) */
#if defined(VK_EXT_debug_marker)
	PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT;
	PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT;
	PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT;
	PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT;
	PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT;
#else
	PFN_vkVoidFunction padding_89986968[5];
#endif /* defined(VK_EXT_debug_marker) */
#if defined(VK_EXT_depth_bias_control)
	PFN_vkCmdSetDepthBias2EXT vkCmdSetDepthBias2EXT;
#else
	PFN_vkVoidFunction padding_bcddab4d[1];
#endif /* defined(VK_EXT_depth_bias_control) */
#if defined(VK_EXT_descriptor_buffer)
	PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT vkCmdBindDescriptorBufferEmbeddedSamplersEXT;
	PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT;
	PFN_vkCmdSetDescriptorBufferOffsetsEXT vkCmdSetDescriptorBufferOffsetsEXT;
	PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT vkGetBufferOpaqueCaptureDescriptorDataEXT;
	PFN_vkGetDescriptorEXT vkGetDescriptorEXT;
	PFN_vkGetDescriptorSetLayoutBindingOffsetEXT vkGetDescriptorSetLayoutBindingOffsetEXT;
	PFN_vkGetDescriptorSetLayoutSizeEXT vkGetDescriptorSetLayoutSizeEXT;
	PFN_vkGetImageOpaqueCaptureDescriptorDataEXT vkGetImageOpaqueCaptureDescriptorDataEXT;
	PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT vkGetImageViewOpaqueCaptureDescriptorDataEXT;
	PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT vkGetSamplerOpaqueCaptureDescriptorDataEXT;
#else
	PFN_vkVoidFunction padding_80aa973c[10];
#endif /* defined(VK_EXT_descriptor_buffer) */
#if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
	PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT;
#else
	PFN_vkVoidFunction padding_98d0fb33[1];
#endif /* defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing)) */
#if defined(VK_EXT_device_fault)
	PFN_vkGetDeviceFaultInfoEXT vkGetDeviceFaultInfoEXT;
#else
	PFN_vkVoidFunction padding_55095419[1];
#endif /* defined(VK_EXT_device_fault) */
#if defined(VK_EXT_device_generated_commands)
	PFN_vkCmdExecuteGeneratedCommandsEXT vkCmdExecuteGeneratedCommandsEXT;
	PFN_vkCmdPreprocessGeneratedCommandsEXT vkCmdPreprocessGeneratedCommandsEXT;
	PFN_vkCreateIndirectCommandsLayoutEXT vkCreateIndirectCommandsLayoutEXT;
	PFN_vkCreateIndirectExecutionSetEXT vkCreateIndirectExecutionSetEXT;
	PFN_vkDestroyIndirectCommandsLayoutEXT vkDestroyIndirectCommandsLayoutEXT;
	PFN_vkDestroyIndirectExecutionSetEXT vkDestroyIndirectExecutionSetEXT;
	PFN_vkGetGeneratedCommandsMemoryRequirementsEXT vkGetGeneratedCommandsMemoryRequirementsEXT;
	PFN_vkUpdateIndirectExecutionSetPipelineEXT vkUpdateIndirectExecutionSetPipelineEXT;
	PFN_vkUpdateIndirectExecutionSetShaderEXT vkUpdateIndirectExecutionSetShaderEXT;
#else
	PFN_vkVoidFunction padding_7ba7ebaa[9];
#endif /* defined(VK_EXT_device_generated_commands) */
#if defined(VK_EXT_discard_rectangles)
	PFN_vkCmdSetDiscardRectangleEXT vkCmdSetDiscardRectangleEXT;
#else
	PFN_vkVoidFunction padding_d6355c2[1];
#endif /* defined(VK_EXT_discard_rectangles) */
#if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
	PFN_vkCmdSetDiscardRectangleEnableEXT vkCmdSetDiscardRectangleEnableEXT;
	PFN_vkCmdSetDiscardRectangleModeEXT vkCmdSetDiscardRectangleModeEXT;
#else
	PFN_vkVoidFunction padding_7bb44f77[2];
#endif /* defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2 */
#if defined(VK_EXT_display_control)
	PFN_vkDisplayPowerControlEXT vkDisplayPowerControlEXT;
	PFN_vkGetSwapchainCounterEXT vkGetSwapchainCounterEXT;
	PFN_vkRegisterDeviceEventEXT vkRegisterDeviceEventEXT;
	PFN_vkRegisterDisplayEventEXT vkRegisterDisplayEventEXT;
#else
	PFN_vkVoidFunction padding_d30dfaaf[4];
#endif /* defined(VK_EXT_display_control) */
#if defined(VK_EXT_external_memory_host)
	PFN_vkGetMemoryHostPointerPropertiesEXT vkGetMemoryHostPointerPropertiesEXT;
#else
	PFN_vkVoidFunction padding_357656e9[1];
#endif /* defined(VK_EXT_external_memory_host) */
#if defined(VK_EXT_external_memory_metal)
	PFN_vkGetMemoryMetalHandleEXT vkGetMemoryMetalHandleEXT;
	PFN_vkGetMemoryMetalHandlePropertiesEXT vkGetMemoryMetalHandlePropertiesEXT;
#else
	PFN_vkVoidFunction padding_37d43fb[2];
#endif /* defined(VK_EXT_external_memory_metal) */
#if defined(VK_EXT_fragment_density_map_offset)
	PFN_vkCmdEndRendering2EXT vkCmdEndRendering2EXT;
#else
	PFN_vkVoidFunction padding_9c90cf11[1];
#endif /* defined(VK_EXT_fragment_density_map_offset) */
#if defined(VK_EXT_full_screen_exclusive)
	PFN_vkAcquireFullScreenExclusiveModeEXT vkAcquireFullScreenExclusiveModeEXT;
	PFN_vkReleaseFullScreenExclusiveModeEXT vkReleaseFullScreenExclusiveModeEXT;
#else
	PFN_vkVoidFunction padding_3859df46[2];
#endif /* defined(VK_EXT_full_screen_exclusive) */
#if defined(VK_EXT_full_screen_exclusive) && (defined(VK_KHR_device_group) || defined(VK_VERSION_1_1))
	PFN_vkGetDeviceGroupSurfacePresentModes2EXT vkGetDeviceGroupSurfacePresentModes2EXT;
#else
	PFN_vkVoidFunction padding_e5b48b5b[1];
#endif /* defined(VK_EXT_full_screen_exclusive) && (defined(VK_KHR_device_group) || defined(VK_VERSION_1_1)) */
#if defined(VK_EXT_hdr_metadata)
	PFN_vkSetHdrMetadataEXT vkSetHdrMetadataEXT;
#else
	PFN_vkVoidFunction padding_ca6d733c[1];
#endif /* defined(VK_EXT_hdr_metadata) */
#if defined(VK_EXT_host_image_copy)
	PFN_vkCopyImageToImageEXT vkCopyImageToImageEXT;
	PFN_vkCopyImageToMemoryEXT vkCopyImageToMemoryEXT;
	PFN_vkCopyMemoryToImageEXT vkCopyMemoryToImageEXT;
	PFN_vkTransitionImageLayoutEXT vkTransitionImageLayoutEXT;
#else
	PFN_vkVoidFunction padding_dd6d9b61[4];
#endif /* defined(VK_EXT_host_image_copy) */
#if defined(VK_EXT_host_query_reset)
	PFN_vkResetQueryPoolEXT vkResetQueryPoolEXT;
#else
	PFN_vkVoidFunction padding_34e58bd3[1];
#endif /* defined(VK_EXT_host_query_reset) */
#if defined(VK_EXT_image_drm_format_modifier)
	PFN_vkGetImageDrmFormatModifierPropertiesEXT vkGetImageDrmFormatModifierPropertiesEXT;
#else
	PFN_vkVoidFunction padding_eb50dc14[1];
#endif /* defined(VK_EXT_image_drm_format_modifier) */
#if defined(VK_EXT_line_rasterization)
	PFN_vkCmdSetLineStippleEXT vkCmdSetLineStippleEXT;
#else
	PFN_vkVoidFunction padding_8a212c37[1];
#endif /* defined(VK_EXT_line_rasterization) */
#if defined(VK_EXT_memory_decompression)
	PFN_vkCmdDecompressMemoryEXT vkCmdDecompressMemoryEXT;
	PFN_vkCmdDecompressMemoryIndirectCountEXT vkCmdDecompressMemoryIndirectCountEXT;
#else
	PFN_vkVoidFunction padding_c3b649ee[2];
#endif /* defined(VK_EXT_memory_decompression) */
#if defined(VK_EXT_mesh_shader)
	PFN_vkCmdDrawMeshTasksEXT vkCmdDrawMeshTasksEXT;
	PFN_vkCmdDrawMeshTasksIndirectEXT vkCmdDrawMeshTasksIndirectEXT;
#else
	PFN_vkVoidFunction padding_f65e838[2];
#endif /* defined(VK_EXT_mesh_shader) */
#if defined(VK_EXT_mesh_shader) && (defined(VK_VERSION_1_2) || defined(VK_KHR_draw_indirect_count) || defined(VK_AMD_draw_indirect_count))
	PFN_vkCmdDrawMeshTasksIndirectCountEXT vkCmdDrawMeshTasksIndirectCountEXT;
#else
	PFN_vkVoidFunction padding_dcbaac2f[1];
#endif /* defined(VK_EXT_mesh_shader) && (defined(VK_VERSION_1_2) || defined(VK_KHR_draw_indirect_count) || defined(VK_AMD_draw_indirect_count)) */
#if defined(VK_EXT_metal_objects)
	PFN_vkExportMetalObjectsEXT vkExportMetalObjectsEXT;
#else
	PFN_vkVoidFunction padding_df21f735[1];
#endif /* defined(VK_EXT_metal_objects) */
#if defined(VK_EXT_multi_draw)
	PFN_vkCmdDrawMultiEXT vkCmdDrawMultiEXT;
	PFN_vkCmdDrawMultiIndexedEXT vkCmdDrawMultiIndexedEXT;
#else
	PFN_vkVoidFunction padding_ce8b93b6[2];
#endif /* defined(VK_EXT_multi_draw) */
#if defined(VK_EXT_opacity_micromap)
	PFN_vkBuildMicromapsEXT vkBuildMicromapsEXT;
	PFN_vkCmdBuildMicromapsEXT vkCmdBuildMicromapsEXT;
	PFN_vkCmdCopyMemoryToMicromapEXT vkCmdCopyMemoryToMicromapEXT;
	PFN_vkCmdCopyMicromapEXT vkCmdCopyMicromapEXT;
	PFN_vkCmdCopyMicromapToMemoryEXT vkCmdCopyMicromapToMemoryEXT;
	PFN_vkCmdWriteMicromapsPropertiesEXT vkCmdWriteMicromapsPropertiesEXT;
	PFN_vkCopyMemoryToMicromapEXT vkCopyMemoryToMicromapEXT;
	PFN_vkCopyMicromapEXT vkCopyMicromapEXT;
	PFN_vkCopyMicromapToMemoryEXT vkCopyMicromapToMemoryEXT;
	PFN_vkCreateMicromapEXT vkCreateMicromapEXT;
	PFN_vkDestroyMicromapEXT vkDestroyMicromapEXT;
	PFN_vkGetDeviceMicromapCompatibilityEXT vkGetDeviceMicromapCompatibilityEXT;
	PFN_vkGetMicromapBuildSizesEXT vkGetMicromapBuildSizesEXT;
	PFN_vkWriteMicromapsPropertiesEXT vkWriteMicromapsPropertiesEXT;
#else
	PFN_vkVoidFunction padding_fa41e53c[14];
#endif /* defined(VK_EXT_opacity_micromap) */
#if defined(VK_EXT_pageable_device_local_memory)
	PFN_vkSetDeviceMemoryPriorityEXT vkSetDeviceMemoryPriorityEXT;
#else
	PFN_vkVoidFunction padding_b2d2c2d7[1];
#endif /* defined(VK_EXT_pageable_device_local_memory) */
#if defined(VK_EXT_pipeline_properties)
	PFN_vkGetPipelinePropertiesEXT vkGetPipelinePropertiesEXT;
#else
	PFN_vkVoidFunction padding_11313020[1];
#endif /* defined(VK_EXT_pipeline_properties) */
#if defined(VK_EXT_present_timing)
	PFN_vkGetPastPresentationTimingEXT vkGetPastPresentationTimingEXT;
	PFN_vkGetSwapchainTimeDomainPropertiesEXT vkGetSwapchainTimeDomainPropertiesEXT;
	PFN_vkGetSwapchainTimingPropertiesEXT vkGetSwapchainTimingPropertiesEXT;
	PFN_vkSetSwapchainPresentTimingQueueSizeEXT vkSetSwapchainPresentTimingQueueSizeEXT;
#else
	PFN_vkVoidFunction padding_8751feb5[4];
#endif /* defined(VK_EXT_present_timing) */
#if defined(VK_EXT_private_data)
	PFN_vkCreatePrivateDataSlotEXT vkCreatePrivateDataSlotEXT;
	PFN_vkDestroyPrivateDataSlotEXT vkDestroyPrivateDataSlotEXT;
	PFN_vkGetPrivateDataEXT vkGetPrivateDataEXT;
	PFN_vkSetPrivateDataEXT vkSetPrivateDataEXT;
#else
	PFN_vkVoidFunction padding_108010f[4];
#endif /* defined(VK_EXT_private_data) */
#if defined(VK_EXT_sample_locations)
	PFN_vkCmdSetSampleLocationsEXT vkCmdSetSampleLocationsEXT;
#else
	PFN_vkVoidFunction padding_26f9079f[1];
#endif /* defined(VK_EXT_sample_locations) */
#if defined(VK_EXT_shader_module_identifier)
	PFN_vkGetShaderModuleCreateInfoIdentifierEXT vkGetShaderModuleCreateInfoIdentifierEXT;
	PFN_vkGetShaderModuleIdentifierEXT vkGetShaderModuleIdentifierEXT;
#else
	PFN_vkVoidFunction padding_e10c8f86[2];
#endif /* defined(VK_EXT_shader_module_identifier) */
#if defined(VK_EXT_shader_object)
	PFN_vkCmdBindShadersEXT vkCmdBindShadersEXT;
	PFN_vkCreateShadersEXT vkCreateShadersEXT;
	PFN_vkDestroyShaderEXT vkDestroyShaderEXT;
	PFN_vkGetShaderBinaryDataEXT vkGetShaderBinaryDataEXT;
#else
	PFN_vkVoidFunction padding_374f3e18[4];
#endif /* defined(VK_EXT_shader_object) */
#if defined(VK_EXT_swapchain_maintenance1)
	PFN_vkReleaseSwapchainImagesEXT vkReleaseSwapchainImagesEXT;
#else
	PFN_vkVoidFunction padding_ea55bf74[1];
#endif /* defined(VK_EXT_swapchain_maintenance1) */
#if defined(VK_EXT_transform_feedback)
	PFN_vkCmdBeginQueryIndexedEXT vkCmdBeginQueryIndexedEXT;
	PFN_vkCmdBeginTransformFeedbackEXT vkCmdBeginTransformFeedbackEXT;
	PFN_vkCmdBindTransformFeedbackBuffersEXT vkCmdBindTransformFeedbackBuffersEXT;
	PFN_vkCmdDrawIndirectByteCountEXT vkCmdDrawIndirectByteCountEXT;
	PFN_vkCmdEndQueryIndexedEXT vkCmdEndQueryIndexedEXT;
	PFN_vkCmdEndTransformFeedbackEXT vkCmdEndTransformFeedbackEXT;
#else
	PFN_vkVoidFunction padding_36980658[6];
#endif /* defined(VK_EXT_transform_feedback) */
#if defined(VK_EXT_validation_cache)
	PFN_vkCreateValidationCacheEXT vkCreateValidationCacheEXT;
	PFN_vkDestroyValidationCacheEXT vkDestroyValidationCacheEXT;
	PFN_vkGetValidationCacheDataEXT vkGetValidationCacheDataEXT;
	PFN_vkMergeValidationCachesEXT vkMergeValidationCachesEXT;
#else
	PFN_vkVoidFunction padding_b4f2df29[4];
#endif /* defined(VK_EXT_validation_cache) */
#if defined(VK_FUCHSIA_buffer_collection)
	PFN_vkCreateBufferCollectionFUCHSIA vkCreateBufferCollectionFUCHSIA;
	PFN_vkDestroyBufferCollectionFUCHSIA vkDestroyBufferCollectionFUCHSIA;
	PFN_vkGetBufferCollectionPropertiesFUCHSIA vkGetBufferCollectionPropertiesFUCHSIA;
	PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA vkSetBufferCollectionBufferConstraintsFUCHSIA;
	PFN_vkSetBufferCollectionImageConstraintsFUCHSIA vkSetBufferCollectionImageConstraintsFUCHSIA;
#else
	PFN_vkVoidFunction padding_8eaa27bc[5];
#endif /* defined(VK_FUCHSIA_buffer_collection) */
#if defined(VK_FUCHSIA_external_memory)
	PFN_vkGetMemoryZirconHandleFUCHSIA vkGetMemoryZirconHandleFUCHSIA;
	PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA vkGetMemoryZirconHandlePropertiesFUCHSIA;
#else
	PFN_vkVoidFunction padding_e3cb8a67[2];
#endif /* defined(VK_FUCHSIA_external_memory) */
#if defined(VK_FUCHSIA_external_semaphore)
	PFN_vkGetSemaphoreZirconHandleFUCHSIA vkGetSemaphoreZirconHandleFUCHSIA;
	PFN_vkImportSemaphoreZirconHandleFUCHSIA vkImportSemaphoreZirconHandleFUCHSIA;
#else
	PFN_vkVoidFunction padding_3df6f656[2];
#endif /* defined(VK_FUCHSIA_external_semaphore) */
#if defined(VK_GOOGLE_display_timing)
	PFN_vkGetPastPresentationTimingGOOGLE vkGetPastPresentationTimingGOOGLE;
	PFN_vkGetRefreshCycleDurationGOOGLE vkGetRefreshCycleDurationGOOGLE;
#else
	PFN_vkVoidFunction padding_2a6f50cd[2];
#endif /* defined(VK_GOOGLE_display_timing) */
#if defined(VK_HUAWEI_cluster_culling_shader)
	PFN_vkCmdDrawClusterHUAWEI vkCmdDrawClusterHUAWEI;
	PFN_vkCmdDrawClusterIndirectHUAWEI vkCmdDrawClusterIndirectHUAWEI;
#else
	PFN_vkVoidFunction padding_75b97be6[2];
#endif /* defined(VK_HUAWEI_cluster_culling_shader) */
#if defined(VK_HUAWEI_invocation_mask)
	PFN_vkCmdBindInvocationMaskHUAWEI vkCmdBindInvocationMaskHUAWEI;
#else
	PFN_vkVoidFunction padding_c3a4569f[1];
#endif /* defined(VK_HUAWEI_invocation_mask) */
#if defined(VK_HUAWEI_subpass_shading) && VK_HUAWEI_SUBPASS_SHADING_SPEC_VERSION >= 2
	PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI;
#else
	PFN_vkVoidFunction padding_2e923f32[1];
#endif /* defined(VK_HUAWEI_subpass_shading) && VK_HUAWEI_SUBPASS_SHADING_SPEC_VERSION >= 2 */
#if defined(VK_HUAWEI_subpass_shading)
	PFN_vkCmdSubpassShadingHUAWEI vkCmdSubpassShadingHUAWEI;
#else
	PFN_vkVoidFunction padding_f766fdf5[1];
#endif /* defined(VK_HUAWEI_subpass_shading) */
#if defined(VK_INTEL_performance_query)
	PFN_vkAcquirePerformanceConfigurationINTEL vkAcquirePerformanceConfigurationINTEL;
	PFN_vkCmdSetPerformanceMarkerINTEL vkCmdSetPerformanceMarkerINTEL;
	PFN_vkCmdSetPerformanceOverrideINTEL vkCmdSetPerformanceOverrideINTEL;
	PFN_vkCmdSetPerformanceStreamMarkerINTEL vkCmdSetPerformanceStreamMarkerINTEL;
	PFN_vkGetPerformanceParameterINTEL vkGetPerformanceParameterINTEL;
	PFN_vkInitializePerformanceApiINTEL vkInitializePerformanceApiINTEL;
	PFN_vkQueueSetPerformanceConfigurationINTEL vkQueueSetPerformanceConfigurationINTEL;
	PFN_vkReleasePerformanceConfigurationINTEL vkReleasePerformanceConfigurationINTEL;
	PFN_vkUninitializePerformanceApiINTEL vkUninitializePerformanceApiINTEL;
#else
	PFN_vkVoidFunction padding_495a0a0b[9];
#endif /* defined(VK_INTEL_performance_query) */
#if defined(VK_KHR_acceleration_structure)
	PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
	PFN_vkCmdBuildAccelerationStructuresIndirectKHR vkCmdBuildAccelerationStructuresIndirectKHR;
	PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
	PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR;
	PFN_vkCmdCopyAccelerationStructureToMemoryKHR vkCmdCopyAccelerationStructureToMemoryKHR;
	PFN_vkCmdCopyMemoryToAccelerationStructureKHR vkCmdCopyMemoryToAccelerationStructureKHR;
	PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR;
	PFN_vkCopyAccelerationStructureKHR vkCopyAccelerationStructureKHR;
	PFN_vkCopyAccelerationStructureToMemoryKHR vkCopyAccelerationStructureToMemoryKHR;
	PFN_vkCopyMemoryToAccelerationStructureKHR vkCopyMemoryToAccelerationStructureKHR;
	PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
	PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
	PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
	PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
	PFN_vkGetDeviceAccelerationStructureCompatibilityKHR vkGetDeviceAccelerationStructureCompatibilityKHR;
	PFN_vkWriteAccelerationStructuresPropertiesKHR vkWriteAccelerationStructuresPropertiesKHR;
#else
	PFN_vkVoidFunction padding_5a999b78[16];
#endif /* defined(VK_KHR_acceleration_structure) */
#if defined(VK_KHR_bind_memory2)
	PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR;
	PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR;
#else
	PFN_vkVoidFunction padding_ed8481f5[2];
#endif /* defined(VK_KHR_bind_memory2) */
#if defined(VK_KHR_buffer_device_address)
	PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
	PFN_vkGetBufferOpaqueCaptureAddressKHR vkGetBufferOpaqueCaptureAddressKHR;
	PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR vkGetDeviceMemoryOpaqueCaptureAddressKHR;
#else
	PFN_vkVoidFunction padding_178fdf81[3];
#endif /* defined(VK_KHR_buffer_device_address) */
#if defined(VK_KHR_calibrated_timestamps)
	PFN_vkGetCalibratedTimestampsKHR vkGetCalibratedTimestampsKHR;
#else
	PFN_vkVoidFunction padding_8fd6f40d[1];
#endif /* defined(VK_KHR_calibrated_timestamps) */
#if defined(VK_KHR_copy_commands2)
	PFN_vkCmdBlitImage2KHR vkCmdBlitImage2KHR;
	PFN_vkCmdCopyBuffer2KHR vkCmdCopyBuffer2KHR;
	PFN_vkCmdCopyBufferToImage2KHR vkCmdCopyBufferToImage2KHR;
	PFN_vkCmdCopyImage2KHR vkCmdCopyImage2KHR;
	PFN_vkCmdCopyImageToBuffer2KHR vkCmdCopyImageToBuffer2KHR;
	PFN_vkCmdResolveImage2KHR vkCmdResolveImage2KHR;
#else
	PFN_vkVoidFunction padding_4c841ff2[6];
#endif /* defined(VK_KHR_copy_commands2) */
#if defined(VK_KHR_copy_memory_indirect)
	PFN_vkCmdCopyMemoryIndirectKHR vkCmdCopyMemoryIndirectKHR;
	PFN_vkCmdCopyMemoryToImageIndirectKHR vkCmdCopyMemoryToImageIndirectKHR;
#else
	PFN_vkVoidFunction padding_95995957[2];
#endif /* defined(VK_KHR_copy_memory_indirect) */
#if defined(VK_KHR_create_renderpass2)
	PFN_vkCmdBeginRenderPass2KHR vkCmdBeginRenderPass2KHR;
	PFN_vkCmdEndRenderPass2KHR vkCmdEndRenderPass2KHR;
	PFN_vkCmdNextSubpass2KHR vkCmdNextSubpass2KHR;
	PFN_vkCreateRenderPass2KHR vkCreateRenderPass2KHR;
#else
	PFN_vkVoidFunction padding_2a0a8727[4];
#endif /* defined(VK_KHR_create_renderpass2) */
#if defined(VK_KHR_deferred_host_operations)
	PFN_vkCreateDeferredOperationKHR vkCreateDeferredOperationKHR;
	PFN_vkDeferredOperationJoinKHR vkDeferredOperationJoinKHR;
	PFN_vkDestroyDeferredOperationKHR vkDestroyDeferredOperationKHR;
	PFN_vkGetDeferredOperationMaxConcurrencyKHR vkGetDeferredOperationMaxConcurrencyKHR;
	PFN_vkGetDeferredOperationResultKHR vkGetDeferredOperationResultKHR;
#else
	PFN_vkVoidFunction padding_346287bb[5];
#endif /* defined(VK_KHR_deferred_host_operations) */
#if defined(VK_KHR_descriptor_update_template)
	PFN_vkCreateDescriptorUpdateTemplateKHR vkCreateDescriptorUpdateTemplateKHR;
	PFN_vkDestroyDescriptorUpdateTemplateKHR vkDestroyDescriptorUpdateTemplateKHR;
	PFN_vkUpdateDescriptorSetWithTemplateKHR vkUpdateDescriptorSetWithTemplateKHR;
#else
	PFN_vkVoidFunction padding_3d63aec0[3];
#endif /* defined(VK_KHR_descriptor_update_template) */
#if defined(VK_KHR_device_group)
	PFN_vkCmdDispatchBaseKHR vkCmdDispatchBaseKHR;
	PFN_vkCmdSetDeviceMaskKHR vkCmdSetDeviceMaskKHR;
	PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR vkGetDeviceGroupPeerMemoryFeaturesKHR;
#else
	PFN_vkVoidFunction padding_5ebe16bd[3];
#endif /* defined(VK_KHR_device_group) */
#if defined(VK_KHR_display_swapchain)
	PFN_vkCreateSharedSwapchainsKHR vkCreateSharedSwapchainsKHR;
#else
	PFN_vkVoidFunction padding_12099367[1];
#endif /* defined(VK_KHR_display_swapchain) */
#if defined(VK_KHR_draw_indirect_count)
	PFN_vkCmdDrawIndexedIndirectCountKHR vkCmdDrawIndexedIndirectCountKHR;
	PFN_vkCmdDrawIndirectCountKHR vkCmdDrawIndirectCountKHR;
#else
	PFN_vkVoidFunction padding_7b5bc4c1[2];
#endif /* defined(VK_KHR_draw_indirect_count) */
#if defined(VK_KHR_dynamic_rendering)
	PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR;
	PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR;
#else
	PFN_vkVoidFunction padding_b80f75a5[2];
#endif /* defined(VK_KHR_dynamic_rendering) */
#if defined(VK_KHR_dynamic_rendering_local_read)
	PFN_vkCmdSetRenderingAttachmentLocationsKHR vkCmdSetRenderingAttachmentLocationsKHR;
	PFN_vkCmdSetRenderingInputAttachmentIndicesKHR vkCmdSetRenderingInputAttachmentIndicesKHR;
#else
	PFN_vkVoidFunction padding_b1510532[2];
#endif /* defined(VK_KHR_dynamic_rendering_local_read) */
#if defined(VK_KHR_external_fence_fd)
	PFN_vkGetFenceFdKHR vkGetFenceFdKHR;
	PFN_vkImportFenceFdKHR vkImportFenceFdKHR;
#else
	PFN_vkVoidFunction padding_a2c787d5[2];
#endif /* defined(VK_KHR_external_fence_fd) */
#if defined(VK_KHR_external_fence_win32)
	PFN_vkGetFenceWin32HandleKHR vkGetFenceWin32HandleKHR;
	PFN_vkImportFenceWin32HandleKHR vkImportFenceWin32HandleKHR;
#else
	PFN_vkVoidFunction padding_55d8e6a9[2];
#endif /* defined(VK_KHR_external_fence_win32) */
#if defined(VK_KHR_external_memory_fd)
	PFN_vkGetMemoryFdKHR vkGetMemoryFdKHR;
	PFN_vkGetMemoryFdPropertiesKHR vkGetMemoryFdPropertiesKHR;
#else
	PFN_vkVoidFunction padding_982d9e19[2];
#endif /* defined(VK_KHR_external_memory_fd) */
#if defined(VK_KHR_external_memory_win32)
	PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR;
	PFN_vkGetMemoryWin32HandlePropertiesKHR vkGetMemoryWin32HandlePropertiesKHR;
#else
	PFN_vkVoidFunction padding_4af9e25a[2];
#endif /* defined(VK_KHR_external_memory_win32) */
#if defined(VK_KHR_external_semaphore_fd)
	PFN_vkGetSemaphoreFdKHR vkGetSemaphoreFdKHR;
	PFN_vkImportSemaphoreFdKHR vkImportSemaphoreFdKHR;
#else
	PFN_vkVoidFunction padding_2237b7cf[2];
#endif /* defined(VK_KHR_external_semaphore_fd) */
#if defined(VK_KHR_external_semaphore_win32)
	PFN_vkGetSemaphoreWin32HandleKHR vkGetSemaphoreWin32HandleKHR;
	PFN_vkImportSemaphoreWin32HandleKHR vkImportSemaphoreWin32HandleKHR;
#else
	PFN_vkVoidFunction padding_c18dea52[2];
#endif /* defined(VK_KHR_external_semaphore_win32) */
#if defined(VK_KHR_fragment_shading_rate)
	PFN_vkCmdSetFragmentShadingRateKHR vkCmdSetFragmentShadingRateKHR;
#else
	PFN_vkVoidFunction padding_f91b0a90[1];
#endif /* defined(VK_KHR_fragment_shading_rate) */
#if defined(VK_KHR_get_memory_requirements2)
	PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR;
	PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR;
	PFN_vkGetImageSparseMemoryRequirements2KHR vkGetImageSparseMemoryRequirements2KHR;
#else
	PFN_vkVoidFunction padding_79d9c5c4[3];
#endif /* defined(VK_KHR_get_memory_requirements2) */
#if defined(VK_KHR_line_rasterization)
	PFN_vkCmdSetLineStippleKHR vkCmdSetLineStippleKHR;
#else
	PFN_vkVoidFunction padding_83c2939[1];
#endif /* defined(VK_KHR_line_rasterization) */
#if defined(VK_KHR_maintenance1)
	PFN_vkTrimCommandPoolKHR vkTrimCommandPoolKHR;
#else
	PFN_vkVoidFunction padding_4b372c56[1];
#endif /* defined(VK_KHR_maintenance1) */
#if defined(VK_KHR_maintenance10)
	PFN_vkCmdEndRendering2KHR vkCmdEndRendering2KHR;
#else
	PFN_vkVoidFunction padding_c866e6ce[1];
#endif /* defined(VK_KHR_maintenance10) */
#if defined(VK_KHR_maintenance3)
	PFN_vkGetDescriptorSetLayoutSupportKHR vkGetDescriptorSetLayoutSupportKHR;
#else
	PFN_vkVoidFunction padding_5ea7858d[1];
#endif /* defined(VK_KHR_maintenance3) */
#if defined(VK_KHR_maintenance4)
	PFN_vkGetDeviceBufferMemoryRequirementsKHR vkGetDeviceBufferMemoryRequirementsKHR;
	PFN_vkGetDeviceImageMemoryRequirementsKHR vkGetDeviceImageMemoryRequirementsKHR;
	PFN_vkGetDeviceImageSparseMemoryRequirementsKHR vkGetDeviceImageSparseMemoryRequirementsKHR;
#else
	PFN_vkVoidFunction padding_8e2d4198[3];
#endif /* defined(VK_KHR_maintenance4) */
#if defined(VK_KHR_maintenance5)
	PFN_vkCmdBindIndexBuffer2KHR vkCmdBindIndexBuffer2KHR;
	PFN_vkGetDeviceImageSubresourceLayoutKHR vkGetDeviceImageSubresourceLayoutKHR;
	PFN_vkGetImageSubresourceLayout2KHR vkGetImageSubresourceLayout2KHR;
	PFN_vkGetRenderingAreaGranularityKHR vkGetRenderingAreaGranularityKHR;
#else
	PFN_vkVoidFunction padding_37040339[4];
#endif /* defined(VK_KHR_maintenance5) */
#if defined(VK_KHR_maintenance6)
	PFN_vkCmdBindDescriptorSets2KHR vkCmdBindDescriptorSets2KHR;
	PFN_vkCmdPushConstants2KHR vkCmdPushConstants2KHR;
#else
	PFN_vkVoidFunction padding_442955d8[2];
#endif /* defined(VK_KHR_maintenance6) */
#if defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor)
	PFN_vkCmdPushDescriptorSet2KHR vkCmdPushDescriptorSet2KHR;
	PFN_vkCmdPushDescriptorSetWithTemplate2KHR vkCmdPushDescriptorSetWithTemplate2KHR;
#else
	PFN_vkVoidFunction padding_80e8513f[2];
#endif /* defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer)
	PFN_vkCmdBindDescriptorBufferEmbeddedSamplers2EXT vkCmdBindDescriptorBufferEmbeddedSamplers2EXT;
	PFN_vkCmdSetDescriptorBufferOffsets2EXT vkCmdSetDescriptorBufferOffsets2EXT;
#else
	PFN_vkVoidFunction padding_2816b9cd[2];
#endif /* defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer) */
#if defined(VK_KHR_map_memory2)
	PFN_vkMapMemory2KHR vkMapMemory2KHR;
	PFN_vkUnmapMemory2KHR vkUnmapMemory2KHR;
#else
	PFN_vkVoidFunction padding_5a6d8986[2];
#endif /* defined(VK_KHR_map_memory2) */
#if defined(VK_KHR_performance_query)
	PFN_vkAcquireProfilingLockKHR vkAcquireProfilingLockKHR;
	PFN_vkReleaseProfilingLockKHR vkReleaseProfilingLockKHR;
#else
	PFN_vkVoidFunction padding_76f2673b[2];
#endif /* defined(VK_KHR_performance_query) */
#if defined(VK_KHR_pipeline_binary)
	PFN_vkCreatePipelineBinariesKHR vkCreatePipelineBinariesKHR;
	PFN_vkDestroyPipelineBinaryKHR vkDestroyPipelineBinaryKHR;
	PFN_vkGetPipelineBinaryDataKHR vkGetPipelineBinaryDataKHR;
	PFN_vkGetPipelineKeyKHR vkGetPipelineKeyKHR;
	PFN_vkReleaseCapturedPipelineDataKHR vkReleaseCapturedPipelineDataKHR;
#else
	PFN_vkVoidFunction padding_65232810[5];
#endif /* defined(VK_KHR_pipeline_binary) */
#if defined(VK_KHR_pipeline_executable_properties)
	PFN_vkGetPipelineExecutableInternalRepresentationsKHR vkGetPipelineExecutableInternalRepresentationsKHR;
	PFN_vkGetPipelineExecutablePropertiesKHR vkGetPipelineExecutablePropertiesKHR;
	PFN_vkGetPipelineExecutableStatisticsKHR vkGetPipelineExecutableStatisticsKHR;
#else
	PFN_vkVoidFunction padding_f7629b1e[3];
#endif /* defined(VK_KHR_pipeline_executable_properties) */
#if defined(VK_KHR_present_wait)
	PFN_vkWaitForPresentKHR vkWaitForPresentKHR;
#else
	PFN_vkVoidFunction padding_b16cbe03[1];
#endif /* defined(VK_KHR_present_wait) */
#if defined(VK_KHR_present_wait2)
	PFN_vkWaitForPresent2KHR vkWaitForPresent2KHR;
#else
	PFN_vkVoidFunction padding_7401483a[1];
#endif /* defined(VK_KHR_present_wait2) */
#if defined(VK_KHR_push_descriptor)
	PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR;
#else
	PFN_vkVoidFunction padding_8f7712ad[1];
#endif /* defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
	PFN_vkCmdTraceRaysIndirect2KHR vkCmdTraceRaysIndirect2KHR;
#else
	PFN_vkVoidFunction padding_dd5f9b4a[1];
#endif /* defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_ray_tracing_pipeline)
	PFN_vkCmdSetRayTracingPipelineStackSizeKHR vkCmdSetRayTracingPipelineStackSizeKHR;
	PFN_vkCmdTraceRaysIndirectKHR vkCmdTraceRaysIndirectKHR;
	PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
	PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;
	PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR vkGetRayTracingCaptureReplayShaderGroupHandlesKHR;
	PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
	PFN_vkGetRayTracingShaderGroupStackSizeKHR vkGetRayTracingShaderGroupStackSizeKHR;
#else
	PFN_vkVoidFunction padding_af99aedc[7];
#endif /* defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_sampler_ycbcr_conversion)
	PFN_vkCreateSamplerYcbcrConversionKHR vkCreateSamplerYcbcrConversionKHR;
	PFN_vkDestroySamplerYcbcrConversionKHR vkDestroySamplerYcbcrConversionKHR;
#else
	PFN_vkVoidFunction padding_88e61b30[2];
#endif /* defined(VK_KHR_sampler_ycbcr_conversion) */
#if defined(VK_KHR_shared_presentable_image)
	PFN_vkGetSwapchainStatusKHR vkGetSwapchainStatusKHR;
#else
	PFN_vkVoidFunction padding_1ff3379[1];
#endif /* defined(VK_KHR_shared_presentable_image) */
#if defined(VK_KHR_swapchain)
	PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
	PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
	PFN_vkQueuePresentKHR vkQueuePresentKHR;
#else
	PFN_vkVoidFunction padding_a1de893b[5];
#endif /* defined(VK_KHR_swapchain) */
#if defined(VK_KHR_swapchain_maintenance1)
	PFN_vkReleaseSwapchainImagesKHR vkReleaseSwapchainImagesKHR;
#else
	PFN_vkVoidFunction padding_e032d5c4[1];
#endif /* defined(VK_KHR_swapchain_maintenance1) */
#if defined(VK_KHR_synchronization2)
	PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR;
	PFN_vkCmdResetEvent2KHR vkCmdResetEvent2KHR;
	PFN_vkCmdSetEvent2KHR vkCmdSetEvent2KHR;
	PFN_vkCmdWaitEvents2KHR vkCmdWaitEvents2KHR;
	PFN_vkCmdWriteTimestamp2KHR vkCmdWriteTimestamp2KHR;
	PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR;
#else
	PFN_vkVoidFunction padding_e85bf128[6];
#endif /* defined(VK_KHR_synchronization2) */
#if defined(VK_KHR_timeline_semaphore)
	PFN_vkGetSemaphoreCounterValueKHR vkGetSemaphoreCounterValueKHR;
	PFN_vkSignalSemaphoreKHR vkSignalSemaphoreKHR;
	PFN_vkWaitSemaphoresKHR vkWaitSemaphoresKHR;
#else
	PFN_vkVoidFunction padding_c799d931[3];
#endif /* defined(VK_KHR_timeline_semaphore) */
#if defined(VK_KHR_video_decode_queue)
	PFN_vkCmdDecodeVideoKHR vkCmdDecodeVideoKHR;
#else
	PFN_vkVoidFunction padding_7a7cc7ad[1];
#endif /* defined(VK_KHR_video_decode_queue) */
#if defined(VK_KHR_video_encode_queue)
	PFN_vkCmdEncodeVideoKHR vkCmdEncodeVideoKHR;
	PFN_vkGetEncodedVideoSessionParametersKHR vkGetEncodedVideoSessionParametersKHR;
#else
	PFN_vkVoidFunction padding_f2997fb4[2];
#endif /* defined(VK_KHR_video_encode_queue) */
#if defined(VK_KHR_video_queue)
	PFN_vkBindVideoSessionMemoryKHR vkBindVideoSessionMemoryKHR;
	PFN_vkCmdBeginVideoCodingKHR vkCmdBeginVideoCodingKHR;
	PFN_vkCmdControlVideoCodingKHR vkCmdControlVideoCodingKHR;
	PFN_vkCmdEndVideoCodingKHR vkCmdEndVideoCodingKHR;
	PFN_vkCreateVideoSessionKHR vkCreateVideoSessionKHR;
	PFN_vkCreateVideoSessionParametersKHR vkCreateVideoSessionParametersKHR;
	PFN_vkDestroyVideoSessionKHR vkDestroyVideoSessionKHR;
	PFN_vkDestroyVideoSessionParametersKHR vkDestroyVideoSessionParametersKHR;
	PFN_vkGetVideoSessionMemoryRequirementsKHR vkGetVideoSessionMemoryRequirementsKHR;
	PFN_vkUpdateVideoSessionParametersKHR vkUpdateVideoSessionParametersKHR;
#else
	PFN_vkVoidFunction padding_98fb7016[10];
#endif /* defined(VK_KHR_video_queue) */
#if defined(VK_NVX_binary_import)
	PFN_vkCmdCuLaunchKernelNVX vkCmdCuLaunchKernelNVX;
	PFN_vkCreateCuFunctionNVX vkCreateCuFunctionNVX;
	PFN_vkCreateCuModuleNVX vkCreateCuModuleNVX;
	PFN_vkDestroyCuFunctionNVX vkDestroyCuFunctionNVX;
	PFN_vkDestroyCuModuleNVX vkDestroyCuModuleNVX;
#else
	PFN_vkVoidFunction padding_eb54309b[5];
#endif /* defined(VK_NVX_binary_import) */
#if defined(VK_NVX_image_view_handle)
	PFN_vkGetImageViewHandleNVX vkGetImageViewHandleNVX;
#else
	PFN_vkVoidFunction padding_887f6736[1];
#endif /* defined(VK_NVX_image_view_handle) */
#if defined(VK_NVX_image_view_handle) && VK_NVX_IMAGE_VIEW_HANDLE_SPEC_VERSION >= 3
	PFN_vkGetImageViewHandle64NVX vkGetImageViewHandle64NVX;
#else
	PFN_vkVoidFunction padding_64ad40e2[1];
#endif /* defined(VK_NVX_image_view_handle) && VK_NVX_IMAGE_VIEW_HANDLE_SPEC_VERSION >= 3 */
#if defined(VK_NVX_image_view_handle) && VK_NVX_IMAGE_VIEW_HANDLE_SPEC_VERSION >= 2
	PFN_vkGetImageViewAddressNVX vkGetImageViewAddressNVX;
#else
	PFN_vkVoidFunction padding_d290479a[1];
#endif /* defined(VK_NVX_image_view_handle) && VK_NVX_IMAGE_VIEW_HANDLE_SPEC_VERSION >= 2 */
#if defined(VK_NV_clip_space_w_scaling)
	PFN_vkCmdSetViewportWScalingNV vkCmdSetViewportWScalingNV;
#else
	PFN_vkVoidFunction padding_88d7eb2e[1];
#endif /* defined(VK_NV_clip_space_w_scaling) */
#if defined(VK_NV_cluster_acceleration_structure)
	PFN_vkCmdBuildClusterAccelerationStructureIndirectNV vkCmdBuildClusterAccelerationStructureIndirectNV;
	PFN_vkGetClusterAccelerationStructureBuildSizesNV vkGetClusterAccelerationStructureBuildSizesNV;
#else
	PFN_vkVoidFunction padding_60e35395[2];
#endif /* defined(VK_NV_cluster_acceleration_structure) */
#if defined(VK_NV_compute_occupancy_priority)
	PFN_vkCmdSetComputeOccupancyPriorityNV vkCmdSetComputeOccupancyPriorityNV;
#else
	PFN_vkVoidFunction padding_488584ea[1];
#endif /* defined(VK_NV_compute_occupancy_priority) */
#if defined(VK_NV_cooperative_vector)
	PFN_vkCmdConvertCooperativeVectorMatrixNV vkCmdConvertCooperativeVectorMatrixNV;
	PFN_vkConvertCooperativeVectorMatrixNV vkConvertCooperativeVectorMatrixNV;
#else
	PFN_vkVoidFunction padding_f4a887d0[2];
#endif /* defined(VK_NV_cooperative_vector) */
#if defined(VK_NV_copy_memory_indirect)
	PFN_vkCmdCopyMemoryIndirectNV vkCmdCopyMemoryIndirectNV;
	PFN_vkCmdCopyMemoryToImageIndirectNV vkCmdCopyMemoryToImageIndirectNV;
#else
	PFN_vkVoidFunction padding_9536230e[2];
#endif /* defined(VK_NV_copy_memory_indirect) */
#if defined(VK_NV_cuda_kernel_launch)
	PFN_vkCmdCudaLaunchKernelNV vkCmdCudaLaunchKernelNV;
	PFN_vkCreateCudaFunctionNV vkCreateCudaFunctionNV;
	PFN_vkCreateCudaModuleNV vkCreateCudaModuleNV;
	PFN_vkDestroyCudaFunctionNV vkDestroyCudaFunctionNV;
	PFN_vkDestroyCudaModuleNV vkDestroyCudaModuleNV;
	PFN_vkGetCudaModuleCacheNV vkGetCudaModuleCacheNV;
#else
	PFN_vkVoidFunction padding_2eabdf3b[6];
#endif /* defined(VK_NV_cuda_kernel_launch) */
#if defined(VK_NV_device_diagnostic_checkpoints)
	PFN_vkCmdSetCheckpointNV vkCmdSetCheckpointNV;
	PFN_vkGetQueueCheckpointDataNV vkGetQueueCheckpointDataNV;
#else
	PFN_vkVoidFunction padding_adaa5a21[2];
#endif /* defined(VK_NV_device_diagnostic_checkpoints) */
#if defined(VK_NV_device_diagnostic_checkpoints) && (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
	PFN_vkGetQueueCheckpointData2NV vkGetQueueCheckpointData2NV;
#else
	PFN_vkVoidFunction padding_c776633d[1];
#endif /* defined(VK_NV_device_diagnostic_checkpoints) && (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2)) */
#if defined(VK_NV_device_generated_commands)
	PFN_vkCmdBindPipelineShaderGroupNV vkCmdBindPipelineShaderGroupNV;
	PFN_vkCmdExecuteGeneratedCommandsNV vkCmdExecuteGeneratedCommandsNV;
	PFN_vkCmdPreprocessGeneratedCommandsNV vkCmdPreprocessGeneratedCommandsNV;
	PFN_vkCreateIndirectCommandsLayoutNV vkCreateIndirectCommandsLayoutNV;
	PFN_vkDestroyIndirectCommandsLayoutNV vkDestroyIndirectCommandsLayoutNV;
	PFN_vkGetGeneratedCommandsMemoryRequirementsNV vkGetGeneratedCommandsMemoryRequirementsNV;
#else
	PFN_vkVoidFunction padding_4c7e4395[6];
#endif /* defined(VK_NV_device_generated_commands) */
#if defined(VK_NV_device_generated_commands_compute)
	PFN_vkCmdUpdatePipelineIndirectBufferNV vkCmdUpdatePipelineIndirectBufferNV;
	PFN_vkGetPipelineIndirectDeviceAddressNV vkGetPipelineIndirectDeviceAddressNV;
	PFN_vkGetPipelineIndirectMemoryRequirementsNV vkGetPipelineIndirectMemoryRequirementsNV;
#else
	PFN_vkVoidFunction padding_5195094c[3];
#endif /* defined(VK_NV_device_generated_commands_compute) */
#if defined(VK_NV_external_compute_queue)
	PFN_vkCreateExternalComputeQueueNV vkCreateExternalComputeQueueNV;
	PFN_vkDestroyExternalComputeQueueNV vkDestroyExternalComputeQueueNV;
	PFN_vkGetExternalComputeQueueDataNV vkGetExternalComputeQueueDataNV;
#else
	PFN_vkVoidFunction padding_4f947e0b[3];
#endif /* defined(VK_NV_external_compute_queue) */
#if defined(VK_NV_external_memory_rdma)
	PFN_vkGetMemoryRemoteAddressNV vkGetMemoryRemoteAddressNV;
#else
	PFN_vkVoidFunction padding_920e405[1];
#endif /* defined(VK_NV_external_memory_rdma) */
#if defined(VK_NV_external_memory_win32)
	PFN_vkGetMemoryWin32HandleNV vkGetMemoryWin32HandleNV;
#else
	PFN_vkVoidFunction padding_c13d6f3a[1];
#endif /* defined(VK_NV_external_memory_win32) */
#if defined(VK_NV_fragment_shading_rate_enums)
	PFN_vkCmdSetFragmentShadingRateEnumNV vkCmdSetFragmentShadingRateEnumNV;
#else
	PFN_vkVoidFunction padding_4979ca14[1];
#endif /* defined(VK_NV_fragment_shading_rate_enums) */
#if defined(VK_NV_low_latency2)
	PFN_vkGetLatencyTimingsNV vkGetLatencyTimingsNV;
	PFN_vkLatencySleepNV vkLatencySleepNV;
	PFN_vkQueueNotifyOutOfBandNV vkQueueNotifyOutOfBandNV;
	PFN_vkSetLatencyMarkerNV vkSetLatencyMarkerNV;
	PFN_vkSetLatencySleepModeNV vkSetLatencySleepModeNV;
#else
	PFN_vkVoidFunction padding_fabf8b19[5];
#endif /* defined(VK_NV_low_latency2) */
#if defined(VK_NV_memory_decompression)
	PFN_vkCmdDecompressMemoryIndirectCountNV vkCmdDecompressMemoryIndirectCountNV;
	PFN_vkCmdDecompressMemoryNV vkCmdDecompressMemoryNV;
#else
	PFN_vkVoidFunction padding_706009[2];
#endif /* defined(VK_NV_memory_decompression) */
#if defined(VK_NV_mesh_shader)
	PFN_vkCmdDrawMeshTasksIndirectNV vkCmdDrawMeshTasksIndirectNV;
	PFN_vkCmdDrawMeshTasksNV vkCmdDrawMeshTasksNV;
#else
	PFN_vkVoidFunction padding_ac232758[2];
#endif /* defined(VK_NV_mesh_shader) */
#if defined(VK_NV_mesh_shader) && (defined(VK_VERSION_1_2) || defined(VK_KHR_draw_indirect_count) || defined(VK_AMD_draw_indirect_count))
	PFN_vkCmdDrawMeshTasksIndirectCountNV vkCmdDrawMeshTasksIndirectCountNV;
#else
	PFN_vkVoidFunction padding_53495be7[1];
#endif /* defined(VK_NV_mesh_shader) && (defined(VK_VERSION_1_2) || defined(VK_KHR_draw_indirect_count) || defined(VK_AMD_draw_indirect_count)) */
#if defined(VK_NV_optical_flow)
	PFN_vkBindOpticalFlowSessionImageNV vkBindOpticalFlowSessionImageNV;
	PFN_vkCmdOpticalFlowExecuteNV vkCmdOpticalFlowExecuteNV;
	PFN_vkCreateOpticalFlowSessionNV vkCreateOpticalFlowSessionNV;
	PFN_vkDestroyOpticalFlowSessionNV vkDestroyOpticalFlowSessionNV;
#else
	PFN_vkVoidFunction padding_f67571eb[4];
#endif /* defined(VK_NV_optical_flow) */
#if defined(VK_NV_partitioned_acceleration_structure)
	PFN_vkCmdBuildPartitionedAccelerationStructuresNV vkCmdBuildPartitionedAccelerationStructuresNV;
	PFN_vkGetPartitionedAccelerationStructuresBuildSizesNV vkGetPartitionedAccelerationStructuresBuildSizesNV;
#else
	PFN_vkVoidFunction padding_d27c8c6d[2];
#endif /* defined(VK_NV_partitioned_acceleration_structure) */
#if defined(VK_NV_ray_tracing)
	PFN_vkBindAccelerationStructureMemoryNV vkBindAccelerationStructureMemoryNV;
	PFN_vkCmdBuildAccelerationStructureNV vkCmdBuildAccelerationStructureNV;
	PFN_vkCmdCopyAccelerationStructureNV vkCmdCopyAccelerationStructureNV;
	PFN_vkCmdTraceRaysNV vkCmdTraceRaysNV;
	PFN_vkCmdWriteAccelerationStructuresPropertiesNV vkCmdWriteAccelerationStructuresPropertiesNV;
	PFN_vkCompileDeferredNV vkCompileDeferredNV;
	PFN_vkCreateAccelerationStructureNV vkCreateAccelerationStructureNV;
	PFN_vkCreateRayTracingPipelinesNV vkCreateRayTracingPipelinesNV;
	PFN_vkDestroyAccelerationStructureNV vkDestroyAccelerationStructureNV;
	PFN_vkGetAccelerationStructureHandleNV vkGetAccelerationStructureHandleNV;
	PFN_vkGetAccelerationStructureMemoryRequirementsNV vkGetAccelerationStructureMemoryRequirementsNV;
	PFN_vkGetRayTracingShaderGroupHandlesNV vkGetRayTracingShaderGroupHandlesNV;
#else
	PFN_vkVoidFunction padding_feefbeac[12];
#endif /* defined(VK_NV_ray_tracing) */
#if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
	PFN_vkCmdSetExclusiveScissorEnableNV vkCmdSetExclusiveScissorEnableNV;
#else
	PFN_vkVoidFunction padding_e3c24f80[1];
#endif /* defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2 */
#if defined(VK_NV_scissor_exclusive)
	PFN_vkCmdSetExclusiveScissorNV vkCmdSetExclusiveScissorNV;
#else
	PFN_vkVoidFunction padding_8e88d86c[1];
#endif /* defined(VK_NV_scissor_exclusive) */
#if defined(VK_NV_shading_rate_image)
	PFN_vkCmdBindShadingRateImageNV vkCmdBindShadingRateImageNV;
	PFN_vkCmdSetCoarseSampleOrderNV vkCmdSetCoarseSampleOrderNV;
	PFN_vkCmdSetViewportShadingRatePaletteNV vkCmdSetViewportShadingRatePaletteNV;
#else
	PFN_vkVoidFunction padding_92a0767f[3];
#endif /* defined(VK_NV_shading_rate_image) */
#if defined(VK_OHOS_external_memory)
	PFN_vkGetMemoryNativeBufferOHOS vkGetMemoryNativeBufferOHOS;
	PFN_vkGetNativeBufferPropertiesOHOS vkGetNativeBufferPropertiesOHOS;
#else
	PFN_vkVoidFunction padding_9c703846[2];
#endif /* defined(VK_OHOS_external_memory) */
#if defined(VK_OHOS_native_buffer)
	PFN_vkAcquireImageOHOS vkAcquireImageOHOS;
	PFN_vkGetSwapchainGrallocUsageOHOS vkGetSwapchainGrallocUsageOHOS;
	PFN_vkQueueSignalReleaseImageOHOS vkQueueSignalReleaseImageOHOS;
#else
	PFN_vkVoidFunction padding_a26c9a3d[3];
#endif /* defined(VK_OHOS_native_buffer) */
#if defined(VK_QCOM_tile_memory_heap)
	PFN_vkCmdBindTileMemoryQCOM vkCmdBindTileMemoryQCOM;
#else
	PFN_vkVoidFunction padding_e2d55d04[1];
#endif /* defined(VK_QCOM_tile_memory_heap) */
#if defined(VK_QCOM_tile_properties)
	PFN_vkGetDynamicRenderingTilePropertiesQCOM vkGetDynamicRenderingTilePropertiesQCOM;
	PFN_vkGetFramebufferTilePropertiesQCOM vkGetFramebufferTilePropertiesQCOM;
#else
	PFN_vkVoidFunction padding_be12e32[2];
#endif /* defined(VK_QCOM_tile_properties) */
#if defined(VK_QCOM_tile_shading)
	PFN_vkCmdBeginPerTileExecutionQCOM vkCmdBeginPerTileExecutionQCOM;
	PFN_vkCmdDispatchTileQCOM vkCmdDispatchTileQCOM;
	PFN_vkCmdEndPerTileExecutionQCOM vkCmdEndPerTileExecutionQCOM;
#else
	PFN_vkVoidFunction padding_fcd9e1df[3];
#endif /* defined(VK_QCOM_tile_shading) */
#if defined(VK_QNX_external_memory_screen_buffer)
	PFN_vkGetScreenBufferPropertiesQNX vkGetScreenBufferPropertiesQNX;
#else
	PFN_vkVoidFunction padding_1c27735d[1];
#endif /* defined(VK_QNX_external_memory_screen_buffer) */
#if defined(VK_VALVE_descriptor_set_host_mapping)
	PFN_vkGetDescriptorSetHostMappingVALVE vkGetDescriptorSetHostMappingVALVE;
	PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE vkGetDescriptorSetLayoutHostMappingInfoVALVE;
#else
	PFN_vkVoidFunction padding_fd71e4c6[2];
#endif /* defined(VK_VALVE_descriptor_set_host_mapping) */
#if (defined(VK_EXT_depth_clamp_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clamp_control))
	PFN_vkCmdSetDepthClampRangeEXT vkCmdSetDepthClampRangeEXT;
#else
	PFN_vkVoidFunction padding_faa18a61[1];
#endif /* (defined(VK_EXT_depth_clamp_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clamp_control)) */
#if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
	PFN_vkCmdBindVertexBuffers2EXT vkCmdBindVertexBuffers2EXT;
	PFN_vkCmdSetCullModeEXT vkCmdSetCullModeEXT;
	PFN_vkCmdSetDepthBoundsTestEnableEXT vkCmdSetDepthBoundsTestEnableEXT;
	PFN_vkCmdSetDepthCompareOpEXT vkCmdSetDepthCompareOpEXT;
	PFN_vkCmdSetDepthTestEnableEXT vkCmdSetDepthTestEnableEXT;
	PFN_vkCmdSetDepthWriteEnableEXT vkCmdSetDepthWriteEnableEXT;
	PFN_vkCmdSetFrontFaceEXT vkCmdSetFrontFaceEXT;
	PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT;
	PFN_vkCmdSetScissorWithCountEXT vkCmdSetScissorWithCountEXT;
	PFN_vkCmdSetStencilOpEXT vkCmdSetStencilOpEXT;
	PFN_vkCmdSetStencilTestEnableEXT vkCmdSetStencilTestEnableEXT;
	PFN_vkCmdSetViewportWithCountEXT vkCmdSetViewportWithCountEXT;
#else
	PFN_vkVoidFunction padding_3e8c720f[12];
#endif /* (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
	PFN_vkCmdSetDepthBiasEnableEXT vkCmdSetDepthBiasEnableEXT;
	PFN_vkCmdSetLogicOpEXT vkCmdSetLogicOpEXT;
	PFN_vkCmdSetPatchControlPointsEXT vkCmdSetPatchControlPointsEXT;
	PFN_vkCmdSetPrimitiveRestartEnableEXT vkCmdSetPrimitiveRestartEnableEXT;
	PFN_vkCmdSetRasterizerDiscardEnableEXT vkCmdSetRasterizerDiscardEnableEXT;
#else
	PFN_vkVoidFunction padding_b93e02a6[5];
#endif /* (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
	PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnableEXT;
	PFN_vkCmdSetAlphaToOneEnableEXT vkCmdSetAlphaToOneEnableEXT;
	PFN_vkCmdSetColorBlendEnableEXT vkCmdSetColorBlendEnableEXT;
	PFN_vkCmdSetColorBlendEquationEXT vkCmdSetColorBlendEquationEXT;
	PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMaskEXT;
	PFN_vkCmdSetDepthClampEnableEXT vkCmdSetDepthClampEnableEXT;
	PFN_vkCmdSetLogicOpEnableEXT vkCmdSetLogicOpEnableEXT;
	PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT;
	PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT;
	PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMaskEXT;
#else
	PFN_vkVoidFunction padding_ab566e7e[10];
#endif /* (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object))
	PFN_vkCmdSetTessellationDomainOriginEXT vkCmdSetTessellationDomainOriginEXT;
#else
	PFN_vkVoidFunction padding_6730ed0c[1];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback))
	PFN_vkCmdSetRasterizationStreamEXT vkCmdSetRasterizationStreamEXT;
#else
	PFN_vkVoidFunction padding_d3ebb335[1];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization))
	PFN_vkCmdSetConservativeRasterizationModeEXT vkCmdSetConservativeRasterizationModeEXT;
	PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT vkCmdSetExtraPrimitiveOverestimationSizeEXT;
#else
	PFN_vkVoidFunction padding_a21758f4[2];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable))
	PFN_vkCmdSetDepthClipEnableEXT vkCmdSetDepthClipEnableEXT;
#else
	PFN_vkVoidFunction padding_a498a838[1];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations))
	PFN_vkCmdSetSampleLocationsEnableEXT vkCmdSetSampleLocationsEnableEXT;
#else
	PFN_vkVoidFunction padding_67db38de[1];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced))
	PFN_vkCmdSetColorBlendAdvancedEXT vkCmdSetColorBlendAdvancedEXT;
#else
	PFN_vkVoidFunction padding_fbea7481[1];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex))
	PFN_vkCmdSetProvokingVertexModeEXT vkCmdSetProvokingVertexModeEXT;
#else
	PFN_vkVoidFunction padding_3a8ec90e[1];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization))
	PFN_vkCmdSetLineRasterizationModeEXT vkCmdSetLineRasterizationModeEXT;
	PFN_vkCmdSetLineStippleEnableEXT vkCmdSetLineStippleEnableEXT;
#else
	PFN_vkVoidFunction padding_29cdb756[2];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control))
	PFN_vkCmdSetDepthClipNegativeOneToOneEXT vkCmdSetDepthClipNegativeOneToOneEXT;
#else
	PFN_vkVoidFunction padding_815a7240[1];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
	PFN_vkCmdSetViewportWScalingEnableNV vkCmdSetViewportWScalingEnableNV;
#else
	PFN_vkVoidFunction padding_d1f00511[1];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
	PFN_vkCmdSetViewportSwizzleNV vkCmdSetViewportSwizzleNV;
#else
	PFN_vkVoidFunction padding_7a73d553[1];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
	PFN_vkCmdSetCoverageToColorEnableNV vkCmdSetCoverageToColorEnableNV;
	PFN_vkCmdSetCoverageToColorLocationNV vkCmdSetCoverageToColorLocationNV;
#else
	PFN_vkVoidFunction padding_6045fb8c[2];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
	PFN_vkCmdSetCoverageModulationModeNV vkCmdSetCoverageModulationModeNV;
	PFN_vkCmdSetCoverageModulationTableEnableNV vkCmdSetCoverageModulationTableEnableNV;
	PFN_vkCmdSetCoverageModulationTableNV vkCmdSetCoverageModulationTableNV;
#else
	PFN_vkVoidFunction padding_bdc35c80[3];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
	PFN_vkCmdSetShadingRateImageEnableNV vkCmdSetShadingRateImageEnableNV;
#else
	PFN_vkVoidFunction padding_9a5cd6e8[1];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
	PFN_vkCmdSetRepresentativeFragmentTestEnableNV vkCmdSetRepresentativeFragmentTestEnableNV;
#else
	PFN_vkVoidFunction padding_3ee17e96[1];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
	PFN_vkCmdSetCoverageReductionModeNV vkCmdSetCoverageReductionModeNV;
#else
	PFN_vkVoidFunction padding_263d525a[1];
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode)) */
#if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
	PFN_vkGetImageSubresourceLayout2EXT vkGetImageSubresourceLayout2EXT;
#else
	PFN_vkVoidFunction padding_ecddace1[1];
#endif /* (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control)) */
#if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
	PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT;
#else
	PFN_vkVoidFunction padding_d83e1de1[1];
#endif /* (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state)) */
#if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && (defined(VK_VERSION_1_1) || defined(VK_KHR_descriptor_update_template)))
	PFN_vkCmdPushDescriptorSetWithTemplateKHR vkCmdPushDescriptorSetWithTemplateKHR;
#else
	PFN_vkVoidFunction padding_60f8358a[1];
#endif /* (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && (defined(VK_VERSION_1_1) || defined(VK_KHR_descriptor_update_template))) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR;
	PFN_vkGetDeviceGroupSurfacePresentModesKHR vkGetDeviceGroupSurfacePresentModesKHR;
#else
	PFN_vkVoidFunction padding_460290c6[2];
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	PFN_vkAcquireNextImage2KHR vkAcquireNextImage2KHR;
#else
	PFN_vkVoidFunction padding_cffc198[1];
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
	/* VOLK_GENERATE_DEVICE_TABLE */
};

/* VOLK_GENERATE_PROTOTYPES_H */
#if defined(VK_VERSION_1_0)
extern VOLK_API PFN_vkCreateDevice vkCreateDevice;
extern VOLK_API PFN_vkCreateInstance vkCreateInstance;
extern VOLK_API PFN_vkDestroyInstance vkDestroyInstance;
extern VOLK_API PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
extern VOLK_API PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties;
extern VOLK_API PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
extern VOLK_API PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
extern VOLK_API PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
extern VOLK_API PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
extern VOLK_API PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
extern VOLK_API PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
extern VOLK_API PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties;
extern VOLK_API PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties;
extern VOLK_API PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
extern VOLK_API PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
extern VOLK_API PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
extern VOLK_API PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties;
#endif /* defined(VK_VERSION_1_0) */
#if defined(VK_VERSION_1_1)
extern VOLK_API PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
extern VOLK_API PFN_vkEnumeratePhysicalDeviceGroups vkEnumeratePhysicalDeviceGroups;
extern VOLK_API PFN_vkGetPhysicalDeviceExternalBufferProperties vkGetPhysicalDeviceExternalBufferProperties;
extern VOLK_API PFN_vkGetPhysicalDeviceExternalFenceProperties vkGetPhysicalDeviceExternalFenceProperties;
extern VOLK_API PFN_vkGetPhysicalDeviceExternalSemaphoreProperties vkGetPhysicalDeviceExternalSemaphoreProperties;
extern VOLK_API PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2;
extern VOLK_API PFN_vkGetPhysicalDeviceFormatProperties2 vkGetPhysicalDeviceFormatProperties2;
extern VOLK_API PFN_vkGetPhysicalDeviceImageFormatProperties2 vkGetPhysicalDeviceImageFormatProperties2;
extern VOLK_API PFN_vkGetPhysicalDeviceMemoryProperties2 vkGetPhysicalDeviceMemoryProperties2;
extern VOLK_API PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2;
extern VOLK_API PFN_vkGetPhysicalDeviceQueueFamilyProperties2 vkGetPhysicalDeviceQueueFamilyProperties2;
extern VOLK_API PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 vkGetPhysicalDeviceSparseImageFormatProperties2;
#endif /* defined(VK_VERSION_1_1) */
#if defined(VK_VERSION_1_3)
extern VOLK_API PFN_vkGetPhysicalDeviceToolProperties vkGetPhysicalDeviceToolProperties;
#endif /* defined(VK_VERSION_1_3) */
#if defined(VK_ARM_data_graph)
extern VOLK_API PFN_vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM vkGetPhysicalDeviceQueueFamilyDataGraphProcessingEnginePropertiesARM;
extern VOLK_API PFN_vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM vkGetPhysicalDeviceQueueFamilyDataGraphPropertiesARM;
#endif /* defined(VK_ARM_data_graph) */
#if defined(VK_ARM_performance_counters_by_region)
extern VOLK_API PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM vkEnumeratePhysicalDeviceQueueFamilyPerformanceCountersByRegionARM;
#endif /* defined(VK_ARM_performance_counters_by_region) */
#if defined(VK_ARM_tensors)
extern VOLK_API PFN_vkGetPhysicalDeviceExternalTensorPropertiesARM vkGetPhysicalDeviceExternalTensorPropertiesARM;
#endif /* defined(VK_ARM_tensors) */
#if defined(VK_EXT_acquire_drm_display)
extern VOLK_API PFN_vkAcquireDrmDisplayEXT vkAcquireDrmDisplayEXT;
extern VOLK_API PFN_vkGetDrmDisplayEXT vkGetDrmDisplayEXT;
#endif /* defined(VK_EXT_acquire_drm_display) */
#if defined(VK_EXT_acquire_xlib_display)
extern VOLK_API PFN_vkAcquireXlibDisplayEXT vkAcquireXlibDisplayEXT;
extern VOLK_API PFN_vkGetRandROutputDisplayEXT vkGetRandROutputDisplayEXT;
#endif /* defined(VK_EXT_acquire_xlib_display) */
#if defined(VK_EXT_calibrated_timestamps)
extern VOLK_API PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT vkGetPhysicalDeviceCalibrateableTimeDomainsEXT;
#endif /* defined(VK_EXT_calibrated_timestamps) */
#if defined(VK_EXT_debug_report)
extern VOLK_API PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
extern VOLK_API PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT;
extern VOLK_API PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;
#endif /* defined(VK_EXT_debug_report) */
#if defined(VK_EXT_debug_utils)
extern VOLK_API PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
extern VOLK_API PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;
extern VOLK_API PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT;
extern VOLK_API PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
extern VOLK_API PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
extern VOLK_API PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT;
extern VOLK_API PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT;
extern VOLK_API PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT;
extern VOLK_API PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
extern VOLK_API PFN_vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT;
extern VOLK_API PFN_vkSubmitDebugUtilsMessageEXT vkSubmitDebugUtilsMessageEXT;
#endif /* defined(VK_EXT_debug_utils) */
#if defined(VK_EXT_direct_mode_display)
extern VOLK_API PFN_vkReleaseDisplayEXT vkReleaseDisplayEXT;
#endif /* defined(VK_EXT_direct_mode_display) */
#if defined(VK_EXT_directfb_surface)
extern VOLK_API PFN_vkCreateDirectFBSurfaceEXT vkCreateDirectFBSurfaceEXT;
extern VOLK_API PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT vkGetPhysicalDeviceDirectFBPresentationSupportEXT;
#endif /* defined(VK_EXT_directfb_surface) */
#if defined(VK_EXT_display_surface_counter)
extern VOLK_API PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT vkGetPhysicalDeviceSurfaceCapabilities2EXT;
#endif /* defined(VK_EXT_display_surface_counter) */
#if defined(VK_EXT_full_screen_exclusive)
extern VOLK_API PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT vkGetPhysicalDeviceSurfacePresentModes2EXT;
#endif /* defined(VK_EXT_full_screen_exclusive) */
#if defined(VK_EXT_headless_surface)
extern VOLK_API PFN_vkCreateHeadlessSurfaceEXT vkCreateHeadlessSurfaceEXT;
#endif /* defined(VK_EXT_headless_surface) */
#if defined(VK_EXT_metal_surface)
extern VOLK_API PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT;
#endif /* defined(VK_EXT_metal_surface) */
#if defined(VK_EXT_sample_locations)
extern VOLK_API PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT vkGetPhysicalDeviceMultisamplePropertiesEXT;
#endif /* defined(VK_EXT_sample_locations) */
#if defined(VK_EXT_tooling_info)
extern VOLK_API PFN_vkGetPhysicalDeviceToolPropertiesEXT vkGetPhysicalDeviceToolPropertiesEXT;
#endif /* defined(VK_EXT_tooling_info) */
#if defined(VK_FUCHSIA_imagepipe_surface)
extern VOLK_API PFN_vkCreateImagePipeSurfaceFUCHSIA vkCreateImagePipeSurfaceFUCHSIA;
#endif /* defined(VK_FUCHSIA_imagepipe_surface) */
#if defined(VK_GGP_stream_descriptor_surface)
extern VOLK_API PFN_vkCreateStreamDescriptorSurfaceGGP vkCreateStreamDescriptorSurfaceGGP;
#endif /* defined(VK_GGP_stream_descriptor_surface) */
#if defined(VK_KHR_android_surface)
extern VOLK_API PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
#endif /* defined(VK_KHR_android_surface) */
#if defined(VK_KHR_calibrated_timestamps)
extern VOLK_API PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsKHR vkGetPhysicalDeviceCalibrateableTimeDomainsKHR;
#endif /* defined(VK_KHR_calibrated_timestamps) */
#if defined(VK_KHR_cooperative_matrix)
extern VOLK_API PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR;
#endif /* defined(VK_KHR_cooperative_matrix) */
#if defined(VK_KHR_device_group_creation)
extern VOLK_API PFN_vkEnumeratePhysicalDeviceGroupsKHR vkEnumeratePhysicalDeviceGroupsKHR;
#endif /* defined(VK_KHR_device_group_creation) */
#if defined(VK_KHR_display)
extern VOLK_API PFN_vkCreateDisplayModeKHR vkCreateDisplayModeKHR;
extern VOLK_API PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR;
extern VOLK_API PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR;
extern VOLK_API PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR;
extern VOLK_API PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR;
extern VOLK_API PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
extern VOLK_API PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR;
#endif /* defined(VK_KHR_display) */
#if defined(VK_KHR_external_fence_capabilities)
extern VOLK_API PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR vkGetPhysicalDeviceExternalFencePropertiesKHR;
#endif /* defined(VK_KHR_external_fence_capabilities) */
#if defined(VK_KHR_external_memory_capabilities)
extern VOLK_API PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR vkGetPhysicalDeviceExternalBufferPropertiesKHR;
#endif /* defined(VK_KHR_external_memory_capabilities) */
#if defined(VK_KHR_external_semaphore_capabilities)
extern VOLK_API PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR vkGetPhysicalDeviceExternalSemaphorePropertiesKHR;
#endif /* defined(VK_KHR_external_semaphore_capabilities) */
#if defined(VK_KHR_fragment_shading_rate)
extern VOLK_API PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR vkGetPhysicalDeviceFragmentShadingRatesKHR;
#endif /* defined(VK_KHR_fragment_shading_rate) */
#if defined(VK_KHR_get_display_properties2)
extern VOLK_API PFN_vkGetDisplayModeProperties2KHR vkGetDisplayModeProperties2KHR;
extern VOLK_API PFN_vkGetDisplayPlaneCapabilities2KHR vkGetDisplayPlaneCapabilities2KHR;
extern VOLK_API PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR vkGetPhysicalDeviceDisplayPlaneProperties2KHR;
extern VOLK_API PFN_vkGetPhysicalDeviceDisplayProperties2KHR vkGetPhysicalDeviceDisplayProperties2KHR;
#endif /* defined(VK_KHR_get_display_properties2) */
#if defined(VK_KHR_get_physical_device_properties2)
extern VOLK_API PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR;
extern VOLK_API PFN_vkGetPhysicalDeviceFormatProperties2KHR vkGetPhysicalDeviceFormatProperties2KHR;
extern VOLK_API PFN_vkGetPhysicalDeviceImageFormatProperties2KHR vkGetPhysicalDeviceImageFormatProperties2KHR;
extern VOLK_API PFN_vkGetPhysicalDeviceMemoryProperties2KHR vkGetPhysicalDeviceMemoryProperties2KHR;
extern VOLK_API PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR;
extern VOLK_API PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR vkGetPhysicalDeviceQueueFamilyProperties2KHR;
extern VOLK_API PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR vkGetPhysicalDeviceSparseImageFormatProperties2KHR;
#endif /* defined(VK_KHR_get_physical_device_properties2) */
#if defined(VK_KHR_get_surface_capabilities2)
extern VOLK_API PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR;
extern VOLK_API PFN_vkGetPhysicalDeviceSurfaceFormats2KHR vkGetPhysicalDeviceSurfaceFormats2KHR;
#endif /* defined(VK_KHR_get_surface_capabilities2) */
#if defined(VK_KHR_performance_query)
extern VOLK_API PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR;
extern VOLK_API PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR;
#endif /* defined(VK_KHR_performance_query) */
#if defined(VK_KHR_surface)
extern VOLK_API PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
extern VOLK_API PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
extern VOLK_API PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
extern VOLK_API PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
extern VOLK_API PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
#endif /* defined(VK_KHR_surface) */
#if defined(VK_KHR_video_encode_queue)
extern VOLK_API PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR;
#endif /* defined(VK_KHR_video_encode_queue) */
#if defined(VK_KHR_video_queue)
extern VOLK_API PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR vkGetPhysicalDeviceVideoCapabilitiesKHR;
extern VOLK_API PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR vkGetPhysicalDeviceVideoFormatPropertiesKHR;
#endif /* defined(VK_KHR_video_queue) */
#if defined(VK_KHR_wayland_surface)
extern VOLK_API PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR;
extern VOLK_API PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR;
#endif /* defined(VK_KHR_wayland_surface) */
#if defined(VK_KHR_win32_surface)
extern VOLK_API PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
extern VOLK_API PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR;
#endif /* defined(VK_KHR_win32_surface) */
#if defined(VK_KHR_xcb_surface)
extern VOLK_API PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR;
extern VOLK_API PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR vkGetPhysicalDeviceXcbPresentationSupportKHR;
#endif /* defined(VK_KHR_xcb_surface) */
#if defined(VK_KHR_xlib_surface)
extern VOLK_API PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;
extern VOLK_API PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR vkGetPhysicalDeviceXlibPresentationSupportKHR;
#endif /* defined(VK_KHR_xlib_surface) */
#if defined(VK_MVK_ios_surface)
extern VOLK_API PFN_vkCreateIOSSurfaceMVK vkCreateIOSSurfaceMVK;
#endif /* defined(VK_MVK_ios_surface) */
#if defined(VK_MVK_macos_surface)
extern VOLK_API PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK;
#endif /* defined(VK_MVK_macos_surface) */
#if defined(VK_NN_vi_surface)
extern VOLK_API PFN_vkCreateViSurfaceNN vkCreateViSurfaceNN;
#endif /* defined(VK_NN_vi_surface) */
#if defined(VK_NV_acquire_winrt_display)
extern VOLK_API PFN_vkAcquireWinrtDisplayNV vkAcquireWinrtDisplayNV;
extern VOLK_API PFN_vkGetWinrtDisplayNV vkGetWinrtDisplayNV;
#endif /* defined(VK_NV_acquire_winrt_display) */
#if defined(VK_NV_cooperative_matrix)
extern VOLK_API PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV vkGetPhysicalDeviceCooperativeMatrixPropertiesNV;
#endif /* defined(VK_NV_cooperative_matrix) */
#if defined(VK_NV_cooperative_matrix2)
extern VOLK_API PFN_vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV vkGetPhysicalDeviceCooperativeMatrixFlexibleDimensionsPropertiesNV;
#endif /* defined(VK_NV_cooperative_matrix2) */
#if defined(VK_NV_cooperative_vector)
extern VOLK_API PFN_vkGetPhysicalDeviceCooperativeVectorPropertiesNV vkGetPhysicalDeviceCooperativeVectorPropertiesNV;
#endif /* defined(VK_NV_cooperative_vector) */
#if defined(VK_NV_coverage_reduction_mode)
extern VOLK_API PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV;
#endif /* defined(VK_NV_coverage_reduction_mode) */
#if defined(VK_NV_external_memory_capabilities)
extern VOLK_API PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV vkGetPhysicalDeviceExternalImageFormatPropertiesNV;
#endif /* defined(VK_NV_external_memory_capabilities) */
#if defined(VK_NV_optical_flow)
extern VOLK_API PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV vkGetPhysicalDeviceOpticalFlowImageFormatsNV;
#endif /* defined(VK_NV_optical_flow) */
#if defined(VK_OHOS_surface)
extern VOLK_API PFN_vkCreateSurfaceOHOS vkCreateSurfaceOHOS;
#endif /* defined(VK_OHOS_surface) */
#if defined(VK_QNX_screen_surface)
extern VOLK_API PFN_vkCreateScreenSurfaceQNX vkCreateScreenSurfaceQNX;
extern VOLK_API PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX vkGetPhysicalDeviceScreenPresentationSupportQNX;
#endif /* defined(VK_QNX_screen_surface) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
extern VOLK_API PFN_vkGetPhysicalDevicePresentRectanglesKHR vkGetPhysicalDevicePresentRectanglesKHR;
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
/* VOLK_GENERATE_PROTOTYPES_H */

#ifndef VOLK_NO_DEVICE_PROTOTYPES
/* VOLK_GENERATE_PROTOTYPES_H_DEVICE */
#if defined(VK_VERSION_1_0)
extern VOLK_API PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
extern VOLK_API PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
extern VOLK_API PFN_vkAllocateMemory vkAllocateMemory;
extern VOLK_API PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
extern VOLK_API PFN_vkBindBufferMemory vkBindBufferMemory;
extern VOLK_API PFN_vkBindImageMemory vkBindImageMemory;
extern VOLK_API PFN_vkCmdBeginQuery vkCmdBeginQuery;
extern VOLK_API PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
extern VOLK_API PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
extern VOLK_API PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
extern VOLK_API PFN_vkCmdBindPipeline vkCmdBindPipeline;
extern VOLK_API PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
extern VOLK_API PFN_vkCmdBlitImage vkCmdBlitImage;
extern VOLK_API PFN_vkCmdClearAttachments vkCmdClearAttachments;
extern VOLK_API PFN_vkCmdClearColorImage vkCmdClearColorImage;
extern VOLK_API PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage;
extern VOLK_API PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
extern VOLK_API PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
extern VOLK_API PFN_vkCmdCopyImage vkCmdCopyImage;
extern VOLK_API PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer;
extern VOLK_API PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults;
extern VOLK_API PFN_vkCmdDispatch vkCmdDispatch;
extern VOLK_API PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect;
extern VOLK_API PFN_vkCmdDraw vkCmdDraw;
extern VOLK_API PFN_vkCmdDrawIndexed vkCmdDrawIndexed;
extern VOLK_API PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect;
extern VOLK_API PFN_vkCmdDrawIndirect vkCmdDrawIndirect;
extern VOLK_API PFN_vkCmdEndQuery vkCmdEndQuery;
extern VOLK_API PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
extern VOLK_API PFN_vkCmdExecuteCommands vkCmdExecuteCommands;
extern VOLK_API PFN_vkCmdFillBuffer vkCmdFillBuffer;
extern VOLK_API PFN_vkCmdNextSubpass vkCmdNextSubpass;
extern VOLK_API PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
extern VOLK_API PFN_vkCmdPushConstants vkCmdPushConstants;
extern VOLK_API PFN_vkCmdResetEvent vkCmdResetEvent;
extern VOLK_API PFN_vkCmdResetQueryPool vkCmdResetQueryPool;
extern VOLK_API PFN_vkCmdResolveImage vkCmdResolveImage;
extern VOLK_API PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants;
extern VOLK_API PFN_vkCmdSetDepthBias vkCmdSetDepthBias;
extern VOLK_API PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds;
extern VOLK_API PFN_vkCmdSetEvent vkCmdSetEvent;
extern VOLK_API PFN_vkCmdSetLineWidth vkCmdSetLineWidth;
extern VOLK_API PFN_vkCmdSetScissor vkCmdSetScissor;
extern VOLK_API PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask;
extern VOLK_API PFN_vkCmdSetStencilReference vkCmdSetStencilReference;
extern VOLK_API PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask;
extern VOLK_API PFN_vkCmdSetViewport vkCmdSetViewport;
extern VOLK_API PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer;
extern VOLK_API PFN_vkCmdWaitEvents vkCmdWaitEvents;
extern VOLK_API PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp;
extern VOLK_API PFN_vkCreateBuffer vkCreateBuffer;
extern VOLK_API PFN_vkCreateBufferView vkCreateBufferView;
extern VOLK_API PFN_vkCreateCommandPool vkCreateCommandPool;
extern VOLK_API PFN_vkCreateComputePipelines vkCreateComputePipelines;
extern VOLK_API PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
extern VOLK_API PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
extern VOLK_API PFN_vkCreateEvent vkCreateEvent;
extern VOLK_API PFN_vkCreateFence vkCreateFence;
extern VOLK_API PFN_vkCreateFramebuffer vkCreateFramebuffer;
extern VOLK_API PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
extern VOLK_API PFN_vkCreateImage vkCreateImage;
extern VOLK_API PFN_vkCreateImageView vkCreateImageView;
extern VOLK_API PFN_vkCreatePipelineCache vkCreatePipelineCache;
extern VOLK_API PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
extern VOLK_API PFN_vkCreateQueryPool vkCreateQueryPool;
extern VOLK_API PFN_vkCreateRenderPass vkCreateRenderPass;
extern VOLK_API PFN_vkCreateSampler vkCreateSampler;
extern VOLK_API PFN_vkCreateSemaphore vkCreateSemaphore;
extern VOLK_API PFN_vkCreateShaderModule vkCreateShaderModule;
extern VOLK_API PFN_vkDestroyBuffer vkDestroyBuffer;
extern VOLK_API PFN_vkDestroyBufferView vkDestroyBufferView;
extern VOLK_API PFN_vkDestroyCommandPool vkDestroyCommandPool;
extern VOLK_API PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
extern VOLK_API PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
extern VOLK_API PFN_vkDestroyDevice vkDestroyDevice;
extern VOLK_API PFN_vkDestroyEvent vkDestroyEvent;
extern VOLK_API PFN_vkDestroyFence vkDestroyFence;
extern VOLK_API PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
extern VOLK_API PFN_vkDestroyImage vkDestroyImage;
extern VOLK_API PFN_vkDestroyImageView vkDestroyImageView;
extern VOLK_API PFN_vkDestroyPipeline vkDestroyPipeline;
extern VOLK_API PFN_vkDestroyPipelineCache vkDestroyPipelineCache;
extern VOLK_API PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
extern VOLK_API PFN_vkDestroyQueryPool vkDestroyQueryPool;
extern VOLK_API PFN_vkDestroyRenderPass vkDestroyRenderPass;
extern VOLK_API PFN_vkDestroySampler vkDestroySampler;
extern VOLK_API PFN_vkDestroySemaphore vkDestroySemaphore;
extern VOLK_API PFN_vkDestroyShaderModule vkDestroyShaderModule;
extern VOLK_API PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
extern VOLK_API PFN_vkEndCommandBuffer vkEndCommandBuffer;
extern VOLK_API PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges;
extern VOLK_API PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
extern VOLK_API PFN_vkFreeDescriptorSets vkFreeDescriptorSets;
extern VOLK_API PFN_vkFreeMemory vkFreeMemory;
extern VOLK_API PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
extern VOLK_API PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment;
extern VOLK_API PFN_vkGetDeviceQueue vkGetDeviceQueue;
extern VOLK_API PFN_vkGetEventStatus vkGetEventStatus;
extern VOLK_API PFN_vkGetFenceStatus vkGetFenceStatus;
extern VOLK_API PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
extern VOLK_API PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements;
extern VOLK_API PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout;
extern VOLK_API PFN_vkGetPipelineCacheData vkGetPipelineCacheData;
extern VOLK_API PFN_vkGetQueryPoolResults vkGetQueryPoolResults;
extern VOLK_API PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity;
extern VOLK_API PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges;
extern VOLK_API PFN_vkMapMemory vkMapMemory;
extern VOLK_API PFN_vkMergePipelineCaches vkMergePipelineCaches;
extern VOLK_API PFN_vkQueueBindSparse vkQueueBindSparse;
extern VOLK_API PFN_vkQueueSubmit vkQueueSubmit;
extern VOLK_API PFN_vkQueueWaitIdle vkQueueWaitIdle;
extern VOLK_API PFN_vkResetCommandBuffer vkResetCommandBuffer;
extern VOLK_API PFN_vkResetCommandPool vkResetCommandPool;
extern VOLK_API PFN_vkResetDescriptorPool vkResetDescriptorPool;
extern VOLK_API PFN_vkResetEvent vkResetEvent;
extern VOLK_API PFN_vkResetFences vkResetFences;
extern VOLK_API PFN_vkSetEvent vkSetEvent;
extern VOLK_API PFN_vkUnmapMemory vkUnmapMemory;
extern VOLK_API PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
extern VOLK_API PFN_vkWaitForFences vkWaitForFences;
#endif /* defined(VK_VERSION_1_0) */
#if defined(VK_VERSION_1_1)
extern VOLK_API PFN_vkBindBufferMemory2 vkBindBufferMemory2;
extern VOLK_API PFN_vkBindImageMemory2 vkBindImageMemory2;
extern VOLK_API PFN_vkCmdDispatchBase vkCmdDispatchBase;
extern VOLK_API PFN_vkCmdSetDeviceMask vkCmdSetDeviceMask;
extern VOLK_API PFN_vkCreateDescriptorUpdateTemplate vkCreateDescriptorUpdateTemplate;
extern VOLK_API PFN_vkCreateSamplerYcbcrConversion vkCreateSamplerYcbcrConversion;
extern VOLK_API PFN_vkDestroyDescriptorUpdateTemplate vkDestroyDescriptorUpdateTemplate;
extern VOLK_API PFN_vkDestroySamplerYcbcrConversion vkDestroySamplerYcbcrConversion;
extern VOLK_API PFN_vkGetBufferMemoryRequirements2 vkGetBufferMemoryRequirements2;
extern VOLK_API PFN_vkGetDescriptorSetLayoutSupport vkGetDescriptorSetLayoutSupport;
extern VOLK_API PFN_vkGetDeviceGroupPeerMemoryFeatures vkGetDeviceGroupPeerMemoryFeatures;
extern VOLK_API PFN_vkGetDeviceQueue2 vkGetDeviceQueue2;
extern VOLK_API PFN_vkGetImageMemoryRequirements2 vkGetImageMemoryRequirements2;
extern VOLK_API PFN_vkGetImageSparseMemoryRequirements2 vkGetImageSparseMemoryRequirements2;
extern VOLK_API PFN_vkTrimCommandPool vkTrimCommandPool;
extern VOLK_API PFN_vkUpdateDescriptorSetWithTemplate vkUpdateDescriptorSetWithTemplate;
#endif /* defined(VK_VERSION_1_1) */
#if defined(VK_VERSION_1_2)
extern VOLK_API PFN_vkCmdBeginRenderPass2 vkCmdBeginRenderPass2;
extern VOLK_API PFN_vkCmdDrawIndexedIndirectCount vkCmdDrawIndexedIndirectCount;
extern VOLK_API PFN_vkCmdDrawIndirectCount vkCmdDrawIndirectCount;
extern VOLK_API PFN_vkCmdEndRenderPass2 vkCmdEndRenderPass2;
extern VOLK_API PFN_vkCmdNextSubpass2 vkCmdNextSubpass2;
extern VOLK_API PFN_vkCreateRenderPass2 vkCreateRenderPass2;
extern VOLK_API PFN_vkGetBufferDeviceAddress vkGetBufferDeviceAddress;
extern VOLK_API PFN_vkGetBufferOpaqueCaptureAddress vkGetBufferOpaqueCaptureAddress;
extern VOLK_API PFN_vkGetDeviceMemoryOpaqueCaptureAddress vkGetDeviceMemoryOpaqueCaptureAddress;
extern VOLK_API PFN_vkGetSemaphoreCounterValue vkGetSemaphoreCounterValue;
extern VOLK_API PFN_vkResetQueryPool vkResetQueryPool;
extern VOLK_API PFN_vkSignalSemaphore vkSignalSemaphore;
extern VOLK_API PFN_vkWaitSemaphores vkWaitSemaphores;
#endif /* defined(VK_VERSION_1_2) */
#if defined(VK_VERSION_1_3)
extern VOLK_API PFN_vkCmdBeginRendering vkCmdBeginRendering;
extern VOLK_API PFN_vkCmdBindVertexBuffers2 vkCmdBindVertexBuffers2;
extern VOLK_API PFN_vkCmdBlitImage2 vkCmdBlitImage2;
extern VOLK_API PFN_vkCmdCopyBuffer2 vkCmdCopyBuffer2;
extern VOLK_API PFN_vkCmdCopyBufferToImage2 vkCmdCopyBufferToImage2;
extern VOLK_API PFN_vkCmdCopyImage2 vkCmdCopyImage2;
extern VOLK_API PFN_vkCmdCopyImageToBuffer2 vkCmdCopyImageToBuffer2;
extern VOLK_API PFN_vkCmdEndRendering vkCmdEndRendering;
extern VOLK_API PFN_vkCmdPipelineBarrier2 vkCmdPipelineBarrier2;
extern VOLK_API PFN_vkCmdResetEvent2 vkCmdResetEvent2;
extern VOLK_API PFN_vkCmdResolveImage2 vkCmdResolveImage2;
extern VOLK_API PFN_vkCmdSetCullMode vkCmdSetCullMode;
extern VOLK_API PFN_vkCmdSetDepthBiasEnable vkCmdSetDepthBiasEnable;
extern VOLK_API PFN_vkCmdSetDepthBoundsTestEnable vkCmdSetDepthBoundsTestEnable;
extern VOLK_API PFN_vkCmdSetDepthCompareOp vkCmdSetDepthCompareOp;
extern VOLK_API PFN_vkCmdSetDepthTestEnable vkCmdSetDepthTestEnable;
extern VOLK_API PFN_vkCmdSetDepthWriteEnable vkCmdSetDepthWriteEnable;
extern VOLK_API PFN_vkCmdSetEvent2 vkCmdSetEvent2;
extern VOLK_API PFN_vkCmdSetFrontFace vkCmdSetFrontFace;
extern VOLK_API PFN_vkCmdSetPrimitiveRestartEnable vkCmdSetPrimitiveRestartEnable;
extern VOLK_API PFN_vkCmdSetPrimitiveTopology vkCmdSetPrimitiveTopology;
extern VOLK_API PFN_vkCmdSetRasterizerDiscardEnable vkCmdSetRasterizerDiscardEnable;
extern VOLK_API PFN_vkCmdSetScissorWithCount vkCmdSetScissorWithCount;
extern VOLK_API PFN_vkCmdSetStencilOp vkCmdSetStencilOp;
extern VOLK_API PFN_vkCmdSetStencilTestEnable vkCmdSetStencilTestEnable;
extern VOLK_API PFN_vkCmdSetViewportWithCount vkCmdSetViewportWithCount;
extern VOLK_API PFN_vkCmdWaitEvents2 vkCmdWaitEvents2;
extern VOLK_API PFN_vkCmdWriteTimestamp2 vkCmdWriteTimestamp2;
extern VOLK_API PFN_vkCreatePrivateDataSlot vkCreatePrivateDataSlot;
extern VOLK_API PFN_vkDestroyPrivateDataSlot vkDestroyPrivateDataSlot;
extern VOLK_API PFN_vkGetDeviceBufferMemoryRequirements vkGetDeviceBufferMemoryRequirements;
extern VOLK_API PFN_vkGetDeviceImageMemoryRequirements vkGetDeviceImageMemoryRequirements;
extern VOLK_API PFN_vkGetDeviceImageSparseMemoryRequirements vkGetDeviceImageSparseMemoryRequirements;
extern VOLK_API PFN_vkGetPrivateData vkGetPrivateData;
extern VOLK_API PFN_vkQueueSubmit2 vkQueueSubmit2;
extern VOLK_API PFN_vkSetPrivateData vkSetPrivateData;
#endif /* defined(VK_VERSION_1_3) */
#if defined(VK_VERSION_1_4)
extern VOLK_API PFN_vkCmdBindDescriptorSets2 vkCmdBindDescriptorSets2;
extern VOLK_API PFN_vkCmdBindIndexBuffer2 vkCmdBindIndexBuffer2;
extern VOLK_API PFN_vkCmdPushConstants2 vkCmdPushConstants2;
extern VOLK_API PFN_vkCmdPushDescriptorSet vkCmdPushDescriptorSet;
extern VOLK_API PFN_vkCmdPushDescriptorSet2 vkCmdPushDescriptorSet2;
extern VOLK_API PFN_vkCmdPushDescriptorSetWithTemplate vkCmdPushDescriptorSetWithTemplate;
extern VOLK_API PFN_vkCmdPushDescriptorSetWithTemplate2 vkCmdPushDescriptorSetWithTemplate2;
extern VOLK_API PFN_vkCmdSetLineStipple vkCmdSetLineStipple;
extern VOLK_API PFN_vkCmdSetRenderingAttachmentLocations vkCmdSetRenderingAttachmentLocations;
extern VOLK_API PFN_vkCmdSetRenderingInputAttachmentIndices vkCmdSetRenderingInputAttachmentIndices;
extern VOLK_API PFN_vkCopyImageToImage vkCopyImageToImage;
extern VOLK_API PFN_vkCopyImageToMemory vkCopyImageToMemory;
extern VOLK_API PFN_vkCopyMemoryToImage vkCopyMemoryToImage;
extern VOLK_API PFN_vkGetDeviceImageSubresourceLayout vkGetDeviceImageSubresourceLayout;
extern VOLK_API PFN_vkGetImageSubresourceLayout2 vkGetImageSubresourceLayout2;
extern VOLK_API PFN_vkGetRenderingAreaGranularity vkGetRenderingAreaGranularity;
extern VOLK_API PFN_vkMapMemory2 vkMapMemory2;
extern VOLK_API PFN_vkTransitionImageLayout vkTransitionImageLayout;
extern VOLK_API PFN_vkUnmapMemory2 vkUnmapMemory2;
#endif /* defined(VK_VERSION_1_4) */
#if defined(VK_AMDX_shader_enqueue)
extern VOLK_API PFN_vkCmdDispatchGraphAMDX vkCmdDispatchGraphAMDX;
extern VOLK_API PFN_vkCmdDispatchGraphIndirectAMDX vkCmdDispatchGraphIndirectAMDX;
extern VOLK_API PFN_vkCmdDispatchGraphIndirectCountAMDX vkCmdDispatchGraphIndirectCountAMDX;
extern VOLK_API PFN_vkCmdInitializeGraphScratchMemoryAMDX vkCmdInitializeGraphScratchMemoryAMDX;
extern VOLK_API PFN_vkCreateExecutionGraphPipelinesAMDX vkCreateExecutionGraphPipelinesAMDX;
extern VOLK_API PFN_vkGetExecutionGraphPipelineNodeIndexAMDX vkGetExecutionGraphPipelineNodeIndexAMDX;
extern VOLK_API PFN_vkGetExecutionGraphPipelineScratchSizeAMDX vkGetExecutionGraphPipelineScratchSizeAMDX;
#endif /* defined(VK_AMDX_shader_enqueue) */
#if defined(VK_AMD_anti_lag)
extern VOLK_API PFN_vkAntiLagUpdateAMD vkAntiLagUpdateAMD;
#endif /* defined(VK_AMD_anti_lag) */
#if defined(VK_AMD_buffer_marker)
extern VOLK_API PFN_vkCmdWriteBufferMarkerAMD vkCmdWriteBufferMarkerAMD;
#endif /* defined(VK_AMD_buffer_marker) */
#if defined(VK_AMD_buffer_marker) && (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
extern VOLK_API PFN_vkCmdWriteBufferMarker2AMD vkCmdWriteBufferMarker2AMD;
#endif /* defined(VK_AMD_buffer_marker) && (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2)) */
#if defined(VK_AMD_display_native_hdr)
extern VOLK_API PFN_vkSetLocalDimmingAMD vkSetLocalDimmingAMD;
#endif /* defined(VK_AMD_display_native_hdr) */
#if defined(VK_AMD_draw_indirect_count)
extern VOLK_API PFN_vkCmdDrawIndexedIndirectCountAMD vkCmdDrawIndexedIndirectCountAMD;
extern VOLK_API PFN_vkCmdDrawIndirectCountAMD vkCmdDrawIndirectCountAMD;
#endif /* defined(VK_AMD_draw_indirect_count) */
#if defined(VK_AMD_shader_info)
extern VOLK_API PFN_vkGetShaderInfoAMD vkGetShaderInfoAMD;
#endif /* defined(VK_AMD_shader_info) */
#if defined(VK_ANDROID_external_memory_android_hardware_buffer)
extern VOLK_API PFN_vkGetAndroidHardwareBufferPropertiesANDROID vkGetAndroidHardwareBufferPropertiesANDROID;
extern VOLK_API PFN_vkGetMemoryAndroidHardwareBufferANDROID vkGetMemoryAndroidHardwareBufferANDROID;
#endif /* defined(VK_ANDROID_external_memory_android_hardware_buffer) */
#if defined(VK_ARM_data_graph)
extern VOLK_API PFN_vkBindDataGraphPipelineSessionMemoryARM vkBindDataGraphPipelineSessionMemoryARM;
extern VOLK_API PFN_vkCmdDispatchDataGraphARM vkCmdDispatchDataGraphARM;
extern VOLK_API PFN_vkCreateDataGraphPipelineSessionARM vkCreateDataGraphPipelineSessionARM;
extern VOLK_API PFN_vkCreateDataGraphPipelinesARM vkCreateDataGraphPipelinesARM;
extern VOLK_API PFN_vkDestroyDataGraphPipelineSessionARM vkDestroyDataGraphPipelineSessionARM;
extern VOLK_API PFN_vkGetDataGraphPipelineAvailablePropertiesARM vkGetDataGraphPipelineAvailablePropertiesARM;
extern VOLK_API PFN_vkGetDataGraphPipelinePropertiesARM vkGetDataGraphPipelinePropertiesARM;
extern VOLK_API PFN_vkGetDataGraphPipelineSessionBindPointRequirementsARM vkGetDataGraphPipelineSessionBindPointRequirementsARM;
extern VOLK_API PFN_vkGetDataGraphPipelineSessionMemoryRequirementsARM vkGetDataGraphPipelineSessionMemoryRequirementsARM;
#endif /* defined(VK_ARM_data_graph) */
#if defined(VK_ARM_tensors)
extern VOLK_API PFN_vkBindTensorMemoryARM vkBindTensorMemoryARM;
extern VOLK_API PFN_vkCmdCopyTensorARM vkCmdCopyTensorARM;
extern VOLK_API PFN_vkCreateTensorARM vkCreateTensorARM;
extern VOLK_API PFN_vkCreateTensorViewARM vkCreateTensorViewARM;
extern VOLK_API PFN_vkDestroyTensorARM vkDestroyTensorARM;
extern VOLK_API PFN_vkDestroyTensorViewARM vkDestroyTensorViewARM;
extern VOLK_API PFN_vkGetDeviceTensorMemoryRequirementsARM vkGetDeviceTensorMemoryRequirementsARM;
extern VOLK_API PFN_vkGetTensorMemoryRequirementsARM vkGetTensorMemoryRequirementsARM;
#endif /* defined(VK_ARM_tensors) */
#if defined(VK_ARM_tensors) && defined(VK_EXT_descriptor_buffer)
extern VOLK_API PFN_vkGetTensorOpaqueCaptureDescriptorDataARM vkGetTensorOpaqueCaptureDescriptorDataARM;
extern VOLK_API PFN_vkGetTensorViewOpaqueCaptureDescriptorDataARM vkGetTensorViewOpaqueCaptureDescriptorDataARM;
#endif /* defined(VK_ARM_tensors) && defined(VK_EXT_descriptor_buffer) */
#if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
extern VOLK_API PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT vkCmdSetAttachmentFeedbackLoopEnableEXT;
#endif /* defined(VK_EXT_attachment_feedback_loop_dynamic_state) */
#if defined(VK_EXT_buffer_device_address)
extern VOLK_API PFN_vkGetBufferDeviceAddressEXT vkGetBufferDeviceAddressEXT;
#endif /* defined(VK_EXT_buffer_device_address) */
#if defined(VK_EXT_calibrated_timestamps)
extern VOLK_API PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT;
#endif /* defined(VK_EXT_calibrated_timestamps) */
#if defined(VK_EXT_color_write_enable)
extern VOLK_API PFN_vkCmdSetColorWriteEnableEXT vkCmdSetColorWriteEnableEXT;
#endif /* defined(VK_EXT_color_write_enable) */
#if defined(VK_EXT_conditional_rendering)
extern VOLK_API PFN_vkCmdBeginConditionalRenderingEXT vkCmdBeginConditionalRenderingEXT;
extern VOLK_API PFN_vkCmdEndConditionalRenderingEXT vkCmdEndConditionalRenderingEXT;
#endif /* defined(VK_EXT_conditional_rendering) */
#if defined(VK_EXT_custom_resolve) && (defined(VK_KHR_dynamic_rendering) || defined(VK_VERSION_1_3))
extern PFN_vkCmdBeginCustomResolveEXT vkCmdBeginCustomResolveEXT;
#endif /* defined(VK_EXT_custom_resolve) && (defined(VK_KHR_dynamic_rendering) || defined(VK_VERSION_1_3)) */
#if defined(VK_EXT_debug_marker)
extern VOLK_API PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT;
extern VOLK_API PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT;
extern VOLK_API PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT;
extern VOLK_API PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT;
extern VOLK_API PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT;
#endif /* defined(VK_EXT_debug_marker) */
#if defined(VK_EXT_depth_bias_control)
extern VOLK_API PFN_vkCmdSetDepthBias2EXT vkCmdSetDepthBias2EXT;
#endif /* defined(VK_EXT_depth_bias_control) */
#if defined(VK_EXT_descriptor_buffer)
extern VOLK_API PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT vkCmdBindDescriptorBufferEmbeddedSamplersEXT;
extern VOLK_API PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT;
extern VOLK_API PFN_vkCmdSetDescriptorBufferOffsetsEXT vkCmdSetDescriptorBufferOffsetsEXT;
extern VOLK_API PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT vkGetBufferOpaqueCaptureDescriptorDataEXT;
extern VOLK_API PFN_vkGetDescriptorEXT vkGetDescriptorEXT;
extern VOLK_API PFN_vkGetDescriptorSetLayoutBindingOffsetEXT vkGetDescriptorSetLayoutBindingOffsetEXT;
extern VOLK_API PFN_vkGetDescriptorSetLayoutSizeEXT vkGetDescriptorSetLayoutSizeEXT;
extern VOLK_API PFN_vkGetImageOpaqueCaptureDescriptorDataEXT vkGetImageOpaqueCaptureDescriptorDataEXT;
extern VOLK_API PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT vkGetImageViewOpaqueCaptureDescriptorDataEXT;
extern VOLK_API PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT vkGetSamplerOpaqueCaptureDescriptorDataEXT;
#endif /* defined(VK_EXT_descriptor_buffer) */
#if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
extern VOLK_API PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT;
#endif /* defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing)) */
#if defined(VK_EXT_device_fault)
extern VOLK_API PFN_vkGetDeviceFaultInfoEXT vkGetDeviceFaultInfoEXT;
#endif /* defined(VK_EXT_device_fault) */
#if defined(VK_EXT_device_generated_commands)
extern VOLK_API PFN_vkCmdExecuteGeneratedCommandsEXT vkCmdExecuteGeneratedCommandsEXT;
extern VOLK_API PFN_vkCmdPreprocessGeneratedCommandsEXT vkCmdPreprocessGeneratedCommandsEXT;
extern VOLK_API PFN_vkCreateIndirectCommandsLayoutEXT vkCreateIndirectCommandsLayoutEXT;
extern VOLK_API PFN_vkCreateIndirectExecutionSetEXT vkCreateIndirectExecutionSetEXT;
extern VOLK_API PFN_vkDestroyIndirectCommandsLayoutEXT vkDestroyIndirectCommandsLayoutEXT;
extern VOLK_API PFN_vkDestroyIndirectExecutionSetEXT vkDestroyIndirectExecutionSetEXT;
extern VOLK_API PFN_vkGetGeneratedCommandsMemoryRequirementsEXT vkGetGeneratedCommandsMemoryRequirementsEXT;
extern VOLK_API PFN_vkUpdateIndirectExecutionSetPipelineEXT vkUpdateIndirectExecutionSetPipelineEXT;
extern VOLK_API PFN_vkUpdateIndirectExecutionSetShaderEXT vkUpdateIndirectExecutionSetShaderEXT;
#endif /* defined(VK_EXT_device_generated_commands) */
#if defined(VK_EXT_discard_rectangles)
extern VOLK_API PFN_vkCmdSetDiscardRectangleEXT vkCmdSetDiscardRectangleEXT;
#endif /* defined(VK_EXT_discard_rectangles) */
#if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
extern VOLK_API PFN_vkCmdSetDiscardRectangleEnableEXT vkCmdSetDiscardRectangleEnableEXT;
extern VOLK_API PFN_vkCmdSetDiscardRectangleModeEXT vkCmdSetDiscardRectangleModeEXT;
#endif /* defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2 */
#if defined(VK_EXT_display_control)
extern VOLK_API PFN_vkDisplayPowerControlEXT vkDisplayPowerControlEXT;
extern VOLK_API PFN_vkGetSwapchainCounterEXT vkGetSwapchainCounterEXT;
extern VOLK_API PFN_vkRegisterDeviceEventEXT vkRegisterDeviceEventEXT;
extern VOLK_API PFN_vkRegisterDisplayEventEXT vkRegisterDisplayEventEXT;
#endif /* defined(VK_EXT_display_control) */
#if defined(VK_EXT_external_memory_host)
extern VOLK_API PFN_vkGetMemoryHostPointerPropertiesEXT vkGetMemoryHostPointerPropertiesEXT;
#endif /* defined(VK_EXT_external_memory_host) */
#if defined(VK_EXT_external_memory_metal)
extern VOLK_API PFN_vkGetMemoryMetalHandleEXT vkGetMemoryMetalHandleEXT;
extern VOLK_API PFN_vkGetMemoryMetalHandlePropertiesEXT vkGetMemoryMetalHandlePropertiesEXT;
#endif /* defined(VK_EXT_external_memory_metal) */
#if defined(VK_EXT_fragment_density_map_offset)
extern VOLK_API PFN_vkCmdEndRendering2EXT vkCmdEndRendering2EXT;
#endif /* defined(VK_EXT_fragment_density_map_offset) */
#if defined(VK_EXT_full_screen_exclusive)
extern VOLK_API PFN_vkAcquireFullScreenExclusiveModeEXT vkAcquireFullScreenExclusiveModeEXT;
extern VOLK_API PFN_vkReleaseFullScreenExclusiveModeEXT vkReleaseFullScreenExclusiveModeEXT;
#endif /* defined(VK_EXT_full_screen_exclusive) */
#if defined(VK_EXT_full_screen_exclusive) && (defined(VK_KHR_device_group) || defined(VK_VERSION_1_1))
extern VOLK_API PFN_vkGetDeviceGroupSurfacePresentModes2EXT vkGetDeviceGroupSurfacePresentModes2EXT;
#endif /* defined(VK_EXT_full_screen_exclusive) && (defined(VK_KHR_device_group) || defined(VK_VERSION_1_1)) */
#if defined(VK_EXT_hdr_metadata)
extern VOLK_API PFN_vkSetHdrMetadataEXT vkSetHdrMetadataEXT;
#endif /* defined(VK_EXT_hdr_metadata) */
#if defined(VK_EXT_host_image_copy)
extern VOLK_API PFN_vkCopyImageToImageEXT vkCopyImageToImageEXT;
extern VOLK_API PFN_vkCopyImageToMemoryEXT vkCopyImageToMemoryEXT;
extern VOLK_API PFN_vkCopyMemoryToImageEXT vkCopyMemoryToImageEXT;
extern VOLK_API PFN_vkTransitionImageLayoutEXT vkTransitionImageLayoutEXT;
#endif /* defined(VK_EXT_host_image_copy) */
#if defined(VK_EXT_host_query_reset)
extern VOLK_API PFN_vkResetQueryPoolEXT vkResetQueryPoolEXT;
#endif /* defined(VK_EXT_host_query_reset) */
#if defined(VK_EXT_image_drm_format_modifier)
extern VOLK_API PFN_vkGetImageDrmFormatModifierPropertiesEXT vkGetImageDrmFormatModifierPropertiesEXT;
#endif /* defined(VK_EXT_image_drm_format_modifier) */
#if defined(VK_EXT_line_rasterization)
extern VOLK_API PFN_vkCmdSetLineStippleEXT vkCmdSetLineStippleEXT;
#endif /* defined(VK_EXT_line_rasterization) */
#if defined(VK_EXT_memory_decompression)
extern VOLK_API PFN_vkCmdDecompressMemoryEXT vkCmdDecompressMemoryEXT;
extern VOLK_API PFN_vkCmdDecompressMemoryIndirectCountEXT vkCmdDecompressMemoryIndirectCountEXT;
#endif /* defined(VK_EXT_memory_decompression) */
#if defined(VK_EXT_mesh_shader)
extern VOLK_API PFN_vkCmdDrawMeshTasksEXT vkCmdDrawMeshTasksEXT;
extern VOLK_API PFN_vkCmdDrawMeshTasksIndirectEXT vkCmdDrawMeshTasksIndirectEXT;
#endif /* defined(VK_EXT_mesh_shader) */
#if defined(VK_EXT_mesh_shader) && (defined(VK_VERSION_1_2) || defined(VK_KHR_draw_indirect_count) || defined(VK_AMD_draw_indirect_count))
extern VOLK_API PFN_vkCmdDrawMeshTasksIndirectCountEXT vkCmdDrawMeshTasksIndirectCountEXT;
#endif /* defined(VK_EXT_mesh_shader) && (defined(VK_VERSION_1_2) || defined(VK_KHR_draw_indirect_count) || defined(VK_AMD_draw_indirect_count)) */
#if defined(VK_EXT_metal_objects)
extern VOLK_API PFN_vkExportMetalObjectsEXT vkExportMetalObjectsEXT;
#endif /* defined(VK_EXT_metal_objects) */
#if defined(VK_EXT_multi_draw)
extern VOLK_API PFN_vkCmdDrawMultiEXT vkCmdDrawMultiEXT;
extern VOLK_API PFN_vkCmdDrawMultiIndexedEXT vkCmdDrawMultiIndexedEXT;
#endif /* defined(VK_EXT_multi_draw) */
#if defined(VK_EXT_opacity_micromap)
extern VOLK_API PFN_vkBuildMicromapsEXT vkBuildMicromapsEXT;
extern VOLK_API PFN_vkCmdBuildMicromapsEXT vkCmdBuildMicromapsEXT;
extern VOLK_API PFN_vkCmdCopyMemoryToMicromapEXT vkCmdCopyMemoryToMicromapEXT;
extern VOLK_API PFN_vkCmdCopyMicromapEXT vkCmdCopyMicromapEXT;
extern VOLK_API PFN_vkCmdCopyMicromapToMemoryEXT vkCmdCopyMicromapToMemoryEXT;
extern VOLK_API PFN_vkCmdWriteMicromapsPropertiesEXT vkCmdWriteMicromapsPropertiesEXT;
extern VOLK_API PFN_vkCopyMemoryToMicromapEXT vkCopyMemoryToMicromapEXT;
extern VOLK_API PFN_vkCopyMicromapEXT vkCopyMicromapEXT;
extern VOLK_API PFN_vkCopyMicromapToMemoryEXT vkCopyMicromapToMemoryEXT;
extern VOLK_API PFN_vkCreateMicromapEXT vkCreateMicromapEXT;
extern VOLK_API PFN_vkDestroyMicromapEXT vkDestroyMicromapEXT;
extern VOLK_API PFN_vkGetDeviceMicromapCompatibilityEXT vkGetDeviceMicromapCompatibilityEXT;
extern VOLK_API PFN_vkGetMicromapBuildSizesEXT vkGetMicromapBuildSizesEXT;
extern VOLK_API PFN_vkWriteMicromapsPropertiesEXT vkWriteMicromapsPropertiesEXT;
#endif /* defined(VK_EXT_opacity_micromap) */
#if defined(VK_EXT_pageable_device_local_memory)
extern VOLK_API PFN_vkSetDeviceMemoryPriorityEXT vkSetDeviceMemoryPriorityEXT;
#endif /* defined(VK_EXT_pageable_device_local_memory) */
#if defined(VK_EXT_pipeline_properties)
extern VOLK_API PFN_vkGetPipelinePropertiesEXT vkGetPipelinePropertiesEXT;
#endif /* defined(VK_EXT_pipeline_properties) */
#if defined(VK_EXT_present_timing)
extern PFN_vkGetPastPresentationTimingEXT vkGetPastPresentationTimingEXT;
extern PFN_vkGetSwapchainTimeDomainPropertiesEXT vkGetSwapchainTimeDomainPropertiesEXT;
extern PFN_vkGetSwapchainTimingPropertiesEXT vkGetSwapchainTimingPropertiesEXT;
extern PFN_vkSetSwapchainPresentTimingQueueSizeEXT vkSetSwapchainPresentTimingQueueSizeEXT;
#endif /* defined(VK_EXT_present_timing) */
#if defined(VK_EXT_private_data)
extern VOLK_API PFN_vkCreatePrivateDataSlotEXT vkCreatePrivateDataSlotEXT;
extern VOLK_API PFN_vkDestroyPrivateDataSlotEXT vkDestroyPrivateDataSlotEXT;
extern VOLK_API PFN_vkGetPrivateDataEXT vkGetPrivateDataEXT;
extern VOLK_API PFN_vkSetPrivateDataEXT vkSetPrivateDataEXT;
#endif /* defined(VK_EXT_private_data) */
#if defined(VK_EXT_sample_locations)
extern VOLK_API PFN_vkCmdSetSampleLocationsEXT vkCmdSetSampleLocationsEXT;
#endif /* defined(VK_EXT_sample_locations) */
#if defined(VK_EXT_shader_module_identifier)
extern VOLK_API PFN_vkGetShaderModuleCreateInfoIdentifierEXT vkGetShaderModuleCreateInfoIdentifierEXT;
extern VOLK_API PFN_vkGetShaderModuleIdentifierEXT vkGetShaderModuleIdentifierEXT;
#endif /* defined(VK_EXT_shader_module_identifier) */
#if defined(VK_EXT_shader_object)
extern VOLK_API PFN_vkCmdBindShadersEXT vkCmdBindShadersEXT;
extern VOLK_API PFN_vkCreateShadersEXT vkCreateShadersEXT;
extern VOLK_API PFN_vkDestroyShaderEXT vkDestroyShaderEXT;
extern VOLK_API PFN_vkGetShaderBinaryDataEXT vkGetShaderBinaryDataEXT;
#endif /* defined(VK_EXT_shader_object) */
#if defined(VK_EXT_swapchain_maintenance1)
extern VOLK_API PFN_vkReleaseSwapchainImagesEXT vkReleaseSwapchainImagesEXT;
#endif /* defined(VK_EXT_swapchain_maintenance1) */
#if defined(VK_EXT_transform_feedback)
extern VOLK_API PFN_vkCmdBeginQueryIndexedEXT vkCmdBeginQueryIndexedEXT;
extern VOLK_API PFN_vkCmdBeginTransformFeedbackEXT vkCmdBeginTransformFeedbackEXT;
extern VOLK_API PFN_vkCmdBindTransformFeedbackBuffersEXT vkCmdBindTransformFeedbackBuffersEXT;
extern VOLK_API PFN_vkCmdDrawIndirectByteCountEXT vkCmdDrawIndirectByteCountEXT;
extern VOLK_API PFN_vkCmdEndQueryIndexedEXT vkCmdEndQueryIndexedEXT;
extern VOLK_API PFN_vkCmdEndTransformFeedbackEXT vkCmdEndTransformFeedbackEXT;
#endif /* defined(VK_EXT_transform_feedback) */
#if defined(VK_EXT_validation_cache)
extern VOLK_API PFN_vkCreateValidationCacheEXT vkCreateValidationCacheEXT;
extern VOLK_API PFN_vkDestroyValidationCacheEXT vkDestroyValidationCacheEXT;
extern VOLK_API PFN_vkGetValidationCacheDataEXT vkGetValidationCacheDataEXT;
extern VOLK_API PFN_vkMergeValidationCachesEXT vkMergeValidationCachesEXT;
#endif /* defined(VK_EXT_validation_cache) */
#if defined(VK_FUCHSIA_buffer_collection)
extern VOLK_API PFN_vkCreateBufferCollectionFUCHSIA vkCreateBufferCollectionFUCHSIA;
extern VOLK_API PFN_vkDestroyBufferCollectionFUCHSIA vkDestroyBufferCollectionFUCHSIA;
extern VOLK_API PFN_vkGetBufferCollectionPropertiesFUCHSIA vkGetBufferCollectionPropertiesFUCHSIA;
extern VOLK_API PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA vkSetBufferCollectionBufferConstraintsFUCHSIA;
extern VOLK_API PFN_vkSetBufferCollectionImageConstraintsFUCHSIA vkSetBufferCollectionImageConstraintsFUCHSIA;
#endif /* defined(VK_FUCHSIA_buffer_collection) */
#if defined(VK_FUCHSIA_external_memory)
extern VOLK_API PFN_vkGetMemoryZirconHandleFUCHSIA vkGetMemoryZirconHandleFUCHSIA;
extern VOLK_API PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA vkGetMemoryZirconHandlePropertiesFUCHSIA;
#endif /* defined(VK_FUCHSIA_external_memory) */
#if defined(VK_FUCHSIA_external_semaphore)
extern VOLK_API PFN_vkGetSemaphoreZirconHandleFUCHSIA vkGetSemaphoreZirconHandleFUCHSIA;
extern VOLK_API PFN_vkImportSemaphoreZirconHandleFUCHSIA vkImportSemaphoreZirconHandleFUCHSIA;
#endif /* defined(VK_FUCHSIA_external_semaphore) */
#if defined(VK_GOOGLE_display_timing)
extern VOLK_API PFN_vkGetPastPresentationTimingGOOGLE vkGetPastPresentationTimingGOOGLE;
extern VOLK_API PFN_vkGetRefreshCycleDurationGOOGLE vkGetRefreshCycleDurationGOOGLE;
#endif /* defined(VK_GOOGLE_display_timing) */
#if defined(VK_HUAWEI_cluster_culling_shader)
extern VOLK_API PFN_vkCmdDrawClusterHUAWEI vkCmdDrawClusterHUAWEI;
extern VOLK_API PFN_vkCmdDrawClusterIndirectHUAWEI vkCmdDrawClusterIndirectHUAWEI;
#endif /* defined(VK_HUAWEI_cluster_culling_shader) */
#if defined(VK_HUAWEI_invocation_mask)
extern VOLK_API PFN_vkCmdBindInvocationMaskHUAWEI vkCmdBindInvocationMaskHUAWEI;
#endif /* defined(VK_HUAWEI_invocation_mask) */
#if defined(VK_HUAWEI_subpass_shading) && VK_HUAWEI_SUBPASS_SHADING_SPEC_VERSION >= 2
extern VOLK_API PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI;
#endif /* defined(VK_HUAWEI_subpass_shading) && VK_HUAWEI_SUBPASS_SHADING_SPEC_VERSION >= 2 */
#if defined(VK_HUAWEI_subpass_shading)
extern VOLK_API PFN_vkCmdSubpassShadingHUAWEI vkCmdSubpassShadingHUAWEI;
#endif /* defined(VK_HUAWEI_subpass_shading) */
#if defined(VK_INTEL_performance_query)
extern VOLK_API PFN_vkAcquirePerformanceConfigurationINTEL vkAcquirePerformanceConfigurationINTEL;
extern VOLK_API PFN_vkCmdSetPerformanceMarkerINTEL vkCmdSetPerformanceMarkerINTEL;
extern VOLK_API PFN_vkCmdSetPerformanceOverrideINTEL vkCmdSetPerformanceOverrideINTEL;
extern VOLK_API PFN_vkCmdSetPerformanceStreamMarkerINTEL vkCmdSetPerformanceStreamMarkerINTEL;
extern VOLK_API PFN_vkGetPerformanceParameterINTEL vkGetPerformanceParameterINTEL;
extern VOLK_API PFN_vkInitializePerformanceApiINTEL vkInitializePerformanceApiINTEL;
extern VOLK_API PFN_vkQueueSetPerformanceConfigurationINTEL vkQueueSetPerformanceConfigurationINTEL;
extern VOLK_API PFN_vkReleasePerformanceConfigurationINTEL vkReleasePerformanceConfigurationINTEL;
extern VOLK_API PFN_vkUninitializePerformanceApiINTEL vkUninitializePerformanceApiINTEL;
#endif /* defined(VK_INTEL_performance_query) */
#if defined(VK_KHR_acceleration_structure)
extern VOLK_API PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
extern VOLK_API PFN_vkCmdBuildAccelerationStructuresIndirectKHR vkCmdBuildAccelerationStructuresIndirectKHR;
extern VOLK_API PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
extern VOLK_API PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR;
extern VOLK_API PFN_vkCmdCopyAccelerationStructureToMemoryKHR vkCmdCopyAccelerationStructureToMemoryKHR;
extern VOLK_API PFN_vkCmdCopyMemoryToAccelerationStructureKHR vkCmdCopyMemoryToAccelerationStructureKHR;
extern VOLK_API PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR;
extern VOLK_API PFN_vkCopyAccelerationStructureKHR vkCopyAccelerationStructureKHR;
extern VOLK_API PFN_vkCopyAccelerationStructureToMemoryKHR vkCopyAccelerationStructureToMemoryKHR;
extern VOLK_API PFN_vkCopyMemoryToAccelerationStructureKHR vkCopyMemoryToAccelerationStructureKHR;
extern VOLK_API PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
extern VOLK_API PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
extern VOLK_API PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;
extern VOLK_API PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
extern VOLK_API PFN_vkGetDeviceAccelerationStructureCompatibilityKHR vkGetDeviceAccelerationStructureCompatibilityKHR;
extern VOLK_API PFN_vkWriteAccelerationStructuresPropertiesKHR vkWriteAccelerationStructuresPropertiesKHR;
#endif /* defined(VK_KHR_acceleration_structure) */
#if defined(VK_KHR_bind_memory2)
extern VOLK_API PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR;
extern VOLK_API PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR;
#endif /* defined(VK_KHR_bind_memory2) */
#if defined(VK_KHR_buffer_device_address)
extern VOLK_API PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR;
extern VOLK_API PFN_vkGetBufferOpaqueCaptureAddressKHR vkGetBufferOpaqueCaptureAddressKHR;
extern VOLK_API PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR vkGetDeviceMemoryOpaqueCaptureAddressKHR;
#endif /* defined(VK_KHR_buffer_device_address) */
#if defined(VK_KHR_calibrated_timestamps)
extern VOLK_API PFN_vkGetCalibratedTimestampsKHR vkGetCalibratedTimestampsKHR;
#endif /* defined(VK_KHR_calibrated_timestamps) */
#if defined(VK_KHR_copy_commands2)
extern VOLK_API PFN_vkCmdBlitImage2KHR vkCmdBlitImage2KHR;
extern VOLK_API PFN_vkCmdCopyBuffer2KHR vkCmdCopyBuffer2KHR;
extern VOLK_API PFN_vkCmdCopyBufferToImage2KHR vkCmdCopyBufferToImage2KHR;
extern VOLK_API PFN_vkCmdCopyImage2KHR vkCmdCopyImage2KHR;
extern VOLK_API PFN_vkCmdCopyImageToBuffer2KHR vkCmdCopyImageToBuffer2KHR;
extern VOLK_API PFN_vkCmdResolveImage2KHR vkCmdResolveImage2KHR;
#endif /* defined(VK_KHR_copy_commands2) */
#if defined(VK_KHR_copy_memory_indirect)
extern VOLK_API PFN_vkCmdCopyMemoryIndirectKHR vkCmdCopyMemoryIndirectKHR;
extern VOLK_API PFN_vkCmdCopyMemoryToImageIndirectKHR vkCmdCopyMemoryToImageIndirectKHR;
#endif /* defined(VK_KHR_copy_memory_indirect) */
#if defined(VK_KHR_create_renderpass2)
extern VOLK_API PFN_vkCmdBeginRenderPass2KHR vkCmdBeginRenderPass2KHR;
extern VOLK_API PFN_vkCmdEndRenderPass2KHR vkCmdEndRenderPass2KHR;
extern VOLK_API PFN_vkCmdNextSubpass2KHR vkCmdNextSubpass2KHR;
extern VOLK_API PFN_vkCreateRenderPass2KHR vkCreateRenderPass2KHR;
#endif /* defined(VK_KHR_create_renderpass2) */
#if defined(VK_KHR_deferred_host_operations)
extern VOLK_API PFN_vkCreateDeferredOperationKHR vkCreateDeferredOperationKHR;
extern VOLK_API PFN_vkDeferredOperationJoinKHR vkDeferredOperationJoinKHR;
extern VOLK_API PFN_vkDestroyDeferredOperationKHR vkDestroyDeferredOperationKHR;
extern VOLK_API PFN_vkGetDeferredOperationMaxConcurrencyKHR vkGetDeferredOperationMaxConcurrencyKHR;
extern VOLK_API PFN_vkGetDeferredOperationResultKHR vkGetDeferredOperationResultKHR;
#endif /* defined(VK_KHR_deferred_host_operations) */
#if defined(VK_KHR_descriptor_update_template)
extern VOLK_API PFN_vkCreateDescriptorUpdateTemplateKHR vkCreateDescriptorUpdateTemplateKHR;
extern VOLK_API PFN_vkDestroyDescriptorUpdateTemplateKHR vkDestroyDescriptorUpdateTemplateKHR;
extern VOLK_API PFN_vkUpdateDescriptorSetWithTemplateKHR vkUpdateDescriptorSetWithTemplateKHR;
#endif /* defined(VK_KHR_descriptor_update_template) */
#if defined(VK_KHR_device_group)
extern VOLK_API PFN_vkCmdDispatchBaseKHR vkCmdDispatchBaseKHR;
extern VOLK_API PFN_vkCmdSetDeviceMaskKHR vkCmdSetDeviceMaskKHR;
extern VOLK_API PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR vkGetDeviceGroupPeerMemoryFeaturesKHR;
#endif /* defined(VK_KHR_device_group) */
#if defined(VK_KHR_display_swapchain)
extern VOLK_API PFN_vkCreateSharedSwapchainsKHR vkCreateSharedSwapchainsKHR;
#endif /* defined(VK_KHR_display_swapchain) */
#if defined(VK_KHR_draw_indirect_count)
extern VOLK_API PFN_vkCmdDrawIndexedIndirectCountKHR vkCmdDrawIndexedIndirectCountKHR;
extern VOLK_API PFN_vkCmdDrawIndirectCountKHR vkCmdDrawIndirectCountKHR;
#endif /* defined(VK_KHR_draw_indirect_count) */
#if defined(VK_KHR_dynamic_rendering)
extern VOLK_API PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR;
extern VOLK_API PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR;
#endif /* defined(VK_KHR_dynamic_rendering) */
#if defined(VK_KHR_dynamic_rendering_local_read)
extern VOLK_API PFN_vkCmdSetRenderingAttachmentLocationsKHR vkCmdSetRenderingAttachmentLocationsKHR;
extern VOLK_API PFN_vkCmdSetRenderingInputAttachmentIndicesKHR vkCmdSetRenderingInputAttachmentIndicesKHR;
#endif /* defined(VK_KHR_dynamic_rendering_local_read) */
#if defined(VK_KHR_external_fence_fd)
extern VOLK_API PFN_vkGetFenceFdKHR vkGetFenceFdKHR;
extern VOLK_API PFN_vkImportFenceFdKHR vkImportFenceFdKHR;
#endif /* defined(VK_KHR_external_fence_fd) */
#if defined(VK_KHR_external_fence_win32)
extern VOLK_API PFN_vkGetFenceWin32HandleKHR vkGetFenceWin32HandleKHR;
extern VOLK_API PFN_vkImportFenceWin32HandleKHR vkImportFenceWin32HandleKHR;
#endif /* defined(VK_KHR_external_fence_win32) */
#if defined(VK_KHR_external_memory_fd)
extern VOLK_API PFN_vkGetMemoryFdKHR vkGetMemoryFdKHR;
extern VOLK_API PFN_vkGetMemoryFdPropertiesKHR vkGetMemoryFdPropertiesKHR;
#endif /* defined(VK_KHR_external_memory_fd) */
#if defined(VK_KHR_external_memory_win32)
extern VOLK_API PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR;
extern VOLK_API PFN_vkGetMemoryWin32HandlePropertiesKHR vkGetMemoryWin32HandlePropertiesKHR;
#endif /* defined(VK_KHR_external_memory_win32) */
#if defined(VK_KHR_external_semaphore_fd)
extern VOLK_API PFN_vkGetSemaphoreFdKHR vkGetSemaphoreFdKHR;
extern VOLK_API PFN_vkImportSemaphoreFdKHR vkImportSemaphoreFdKHR;
#endif /* defined(VK_KHR_external_semaphore_fd) */
#if defined(VK_KHR_external_semaphore_win32)
extern VOLK_API PFN_vkGetSemaphoreWin32HandleKHR vkGetSemaphoreWin32HandleKHR;
extern VOLK_API PFN_vkImportSemaphoreWin32HandleKHR vkImportSemaphoreWin32HandleKHR;
#endif /* defined(VK_KHR_external_semaphore_win32) */
#if defined(VK_KHR_fragment_shading_rate)
extern VOLK_API PFN_vkCmdSetFragmentShadingRateKHR vkCmdSetFragmentShadingRateKHR;
#endif /* defined(VK_KHR_fragment_shading_rate) */
#if defined(VK_KHR_get_memory_requirements2)
extern VOLK_API PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR;
extern VOLK_API PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR;
extern VOLK_API PFN_vkGetImageSparseMemoryRequirements2KHR vkGetImageSparseMemoryRequirements2KHR;
#endif /* defined(VK_KHR_get_memory_requirements2) */
#if defined(VK_KHR_line_rasterization)
extern VOLK_API PFN_vkCmdSetLineStippleKHR vkCmdSetLineStippleKHR;
#endif /* defined(VK_KHR_line_rasterization) */
#if defined(VK_KHR_maintenance1)
extern VOLK_API PFN_vkTrimCommandPoolKHR vkTrimCommandPoolKHR;
#endif /* defined(VK_KHR_maintenance1) */
#if defined(VK_KHR_maintenance10)
extern VOLK_API PFN_vkCmdEndRendering2KHR vkCmdEndRendering2KHR;
#endif /* defined(VK_KHR_maintenance10) */
#if defined(VK_KHR_maintenance3)
extern VOLK_API PFN_vkGetDescriptorSetLayoutSupportKHR vkGetDescriptorSetLayoutSupportKHR;
#endif /* defined(VK_KHR_maintenance3) */
#if defined(VK_KHR_maintenance4)
extern VOLK_API PFN_vkGetDeviceBufferMemoryRequirementsKHR vkGetDeviceBufferMemoryRequirementsKHR;
extern VOLK_API PFN_vkGetDeviceImageMemoryRequirementsKHR vkGetDeviceImageMemoryRequirementsKHR;
extern VOLK_API PFN_vkGetDeviceImageSparseMemoryRequirementsKHR vkGetDeviceImageSparseMemoryRequirementsKHR;
#endif /* defined(VK_KHR_maintenance4) */
#if defined(VK_KHR_maintenance5)
extern VOLK_API PFN_vkCmdBindIndexBuffer2KHR vkCmdBindIndexBuffer2KHR;
extern VOLK_API PFN_vkGetDeviceImageSubresourceLayoutKHR vkGetDeviceImageSubresourceLayoutKHR;
extern VOLK_API PFN_vkGetImageSubresourceLayout2KHR vkGetImageSubresourceLayout2KHR;
extern VOLK_API PFN_vkGetRenderingAreaGranularityKHR vkGetRenderingAreaGranularityKHR;
#endif /* defined(VK_KHR_maintenance5) */
#if defined(VK_KHR_maintenance6)
extern VOLK_API PFN_vkCmdBindDescriptorSets2KHR vkCmdBindDescriptorSets2KHR;
extern VOLK_API PFN_vkCmdPushConstants2KHR vkCmdPushConstants2KHR;
#endif /* defined(VK_KHR_maintenance6) */
#if defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor)
extern VOLK_API PFN_vkCmdPushDescriptorSet2KHR vkCmdPushDescriptorSet2KHR;
extern VOLK_API PFN_vkCmdPushDescriptorSetWithTemplate2KHR vkCmdPushDescriptorSetWithTemplate2KHR;
#endif /* defined(VK_KHR_maintenance6) && defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer)
extern VOLK_API PFN_vkCmdBindDescriptorBufferEmbeddedSamplers2EXT vkCmdBindDescriptorBufferEmbeddedSamplers2EXT;
extern VOLK_API PFN_vkCmdSetDescriptorBufferOffsets2EXT vkCmdSetDescriptorBufferOffsets2EXT;
#endif /* defined(VK_KHR_maintenance6) && defined(VK_EXT_descriptor_buffer) */
#if defined(VK_KHR_map_memory2)
extern VOLK_API PFN_vkMapMemory2KHR vkMapMemory2KHR;
extern VOLK_API PFN_vkUnmapMemory2KHR vkUnmapMemory2KHR;
#endif /* defined(VK_KHR_map_memory2) */
#if defined(VK_KHR_performance_query)
extern VOLK_API PFN_vkAcquireProfilingLockKHR vkAcquireProfilingLockKHR;
extern VOLK_API PFN_vkReleaseProfilingLockKHR vkReleaseProfilingLockKHR;
#endif /* defined(VK_KHR_performance_query) */
#if defined(VK_KHR_pipeline_binary)
extern VOLK_API PFN_vkCreatePipelineBinariesKHR vkCreatePipelineBinariesKHR;
extern VOLK_API PFN_vkDestroyPipelineBinaryKHR vkDestroyPipelineBinaryKHR;
extern VOLK_API PFN_vkGetPipelineBinaryDataKHR vkGetPipelineBinaryDataKHR;
extern VOLK_API PFN_vkGetPipelineKeyKHR vkGetPipelineKeyKHR;
extern VOLK_API PFN_vkReleaseCapturedPipelineDataKHR vkReleaseCapturedPipelineDataKHR;
#endif /* defined(VK_KHR_pipeline_binary) */
#if defined(VK_KHR_pipeline_executable_properties)
extern VOLK_API PFN_vkGetPipelineExecutableInternalRepresentationsKHR vkGetPipelineExecutableInternalRepresentationsKHR;
extern VOLK_API PFN_vkGetPipelineExecutablePropertiesKHR vkGetPipelineExecutablePropertiesKHR;
extern VOLK_API PFN_vkGetPipelineExecutableStatisticsKHR vkGetPipelineExecutableStatisticsKHR;
#endif /* defined(VK_KHR_pipeline_executable_properties) */
#if defined(VK_KHR_present_wait)
extern VOLK_API PFN_vkWaitForPresentKHR vkWaitForPresentKHR;
#endif /* defined(VK_KHR_present_wait) */
#if defined(VK_KHR_present_wait2)
extern VOLK_API PFN_vkWaitForPresent2KHR vkWaitForPresent2KHR;
#endif /* defined(VK_KHR_present_wait2) */
#if defined(VK_KHR_push_descriptor)
extern VOLK_API PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR;
#endif /* defined(VK_KHR_push_descriptor) */
#if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
extern VOLK_API PFN_vkCmdTraceRaysIndirect2KHR vkCmdTraceRaysIndirect2KHR;
#endif /* defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_ray_tracing_pipeline)
extern VOLK_API PFN_vkCmdSetRayTracingPipelineStackSizeKHR vkCmdSetRayTracingPipelineStackSizeKHR;
extern VOLK_API PFN_vkCmdTraceRaysIndirectKHR vkCmdTraceRaysIndirectKHR;
extern VOLK_API PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
extern VOLK_API PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;
extern VOLK_API PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR vkGetRayTracingCaptureReplayShaderGroupHandlesKHR;
extern VOLK_API PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
extern VOLK_API PFN_vkGetRayTracingShaderGroupStackSizeKHR vkGetRayTracingShaderGroupStackSizeKHR;
#endif /* defined(VK_KHR_ray_tracing_pipeline) */
#if defined(VK_KHR_sampler_ycbcr_conversion)
extern VOLK_API PFN_vkCreateSamplerYcbcrConversionKHR vkCreateSamplerYcbcrConversionKHR;
extern VOLK_API PFN_vkDestroySamplerYcbcrConversionKHR vkDestroySamplerYcbcrConversionKHR;
#endif /* defined(VK_KHR_sampler_ycbcr_conversion) */
#if defined(VK_KHR_shared_presentable_image)
extern VOLK_API PFN_vkGetSwapchainStatusKHR vkGetSwapchainStatusKHR;
#endif /* defined(VK_KHR_shared_presentable_image) */
#if defined(VK_KHR_swapchain)
extern VOLK_API PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
extern VOLK_API PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
extern VOLK_API PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
extern VOLK_API PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
extern VOLK_API PFN_vkQueuePresentKHR vkQueuePresentKHR;
#endif /* defined(VK_KHR_swapchain) */
#if defined(VK_KHR_swapchain_maintenance1)
extern VOLK_API PFN_vkReleaseSwapchainImagesKHR vkReleaseSwapchainImagesKHR;
#endif /* defined(VK_KHR_swapchain_maintenance1) */
#if defined(VK_KHR_synchronization2)
extern VOLK_API PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR;
extern VOLK_API PFN_vkCmdResetEvent2KHR vkCmdResetEvent2KHR;
extern VOLK_API PFN_vkCmdSetEvent2KHR vkCmdSetEvent2KHR;
extern VOLK_API PFN_vkCmdWaitEvents2KHR vkCmdWaitEvents2KHR;
extern VOLK_API PFN_vkCmdWriteTimestamp2KHR vkCmdWriteTimestamp2KHR;
extern VOLK_API PFN_vkQueueSubmit2KHR vkQueueSubmit2KHR;
#endif /* defined(VK_KHR_synchronization2) */
#if defined(VK_KHR_timeline_semaphore)
extern VOLK_API PFN_vkGetSemaphoreCounterValueKHR vkGetSemaphoreCounterValueKHR;
extern VOLK_API PFN_vkSignalSemaphoreKHR vkSignalSemaphoreKHR;
extern VOLK_API PFN_vkWaitSemaphoresKHR vkWaitSemaphoresKHR;
#endif /* defined(VK_KHR_timeline_semaphore) */
#if defined(VK_KHR_video_decode_queue)
extern VOLK_API PFN_vkCmdDecodeVideoKHR vkCmdDecodeVideoKHR;
#endif /* defined(VK_KHR_video_decode_queue) */
#if defined(VK_KHR_video_encode_queue)
extern VOLK_API PFN_vkCmdEncodeVideoKHR vkCmdEncodeVideoKHR;
extern VOLK_API PFN_vkGetEncodedVideoSessionParametersKHR vkGetEncodedVideoSessionParametersKHR;
#endif /* defined(VK_KHR_video_encode_queue) */
#if defined(VK_KHR_video_queue)
extern VOLK_API PFN_vkBindVideoSessionMemoryKHR vkBindVideoSessionMemoryKHR;
extern VOLK_API PFN_vkCmdBeginVideoCodingKHR vkCmdBeginVideoCodingKHR;
extern VOLK_API PFN_vkCmdControlVideoCodingKHR vkCmdControlVideoCodingKHR;
extern VOLK_API PFN_vkCmdEndVideoCodingKHR vkCmdEndVideoCodingKHR;
extern VOLK_API PFN_vkCreateVideoSessionKHR vkCreateVideoSessionKHR;
extern VOLK_API PFN_vkCreateVideoSessionParametersKHR vkCreateVideoSessionParametersKHR;
extern VOLK_API PFN_vkDestroyVideoSessionKHR vkDestroyVideoSessionKHR;
extern VOLK_API PFN_vkDestroyVideoSessionParametersKHR vkDestroyVideoSessionParametersKHR;
extern VOLK_API PFN_vkGetVideoSessionMemoryRequirementsKHR vkGetVideoSessionMemoryRequirementsKHR;
extern VOLK_API PFN_vkUpdateVideoSessionParametersKHR vkUpdateVideoSessionParametersKHR;
#endif /* defined(VK_KHR_video_queue) */
#if defined(VK_NVX_binary_import)
extern VOLK_API PFN_vkCmdCuLaunchKernelNVX vkCmdCuLaunchKernelNVX;
extern VOLK_API PFN_vkCreateCuFunctionNVX vkCreateCuFunctionNVX;
extern VOLK_API PFN_vkCreateCuModuleNVX vkCreateCuModuleNVX;
extern VOLK_API PFN_vkDestroyCuFunctionNVX vkDestroyCuFunctionNVX;
extern VOLK_API PFN_vkDestroyCuModuleNVX vkDestroyCuModuleNVX;
#endif /* defined(VK_NVX_binary_import) */
#if defined(VK_NVX_image_view_handle)
extern VOLK_API PFN_vkGetImageViewHandleNVX vkGetImageViewHandleNVX;
#endif /* defined(VK_NVX_image_view_handle) */
#if defined(VK_NVX_image_view_handle) && VK_NVX_IMAGE_VIEW_HANDLE_SPEC_VERSION >= 3
extern VOLK_API PFN_vkGetImageViewHandle64NVX vkGetImageViewHandle64NVX;
#endif /* defined(VK_NVX_image_view_handle) && VK_NVX_IMAGE_VIEW_HANDLE_SPEC_VERSION >= 3 */
#if defined(VK_NVX_image_view_handle) && VK_NVX_IMAGE_VIEW_HANDLE_SPEC_VERSION >= 2
extern VOLK_API PFN_vkGetImageViewAddressNVX vkGetImageViewAddressNVX;
#endif /* defined(VK_NVX_image_view_handle) && VK_NVX_IMAGE_VIEW_HANDLE_SPEC_VERSION >= 2 */
#if defined(VK_NV_clip_space_w_scaling)
extern VOLK_API PFN_vkCmdSetViewportWScalingNV vkCmdSetViewportWScalingNV;
#endif /* defined(VK_NV_clip_space_w_scaling) */
#if defined(VK_NV_cluster_acceleration_structure)
extern VOLK_API PFN_vkCmdBuildClusterAccelerationStructureIndirectNV vkCmdBuildClusterAccelerationStructureIndirectNV;
extern VOLK_API PFN_vkGetClusterAccelerationStructureBuildSizesNV vkGetClusterAccelerationStructureBuildSizesNV;
#endif /* defined(VK_NV_cluster_acceleration_structure) */
#if defined(VK_NV_compute_occupancy_priority)
extern PFN_vkCmdSetComputeOccupancyPriorityNV vkCmdSetComputeOccupancyPriorityNV;
#endif /* defined(VK_NV_compute_occupancy_priority) */
#if defined(VK_NV_cooperative_vector)
extern VOLK_API PFN_vkCmdConvertCooperativeVectorMatrixNV vkCmdConvertCooperativeVectorMatrixNV;
extern VOLK_API PFN_vkConvertCooperativeVectorMatrixNV vkConvertCooperativeVectorMatrixNV;
#endif /* defined(VK_NV_cooperative_vector) */
#if defined(VK_NV_copy_memory_indirect)
extern VOLK_API PFN_vkCmdCopyMemoryIndirectNV vkCmdCopyMemoryIndirectNV;
extern VOLK_API PFN_vkCmdCopyMemoryToImageIndirectNV vkCmdCopyMemoryToImageIndirectNV;
#endif /* defined(VK_NV_copy_memory_indirect) */
#if defined(VK_NV_cuda_kernel_launch)
extern VOLK_API PFN_vkCmdCudaLaunchKernelNV vkCmdCudaLaunchKernelNV;
extern VOLK_API PFN_vkCreateCudaFunctionNV vkCreateCudaFunctionNV;
extern VOLK_API PFN_vkCreateCudaModuleNV vkCreateCudaModuleNV;
extern VOLK_API PFN_vkDestroyCudaFunctionNV vkDestroyCudaFunctionNV;
extern VOLK_API PFN_vkDestroyCudaModuleNV vkDestroyCudaModuleNV;
extern VOLK_API PFN_vkGetCudaModuleCacheNV vkGetCudaModuleCacheNV;
#endif /* defined(VK_NV_cuda_kernel_launch) */
#if defined(VK_NV_device_diagnostic_checkpoints)
extern VOLK_API PFN_vkCmdSetCheckpointNV vkCmdSetCheckpointNV;
extern VOLK_API PFN_vkGetQueueCheckpointDataNV vkGetQueueCheckpointDataNV;
#endif /* defined(VK_NV_device_diagnostic_checkpoints) */
#if defined(VK_NV_device_diagnostic_checkpoints) && (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2))
extern VOLK_API PFN_vkGetQueueCheckpointData2NV vkGetQueueCheckpointData2NV;
#endif /* defined(VK_NV_device_diagnostic_checkpoints) && (defined(VK_VERSION_1_3) || defined(VK_KHR_synchronization2)) */
#if defined(VK_NV_device_generated_commands)
extern VOLK_API PFN_vkCmdBindPipelineShaderGroupNV vkCmdBindPipelineShaderGroupNV;
extern VOLK_API PFN_vkCmdExecuteGeneratedCommandsNV vkCmdExecuteGeneratedCommandsNV;
extern VOLK_API PFN_vkCmdPreprocessGeneratedCommandsNV vkCmdPreprocessGeneratedCommandsNV;
extern VOLK_API PFN_vkCreateIndirectCommandsLayoutNV vkCreateIndirectCommandsLayoutNV;
extern VOLK_API PFN_vkDestroyIndirectCommandsLayoutNV vkDestroyIndirectCommandsLayoutNV;
extern VOLK_API PFN_vkGetGeneratedCommandsMemoryRequirementsNV vkGetGeneratedCommandsMemoryRequirementsNV;
#endif /* defined(VK_NV_device_generated_commands) */
#if defined(VK_NV_device_generated_commands_compute)
extern VOLK_API PFN_vkCmdUpdatePipelineIndirectBufferNV vkCmdUpdatePipelineIndirectBufferNV;
extern VOLK_API PFN_vkGetPipelineIndirectDeviceAddressNV vkGetPipelineIndirectDeviceAddressNV;
extern VOLK_API PFN_vkGetPipelineIndirectMemoryRequirementsNV vkGetPipelineIndirectMemoryRequirementsNV;
#endif /* defined(VK_NV_device_generated_commands_compute) */
#if defined(VK_NV_external_compute_queue)
extern VOLK_API PFN_vkCreateExternalComputeQueueNV vkCreateExternalComputeQueueNV;
extern VOLK_API PFN_vkDestroyExternalComputeQueueNV vkDestroyExternalComputeQueueNV;
extern VOLK_API PFN_vkGetExternalComputeQueueDataNV vkGetExternalComputeQueueDataNV;
#endif /* defined(VK_NV_external_compute_queue) */
#if defined(VK_NV_external_memory_rdma)
extern VOLK_API PFN_vkGetMemoryRemoteAddressNV vkGetMemoryRemoteAddressNV;
#endif /* defined(VK_NV_external_memory_rdma) */
#if defined(VK_NV_external_memory_win32)
extern VOLK_API PFN_vkGetMemoryWin32HandleNV vkGetMemoryWin32HandleNV;
#endif /* defined(VK_NV_external_memory_win32) */
#if defined(VK_NV_fragment_shading_rate_enums)
extern VOLK_API PFN_vkCmdSetFragmentShadingRateEnumNV vkCmdSetFragmentShadingRateEnumNV;
#endif /* defined(VK_NV_fragment_shading_rate_enums) */
#if defined(VK_NV_low_latency2)
extern VOLK_API PFN_vkGetLatencyTimingsNV vkGetLatencyTimingsNV;
extern VOLK_API PFN_vkLatencySleepNV vkLatencySleepNV;
extern VOLK_API PFN_vkQueueNotifyOutOfBandNV vkQueueNotifyOutOfBandNV;
extern VOLK_API PFN_vkSetLatencyMarkerNV vkSetLatencyMarkerNV;
extern VOLK_API PFN_vkSetLatencySleepModeNV vkSetLatencySleepModeNV;
#endif /* defined(VK_NV_low_latency2) */
#if defined(VK_NV_memory_decompression)
extern VOLK_API PFN_vkCmdDecompressMemoryIndirectCountNV vkCmdDecompressMemoryIndirectCountNV;
extern VOLK_API PFN_vkCmdDecompressMemoryNV vkCmdDecompressMemoryNV;
#endif /* defined(VK_NV_memory_decompression) */
#if defined(VK_NV_mesh_shader)
extern VOLK_API PFN_vkCmdDrawMeshTasksIndirectNV vkCmdDrawMeshTasksIndirectNV;
extern VOLK_API PFN_vkCmdDrawMeshTasksNV vkCmdDrawMeshTasksNV;
#endif /* defined(VK_NV_mesh_shader) */
#if defined(VK_NV_mesh_shader) && (defined(VK_VERSION_1_2) || defined(VK_KHR_draw_indirect_count) || defined(VK_AMD_draw_indirect_count))
extern VOLK_API PFN_vkCmdDrawMeshTasksIndirectCountNV vkCmdDrawMeshTasksIndirectCountNV;
#endif /* defined(VK_NV_mesh_shader) && (defined(VK_VERSION_1_2) || defined(VK_KHR_draw_indirect_count) || defined(VK_AMD_draw_indirect_count)) */
#if defined(VK_NV_optical_flow)
extern VOLK_API PFN_vkBindOpticalFlowSessionImageNV vkBindOpticalFlowSessionImageNV;
extern VOLK_API PFN_vkCmdOpticalFlowExecuteNV vkCmdOpticalFlowExecuteNV;
extern VOLK_API PFN_vkCreateOpticalFlowSessionNV vkCreateOpticalFlowSessionNV;
extern VOLK_API PFN_vkDestroyOpticalFlowSessionNV vkDestroyOpticalFlowSessionNV;
#endif /* defined(VK_NV_optical_flow) */
#if defined(VK_NV_partitioned_acceleration_structure)
extern VOLK_API PFN_vkCmdBuildPartitionedAccelerationStructuresNV vkCmdBuildPartitionedAccelerationStructuresNV;
extern VOLK_API PFN_vkGetPartitionedAccelerationStructuresBuildSizesNV vkGetPartitionedAccelerationStructuresBuildSizesNV;
#endif /* defined(VK_NV_partitioned_acceleration_structure) */
#if defined(VK_NV_ray_tracing)
extern VOLK_API PFN_vkBindAccelerationStructureMemoryNV vkBindAccelerationStructureMemoryNV;
extern VOLK_API PFN_vkCmdBuildAccelerationStructureNV vkCmdBuildAccelerationStructureNV;
extern VOLK_API PFN_vkCmdCopyAccelerationStructureNV vkCmdCopyAccelerationStructureNV;
extern VOLK_API PFN_vkCmdTraceRaysNV vkCmdTraceRaysNV;
extern VOLK_API PFN_vkCmdWriteAccelerationStructuresPropertiesNV vkCmdWriteAccelerationStructuresPropertiesNV;
extern VOLK_API PFN_vkCompileDeferredNV vkCompileDeferredNV;
extern VOLK_API PFN_vkCreateAccelerationStructureNV vkCreateAccelerationStructureNV;
extern VOLK_API PFN_vkCreateRayTracingPipelinesNV vkCreateRayTracingPipelinesNV;
extern VOLK_API PFN_vkDestroyAccelerationStructureNV vkDestroyAccelerationStructureNV;
extern VOLK_API PFN_vkGetAccelerationStructureHandleNV vkGetAccelerationStructureHandleNV;
extern VOLK_API PFN_vkGetAccelerationStructureMemoryRequirementsNV vkGetAccelerationStructureMemoryRequirementsNV;
extern VOLK_API PFN_vkGetRayTracingShaderGroupHandlesNV vkGetRayTracingShaderGroupHandlesNV;
#endif /* defined(VK_NV_ray_tracing) */
#if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
extern VOLK_API PFN_vkCmdSetExclusiveScissorEnableNV vkCmdSetExclusiveScissorEnableNV;
#endif /* defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2 */
#if defined(VK_NV_scissor_exclusive)
extern VOLK_API PFN_vkCmdSetExclusiveScissorNV vkCmdSetExclusiveScissorNV;
#endif /* defined(VK_NV_scissor_exclusive) */
#if defined(VK_NV_shading_rate_image)
extern VOLK_API PFN_vkCmdBindShadingRateImageNV vkCmdBindShadingRateImageNV;
extern VOLK_API PFN_vkCmdSetCoarseSampleOrderNV vkCmdSetCoarseSampleOrderNV;
extern VOLK_API PFN_vkCmdSetViewportShadingRatePaletteNV vkCmdSetViewportShadingRatePaletteNV;
#endif /* defined(VK_NV_shading_rate_image) */
#if defined(VK_OHOS_external_memory)
extern VOLK_API PFN_vkGetMemoryNativeBufferOHOS vkGetMemoryNativeBufferOHOS;
extern VOLK_API PFN_vkGetNativeBufferPropertiesOHOS vkGetNativeBufferPropertiesOHOS;
#endif /* defined(VK_OHOS_external_memory) */
#if defined(VK_OHOS_native_buffer)
extern VOLK_API PFN_vkAcquireImageOHOS vkAcquireImageOHOS;
extern VOLK_API PFN_vkGetSwapchainGrallocUsageOHOS vkGetSwapchainGrallocUsageOHOS;
extern VOLK_API PFN_vkQueueSignalReleaseImageOHOS vkQueueSignalReleaseImageOHOS;
#endif /* defined(VK_OHOS_native_buffer) */
#if defined(VK_QCOM_tile_memory_heap)
extern VOLK_API PFN_vkCmdBindTileMemoryQCOM vkCmdBindTileMemoryQCOM;
#endif /* defined(VK_QCOM_tile_memory_heap) */
#if defined(VK_QCOM_tile_properties)
extern VOLK_API PFN_vkGetDynamicRenderingTilePropertiesQCOM vkGetDynamicRenderingTilePropertiesQCOM;
extern VOLK_API PFN_vkGetFramebufferTilePropertiesQCOM vkGetFramebufferTilePropertiesQCOM;
#endif /* defined(VK_QCOM_tile_properties) */
#if defined(VK_QCOM_tile_shading)
extern VOLK_API PFN_vkCmdBeginPerTileExecutionQCOM vkCmdBeginPerTileExecutionQCOM;
extern VOLK_API PFN_vkCmdDispatchTileQCOM vkCmdDispatchTileQCOM;
extern VOLK_API PFN_vkCmdEndPerTileExecutionQCOM vkCmdEndPerTileExecutionQCOM;
#endif /* defined(VK_QCOM_tile_shading) */
#if defined(VK_QNX_external_memory_screen_buffer)
extern VOLK_API PFN_vkGetScreenBufferPropertiesQNX vkGetScreenBufferPropertiesQNX;
#endif /* defined(VK_QNX_external_memory_screen_buffer) */
#if defined(VK_VALVE_descriptor_set_host_mapping)
extern VOLK_API PFN_vkGetDescriptorSetHostMappingVALVE vkGetDescriptorSetHostMappingVALVE;
extern VOLK_API PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE vkGetDescriptorSetLayoutHostMappingInfoVALVE;
#endif /* defined(VK_VALVE_descriptor_set_host_mapping) */
#if (defined(VK_EXT_depth_clamp_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clamp_control))
extern VOLK_API PFN_vkCmdSetDepthClampRangeEXT vkCmdSetDepthClampRangeEXT;
#endif /* (defined(VK_EXT_depth_clamp_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clamp_control)) */
#if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
extern VOLK_API PFN_vkCmdBindVertexBuffers2EXT vkCmdBindVertexBuffers2EXT;
extern VOLK_API PFN_vkCmdSetCullModeEXT vkCmdSetCullModeEXT;
extern VOLK_API PFN_vkCmdSetDepthBoundsTestEnableEXT vkCmdSetDepthBoundsTestEnableEXT;
extern VOLK_API PFN_vkCmdSetDepthCompareOpEXT vkCmdSetDepthCompareOpEXT;
extern VOLK_API PFN_vkCmdSetDepthTestEnableEXT vkCmdSetDepthTestEnableEXT;
extern VOLK_API PFN_vkCmdSetDepthWriteEnableEXT vkCmdSetDepthWriteEnableEXT;
extern VOLK_API PFN_vkCmdSetFrontFaceEXT vkCmdSetFrontFaceEXT;
extern VOLK_API PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT;
extern VOLK_API PFN_vkCmdSetScissorWithCountEXT vkCmdSetScissorWithCountEXT;
extern VOLK_API PFN_vkCmdSetStencilOpEXT vkCmdSetStencilOpEXT;
extern VOLK_API PFN_vkCmdSetStencilTestEnableEXT vkCmdSetStencilTestEnableEXT;
extern VOLK_API PFN_vkCmdSetViewportWithCountEXT vkCmdSetViewportWithCountEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
extern VOLK_API PFN_vkCmdSetDepthBiasEnableEXT vkCmdSetDepthBiasEnableEXT;
extern VOLK_API PFN_vkCmdSetLogicOpEXT vkCmdSetLogicOpEXT;
extern VOLK_API PFN_vkCmdSetPatchControlPointsEXT vkCmdSetPatchControlPointsEXT;
extern VOLK_API PFN_vkCmdSetPrimitiveRestartEnableEXT vkCmdSetPrimitiveRestartEnableEXT;
extern VOLK_API PFN_vkCmdSetRasterizerDiscardEnableEXT vkCmdSetRasterizerDiscardEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
extern VOLK_API PFN_vkCmdSetAlphaToCoverageEnableEXT vkCmdSetAlphaToCoverageEnableEXT;
extern VOLK_API PFN_vkCmdSetAlphaToOneEnableEXT vkCmdSetAlphaToOneEnableEXT;
extern VOLK_API PFN_vkCmdSetColorBlendEnableEXT vkCmdSetColorBlendEnableEXT;
extern VOLK_API PFN_vkCmdSetColorBlendEquationEXT vkCmdSetColorBlendEquationEXT;
extern VOLK_API PFN_vkCmdSetColorWriteMaskEXT vkCmdSetColorWriteMaskEXT;
extern VOLK_API PFN_vkCmdSetDepthClampEnableEXT vkCmdSetDepthClampEnableEXT;
extern VOLK_API PFN_vkCmdSetLogicOpEnableEXT vkCmdSetLogicOpEnableEXT;
extern VOLK_API PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT;
extern VOLK_API PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT;
extern VOLK_API PFN_vkCmdSetSampleMaskEXT vkCmdSetSampleMaskEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object))
extern VOLK_API PFN_vkCmdSetTessellationDomainOriginEXT vkCmdSetTessellationDomainOriginEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && (defined(VK_KHR_maintenance2) || defined(VK_VERSION_1_1))) || (defined(VK_EXT_shader_object)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback))
extern VOLK_API PFN_vkCmdSetRasterizationStreamEXT vkCmdSetRasterizationStreamEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_transform_feedback)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_transform_feedback)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization))
extern VOLK_API PFN_vkCmdSetConservativeRasterizationModeEXT vkCmdSetConservativeRasterizationModeEXT;
extern VOLK_API PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT vkCmdSetExtraPrimitiveOverestimationSizeEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_conservative_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_conservative_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable))
extern VOLK_API PFN_vkCmdSetDepthClipEnableEXT vkCmdSetDepthClipEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_enable)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_enable)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations))
extern VOLK_API PFN_vkCmdSetSampleLocationsEnableEXT vkCmdSetSampleLocationsEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_sample_locations)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_sample_locations)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced))
extern VOLK_API PFN_vkCmdSetColorBlendAdvancedEXT vkCmdSetColorBlendAdvancedEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_blend_operation_advanced)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_blend_operation_advanced)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex))
extern VOLK_API PFN_vkCmdSetProvokingVertexModeEXT vkCmdSetProvokingVertexModeEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_provoking_vertex)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_provoking_vertex)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization))
extern VOLK_API PFN_vkCmdSetLineRasterizationModeEXT vkCmdSetLineRasterizationModeEXT;
extern VOLK_API PFN_vkCmdSetLineStippleEnableEXT vkCmdSetLineStippleEnableEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_line_rasterization)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_line_rasterization)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control))
extern VOLK_API PFN_vkCmdSetDepthClipNegativeOneToOneEXT vkCmdSetDepthClipNegativeOneToOneEXT;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_EXT_depth_clip_control)) || (defined(VK_EXT_shader_object) && defined(VK_EXT_depth_clip_control)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
extern VOLK_API PFN_vkCmdSetViewportWScalingEnableNV vkCmdSetViewportWScalingEnableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
extern VOLK_API PFN_vkCmdSetViewportSwizzleNV vkCmdSetViewportSwizzleNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
extern VOLK_API PFN_vkCmdSetCoverageToColorEnableNV vkCmdSetCoverageToColorEnableNV;
extern VOLK_API PFN_vkCmdSetCoverageToColorLocationNV vkCmdSetCoverageToColorLocationNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
extern VOLK_API PFN_vkCmdSetCoverageModulationModeNV vkCmdSetCoverageModulationModeNV;
extern VOLK_API PFN_vkCmdSetCoverageModulationTableEnableNV vkCmdSetCoverageModulationTableEnableNV;
extern VOLK_API PFN_vkCmdSetCoverageModulationTableNV vkCmdSetCoverageModulationTableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
extern VOLK_API PFN_vkCmdSetShadingRateImageEnableNV vkCmdSetShadingRateImageEnableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
extern VOLK_API PFN_vkCmdSetRepresentativeFragmentTestEnableNV vkCmdSetRepresentativeFragmentTestEnableNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test)) */
#if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
extern VOLK_API PFN_vkCmdSetCoverageReductionModeNV vkCmdSetCoverageReductionModeNV;
#endif /* (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode)) */
#if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
extern VOLK_API PFN_vkGetImageSubresourceLayout2EXT vkGetImageSubresourceLayout2EXT;
#endif /* (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control)) */
#if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
extern VOLK_API PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT;
#endif /* (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state)) */
#if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && (defined(VK_VERSION_1_1) || defined(VK_KHR_descriptor_update_template)))
extern VOLK_API PFN_vkCmdPushDescriptorSetWithTemplateKHR vkCmdPushDescriptorSetWithTemplateKHR;
#endif /* (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && (defined(VK_VERSION_1_1) || defined(VK_KHR_descriptor_update_template))) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
extern VOLK_API PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR;
extern VOLK_API PFN_vkGetDeviceGroupSurfacePresentModesKHR vkGetDeviceGroupSurfacePresentModesKHR;
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
#if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
extern VOLK_API PFN_vkAcquireNextImage2KHR vkAcquireNextImage2KHR;
#endif /* (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1)) */
/* VOLK_GENERATE_PROTOTYPES_H_DEVICE */
#endif

#ifdef __cplusplus
} // extern "C" / namespace volk
#endif

#ifdef VOLK_NAMESPACE
using namespace volk;
#endif

#endif // VOLK_H

#ifdef VOLK_IMPLEMENTATION
#undef VOLK_IMPLEMENTATION
/* Prevent tools like dependency checkers from detecting a cyclic dependency */
#define VOLK_SOURCE "volk.c"
#include VOLK_SOURCE
#endif

/**
 * Copyright (c) 2018-2025 Arseny Kapoulkine
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
/* clang-format on */
