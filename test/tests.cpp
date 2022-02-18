#include <gtest/gtest.h>

unsigned int Factorial(unsigned int number) // NOLINT(misc-no-recursion)
{
    return number <= 1 ? number : Factorial(number - 1) * number;
}

class FooTest : public ::testing::Test {
protected:
    FooTest() {
        // You can do set-up work for each test here.
    }

    ~FooTest() override {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    void SetUp() override {
    }

    void TearDown() override {
    }

    // Class members declared here can be used by all tests in the test suite
    // for Foo.
};

TEST_F(FooTest, FactorialTest) {
    ASSERT_TRUE(Factorial(1) == 1);
    ASSERT_TRUE(Factorial(2) == 2);
    ASSERT_TRUE(Factorial(3) == 6);
    ASSERT_TRUE(Factorial(10) == 3628800);
}