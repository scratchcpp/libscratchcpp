// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace libscratchcpp
{

class ICodeBuilder;
class Target;

class ICodeBuilderFactory
{
    public:
        virtual ~ICodeBuilderFactory() { }

        virtual std::shared_ptr<ICodeBuilder> create(Target *target, const std::string &id, bool warp) const = 0;
};

} // namespace libscratchcpp
