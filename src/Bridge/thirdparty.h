#pragma once

// 第三方库统一包含
#ifdef ENABLE_SDL
#include <SDL3/SDL.h>
#endif

#ifdef ENABLE_VULKAN
#include <vulkan/vulkan.hpp>
#endif

// MuJoCo Physics
#include <mujoco/mujoco.h>
