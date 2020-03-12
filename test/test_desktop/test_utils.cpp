#include <unity.h>
#include <utils.h>
#include <stdio.h>

#define MAX_ANALOG_READ 439.0
#define MIN_ANALOG_READ 0.0

void test_round(void) 
{
	TEST_ASSERT_EQUAL_FLOAT(0.01, round_x(0.0141, 2));
	TEST_ASSERT_EQUAL_FLOAT(0.005, round_x(0.0051, 3));
	TEST_ASSERT_EQUAL_FLOAT(0.0, round_x(0.123751, 0));
	TEST_ASSERT_EQUAL_FLOAT(0.2, round_x(0.153751, 1));
}

void test_normalize()
{
	TEST_ASSERT_EQUAL_FLOAT(0.0, normalize(MAX_ANALOG_READ));
	TEST_ASSERT_EQUAL_FLOAT(1.0, normalize(MIN_ANALOG_READ));
	TEST_ASSERT_EQUAL_FLOAT(0.5, normalize(MAX_ANALOG_READ / 2));
	TEST_ASSERT_EQUAL_FLOAT(0.5, normalize(MAX_ANALOG_READ / 2 + 0.00123));
}

int main()
{
	UNITY_BEGIN();
	RUN_TEST(test_round);
	RUN_TEST(test_normalize);
	UNITY_END();
}