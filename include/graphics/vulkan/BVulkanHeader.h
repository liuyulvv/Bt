#pragma once

/**
 * @file BVulkanHeader.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#if defined(_WIN32)

#if !defined(UNICODE)
#define UNICODE
#endif  // UNICODE

#if !defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_USE_PLATFORM_WIN32_KHR
#endif  // VK_USE_PLATFORM_WIN32_KHR

#endif

#include "vulkan/vulkan.hpp"

#if !defined(GLM_FORCE_SILENT_WARNINGS)
#define GLM_FORCE_SILENT_WARNINGS
#endif  // GLM_FORCE_SILENT_WARNINGS

#if !defined(GLM_FORCE_RADIANS)
#define GLM_FORCE_RADIANS
#endif  // GLM_FORCE_RADIANS

#if !defined(GLM_DEPTH_ZERO_TO_ONE)
#define GLM_DEPTH_ZERO_TO_ONE
#endif  // GLM_DEPTH_ZERO_TO_ONE

#if !defined(GLM_ENABLE_EXPERIMENTAL)
#define GLM_ENABLE_EXPERIMENTAL
#endif  // GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/glm/gtx/hash.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"