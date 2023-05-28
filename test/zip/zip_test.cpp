#include <gtest/gtest.h>
#include <zip.h>
#include <filesystem>
#include "../common.h"

std::string readSb3Json(const std::string &fileName)
{
    // TODO: Move this to a class and use it in Scratch3Reader
    // Open the zip file
    unsigned char *buf;
    size_t bufsize;
    struct zip_t *zip = zip_open(fileName.c_str(), 0, 'r');

    // Extract project.json
    zip_entry_open(zip, "project.json");
    bufsize = zip_entry_size(zip);
    buf = (unsigned char *)calloc(sizeof(unsigned char), bufsize);
    zip_entry_noallocread(zip, (void *)buf, bufsize);
    zip_entry_close(zip);
    std::string str(reinterpret_cast<char const *>(buf));
    free(buf);

    // Remove garbage after the JSON
    int end;
    for (end = str.size(); end >= 0; end--) {
        char ch = str[end];
        if (ch == '}') {
            break;
        }
    }
    return str.substr(0, end + 1);
}

TEST(ZipTest, EmptyProject)
{
    ASSERT_EQ(readSb3Json("empty_project.sb3"), readFileStr("empty_project.json"));
}

TEST(ZipTest, DefaultProject)
{
    ASSERT_EQ(readSb3Json("default_project.sb3"), readFileStr("default_project.json"));
}

TEST(ZipTest, BubbleSort)
{
    ASSERT_EQ(readSb3Json("bubble_sort.sb3"), readFileStr("bubble_sort.json"));
}

TEST(ZipTest, FileManager)
{
    ASSERT_EQ(readSb3Json("file_manager.sb3"), readFileStr("file_manager.json"));
}
