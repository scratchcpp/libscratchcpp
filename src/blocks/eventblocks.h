// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../engine/iblocksection.h"

namespace libscratchcpp
{

/*! \brief The EventBlocks class contains the implementation of event blocks. */
class LIBSCRATCHCPP_EXPORT EventBlocks : public IBlockSection
{
    public:
        enum Inputs
        {
            BROADCAST_INPUT
        };

        enum Fields
        {
            BROADCAST_OPTION
        };

        EventBlocks();

        std::string name() const override;

        static void compileBroadcast(Compiler *compiler);
        static void compileWhenBroadcastReceived(Compiler *compiler);

    private:
        static unsigned int broadcast(VirtualMachine *vm);
        static unsigned int broadcastByIndex(VirtualMachine *vm);
};

} // namespace libscratchcpp
