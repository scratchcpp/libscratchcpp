// SPDX-License-Identifier: Apache-2.0

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

#include <string>

/*! \brief The main namespace of the library. */
namespace libscratchcpp
{

class Compiler;
class CompilerValue;
class Block;
class Value;

/*!
 * \typedef BlockComp
 *
 * BlockComp is a function pointer for functions which are used to compile blocks.
 */
using BlockComp = CompilerValue *(*)(Compiler *);

/*!
 * \typedef MonitorNameFunc
 *
 * MonitorNameFunc is a function pointer for functions which are used to get monitor names.
 */
using MonitorNameFunc = const std::string &(*)(Block *);

/*!
 * \typedef MonitorChangeFunc
 *
 * MonitorChangeFunc is a function pointer for functions which are used to change monitor values.
 */
using MonitorChangeFunc = void (*)(Block *, const Value &newValue);

/*!
 * \typedef HatPredicateCompileFunc
 *
 * HatPredicateCompileFunc is a function pointer for functions which are used to compile edge-activated hat predicates.
 */
using HatPredicateCompileFunc = CompilerValue *(*)(Compiler *vm);

} // namespace libscratchcpp

#endif // LIBSCRATCHCPP_GLOBAL_H
