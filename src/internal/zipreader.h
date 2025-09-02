// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <zip.h>

#include "test_export.h"

namespace libscratchcpp
{

class LIBSCRATCHCPP_TEST_EXPORT ZipReader
{
    public:
        ZipReader(const std::string &fileName);
        ZipReader(const char *fileName);
        ZipReader(const ZipReader &) = delete;
        ~ZipReader();

        bool open();
        void close();

        size_t readFile(const std::string &fileName, void **buf);
        void readFileToString(const std::string &fileName, std::string &dst);

    private:
        std::string m_fileName;
        struct zip_t *m_zip = nullptr;
};

} // namespace libscratchcpp
