// SPDX-License-Identifier: Apache-2.0

#include <optional>
#include <unordered_set>

#include "cppcodegenerator.h"
#include "cppassignment.h"
#include "cppfunctioncall.h"
#include "cppswitch.h"
#include "cppvalue.h"
#include "cppconstidentifier.h"
#include "../typeutils.h"

using namespace libscratchcpp;

const std::unordered_map<Compiler::StaticType, CppIdentifier::Type> CppCodeGenerator::m_valueTypeMap = {
    { Compiler::StaticType::Number, CppIdentifier::Type::Double },
    { Compiler::StaticType::Bool, CppIdentifier::Type::Bool },
    { Compiler::StaticType::String, CppIdentifier::Type::StringPtr },
    { Compiler::StaticType::Pointer, CppIdentifier::Type::Pointer },
    { Compiler::StaticType::Unknown, CppIdentifier::Type::ValueData }
};

const std::unordered_map<CppCodeGenerator::Header, std::string> CppCodeGenerator::m_headers = {
    { Header::Limits, "<limits>" },
    { Header::Value, "<scratchcpp/value.h>" },
    { Header::StringPtr, "<scratchcpp/stringptr.h>" },
    { Header::ValueData, "<scratchcpp/valuedata.h>" },
    { Header::ValueFunctions, "<scratchcpp/value_functions.h>" },
    { Header::OwnedValue, "\"cppownedvalue.h\"" },
    { Header::ExecutionContext, "<scratchcpp/executioncontext.h>" },
    { Header::Target, "<scratchcpp/target.h>" }
};

const std::unordered_map<CppCodeGenerator::FuncParam, CppNamedIdentifier> CppCodeGenerator::m_funcParams = {
    { FuncParam::Context, CppNamedIdentifier(CppIdentifier::Type::ContextPointer, "ctx", false) },
    { FuncParam::Target, CppNamedIdentifier(CppIdentifier::Type::TargetPointer, "target", false) },
    { FuncParam::TargetVariables, CppNamedIdentifier(CppIdentifier::Type::VariablePointerArray, "targetVariables", false) },
    { FuncParam::TargetLists, CppNamedIdentifier(CppIdentifier::Type::ListPointerArray, "targetLists", false) }
};

CppCodeGenerator::CppCodeGenerator() :
    m_stringArray(CppIdentifier::Type::StringPtrArray, "stringArray", true)
{
}

CppCodeGenerator::~CppCodeGenerator()
{
}

CppIdentifier::Type CppCodeGenerator::getType(Compiler::StaticType type) const
{
    return m_valueTypeMap.at(TypeUtils::isSingleType(type) ? type : Compiler::StaticType::Unknown);
}

std::string CppCodeGenerator::finalize(const std::string &headerName)
{
    ensureScriptName();
    const std::string generatedHeaderName = headerName.empty() ? m_generatedFileName + ".h" : headerName;
    const CppIdentifier *returnIdentifier = nullptr;

    if (m_returnValue) {
        switch (m_codeType) {
            case Compiler::CodeType::Reporter:
                returnIdentifier = castValue(m_returnValue, Compiler::StaticType::Unknown);
                break;

            case Compiler::CodeType::HatPredicate:
                returnIdentifier = castValue(m_returnValue, Compiler::StaticType::Bool);
                break;

            case Compiler::CodeType::Script:
                break;
        }
    }

    if (!m_strings.empty())
        includeHeader(Header::StringPtr);

    if (m_codeType == Compiler::CodeType::Reporter)
        includeHeader(Header::ValueData);

    if (m_codeType == Compiler::CodeType::Reporter && returnIdentifier)
        includeHeader(Header::ValueFunctions);

    std::ostringstream source;
    source << "#include \"" << generatedHeaderName << "\"\n";

    for (Header header : m_includedHeaders) {
        source << "#include " << m_headers.at(header) << "\n";
    }

    source << "\nnamespace libscratchcpp\n{\n\n";

    for (const auto &entry : m_functionDeclarations)
        source << "extern \"C\" " << entry.second << "\n";

    if (!m_functionDeclarations.empty())
        source << "\n";

    source << m_scriptName << "::" << m_scriptName << "(CppCompilerContext *ctx) :\n";
    source << "    CppExecutableCode(ctx, {";

    for (size_t i = 0; i < m_objectPointers.size(); ++i) {
        if (i)
            source << ",";

        const CppObjectPointerDescriptor &descriptor = m_objectPointers[i];
        const char *kind = "";

        switch (descriptor.kind) {
            case CppObjectPointerDescriptor::Kind::Target:
                kind = "Target";
                break;
            case CppObjectPointerDescriptor::Kind::Variable:
                kind = "Variable";
                break;
            case CppObjectPointerDescriptor::Kind::List:
                kind = "List";
                break;
            case CppObjectPointerDescriptor::Kind::Timer:
                kind = "Timer";
                break;
            case CppObjectPointerDescriptor::Kind::Effect:
                kind = "Effect";
                break;
        }

        source << " { CppObjectPointerDescriptor::Kind::" << kind << ", \"" << escapeCppString(descriptor.key) << "\" }";
    }

    source << "})\n{\n}\n\n";
    const char *methodName = "runScript";
    const char *returnType = "void";

    if (m_codeType == Compiler::CodeType::Reporter) {
        methodName = "runReporter";
        returnType = "ValueData";
    } else if (m_codeType == Compiler::CodeType::HatPredicate) {
        methodName = "runPredicate";
        returnType = "bool";
    }

    source << returnType << " " << m_scriptName << "::" << methodName << "(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists, void **objects)\n{\n";

    std::ostringstream declaration;
    declaration << "namespace libscratchcpp\n{\n\n";
    declaration << "class " << m_scriptName << " : public CppExecutableCode\n{\n";
    declaration << "    public:\n";
    declaration << "        explicit " << m_scriptName << "(CppCompilerContext *ctx);\n\n";
    declaration << "    protected:\n";
    declaration << "        " << returnType << " " << methodName << "(ExecutionContext *ctx, Target *target, ValueData **targetVariables, List **targetLists, void **objects) override;\n";
    declaration << "};\n\n} // namespace libscratchcpp\n";
    m_generatedClassDeclaration = declaration.str();

    std::unordered_set<const CppIdentifier *> declared;
    const auto declare = [&](const CppIdentifier *identifier) {
        if (!identifier || !identifier->isNamed() || !declared.insert(identifier).second)
            return;

        const auto *named = static_cast<const CppNamedIdentifier *>(identifier);

        if (named->name.rfind("objects[", 0) == 0)
            return;

        source << "    " << (m_ownedValues.count(identifier) ? "CppOwnedValue" : identifier->typeString());
        source << " " << named->name << ";\n";
    };

    for (const auto &entry : m_identifiers)
        declare(entry.second.get());

    for (const auto &identifier : m_internalIdentifiers)
        declare(identifier.get());

    for (const auto &identifier : m_strings)
        declare(identifier.get());

    if (!m_strings.empty()) {
        for (size_t i = 0; i < m_strings.size(); ++i)
            source << "    StringPtr " << m_strings[i]->name << "Storage;\n";

        source << "    StringPtr *" << m_stringArray.name << "[" << m_strings.size() << "] = { ";

        for (size_t i = 0; i < m_strings.size(); ++i) {
            if (i)
                source << ", ";
            source << "&" << m_strings[i]->name << "Storage";
        }
        source << " };\n";
    }

    source << m_codeStream.code();

    if (m_codeType == Compiler::CodeType::Reporter) {
        if (returnIdentifier) {
            source << "    ValueData returnValue{};\n";
            source << "    value_assign_copy(&returnValue, &" << static_cast<const CppNamedIdentifier *>(returnIdentifier)->name << ");\n";
            source << "    return returnValue;\n";
        } else
            source << "    return ValueData();\n";
    } else if (m_codeType == Compiler::CodeType::HatPredicate) {
        source << "    return ";

        if (!returnIdentifier)
            source << "false";
        else if (returnIdentifier->isConst())
            source << toCppConstant(static_cast<const CppConstIdentifier *>(returnIdentifier)->value);
        else
            source << static_cast<const CppNamedIdentifier *>(returnIdentifier)->name;

        source << ";\n";
    }

    source << "}\n\n} // namespace libscratchcpp\n";
    return source.str();
}

void CppCodeGenerator::setCodeType(Compiler::CodeType codeType, CppValue *returnValue)
{
    m_codeType = codeType;
    m_returnValue = returnValue;
}

void CppCodeGenerator::setGeneratedName(const std::string &className, const std::string &fileName)
{
    m_scriptName = className;
    m_generatedFileName = fileName;
}

const std::string &CppCodeGenerator::generatedFileName()
{
    ensureScriptName();
    return m_generatedFileName;
}

void CppCodeGenerator::ensureScriptName()
{
    if (!m_scriptName.empty())
        return;

    static unsigned int scriptCount = 0;
    const std::string scriptNumber = std::to_string(scriptCount++);
    m_scriptName = "Script" + scriptNumber;
    m_generatedFileName = "script" + scriptNumber;
}

const CppIdentifier *CppCodeGenerator::castValue(CppValue *value, Compiler::StaticType targetType)
{
    if (auto *pointerValue = dynamic_cast<CppPointerValue *>(value))
        return addPointerValue(value, pointerValue->descriptor);

    if (value->isConst()) {
        if (!TypeUtils::isSingleType(targetType))
            return createValue(value);
        else
            return castConstValue(value->constValue(), targetType);
    }

    if (!TypeUtils::isSingleType(targetType))
        return createValue(value);

    if (value->isRawValue)
        return castRawValue(value, targetType);

    assert(value->type() != Compiler::StaticType::Void && targetType != Compiler::StaticType::Void);
    const CppNamedIdentifier *source = static_cast<const CppNamedIdentifier *>(createValue(value));
    const CppNamedIdentifier *result = createNamedIdentifier(m_valueTypeMap.at(targetType), "castValue", true);
    const CppNamedIdentifier *valueType = createNamedIdentifier(CppIdentifier::Type::ValueType, "valueType", true);

    m_codeStream.beginLine();

    CppAssignment typeAssignment(m_codeStream, valueType);
    m_codeStream.addToLine(source->name + ".type");
    typeAssignment.close();
    m_codeStream.endLine();

    const CppNamedIdentifier *stringResult = targetType == Compiler::StaticType::String ? addStringAlloca() : nullptr;
    CppSwitch sw(m_codeStream, valueType);
    const auto emitCase = [&](ValueType fromType, const std::string &, const std::string &expression) {
        sw.addCase("ValueType::" + (fromType == ValueType::Number ? std::string("Number") : fromType == ValueType::Bool ? "Bool" : "String"));
        m_codeStream.beginLine();
        CppAssignment assignment(m_codeStream, result);
        m_codeStream.addToLine(expression);
        assignment.close();
        m_codeStream.endLine();
        m_codeStream.beginLine();
        m_codeStream.addToLine("break;");
        m_codeStream.endLine();
    };

    includeHeader(Header::ValueData);

    if (targetType == Compiler::StaticType::Number) {
        emitCase(ValueType::Number, "numberValue", source->name + ".numberValue");
        emitCase(ValueType::Bool, "boolValue", "static_cast<double>(" + source->name + ".boolValue)");
        includeHeader(Header::ValueFunctions);
        emitCase(ValueType::String, "stringValue", "value_stringToDouble(" + source->name + ".stringValue)");
    } else if (targetType == Compiler::StaticType::Bool) {
        emitCase(ValueType::Number, "numberValue", "(" + source->name + ".numberValue != 0.0)");
        emitCase(ValueType::Bool, "boolValue", source->name + ".boolValue");
        includeHeader(Header::ValueFunctions);
        emitCase(ValueType::String, "stringValue", "value_stringToBool(" + source->name + ".stringValue)");
    } else if (targetType == Compiler::StaticType::String) {
        includeHeader(Header::ValueFunctions);

        // The string destination is passed to the number conversion routine; bool conversion returns a stable pointer.
        sw.addCase("ValueType::Number");
        m_codeStream.beginLine();

        CppFunctionCall numberCall(m_codeStream, "value_doubleToStringPtr");
        numberCall.param(source->name + ".numberValue").param(stringResult->name);
        numberCall.close();
        m_codeStream.addToLine(";");
        m_codeStream.endLine();
        m_codeStream.beginLine();

        CppAssignment stringAssignment(m_codeStream, result);
        m_codeStream.addToLine(stringResult->name);
        stringAssignment.close();
        m_codeStream.endLine();
        m_codeStream.beginLine();
        m_codeStream.addToLine("break;");
        m_codeStream.endLine();
        sw.addCase("ValueType::Bool");
        m_codeStream.beginLine();

        CppAssignment boolAssignment(m_codeStream, result);
        m_codeStream.addToLine("const_cast<StringPtr *>(value_boolToStringPtr(" + source->name + ".boolValue))");
        boolAssignment.close();
        m_codeStream.endLine();
        m_codeStream.beginLine();
        m_codeStream.addToLine("break;");
        m_codeStream.endLine();
        sw.addCase("ValueType::String");
        m_codeStream.beginLine();

        CppAssignment identityAssignment(m_codeStream, result);
        m_codeStream.addToLine(source->name + ".stringValue");
        identityAssignment.close();
        m_codeStream.endLine();
        m_codeStream.beginLine();
        m_codeStream.addToLine("break;");
        m_codeStream.endLine();
    }

    sw.addDefaultCase("");
    m_codeStream.beginLine();
    m_codeStream.addToLine("__builtin_unreachable();");
    m_codeStream.endLine();
    sw.close();

    return result;
}

const CppIdentifier *CppCodeGenerator::addPointerValue(CppValue *value, const CppObjectPointerDescriptor &descriptor)
{
    auto it = m_identifiers.find(value);

    if (it != m_identifiers.cend())
        return it->second.get();

    const size_t index = m_objectPointers.size();
    m_objectPointers.push_back(descriptor);

    const std::string expression = "objects[" + std::to_string(index) + "]";
    auto identifier = std::make_shared<CppNamedIdentifier>(CppIdentifier::Type::Pointer, expression, false);
    m_identifiers[value] = identifier;
    return identifier.get();
}

void CppCodeGenerator::createCall(const std::string &name, bool ctxArg, bool targetArg, const std::vector<const CppIdentifier *> &args)
{
    const auto preparedArgs = prepareCallArgs(name, args);
    addFunctionDeclaration(name, CppIdentifier::Type::Pointer, false, true, ctxArg, targetArg, preparedArgs);
    m_codeStream.beginLine();
    createCall(name, nullptr, ctxArg, targetArg, preparedArgs);
    m_codeStream.addToLine(";");
    m_codeStream.endLine();
}

void CppCodeGenerator::createCall(const std::string &name, CppIdentifier::Type retType, CppValue *retValue, bool ctxArg, bool targetArg, const std::vector<const CppIdentifier *> &args)
{
    if (retType == CppIdentifier::Type::ValueData)
        includeHeader(Header::ValueData);

    if (identifierExists(retValue)) {
        throw std::runtime_error("assigning to existing return value identifier when calling function: " + name);
    }

    const CppNamedIdentifier *strRet = nullptr;

    if (retType == CppIdentifier::Type::StringPtr) {
        // Strings are returned through an output parameter
        includeHeader(Header::ExecutionContext);
        strRet = addStringAlloca();

        if (retValue)
            m_identifiers[retValue] = m_strings.back();
    }

    const auto preparedArgs = prepareCallArgs(name, args);
    addFunctionDeclaration(name, retType, retType == CppIdentifier::Type::StringPtr, false, ctxArg, targetArg, preparedArgs);

    m_codeStream.beginLine();

    std::unique_ptr<CppAssignment> strAssignment;

    if (retType != CppIdentifier::Type::StringPtr) {
        const auto *result = getNamedIdentifier(retValue, retType, "ret", true);
        if (retType == CppIdentifier::Type::ValueData) {
            includeHeader(Header::OwnedValue);
            m_ownedValues.insert(result);
        }
        strAssignment = std::make_unique<CppAssignment>(m_codeStream, result);
    }

    createCall(name, strRet, ctxArg, targetArg, preparedArgs);

    if (strAssignment)
        strAssignment->close();
    else
        m_codeStream.addToLine(";");

    m_codeStream.endLine();
}

void CppCodeGenerator::createCall(const std::string &name, const CppNamedIdentifier *strRet, bool ctxArg, bool targetArg, const std::vector<const CppIdentifier *> &args)
{
    CppFunctionCall call(m_codeStream, name);

    if (strRet)
        call.param(strRet);

    if (ctxArg)
        call.param(getParam(FuncParam::Context));

    if (targetArg)
        call.param(getParam(FuncParam::Target));

    for (const CppIdentifier *arg : args) {
        try {
            if (arg->isConst())
                call.param(toCppConstant(static_cast<const CppConstIdentifier *>(arg)->value));
            else if (arg->isNamed()) {
                const auto *named = static_cast<const CppNamedIdentifier *>(arg);

                if (named->type == CppIdentifier::Type::ValueData)
                    call.param("&" + named->name);
                else
                    call.param(named);
            }
            else
                throw std::runtime_error("unsupported identifier when calling function: " + name);
        } catch (std::exception &e) {
            throw std::runtime_error("error when creating call of function '" + name + "': " + e.what());
        }
    }

    call.close();
}

std::vector<const CppIdentifier *> CppCodeGenerator::prepareCallArgs(const std::string &name, const std::vector<const CppIdentifier *> &args)
{
    std::vector<const CppIdentifier *> preparedArgs;
    preparedArgs.reserve(args.size());

    for (const CppIdentifier *arg : args) {
        try {
            if (!arg->isConst()) {
                preparedArgs.push_back(arg);
                continue;
            }

            const auto *constant = static_cast<const CppConstIdentifier *>(arg);

            if (constant->type != CppIdentifier::Type::StringPtr) {
                preparedArgs.push_back(arg);
                continue;
            }

            includeHeader(Header::Value);
            includeHeader(Header::ValueFunctions);
            const CppNamedIdentifier *value = createNamedIdentifier(CppIdentifier::Type::Value, "constant", true);
            const CppNamedIdentifier *string = addStringAlloca();

            m_codeStream.beginLine();
            CppAssignment valueAssignment(m_codeStream, value);
            m_codeStream.addToLine("Value(" + toCppConstant(constant->value) + ")");
            valueAssignment.close();
            m_codeStream.endLine();

            m_codeStream.beginLine();
            CppFunctionCall stringCall(m_codeStream, "value_toStringPtr");
            stringCall.param("&" + value->name + ".data()").param(string->name);
            stringCall.close();
            m_codeStream.addToLine(";");
            m_codeStream.endLine();
            preparedArgs.push_back(string);
        } catch (std::exception &e) {
            throw std::runtime_error("error when creating call of function '" + name + "': " + e.what());
        }
    }

    return preparedArgs;
}

void CppCodeGenerator::addFunctionDeclaration(const std::string &name, CppIdentifier::Type returnType, bool stringReturn, bool voidReturn, bool ctxArg, bool targetArg, const std::vector<const CppIdentifier *> &args)
{
    // Test helper declarations are provided by their test support headers.
    if (name.rfind("test_", 0) == 0)
        return;

    const auto typeName = [&name](CppIdentifier::Type type, bool argument) {
        switch (type) {
            case CppIdentifier::Type::Double:
                return std::string("double");
            case CppIdentifier::Type::Bool:
                return std::string("bool");
            case CppIdentifier::Type::StringPtr:
                return argument ? std::string("const StringPtr *") : std::string("StringPtr *");
            case CppIdentifier::Type::Pointer:
                return argument ? std::string("const void *") : std::string("void *");
            case CppIdentifier::Type::ValueData:
                return argument ? std::string("const ValueData *") : std::string("ValueData");
            case CppIdentifier::Type::ContextPointer:
                return std::string("ExecutionContext *");
            case CppIdentifier::Type::TargetPointer:
                return std::string("Target *");
            default:
                throw std::runtime_error("unsupported function parameter type when declaring '" + name + "'");
        }
    };

    std::vector<std::string> parameterTypes;

    if (stringReturn) {
        includeHeader(Header::StringPtr);
        parameterTypes.emplace_back("StringPtr *");
    }

    if (ctxArg) {
        includeHeader(Header::ExecutionContext);
        parameterTypes.emplace_back("ExecutionContext *");
    }

    if (targetArg) {
        includeHeader(Header::Target);
        parameterTypes.emplace_back("Target *");
    }

    for (const CppIdentifier *arg : args) {
        if (arg->type == CppIdentifier::Type::StringPtr)
            includeHeader(Header::StringPtr);
        else if (arg->type == CppIdentifier::Type::ValueData)
            includeHeader(Header::ValueData);
        else if (arg->type == CppIdentifier::Type::ContextPointer)
            includeHeader(Header::ExecutionContext);
        else if (arg->type == CppIdentifier::Type::TargetPointer)
            includeHeader(Header::Target);

        parameterTypes.push_back(typeName(arg->type, true));
    }

    std::string declaration = stringReturn || voidReturn ? "void" : typeName(returnType, false);
    declaration += " " + name + "(";

    for (size_t i = 0; i < parameterTypes.size(); ++i) {
        if (i)
            declaration += ", ";
        declaration += parameterTypes[i];
    }

    declaration += ");";
    m_functionDeclarations.emplace(name, std::move(declaration));
}

const CppNamedIdentifier *CppCodeGenerator::getParam(FuncParam param)
{
    return &m_funcParams.at(param);
}

const CppIdentifier *CppCodeGenerator::getIdentifier(CppValue *value)
{
    auto it = m_identifiers.find(value);

    if (it == m_identifiers.cend()) {
        throw std::runtime_error("undefined identifier");
    }

    return it->second.get();
}

const CppNamedIdentifier *CppCodeGenerator::getNamedIdentifier(CppValue *value, CppIdentifier::Type type, const std::string &name, bool isMutable, bool isReference)
{
    auto it = m_identifiers.find(value);

    if (it == m_identifiers.cend()) {
        auto identifier = std::make_shared<CppNamedIdentifier>(type, getNewIdentifierName(name), isMutable, isReference);
        m_identifiers[value] = identifier;
        return identifier.get();
    } else {
        auto identifier = std::dynamic_pointer_cast<CppNamedIdentifier>(it->second);

        if (!identifier) {
            throw std::runtime_error("specified value does not have a named identifier");
        }

        return identifier.get();
    }
}

const CppNamedIdentifier *CppCodeGenerator::createNamedIdentifier(CppIdentifier::Type type, const std::string &name, bool isMutable)
{
    auto identifier = std::make_shared<CppNamedIdentifier>(type, getNewIdentifierName(name), isMutable);
    m_internalIdentifiers.push_back(identifier);
    return identifier.get();
}

std::string CppCodeGenerator::getNewIdentifierName(const std::string &name)
{
    auto it = m_identifierNameCounts.find(name);

    if (it == m_identifierNameCounts.cend()) {
        m_identifierNameCounts[name] = 0;
        return name + "0";
    } else {
        return name + std::to_string(++it->second);
    }
}

void CppCodeGenerator::includeHeader(Header header)
{
    m_includedHeaders.insert(header);
}

const CppNamedIdentifier *CppCodeGenerator::addStringAlloca()
{
    auto identifier = std::make_shared<CppNamedIdentifier>(CppIdentifier::Type::StringPtr, getNewIdentifierName("string"), true);
    m_strings.push_back(identifier);

    m_codeStream.beginLine();

    CppAssignment assignment(m_codeStream, identifier.get());
    addArrayElement(&m_stringArray, m_strings.size() - 1);
    assignment.close();
    m_codeStream.endLine();

    return identifier.get();
}

void CppCodeGenerator::addArrayElement(const CppNamedIdentifier *array, size_t index)
{
    if (!array->isArray()) {
        throw std::runtime_error("'" + array->name + "' is not an array");
    }

    m_codeStream.addToLine(array->name);
    m_codeStream.addToLine("[");
    m_codeStream.addToLine(std::to_string(index));
    m_codeStream.addToLine("]");
}

const CppNamedIdentifier *CppCodeGenerator::getValueDataIdentifier(CppValue *value)
{
    auto it = m_valueDataIdentifiers.find(value);

    if (it != m_valueDataIdentifiers.cend())
        return it->second;

    const CppNamedIdentifier *identifier = createNamedIdentifier(CppIdentifier::Type::ValueData, "value", true);
    m_valueDataIdentifiers[value] = identifier;
    return identifier;
}

const CppConstIdentifier *CppCodeGenerator::castConstValue(const Value &value, Compiler::StaticType targetType)
{
    Value casted = value;

    switch (targetType) {
        case Compiler::StaticType::Number:
            casted = Value(value.toDouble());
            break;

        case Compiler::StaticType::Bool:
            casted = Value(value.toBool());
            break;

        case Compiler::StaticType::String:
            casted = Value(value.toString());
            break;

        case Compiler::StaticType::Pointer:
            casted = Value(value.toPointer());
            break;

        default:
            assert(false);
            break;
    }

    auto ret = std::make_shared<CppConstIdentifier>(m_valueTypeMap.at(targetType), casted);
    m_constants.push_back(ret);
    return ret.get();
}

const CppNamedIdentifier *CppCodeGenerator::castRawValue(CppValue *value, Compiler::StaticType targetType)
{
    assert(value->isRawValue);

    const CppNamedIdentifier *source = static_cast<const CppNamedIdentifier *>(getIdentifier(value));

    if (value->type() == targetType)
        return source;

    const CppNamedIdentifier *result = createNamedIdentifier(m_valueTypeMap.at(targetType), "castRawValue", true);
    std::string expression;

    includeHeader(Header::ValueFunctions);

    if (targetType == Compiler::StaticType::Number) {
        expression = value->type() == Compiler::StaticType::Bool ? "static_cast<double>(" + source->name + ")" : "value_stringToDouble(" + source->name + ")";
    } else if (targetType == Compiler::StaticType::Bool) {
        expression = value->type() == Compiler::StaticType::Number ? "(" + source->name + " != 0.0)" : "value_stringToBool(" + source->name + ")";
    } else if (targetType == Compiler::StaticType::String) {
        if (value->type() == Compiler::StaticType::Number) {
            const CppNamedIdentifier *string = addStringAlloca();

            m_codeStream.beginLine();
            CppFunctionCall call(m_codeStream, "value_doubleToStringPtr");
            call.param(source->name).param(string->name);
            call.close();
            m_codeStream.addToLine(";");
            m_codeStream.endLine();

            expression = string->name;
        } else if (value->type() == Compiler::StaticType::Bool) {
            expression = "const_cast<StringPtr *>(value_boolToStringPtr(" + source->name + "))";
        }
    }

    m_codeStream.beginLine();
    CppAssignment assignment(m_codeStream, result);
    m_codeStream.addToLine(expression);
    assignment.close();
    m_codeStream.endLine();
    return result;
}

const CppNamedIdentifier *CppCodeGenerator::createValue(CppValue *value)
{
    if (value->isConst()) {
        const CppNamedIdentifier *identifier = getValueDataIdentifier(value);

        includeHeader(Header::Value);

        // Value constant = Value(...);
        const CppNamedIdentifier *valueObj = createNamedIdentifier(CppIdentifier::Type::Value, "constant", true);
        m_codeStream.beginLine();

        CppAssignment constAssignment(m_codeStream, valueObj);
        m_codeStream.addToLine(valueObj->typeString());
        m_codeStream.addToLine("(");
        m_codeStream.addToLine(toCppConstant(value->constValue()));
        m_codeStream.addToLine(")");
        m_codeStream.addToLine(";");

        constAssignment.close();
        m_codeStream.endLine();

        // ValueData &value = constant.data();
        m_codeStream.beginLine();

        CppAssignment valueAssignment(m_codeStream, identifier);
        CppFunctionCall dataCall(m_codeStream, valueObj, "data");
        dataCall.close();

        valueAssignment.close();
        m_codeStream.endLine();
        return identifier;
    }

    const auto it = m_identifiers.find(value);

    if (value->isRawValue || (it != m_identifiers.cend() && it->second->type != CppIdentifier::Type::ValueData)) {
        const CppNamedIdentifier *identifier = getValueDataIdentifier(value);

        includeHeader(Header::ValueData);

        // Store value
        const CppIdentifier *valueIdentifier = getIdentifier(value);

        if (!valueIdentifier || !valueIdentifier->isNamed()) {
            throw std::runtime_error("raw values must have a named identifier when creating a ValueData instance");
        }

        std::string member;
        ValueType type = ValueType::Number;

        switch (value->type()) {
            case Compiler::StaticType::Number: {
                member = "numberValue";
                type = ValueType::Number;
                break;
            }

            case Compiler::StaticType::Bool: {
                member = "boolValue";
                type = ValueType::Bool;
                break;
            }

            case Compiler::StaticType::String: {
                member = "stringValue";
                type = ValueType::String;
                break;
            }

            case Compiler::StaticType::Pointer: {
                member = "pointerValue";
                type = ValueType::Pointer;
                break;
            }

            default:
                assert(false);
                break;
        }

        auto valueNamedIdentifier = static_cast<const CppNamedIdentifier *>(valueIdentifier);

        m_codeStream.beginLine();
        CppAssignment valueAssignment(m_codeStream, identifier, member);
        m_codeStream.addToLine(valueNamedIdentifier->name);
        valueAssignment.close();
        m_codeStream.endLine();

        m_codeStream.beginLine();
        CppAssignment typeAssignment(m_codeStream, identifier, "type");
        m_codeStream.addToLine("ValueType::" + (type == ValueType::Number ? std::string("Number") : type == ValueType::Bool ? "Bool" : type == ValueType::String ? "String" : "Pointer"));
        typeAssignment.close();
        m_codeStream.endLine();
        return identifier;
    }

    if (it != m_identifiers.cend())
        return static_cast<const CppNamedIdentifier *>(it->second.get());

    return getNamedIdentifier(value, CppIdentifier::Type::ValueData, "value", false);
}

std::string CppCodeGenerator::toCppConstant(const Value &value)
{
    switch (value.type()) {
        case ValueType::Number:
            return toCppDouble(value.toDouble());

        case ValueType::Bool:
            return value.toString();
            break;

        case ValueType::String:
            return "\"" + escapeCppString(value.toString()) + "\"";

        case ValueType::Pointer:
            throw std::runtime_error("pointer constants not supported in C++ backend");

        default:
            throw std::runtime_error("unsupported constant type");
    }
}

std::string CppCodeGenerator::toCppDouble(double value)
{
    includeHeader(Header::Limits);

    if (std::isnan(value))
        return "std::numeric_limits<double>::quiet_NaN()";

    if (std::isinf(value))
        return value > 0 ? "std::numeric_limits<double>::infinity()" : "-std::numeric_limits<double>::infinity()";

    // Shortest round-trip representation
    std::ostringstream ss;
    ss << std::setprecision(std::numeric_limits<double>::max_digits10) << value;
    std::string s = ss.str();

    // Ensure it parses as double, not int (e.g. "1" -> "1.0")
    if (s.find_first_of(".eEnN") == std::string::npos)
        s += ".0";

    return s;
}

std::string CppCodeGenerator::escapeCppString(const std::string &str)
{
    std::string out;
    out.reserve(str.size() + 8);

    for (unsigned char c : str) {
        switch (c) {
            case '\\':
                out += "\\\\";
                break;
            case '"':
                out += "\\\"";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            case '\0':
                out += "\\0";
                break;
            default:
                if (c < 0x20 || c == 0x7f) {
                    // Control characters: octal escape (fixed 3 digits so a
                    // following digit can't extend the escape sequence)
                    char buf[5];
                    std::snprintf(buf, sizeof(buf), "\\%03o", c);
                    out += buf;
                } else {
                    out += static_cast<char>(c);
                }
        }
    }

    return out;
}
