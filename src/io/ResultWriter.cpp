#include "io/ResultWriter.h"

#include "Domain.h"
#include "Simulation.h"
#include "parallel/DomainDecompBase.h"
#include "utils/Logger.h"

#include <fstream>

using Log::global_log;

void ResultWriter::readXML(XMLfileUnits& xmlconfig) {
	xmlconfig.getNodeValue("writefrequency", _writeFrequency);
	global_log->info() << "[ResultWriter] Write frequency: " << _writeFrequency << std::endl;

	xmlconfig.getNodeValue("outputprefix", _outputPrefix);
	global_log->info() << "[ResultWriter] Output prefix: " << _outputPrefix << std::endl;

	xmlconfig.getNodeValue("writeprecision", _writePrecision);
	global_log->info() << "[ResultWriter] Write precision: " << _writePrecision << std::endl;
	_writeWidth = _writePrecision + 15; // Adding a width of 15 to have enough whitespace between chars
}

void ResultWriter::init(ParticleContainer * /*particleContainer*/,
                        DomainDecompBase *domainDecomp, Domain * /*domain*/) {

	// Only main rank writes data to file
	if(domainDecomp->getRank() == 0) {
		const string resultfile(_outputPrefix+".res");
		std::ofstream resultStream;
		resultStream.open(resultfile.c_str(), std::ios::out);
		resultStream << std::setw(10) << "timestep"
			<< std::setw(_writeWidth) << "time"
			<< std::setw(_writeWidth) << "U_pot_avg"
			<< std::setw(_writeWidth) << "U_kin_avg"
			<< std::setw(_writeWidth) << "U_kinTrans_avg"
			<< std::setw(_writeWidth) << "U_kinRot_avg"
			<< std::setw(_writeWidth) << "p_avg"
			<< std::setw(_writeWidth) << "c_v"
			<< std::setw(_writeWidth) << "N"
			<< std::endl;
		resultStream.close();
	}
}

void ResultWriter::endStep(ParticleContainer *particleContainer, DomainDecompBase *domainDecomp, Domain *domain,
                           unsigned long simstep) {

	const uint64_t globalNumMolecules = domain->getglobalNumMolecules(true, particleContainer, domainDecomp);

	_uPot_acc += domain->getGlobalUpot();
	_p_acc += domain->getGlobalPressure();

	double uKinLocal = 0.0F;
	double uKinGlobal = 0.0F;
	double uKinTransLocal = 0.0F;
	double uKinTransGlobal = 0.0F;
	double uKinRotLocal = 0.0F;
	double uKinRotGlobal = 0.0F;
	#if defined(_OPENMP)
	#pragma omp parallel reduction(+:uKinLocal,uKinTransLocal,uKinRotLocal)
	#endif
	{
		for (auto moleculeIter = particleContainer->iterator(ParticleIterator::ONLY_INNER_AND_BOUNDARY);
			 moleculeIter.isValid(); ++moleculeIter) {
			uKinLocal += moleculeIter->U_kin();
			uKinTransLocal += moleculeIter->U_trans();
			uKinRotLocal += moleculeIter->U_rot();
		}
	}
#ifdef ENABLE_MPI
    MPI_Allreduce(&uKinLocal, &uKinGlobal, 1, MPI_DOUBLE, MPI_SUM, domainDecomp->getCommunicator());
	MPI_Allreduce(&uKinTransLocal, &uKinTransGlobal, 1, MPI_DOUBLE, MPI_SUM, domainDecomp->getCommunicator());
	MPI_Allreduce(&uKinRotLocal, &uKinRotGlobal, 1, MPI_DOUBLE, MPI_SUM, domainDecomp->getCommunicator());
#else
    uKinGlobal = uKinLocal;
	uKinTransGlobal = uKinTransLocal;
	uKinRotGlobal = uKinRotLocal;
#endif
	_uKin_acc += uKinGlobal;
	_uKinTrans_acc += uKinTransGlobal;
	_uKinRot_acc += uKinRotGlobal;

	_numSamples++;

	if ((simstep % _writeFrequency == 0) and (simstep > 0UL)) {
		// Only main rank writes data to file
		if (domainDecomp->getRank() == 0) {
			const string resultfile(_outputPrefix+".res");
			std::ofstream resultStream;
			resultStream.open(resultfile.c_str(), std::ios::app);
			auto printOutput = [&](auto value) {
				resultStream << std::setw(_writeWidth) << std::scientific << std::setprecision(_writePrecision) << value;
			};
			resultStream << std::setw(10) << simstep;
				printOutput(_simulation.getSimulationTime());
				printOutput(_uPot_acc/_numSamples);
				printOutput(_uKin_acc/_numSamples);
				printOutput(_uKinTrans_acc/_numSamples);
				printOutput(_uKinRot_acc/_numSamples);
				printOutput(_p_acc/_numSamples);
				printOutput(domain->cv());
				printOutput(globalNumMolecules);
				resultStream << std::endl;
			resultStream.close();
		}

		// Reset values
		_numSamples = 0UL;
		_uPot_acc = 0.0F;
		_uKin_acc = 0.0F;
		_uKinTrans_acc = 0.0F;
		_uKinRot_acc = 0.0F;
		_p_acc = 0.0F;
	}
}
