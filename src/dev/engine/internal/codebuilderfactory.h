// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "icodebuilderfactory.h"

namespace libscratchcpp
{

class CodeBuilderFactory : public ICodeBuilderFactory
{
    public:
        static std::shared_ptr<CodeBuilderFactory> instance();
        std::shared_ptr<ICodeBuilder> create(const std::string &id, bool warp) const override;

    private:
        static std::shared_ptr<CodeBuilderFactory> m_instance;
};

} // namespace libscratchcpp
