// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef LIBSCRATCHCPP_TEST
#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define LIBSCRATCHCPP_TEST_EXPORT __declspec(dllexport)
#else
#define LIBSCRATCHCPP_TEST_EXPORT __attribute__((visibility("default")))
#endif
#else
#define LIBSCRATCHCPP_TEST_EXPORT
#endif
