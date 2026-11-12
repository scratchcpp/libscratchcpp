// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_map>

#include "cpptypeinfo.h"

namespace libscratchcpp
{

class CppIdentifier
{
    public:
        enum class Type
        {
            Double,
            Bool,
            StringPtr,
            Pointer,
            Value,
            ValueData,
            ContextPointer,
            TargetPointer,
            VariablePointerArray,
            ListPointerArray,
            StringPtrArray
        };

        CppIdentifier() { }

        CppIdentifier(Type type) :
            type(type)
        {
        }

        inline bool isPointer() const { return m_typeInfo.at(type).isPointer; }
        inline bool isMemberAccessible() const { return m_typeInfo.at(type).isMemberAccessible; }
        inline bool isArray() const { return m_typeInfo.at(type).isArray; }

        inline const std::string &typeString() const { return m_typeInfo.at(type).name; }
        static inline const std::string &typeString(CppIdentifier::Type type) { return m_typeInfo.at(type).name; }

        virtual bool isConst() const { return false; }
        virtual bool isNamed() const { return false; }

        const Type type = Type::Pointer;

    private:
        static inline const std::unordered_map<Type, CppTypeInfo> m_typeInfo = {
            { Type::Double, { "double", false, false, false } },
            { Type::Bool, { "bool", false, false, false } },
            { Type::StringPtr, { "StringPtr *", true, true, false } },
            { Type::Pointer, { "void *", false, true, false } },
            { Type::Value, { "Value", true, false, false } },
            { Type::ValueData, { "ValueData", true, false, false } },
            { Type::ContextPointer, { "ExecutionContext *", true, true, false } },
            { Type::TargetPointer, { "Target *", true, true, false } },
            { Type::VariablePointerArray, { "Variable **", false, true, true } },
            { Type::ListPointerArray, { "List **", false, true, true } },
            { Type::StringPtrArray, { "StringPtr **", false, true, true } }
        };
};

} // namespace libscratchcpp
