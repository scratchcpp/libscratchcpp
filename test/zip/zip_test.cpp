#include <internal/zipreader.h>
#include "../common.h"

using namespace libscratchcpp;

inline std::string readSb3Json(const std::string &fileName)
{
    ZipReader reader(fileName);
    reader.open();

    return reader.readFileToString("project.json");
}

TEST(ZipTest, NonexistentProject)
{
    ASSERT_TRUE(readSb3Json("idontexist.sb3").empty());
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
