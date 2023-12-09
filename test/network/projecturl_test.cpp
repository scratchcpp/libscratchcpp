#include <internal/projecturl.h>

#include "../common.h"

using namespace libscratchcpp;

TEST(ProjectUrlTest, Parse)
{
    {
        ProjectUrl url("");
        ASSERT_FALSE(url.isProjectUrl());
        ASSERT_TRUE(url.projectId().empty());
    }

    {
        ProjectUrl url("scratch.mit.edu/projects/886166351");
        ASSERT_TRUE(url.isProjectUrl());
        ASSERT_EQ(url.projectId(), "886166351");
    }

    {
        ProjectUrl url("/scratch.mit.edu/projects/886166351");
        ASSERT_TRUE(url.isProjectUrl());
        ASSERT_EQ(url.projectId(), "886166351");
    }

    {
        ProjectUrl url("//scratch.mit.edu/projects/886166351/");
        ASSERT_TRUE(url.isProjectUrl());
        ASSERT_EQ(url.projectId(), "886166351");
    }

    {
        ProjectUrl url("https://scratch.mit.edu/projects/886166351");
        ASSERT_TRUE(url.isProjectUrl());
        ASSERT_EQ(url.projectId(), "886166351");
    }

    {
        ProjectUrl url("https:///http:/scratch.mit.edu//projects/886166351/");
        ASSERT_TRUE(url.isProjectUrl());
        ASSERT_EQ(url.projectId(), "886166351");
    }

    {
        ProjectUrl url("http://scratch.mit.edu/projects/886166351");
        ASSERT_TRUE(url.isProjectUrl());
        ASSERT_EQ(url.projectId(), "886166351");
    }

    {
        ProjectUrl url("https:/scratch.mit.edu/projects/886166351");
        ASSERT_TRUE(url.isProjectUrl());
        ASSERT_EQ(url.projectId(), "886166351");
    }

    {
        ProjectUrl url("https://scratch.mit.edu/projects/test/886166351");
        ASSERT_FALSE(url.isProjectUrl());
        ASSERT_TRUE(url.projectId().empty());
    }
}
