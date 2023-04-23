#ifndef LIBSCRATCHCPP_GLOBAL_H
#define LIBSCRATCHCPP_GLOBAL_H

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define DECL_EXPORT __declspec(dllexport)
#define DECL_IMPORT __declspec(dllimport)
#else
#define DECL_EXPORT __attribute__((visibility("default")))
#define DECL_IMPORT __attribute__((visibility("default")))
#endif

#if defined(LIBSCRATCHCPP_LIBRARY)
#define LIBSCRATCHCPP_EXPORT DECL_EXPORT
#else
#define LIBSCRATCHCPP_EXPORT DECL_IMPORT
#endif

#endif // LIBSCRATCHCPP_GLOBAL_H
