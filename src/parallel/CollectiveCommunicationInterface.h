/*
 * CollectiveCommunicationInterface.hpp
 *
 *  Created on: May 2, 2017
 *      Author: seckler
 */

#pragma once

#include "CollectiveCommBaseInterface.h"

//! @brief This class provides an interface for the collective communication classes.
//! @details It provides all methods, that are NOT YET provided by CollectiveCommBaseInterface
//! @author Steffen Seckler
class CollectiveCommunicationInterface : public virtual CollectiveCommBaseInterface{
public:
	//! @brief virtual destructor
	virtual ~CollectiveCommunicationInterface() {
	}

	//! @brief allocate memory for the values to be sent, initialize counters for specific
	//! @param key The key of the collective communication, this key will be used
	//! @param communicator MPI communicator for the
	//! @param numValues number of values that shall be communicated
	virtual void init(MPI_Comm communicator, int numValues, int key = 0) = 0;

	//! Performs an all-reduce (sum), however values of previous iterations are permitted.
	//! By allowing values from previous iterations, overlapping communication is possible.
	//! One possible use case for this function is the reduction of slowly changing variables, e.g. the temperature.
	virtual void allreduceSumAllowPrevious() = 0;

	//! Get the MPI communicator
	//! @return MPI communicator
	virtual MPI_Comm getTopology() = 0;

};


