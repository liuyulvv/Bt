#pragma once

/**
 * @file BPlatform.h
 * @author liuyulvv (liuyulvv@outlook.com)
 * @date 2023-04-28
 */

#if defined(_WIN32)

#if !defined(UNICODE)
#define UNICODE
#endif  // UNICODE
#include <Windows.h>

using BCanvasID = HWND;
static const wchar_t* B_CLASS_NAME{L"BWindows"};

#endif