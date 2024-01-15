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

enum class ScratchVersion
{
    Invalid = 0, /*!< An unsupported version. */
    Scratch3 = 3 /*!< Scratch 3.0 */
};

class VirtualMachine;
class Compiler;
class Block;
class Value;

/*!
 * \typedef BlockFunc
 *
 * BlockFunc is a function pointer for block implementation functions.
 */
using BlockFunc = unsigned int (*)(VirtualMachine *vm);

/*!
 * \typedef BlockComp
 *
 * BlockComp is a function pointer for functions which are used to compile blocks to bytecode.
 */
using BlockComp = void (*)(Compiler *);

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

} // namespace libscratchcpp

#endif // LIBSCRATCHCPP_GLOBAL_H
