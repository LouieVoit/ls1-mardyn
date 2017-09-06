/*
 * LoadCalc.cpp
 *
 *  Created on: 10.06.2017
 *      Author: simon
 */

#include "LoadCalc.h"


std::vector<double> TunerLoad::readVec(std::istream& in, int& count1, int& count2) {
	std::vector<double> vec;
	int tempCount1 = 0;
	int tempCount2 = -1;
	while (std::isdigit(in.peek())) {
		++tempCount1;
		std::string line;
		std::getline(in, line);
		auto pos = line.begin();
		int i = 0;

		while (true) {
			++i;
			auto new_pos = std::find(pos, line.end(), ';');
			auto subStr = std::string(pos, new_pos);
			vec.push_back(std::stod(subStr));
			if (new_pos == line.end()) {
				break;
			}
			pos = new_pos + 1;
		}

		if (tempCount2 == -1) {
			tempCount2 = i;
		} else {
			if (i != tempCount2) {
				Log::global_log->error_always_output()
						<< "The file contains data of 2D-vectors with a different amounts of elements in the second dimension!"
						<< std::endl;
				Log::global_log->error_always_output()
						<< "This means the files is corrupted. Please remove it (or disallow the tuner to read from inputfiles) before restarting!"
						<< std::endl;
				Simulation::exit(1);
			}
		}
	}
	count1 = tempCount1;
	count2 = tempCount2;
	return vec;
}

std::array<double, 3> TunerLoad::calcConsts(const std::vector<double>& timeVec, bool inner) {
	//take the median of the last 10% of the times
	std::array<double,3> consts {};
	//the blocks are here to avoid at least some copy paste mistakes
	{
		std::vector<double> temp1 {};
		for (int i = timeVec.size() / _count2 * 9 / 10;i < _count1;++i) {
			double quot = inner ? i * (i - 1)/2 : i * i;
			//if quot is zero (which can happen if i == 0 || (i == 1 && own)), than Infinity would be pushed into the vector
			//so this is caught before it happens
			if (quot == 0) {
				quot = 1;
			}
			temp1.push_back(acessVec(timeVec, i, 0) / quot);
		}
		//calculates the median
		std::sort(temp1.begin(), temp1.end());
		consts[0] = temp1.at(temp1.size() / 2);
	}
	{
		std::vector<double> temp2 {};
		for (int i = timeVec.size() / _count1 * 9 / 10;i < _count2;++i) {
			double quot = inner ? i * (i - 1)/2 : i * i;
			if (quot == 0) {
				quot = 1;
			}
			temp2.push_back(acessVec(timeVec, 0, i) / quot);
		}
		std::sort(temp2.begin(), temp2.end());
		consts[1] = temp2.at(temp2.size() / 2);
	}
	{
		std::vector<double> temp3 {};
		for (int index1 = timeVec.size() / _count2 * 9 / 10;index1 < _count1;++index1) {
			for (int index2 = timeVec.size() / _count1 * 9 / 10;index2 < _count2;++index2) {
				double fac1;
				double fac2;
				double quot;
				if (inner) {
					fac1 = index1 * (index1 - 1);
					fac2 = index2 * (index2 - 1);
					quot = index1 * index2;
				} else {
					fac1 = index1 * index1;
					fac2 = index2 * index2;
					quot = index1 * index2 * 2; // my own type 1 with neighbor type 2, my own type 2 with neighbor type 1
				}
				if (quot == 0) {
					quot = 1;
				}
				temp3.push_back((acessVec(timeVec, index1, index2) - fac1 * consts[0] - fac2 * consts[1]) / quot);
			}
		}
		std::sort(temp3.begin(), temp3.end());
		consts[2] = temp3.at(temp3.size() / 2);
	}
	return consts;
}

TunerLoad::TunerLoad(int count1, int count2,
		std::vector<double>&& ownTime, std::vector<double>&& faceTime,
		std::vector<double>&& edgeTime, std::vector<double>&& cornerTime) :
		_count1 { count1 }, _count2 { count2 },
		_ownTime { std::move(ownTime) }, _faceTime {std::move(faceTime) },
		_edgeTime { std::move(edgeTime) }, _cornerTime { std::move(cornerTime) },
		_ownConst ( calcConsts(_ownTime, true) ), _faceConst ( calcConsts(_faceTime, false) ),
		_edgeConst ( calcConsts(_edgeTime, false) ), _cornerConst ( calcConsts(_cornerTime, false) ) {

	if (_ownTime.size() != size_t(_count1 * _count2)) {
		global_log->error_always_output()
				<< "_edgeTime was initialized with the wrong size of "
				<< _ownTime.size() << " expected: " << _count1 * _count2;
	}

	if (_faceTime.size() != size_t(count1 * _count2)) {
		global_log->error_always_output()
				<< "_edgeTime was initialized with the wrong size of "
				<< _faceTime.size() << " expected: " << _count1 * _count2;
	}

	if (_edgeTime.size() != size_t(_count1 * _count2)) {
		global_log->error_always_output()
				<< "_edgeTime was initialized with the wrong size of "
				<< _edgeTime.size() << " expected: " << _count1 * _count2;
	}

	if (_cornerTime.size() != size_t(_count1 * _count2)) {
		global_log->error_always_output()
				<< "_edgeTime was initialized with the wrong size of "
				<< _cornerTime.size() << " expected: " << _count1 * _count2;
	}
}

TunerLoad TunerLoad::read(std::istream& stream){
	std::string inStr {};
	std::getline(stream, inStr);
	if(inStr != "Vectorization Tuner File"){
		Log::global_log->error() << "The tunerfile is corrupted! Missing header \"Vectorization Tuner File\"";
		Log::global_log->error() << "Please remove it or fix it before restarting!";
		Simulation::exit(1);
	}

	int count1;
	int count2;

	std::getline(stream, inStr);
	if(inStr != "own"){
		Log::global_log->error() << "The tunerfile is corrupted! Missing Section \"own\"";
		Log::global_log->error() << "Please remove it or fix it before restarting!";
		Simulation::exit(1);
	}
	auto ownTime = readVec(stream, count1, count2);
	std::getline(stream, inStr);

	if(inStr != "face"){
		Log::global_log->error() << "The tunerfile is corrupted! Missing Section \"face\"";
		Log::global_log->error() << "Please remove it or fix it before restarting!";
		Simulation::exit(1);
	}
	auto faceTime = readVec(stream, count1, count2);
	std::getline(stream, inStr);

	if(inStr != "edge"){
		Log::global_log->error() << "The tunerfile is corrupted! Missing Section \"edge\"";
		Log::global_log->error() << "Please remove it or fix it before restarting!";
		Simulation::exit(1);
	}
	auto edgeTime = readVec(stream, count1, count2);
	std::getline(stream, inStr);

	if(inStr != "corner"){
		Log::global_log->error() << "The tunerfile is corrupted! Missing Section \"corner\"";
		Log::global_log->error() << "Please remove it or fix it before restarting!";
		Simulation::exit(1);
	}
	auto cornerTime = readVec(stream, count1, count2);
	return TunerLoad {count1, count2, std::move(ownTime), std::move(faceTime), std::move(edgeTime), std::move(cornerTime)};
}
