// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace libscratchcpp
{

class ICodeBuilder;

class ICodeBuilderFactory
{
    public:
        virtual ~ICodeBuilderFactory() { }

        virtual std::shared_ptr<ICodeBuilder> create(const std::string &id) const = 0;
};

} // namespace libscratchcpp
