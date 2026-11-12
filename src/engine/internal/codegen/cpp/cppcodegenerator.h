// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <scratchcpp/compiler.h>
#include <unordered_map>
#include <sstream>

#include "cppnamedidentifier.h"
#include "cppconstidentifier.h"
#include "cppcodestream.h"

namespace libscratchcpp
{

class CppValue;

class CppCodeGenerator
{
    public:
        CppCodeGenerator();
        ~CppCodeGenerator();

        inline CppIdentifier::Type getType(Compiler::StaticType type) const { return m_valueTypeMap.at(type); }

        const CppIdentifier *castValue(CppValue *value, Compiler::StaticType targetType);

        void createCall(const std::string &name, bool ctxArg, bool targetArg, const std::vector<const CppIdentifier *> &args);
        void createCall(const std::string &name, CppIdentifier::Type retType, CppValue *retValue, bool ctxArg, bool targetArg, const std::vector<const CppIdentifier *> &args);

    private:
        enum class Header
        {
            Limits,
            Value,
            ValueData,
            ValueFunctions,
            ExecutionContext
        };

        enum class FuncParam
        {
            Context,
            Target,
            TargetVariables,
            TargetLists
        };

        void createCall(const std::string &name, const CppNamedIdentifier *strRet, bool ctxArg, bool targetArg, const std::vector<const CppIdentifier *> &args);

        const CppNamedIdentifier *getParam(FuncParam param);
        const CppIdentifier *getIdentifier(CppValue *value);
        const CppNamedIdentifier *getNamedIdentifier(CppValue *value, CppIdentifier::Type type, const std::string &name, bool isMutable, bool isReference = false);
        const CppNamedIdentifier *createNamedIdentifier(CppIdentifier::Type type, const std::string &name, bool isMutable);
        std::string getNewIdentifierName(const std::string &name);
        void includeHeader(Header header);

        inline bool identifierExists(CppValue *value) const { return m_identifiers.find(value) != m_identifiers.cend(); }

        const CppNamedIdentifier *addStringAlloca();
        void addArrayElement(const CppNamedIdentifier *array, size_t index);
        const CppConstIdentifier *castConstValue(const Value &value, Compiler::StaticType targetType);
        const CppNamedIdentifier *castRawValue(CppValue *value, Compiler::StaticType targetType);
        const CppNamedIdentifier *createValue(CppValue *value);

        std::string toCppConstant(const Value &value);
        std::string toCppDouble(double value);
        static std::string escapeCppString(const std::string &str);

        static const std::unordered_map<Compiler::StaticType, CppIdentifier::Type> m_valueTypeMap;
        static const std::unordered_map<Header, std::string> m_headers;
        static const std::unordered_map<FuncParam, CppNamedIdentifier> m_funcParams;

        CppCodeStream m_codeStream;

        std::unordered_map<CppValue *, std::shared_ptr<CppIdentifier>> m_identifiers;
        std::unordered_map<std::string, unsigned int> m_identifierNameCounts;
        std::unordered_set<Header> m_includedHeaders;
        std::vector<std::shared_ptr<CppIdentifier>> m_internalIdentifiers;
        std::vector<std::shared_ptr<CppConstIdentifier>> m_constants;

        CppNamedIdentifier m_stringArray;
        std::vector<std::shared_ptr<CppIdentifier>> m_strings;
};

} // namespace libscratchcpp
