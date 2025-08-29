// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/value_functions.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/irandomgenerator.h>

#include "llvmfunctions.h"
#include "llvmcompilercontext.h"

using namespace libscratchcpp;

extern "C"
{
    double llvm_random(ExecutionContext *ctx, ValueData *from, ValueData *to)
    {
        return value_isInt(from) && value_isInt(to) ? ctx->rng()->randint(value_toLong(from), value_toLong(to)) : ctx->rng()->randintDouble(value_toDouble(from), value_toDouble(to));
    }

    double llvm_random_double(ExecutionContext *ctx, double from, double to)
    {
        return value_doubleIsInt(from) && value_doubleIsInt(to) ? ctx->rng()->randint(from, to) : ctx->rng()->randintDouble(from, to);
    }

    int64_t llvm_random_int64(ExecutionContext *ctx, int64_t from, int64_t to)
    {
        return ctx->rng()->randint(from, to);
    }

    double llvm_random_bool(ExecutionContext *ctx, bool from, bool to)
    {
        return ctx->rng()->randint(from, to);
    }
}

LLVMFunctions::LLVMFunctions(LLVMCompilerContext *ctx, llvm::IRBuilder<> *builder) :
    m_ctx(ctx),
    m_builder(builder)
{
    // Custom types
    m_stringPtrType = m_ctx->stringPtrType();
    m_valueDataType = m_ctx->valueDataType();
}

llvm::FunctionCallee LLVMFunctions::resolveFunction(const std::string name, llvm::FunctionType *type)
{
    return m_ctx->module()->getOrInsertFunction(name, type);
}

llvm::FunctionCallee LLVMFunctions::resolve_value_init()
{
    return resolveFunction("value_init", llvm::FunctionType::get(m_builder->getVoidTy(), m_valueDataType->getPointerTo(), false));
}

llvm::FunctionCallee LLVMFunctions::resolve_value_free()
{
    return resolveFunction("value_free", llvm::FunctionType::get(m_builder->getVoidTy(), m_valueDataType->getPointerTo(), false));
}

llvm::FunctionCallee LLVMFunctions::resolve_value_assign_long()
{
    return resolveFunction("value_assign_long", llvm::FunctionType::get(m_builder->getVoidTy(), { m_valueDataType->getPointerTo(), m_builder->getInt64Ty() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_value_assign_double()
{
    return resolveFunction("value_assign_double", llvm::FunctionType::get(m_builder->getVoidTy(), { m_valueDataType->getPointerTo(), m_builder->getDoubleTy() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_value_assign_bool()
{
    return resolveFunction("value_assign_bool", llvm::FunctionType::get(m_builder->getVoidTy(), { m_valueDataType->getPointerTo(), m_builder->getInt1Ty() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_value_assign_stringPtr()
{
    return resolveFunction("value_assign_stringPtr", llvm::FunctionType::get(m_builder->getVoidTy(), { m_valueDataType->getPointerTo(), m_stringPtrType->getPointerTo() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_value_assign_special()
{
    return resolveFunction("value_assign_special", llvm::FunctionType::get(m_builder->getVoidTy(), { m_valueDataType->getPointerTo(), m_builder->getInt32Ty() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_value_assign_copy()
{
    return resolveFunction("value_assign_copy", llvm::FunctionType::get(m_builder->getVoidTy(), { m_valueDataType->getPointerTo(), m_valueDataType->getPointerTo() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_value_toDouble()
{
    llvm::FunctionCallee callee = resolveFunction("value_toDouble", llvm::FunctionType::get(m_builder->getDoubleTy(), m_valueDataType->getPointerTo(), false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_value_toBool()
{
    llvm::FunctionCallee callee = resolveFunction("value_toBool", llvm::FunctionType::get(m_builder->getInt1Ty(), m_valueDataType->getPointerTo(), false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_value_toStringPtr()
{
    llvm::FunctionCallee callee = resolveFunction("value_toStringPtr", llvm::FunctionType::get(m_builder->getVoidTy(), { m_valueDataType->getPointerTo(), m_stringPtrType->getPointerTo() }, false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_value_doubleToStringPtr()
{
    llvm::FunctionCallee callee = resolveFunction("value_doubleToStringPtr", llvm::FunctionType::get(m_builder->getVoidTy(), { m_builder->getDoubleTy(), m_stringPtrType->getPointerTo() }, false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_value_boolToStringPtr()
{
    // NOTE: This function can be marked read-only because it does NOT allocate on the heap ("true" and "false" constants)
    llvm::FunctionCallee callee = resolveFunction("value_boolToStringPtr", llvm::FunctionType::get(m_stringPtrType->getPointerTo(), m_builder->getInt1Ty(), false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_value_stringToDouble()
{
    llvm::FunctionCallee callee =
        resolveFunction("value_stringToDouble", llvm::FunctionType::get(m_builder->getDoubleTy(), llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0), false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_value_stringToDoubleWithCheck()
{
    llvm::Type *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0);
    llvm::FunctionCallee callee = resolveFunction("value_stringToDoubleWithCheck", llvm::FunctionType::get(m_builder->getDoubleTy(), { pointerType, m_builder->getInt1Ty()->getPointerTo() }, false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_value_stringToBool()
{
    llvm::FunctionCallee callee = resolveFunction("value_stringToBool", llvm::FunctionType::get(m_builder->getInt1Ty(), llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0), false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_value_equals()
{
    llvm::Type *valuePtr = m_valueDataType->getPointerTo();
    llvm::FunctionCallee callee = resolveFunction("value_equals", llvm::FunctionType::get(m_builder->getInt1Ty(), { valuePtr, valuePtr }, false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_value_greater()
{
    llvm::Type *valuePtr = m_valueDataType->getPointerTo();
    llvm::FunctionCallee callee = resolveFunction("value_greater", llvm::FunctionType::get(m_builder->getInt1Ty(), { valuePtr, valuePtr }, false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_value_lower()
{
    llvm::Type *valuePtr = m_valueDataType->getPointerTo();
    llvm::FunctionCallee callee = resolveFunction("value_lower", llvm::FunctionType::get(m_builder->getInt1Ty(), { valuePtr, valuePtr }, false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_list_clear()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0);
    return resolveFunction("list_clear", llvm::FunctionType::get(m_builder->getVoidTy(), { listPtr }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_list_remove()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0);
    return resolveFunction("list_remove", llvm::FunctionType::get(m_builder->getVoidTy(), { listPtr, m_builder->getInt64Ty() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_list_append_empty()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0);
    return resolveFunction("list_append_empty", llvm::FunctionType::get(m_valueDataType->getPointerTo(), { listPtr }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_list_insert_empty()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0);
    return resolveFunction("list_insert_empty", llvm::FunctionType::get(m_valueDataType->getPointerTo(), { listPtr, m_builder->getInt64Ty() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_list_data_ptr()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0);
    llvm::FunctionCallee callee = resolveFunction("list_data_ptr", llvm::FunctionType::get(m_valueDataType->getPointerTo()->getPointerTo(), { listPtr }, false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_list_size_ptr()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0);
    llvm::FunctionCallee callee = resolveFunction("list_size_ptr", llvm::FunctionType::get(m_builder->getInt64Ty()->getPointerTo()->getPointerTo(), { listPtr }, false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_list_alloc_size_ptr()
{
    llvm::Type *listPtr = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0);
    llvm::FunctionCallee callee = resolveFunction("list_alloc_size_ptr", llvm::FunctionType::get(m_builder->getInt64Ty()->getPointerTo()->getPointerTo(), { listPtr }, false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_list_to_string()
{
    llvm::Type *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0);
    llvm::FunctionCallee callee = resolveFunction("list_to_string", llvm::FunctionType::get(m_builder->getVoidTy(), { pointerType, m_stringPtrType->getPointerTo() }, false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_llvm_random()
{
    llvm::Type *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0);
    llvm::Type *valuePtr = m_valueDataType->getPointerTo();
    return resolveFunction("llvm_random", llvm::FunctionType::get(m_builder->getDoubleTy(), { pointerType, valuePtr, valuePtr }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_llvm_random_double()
{
    llvm::Type *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0);
    return resolveFunction("llvm_random_double", llvm::FunctionType::get(m_builder->getDoubleTy(), { pointerType, m_builder->getDoubleTy(), m_builder->getDoubleTy() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_llvm_random_int64()
{
    llvm::Type *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0);
    return resolveFunction("llvm_random_int64", llvm::FunctionType::get(m_builder->getInt64Ty(), { pointerType, m_builder->getInt64Ty(), m_builder->getInt64Ty() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_llvm_random_bool()
{
    llvm::Type *pointerType = llvm::PointerType::get(llvm::Type::getInt8Ty(*m_ctx->llvmCtx()), 0);
    return resolveFunction("llvm_random_bool", llvm::FunctionType::get(m_builder->getDoubleTy(), { pointerType, m_builder->getInt1Ty(), m_builder->getInt1Ty() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_string_pool_new()
{
    return resolveFunction("string_pool_new", llvm::FunctionType::get(m_stringPtrType->getPointerTo(), { m_builder->getInt1Ty() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_string_pool_free()
{
    return resolveFunction("string_pool_free", llvm::FunctionType::get(m_builder->getVoidTy(), { m_stringPtrType->getPointerTo() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_string_alloc()
{
    return resolveFunction("string_alloc", llvm::FunctionType::get(m_builder->getVoidTy(), { m_stringPtrType->getPointerTo(), m_builder->getInt64Ty() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_string_assign()
{
    return resolveFunction("string_assign", llvm::FunctionType::get(m_builder->getVoidTy(), { m_stringPtrType->getPointerTo(), m_stringPtrType->getPointerTo() }, false));
}

llvm::FunctionCallee LLVMFunctions::resolve_string_compare_case_sensitive()
{
    llvm::Type *stringPtr = m_stringPtrType->getPointerTo();
    llvm::FunctionCallee callee = resolveFunction("string_compare_case_sensitive", llvm::FunctionType::get(m_builder->getInt32Ty(), { stringPtr, stringPtr }, false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}

llvm::FunctionCallee LLVMFunctions::resolve_string_compare_case_insensitive()
{
    llvm::Type *stringPtr = m_stringPtrType->getPointerTo();
    llvm::FunctionCallee callee = resolveFunction("string_compare_case_insensitive", llvm::FunctionType::get(m_builder->getInt32Ty(), { stringPtr, stringPtr }, false));
    llvm::Function *func = llvm::cast<llvm::Function>(callee.getCallee());
    func->addFnAttr(llvm::Attribute::ReadOnly);
    return callee;
}
