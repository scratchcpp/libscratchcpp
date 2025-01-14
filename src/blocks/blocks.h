// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <memory>

namespace libscratchcpp
{

class IExtension;

class Blocks
{
    public:
        static const std::vector<std::shared_ptr<IExtension>> &extensions();

    private:
        Blocks();
        void registerExtensions();

        static Blocks m_instance; // use static initialization
        std::vector<std::shared_ptr<IExtension>> m_extensions;
};

} // namespace libscratchcpp
