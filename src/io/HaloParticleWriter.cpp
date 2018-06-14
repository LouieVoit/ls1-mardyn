#include "io/HaloParticleWriter.h"

#include <sstream>
#include <string>

#include "Common.h"
#include "Domain.h"
#include "utils/Logger.h"
#include "parallel/DomainDecompBase.h"

using Log::global_log;
using namespace std;


void HaloParticleWriter::readXML(XMLfileUnits& xmlconfig) {
	_writeFrequency = 1;
	xmlconfig.getNodeValue("writefrequency", _writeFrequency);
	global_log->info() << "Write frequency: " << _writeFrequency << endl;

	if(_writeFrequency == 0) {
		global_log->error() << "Write frequency must be a positive nonzero integer, but is " << _writeFrequency << endl;
		Simulation::exit(-1);
	}
	
	std::string HaloParticleType = "unknown";
	xmlconfig.getNodeValue("type", HaloParticleType);

	_outputPrefix = "mardyn";
	xmlconfig.getNodeValue("outputprefix", _outputPrefix);
	global_log->info() << "Output prefix: " << _outputPrefix << endl;

	int incremental = 1;
	xmlconfig.getNodeValue("incremental", incremental);
	_incremental = (incremental != 0);
	global_log->info() << "Incremental numbers: " << _incremental << endl;

	int appendTimestamp = 0;
	xmlconfig.getNodeValue("appendTimestamp", appendTimestamp);
	if(appendTimestamp > 0) {
		_appendTimestamp = true;
	}else{
		_appendTimestamp = false;
	}
	global_log->info() << "Append timestamp: " << _appendTimestamp << endl;
}

void HaloParticleWriter::init(ParticleContainer * /*particleContainer*/, DomainDecompBase * /*domainDecomp*/,
                            Domain * /*domain*/) {}

void HaloParticleWriter::afterForces(ParticleContainer *particleContainer, DomainDecompBase* domainDecomp,
                               unsigned long simstep) {
	if( simstep % _writeFrequency == 0 ) {
		stringstream filenamestream;
		filenamestream << _outputPrefix << "-rank" << domainDecomp->getRank();

		if(_incremental) {
			/* align file numbers with preceding '0's in the required range from 0 to _numberOfTimesteps. */
			unsigned long numTimesteps = _simulation.getNumTimesteps();
			int num_digits = (int) ceil( log( double( numTimesteps / _writeFrequency ) ) / log(10.) );
			filenamestream << "-" << aligned_number( simstep / _writeFrequency, num_digits, '0' );
		}
		if(_appendTimestamp) {
			filenamestream << "-" << gettimestring();
		}

		filenamestream << ".restart.dat";

		string filename = filenamestream.str();

		//global_simulation->getDomain()->writeCheckpoint(filename, particleContainer, domainDecomp, _simulation.getSimulationTime(), false);
		double rmin[3],rmax[3];
		domainDecomp->getBoundingBoxMinMax(global_simulation->getDomain(), rmin,
				rmax);
		std::ofstream checkpointfilestream;
		checkpointfilestream.open(filename.c_str());
		checkpointfilestream.precision(20);
		ParticleIterator tempMolecule;
		for (tempMolecule = particleContainer->iterator();
				tempMolecule.hasNext(); tempMolecule.next()) {
			double r[3];
			for (int i = 0; i < 3; i++) {
				r[i] = tempMolecule->r(i);
			}
			if ((r[0] < rmin[0] or r[0] >= rmax[0])
					or (r[1] < rmin[1] or r[1] >= rmax[1])
					or (r[2] < rmin[2] or r[2] >= rmax[2])) {
				checkpointfilestream <<"cell "<< tempMolecule.getCellIndex() << ": ";
				tempMolecule->write(checkpointfilestream);
			}
		}
		checkpointfilestream.close();

	}
}

void HaloParticleWriter::finish(ParticleContainer * /*particleContainer*/, DomainDecompBase * /*domainDecomp*/,
							  Domain * /*domain*/) {
}
