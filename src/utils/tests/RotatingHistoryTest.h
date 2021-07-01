#pragma once

#include "../Testing.h"

class RotatingHistoryTest : public utils::Test {
	TEST_SUITE(RotatingHistoryTest);
	TEST_METHOD(testDefaultConstructed);
	TEST_METHOD(testFiveElements);
	TEST_METHOD(testZeroSize);
	TEST_SUITE_END();

public:
	static void testDefaultConstructed();
	static void testFiveElements();
	static void testZeroSize();
};
