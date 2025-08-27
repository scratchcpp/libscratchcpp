// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <llvm/IR/IRBuilder.h>

namespace libscratchcpp
{

class LLVMCompilerContext;

class LLVMFunctions
{
    public:
        LLVMFunctions(LLVMCompilerContext *ctx, llvm::IRBuilder<> *builder);

        llvm::FunctionCallee resolveFunction(const std::string name, llvm::FunctionType *type);

        llvm::FunctionCallee resolve_value_init();
        llvm::FunctionCallee resolve_value_free();
        llvm::FunctionCallee resolve_value_assign_long();
        llvm::FunctionCallee resolve_value_assign_double();
        llvm::FunctionCallee resolve_value_assign_bool();
        llvm::FunctionCallee resolve_value_assign_stringPtr();
        llvm::FunctionCallee resolve_value_assign_special();
        llvm::FunctionCallee resolve_value_assign_copy();
        llvm::FunctionCallee resolve_value_toDouble();
        llvm::FunctionCallee resolve_value_toBool();
        llvm::FunctionCallee resolve_value_toStringPtr();
        llvm::FunctionCallee resolve_value_doubleToStringPtr();
        llvm::FunctionCallee resolve_value_boolToStringPtr();
        llvm::FunctionCallee resolve_value_stringToDouble();
        llvm::FunctionCallee resolve_value_stringToDoubleWithCheck();
        llvm::FunctionCallee resolve_value_stringToBool();
        llvm::FunctionCallee resolve_value_equals();
        llvm::FunctionCallee resolve_value_greater();
        llvm::FunctionCallee resolve_value_lower();
        llvm::FunctionCallee resolve_list_clear();
        llvm::FunctionCallee resolve_list_remove();
        llvm::FunctionCallee resolve_list_append_empty();
        llvm::FunctionCallee resolve_list_insert_empty();
        llvm::FunctionCallee resolve_list_data_ptr();
        llvm::FunctionCallee resolve_list_size_ptr();
        llvm::FunctionCallee resolve_list_alloc_size_ptr();
        llvm::FunctionCallee resolve_list_to_string();
        llvm::FunctionCallee resolve_llvm_random();
        llvm::FunctionCallee resolve_llvm_random_double();
        llvm::FunctionCallee resolve_llvm_random_int64();
        llvm::FunctionCallee resolve_llvm_random_bool();
        llvm::FunctionCallee resolve_string_pool_new();
        llvm::FunctionCallee resolve_string_pool_free();
        llvm::FunctionCallee resolve_string_alloc();
        llvm::FunctionCallee resolve_string_assign();
        llvm::FunctionCallee resolve_string_compare_case_sensitive();
        llvm::FunctionCallee resolve_string_compare_case_insensitive();

    private:
        LLVMCompilerContext *m_ctx = nullptr;
        llvm::IRBuilder<> *m_builder = nullptr;

        llvm::StructType *m_stringPtrType = nullptr;
        llvm::StructType *m_valueDataType = nullptr;
};

} // namespace libscratchcpp
