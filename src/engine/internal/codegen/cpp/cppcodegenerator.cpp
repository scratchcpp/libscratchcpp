// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "cppcodegenerator.h"
#include "cppfunctioncall.h"
#include "cppvalue.h"
#include "cppconstidentifier.h"
#include "../typeutils.h"

using namespace libscratchcpp;

const std::unordered_map<Compiler::StaticType, CppIdentifier::Type> CppCodeGenerator::m_valueTypeMap = {
    { Compiler::StaticType::Number, CppIdentifier::Type::Double },
    { Compiler::StaticType::Bool, CppIdentifier::Type::Bool },
    { Compiler::StaticType::String, CppIdentifier::Type::StringPtr },
    { Compiler::StaticType::Pointer, CppIdentifier::Type::Pointer }
};

const std::unordered_map<CppCodeGenerator::Header, std::string> CppCodeGenerator::m_headers = {
    { Header::Limits, "<limits>" },
    { Header::Value, "<scratchcpp/value.h>" },
    { Header::ValueData, "<scratchcpp/valuedata.h>" },
    { Header::ValueFunctions, "<scratchcpp/value_functions.h>" },
    { Header::ExecutionContext, "<scratchcpp/executioncontext.h>" }
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
    std::cout << m_codeStream.code() << std::endl;
}

const CppIdentifier *CppCodeGenerator::castValue(CppValue *value, Compiler::StaticType targetType)
{
    // TODO: Implement this
    if (value->isConst()) {
        if (!TypeUtils::isSingleType(targetType))
            return createValue(value);
        else
            return castConstValue(value->constValue(), targetType);
    }

    if (value->isRawValue)
        return castRawValue(value, targetType);

    return getNamedIdentifier(value, m_valueTypeMap.at(targetType), "castValue", false);
}

void CppCodeGenerator::createCall(const std::string &name, bool ctxArg, bool targetArg, const std::vector<const CppIdentifier *> &args)
{
    m_codeStream.beginLine();
    createCall(name, nullptr, ctxArg, targetArg, args);
    m_codeStream.endLine();
}

void CppCodeGenerator::createCall(const std::string &name, CppIdentifier::Type retType, CppValue *retValue, bool ctxArg, bool targetArg, const std::vector<const CppIdentifier *> &args)
{
    if (identifierExists(retValue)) {
        throw std::runtime_error("assigning to existing return value identifier when calling function: " + name);
    }

    const CppNamedIdentifier *strRet = nullptr;

    if (retType == CppIdentifier::Type::StringPtr) {
        // Strings are returned through an output parameter
        includeHeader(Header::ExecutionContext);
        strRet = addStringAlloca();
    }

    m_codeStream.beginLine();

    if (retType != CppIdentifier::Type::StringPtr) {
        m_codeStream.addToLine(CppIdentifier::typeString(retType));
        m_codeStream.addToLine(" ");
        m_codeStream.addToLine(getNamedIdentifier(retValue, retType, "ret", false)->name);
        m_codeStream.addToLine(" = ");
    }

    createCall(name, strRet, ctxArg, targetArg, args);
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
            if (arg->isConst()) {
                call.param(toCppConstant(static_cast<const CppConstIdentifier *>(arg)->value));
            } else if (arg->isNamed()) {
                call.param(static_cast<const CppNamedIdentifier *>(arg));
            } else {
                throw std::runtime_error("unsupported identifier when calling function: " + name);
            }
        } catch (std::exception &e) {
            throw std::runtime_error("error when creating call of function '" + name + "': " + e.what());
        }
    }

    call.close();
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

const CppNamedIdentifier *CppCodeGenerator::getNamedIdentifier(CppValue *value, CppIdentifier::Type type, const std::string &name, bool isMutable)
{
    auto it = m_identifiers.find(value);

    if (it == m_identifiers.cend()) {
        auto identifier = std::make_shared<CppNamedIdentifier>(type, getNewIdentifierName(name), isMutable);
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
    m_codeStream.addToLine(CppIdentifier::typeString(CppIdentifier::Type::StringPtr));
    m_codeStream.addToLine(" ");
    m_codeStream.addToLine(identifier->name);
    m_codeStream.addToLine(" = ");
    addArrayElement(&m_stringArray, m_strings.size() - 1);
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

const CppConstIdentifier *CppCodeGenerator::castConstValue(const Value &value, Compiler::StaticType targetType)
{
    // TODO: Implement this
    auto identifierType = m_valueTypeMap.at(TypeUtils::mapType(value.type()));
    auto ret = std::make_shared<CppConstIdentifier>(identifierType, value);
    m_constants.push_back(ret);
    return ret.get();
}

const CppNamedIdentifier *CppCodeGenerator::castRawValue(CppValue *value, Compiler::StaticType targetType)
{
    // TODO: Implement this
    assert(value->isRawValue);
    return getNamedIdentifier(value, m_valueTypeMap.at(targetType), "castRawValue", false);
}

const CppNamedIdentifier *CppCodeGenerator::createValue(CppValue *value)
{
    const CppNamedIdentifier *identifier = getNamedIdentifier(value, CppIdentifier::Type::ValueData, "value", false);

    if (value->isConst()) {
        includeHeader(Header::Value);

        // const Value constant(...);
        const CppNamedIdentifier *valueObj = createNamedIdentifier(CppIdentifier::Type::Value, "constant", false);
        m_codeStream.beginLine();
        m_codeStream.addToLine("const ");
        m_codeStream.addToLine(valueObj->typeString());
        m_codeStream.addToLine(" ");
        m_codeStream.addToLine(valueObj->name);
        m_codeStream.addToLine("(");
        m_codeStream.addToLine(toCppConstant(value->constValue()));
        m_codeStream.addToLine(")");
        m_codeStream.addToLine(";");
        m_codeStream.endLine();

        // const ValueData &value = constant.data();
        m_codeStream.beginLine();
        m_codeStream.addToLine("const ");
        m_codeStream.addToLine(identifier->typeString());
        m_codeStream.addToLine(" &");
        m_codeStream.addToLine(identifier->name);
        m_codeStream.addToLine(" = ");

        CppFunctionCall dataCall(m_codeStream, valueObj, "data");
        dataCall.close();

        m_codeStream.addToLine(";");
        m_codeStream.endLine();
    } else if (value->isRawValue) {
        includeHeader(Header::ValueData);

        // const ValueData value;
        m_codeStream.beginLine();
        m_codeStream.addToLine("const ");
        m_codeStream.addToLine(identifier->typeString());
        m_codeStream.addToLine(" ");
        m_codeStream.addToLine(identifier->name);
        m_codeStream.addToLine(";");
        m_codeStream.endLine();

        // Store value
        const CppIdentifier *valueIdentifier = getIdentifier(value);

        if (!valueIdentifier || !valueIdentifier->isNamed()) {
            throw std::runtime_error("raw values must have a named identifier when creating a ValueData instance");
        }

        auto valueNamedIdentifier = static_cast<const CppNamedIdentifier *>(valueIdentifier);
        m_codeStream.addToLine(identifier->name);

        switch (value->type()) {
            case Compiler::StaticType::Number:
                // value.numberValue = rawValue;
                m_codeStream.addToLine(".numberValue = ");
                break;

            case Compiler::StaticType::Bool:
                // value.boolValue = rawValue;
                m_codeStream.addToLine(".boolValue = ");
                break;

            case Compiler::StaticType::String:
                // value.stringValue = rawValue;
                m_codeStream.addToLine(".stringValue = ");
                break;

            case Compiler::StaticType::Pointer:
                // value.pointerValue = rawValue;
                m_codeStream.addToLine(".pointerValue = ");
                break;

            default:
                assert(false);
                break;
        }

        m_codeStream.addToLine(valueNamedIdentifier->name);
        m_codeStream.addToLine(";");
        m_codeStream.endLine();
    }

    return identifier;
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
