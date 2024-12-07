// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../global.h"
#include "../spimpl.h"

namespace libscratchcpp
{

class PromisePrivate;

/*! \brief The Promise class represents the eventual completion of an asynchronous operation. */
class LIBSCRATCHCPP_EXPORT Promise
{
    public:
        Promise();
        Promise(const Promise &) = delete;

        bool isResolved() const;
        void resolve();

    private:
        spimpl::unique_impl_ptr<PromisePrivate> impl;
};

} // namespace libscratchcpp
