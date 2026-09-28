#include <gtest/gtest.h>

#include <Cudev/Cudev.h>
#include <Cudev/Core/Result.h>

TEST(CudevTests, StubTest)
{
	SUCCEED();
}

TEST(CudevTests, ResultStoresSuccessfulValue)
{
	const auto result = Cudev::Result<int>::success(42, "ready");

	EXPECT_TRUE(result.succeeded());
	EXPECT_FALSE(result.failed());
	EXPECT_EQ(result.status(), Cudev::ResultStatus::Success);
	EXPECT_EQ(result.value(), 42);
	EXPECT_EQ(result.message(), "ready");
}

TEST(CudevTests, ResultStoresFailureMessage)
{
	const auto result = Cudev::Result<int>::failure("invalid sequence");

	EXPECT_FALSE(result.succeeded());
	EXPECT_TRUE(result.failed());
	EXPECT_EQ(result.status(), Cudev::ResultStatus::Failure);
	EXPECT_EQ(result.message(), "invalid sequence");
}

TEST(CudevTests, VoidResultStoresStatusAndMessage)
{
	const auto result = Cudev::Result<void>::success("complete");

	EXPECT_TRUE(result.succeeded());
	EXPECT_EQ(result.status(), Cudev::ResultStatus::Success);
	EXPECT_EQ(result.message(), "complete");
}