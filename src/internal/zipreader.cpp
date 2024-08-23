// SPDX-License-Identifier: Apache-2.0

#include "zipreader.h"

using namespace libscratchcpp;

ZipReader::ZipReader(const std::string &fileName) :
    m_fileName(fileName)
{
}

ZipReader::ZipReader(const char *fileName) :
    ZipReader(std::string(fileName))
{
}

ZipReader::~ZipReader()
{
    close();
}

bool ZipReader::open()
{
    m_zip = zip_open(m_fileName.c_str(), 0, 'r');
    return m_zip;
}

void ZipReader::close()
{
    if (m_zip)
        zip_close(m_zip);

    m_zip = nullptr;
}

size_t ZipReader::readFile(const std::string &fileName, void **buf)
{
    if (!m_zip) {
        buf = nullptr;
        return 0;
    }

    size_t bufsize = 0;
    zip_entry_open(m_zip, fileName.c_str());
    zip_entry_read(m_zip, buf, &bufsize);
    zip_entry_close(m_zip);

    return bufsize;
}

void ZipReader::readFileToString(const std::string &fileName, std::string &dst)
{
    void *buf = nullptr;
    size_t bufsize = readFile(fileName, &buf);

    if (buf) {
        dst.assign(reinterpret_cast<char *>(buf), bufsize);
        free(buf);
    } else
        dst = "";
}
