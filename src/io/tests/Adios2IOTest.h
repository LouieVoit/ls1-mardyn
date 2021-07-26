/*
 * Adios2IOTest.h
 *
 * Check whether an ADIOS2 checkpoint can be successfully written and read again.
 *
 *  Created on: July 2021
 *      Author: Heinen, Gralka, Rau
 */
#pragma once

#include "utils/TestWithSimulationSetup.h"
#include "molecules/Component.h"
#include <vector>
#include <array>

class Adios2IOTest : public utils::TestWithSimulationSetup {

	// declare testsuite inputFileTest
	TEST_SUITE(Adios2IOTest);

	// add a method which perform test
	TEST_METHOD(testWriteCheckpoint);

	// add a method which perform test
	TEST_METHOD(testReadCheckpoint);
	
	// add a method which perform test
	TEST_METHOD(checkInput);

	// end suite declaration
	TEST_SUITE_END();

public:
	Adios2IOTest() = default;
	virtual ~Adios2IOTest() = default;

	void initParticles();

	void testWriteCheckpoint();

	void testReadCheckpoint();

	void checkInput();
private:

	std::vector<unsigned long> _ids;
	std::vector<std::array<double,3>> _positions;
	std::vector<std::array<double,3>> _velocities;
	std::vector<std::array<double,3>> _Ds;
	std::vector<std::array<double,4>> _quaternions;
	std::array<double,3> _box_lower;
	std::array<double,3> _box_upper;

	Component _comp;
};
