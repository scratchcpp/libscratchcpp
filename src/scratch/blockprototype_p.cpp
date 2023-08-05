// SPDX-License-Identifier: Apache-2.0

#include "blockprototype_p.h"

using namespace libscratchcpp;

BlockPrototypePrivate::BlockPrototypePrivate()
{
}

BlockPrototypePrivate::BlockPrototypePrivate(const std::string &procCode)
{
    setProcCode(procCode);
}

void BlockPrototypePrivate::setProcCode(const std::string &newProcCode)
{
    procCode = newProcCode;
    argumentDefaults.clear();
    argumentTypes.clear();
    bool arg = false;

    for (auto c : procCode) {
        if (c == '%')
            arg = true;
        else if (arg) {
            arg = false;
            switch (c) {
                case 's':
                    argumentDefaults.push_back("");
                    argumentTypes.push_back(BlockPrototype::ArgType::StringNum);
                    break;

                case 'b':
                    argumentDefaults.push_back(false);
                    argumentTypes.push_back(BlockPrototype::ArgType::Bool);
                    break;

                default:
                    break;
            }
        }
    }
}
