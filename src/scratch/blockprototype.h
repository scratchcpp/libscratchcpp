// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "value.h"
#include <vector>

namespace libscratchcpp
{

/*! \brief The BlockPrototype class represents the prototype of a custom block. */
class LIBSCRATCHCPP_EXPORT BlockPrototype
{
    public:
        enum class ArgType
        {
            StringNum,
            Bool
        };

        BlockPrototype();
        BlockPrototype(const std::string &procCode);

        const std::string &procCode() const;
        void setProcCode(const std::string &newProcCode);

        const std::vector<std::string> &argumentIds() const;
        void setArgumentIds(const std::vector<std::string> &newArgumentIds);

        const std::vector<std::string> &argumentNames() const;
        void setArgumentNames(const std::vector<std::string> &newArgumentNames);

        const std::vector<Value> &argumentDefaults() const;

        const std::vector<ArgType> &argumentTypes() const;

        bool warp() const;
        void setWarp(bool newWarp);

    private:
        std::string m_procCode;
        std::vector<std::string> m_argumentIds;
        std::vector<std::string> m_argumentNames;
        std::vector<Value> m_argumentDefaults;
        std::vector<ArgType> m_argumentTypes;
        bool m_warp = false;
};

} // namespace libscratchcpp
