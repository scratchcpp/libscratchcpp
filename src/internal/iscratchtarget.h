// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../scratch/target.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_EXPORT IScratchTarget
{
    public:
        virtual ~IScratchTarget() { }
        virtual Target *target() const { return m_target; }
        virtual void setTarget(Target *target) final { m_target = target; }

        virtual void setCostume(const char *data) = 0;

    private:
        Target *m_target = nullptr;
};

} // namespace libscratchcpp
