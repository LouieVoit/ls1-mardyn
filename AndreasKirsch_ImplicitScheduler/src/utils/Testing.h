/*
 * Testing.h
 *
 * This file is used to encapsulate the testframework used (especially, I want to
 * be able to switch from CppUnit, which should be default, to the one used within
 * the peano project).
 *
 * @Date: 26.07.2010
 * @Author: Wolfgang Eckhardt
 *
 */

#ifndef TESTING_H_
#define TESTING_H_


//! execute unit tests
//! @return false if no errors occured, true otherwise
bool runTests();

#ifdef UNIT_TESTS

#define USE_CPPUNIT

#ifdef USE_CPPUNIT

#include <cppunit/extensions/HelperMacros.h>

namespace utils {
	typedef CppUnit::TestFixture Test;
}


#define TEST_SUITE_REGISTRATION(name) CPPUNIT_TEST_SUITE_REGISTRATION(name)

#define TEST_SUITE(name) CPPUNIT_TEST_SUITE(name)
#define TEST_METHOD(name) CPPUNIT_TEST(name)
#define TEST_SUITE_END CPPUNIT_TEST_SUITE_END()

/***************  Assertion Macros  ****************/

/**
 * Assert that "expression" evaluates to true.
 * @see CPPUNIT_ASSERT
 */
#define ASSERT_TRUE(expression) CPPUNIT_ASSERT(expression)

/**
 * Assert that "expression" evaluates to true. If it is not, message msg is being output additionally to
 * the diagnostics of the assertion.
 * @see CPPUNIT_ASSERT_MESSAGE
 */
#define ASSERT_TRUE_MSG(msg, expression) CPPUNIT_ASSERT_MESSAGE(msg, expression)

#define ASSERT_FAIL(message) CPPUNIT_FAIL( message )

/**
 * Assert that a == b
 * @see CPPUNIT_ASSERT_EQUAL
 */
#define ASSERT_EQUAL(a,b) CPPUNIT_ASSERT_EQUAL(a,b)

/**
 * Assert that a == b. If it is not, message msg is being output additionally to
 * the diagnostics of the assertion.
 * @see CPPUNIT_ASSERT_EQUAL_MESSAGE
 */
#define ASSERT_EQUAL_MSG(msg, a, b) CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, a, b)

/**
 * Assert that two double are equals given a tolerance.
 * @see CPPUNIT_ASSERT_DOUBLES_EQUAL
 */
#define ASSERT_DOUBLES_EQUAL(expected,actual,delta) CPPUNIT_ASSERT_DOUBLES_EQUAL(expected,actual,delta)

/**
 * Assert that two double are equals given a tolerance. If they are not,
 * message msg is being output additionally to the diagnostics of the assertion.
 * @see CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE
 */
#define ASSERT_DOUBLES_EQUAL_MSG(expected,actual,delta) CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(expected,actual,delta)

#else /***********  DEFINITIONS TO USE THE PEANO Test-Package    ****/



#include <tarch/tests/TestCase.h>
#include <tarch/tests/TestCaseFactory.h>

namespace utils {
	typedef tarch::tests::TestCase Test;
}


#define TEST_SUITE_REGISTRATION(name) registerTest(name)

#define TEST_SUITE(name) void run() {
#define TEST_METHOD(name) testMethod(name)
#define TEST_SUITE_END() }

/***************  Assertion Macros  ****************/

/**
 * Assert that "expression" evaluates to true.
 * @see CPPUNIT_ASSERT
 */
#define ASSERT_TRUE(expression) validate(expression)

/**
 * Assert that "expression" evaluates to true. If it is not, message msg is being output additionally to
 * the diagnostics of the assertion.
 * @see CPPUNIT_ASSERT_MESSAGE
 */
#define ASSERT_TRUE_MSG(msg, expression) validateWithMessage(expression, msg)

#define ASSERT_FAIL(message) validateWithMessage(false, msg)

/**
 * Assert that a == b
 * @see CPPUNIT_ASSERT_EQUAL
 */
#define ASSERT_EQUAL(a,b) validateEquals(a, b)

/**
 * Assert that a == b. If it is not, message msg is being output additionally to
 * the diagnostics of the assertion.
 * @see CPPUNIT_ASSERT_EQUAL_MESSAGE
 */
#define ASSERT_EQUAL_MSG(msg, a, b) validateEqualsWithMessage(a, b, msg)

/**
 * Assert that two double are equals given a tolerance.
 * @see CPPUNIT_ASSERT_DOUBLES_EQUAL
 */
#define ASSERT_DOUBLES_EQUAL(expected,actual,delta) validateNumericalEqualsWithEps(actual, expected, delta)

/**
 * Assert that two double are equals given a tolerance. If they are not,
 * message msg is being output additionally to the diagnostics of the assertion.
 * @see CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE
 */
#define ASSERT_DOUBLES_EQUAL_MSG(expected,actual,delta) validateNumericalEqualsWithEps(actual, expected, delta)


#endif /* USE_CPPUNIT */

#endif /* UNIT_TESTS */

#endif /* TESTING_H_ */
