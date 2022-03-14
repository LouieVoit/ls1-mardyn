/*
 * ChemPotSampling.cpp
 *
 *  Created on: Feb 2022
 *      Author: homes
 */

#include "ChemPotSampling.h"

#include "particleContainer/ParticleContainer.h"
#include "particleContainer/adapter/ParticlePairs2PotForceAdapter.h"
#include "utils/Random.h"

#include <math.h>


ChemPotSampling::ChemPotSampling()
    :
    _binwidth(1.0f),
    _factorNumTest(4.0f),
    _startSampling(0ul),
    _writeFrequency(10000ul),
    _samplefrequency(50ul),
    _stopSampling(1000000000ul),
    _lattice(true)
{}

ChemPotSampling::~ChemPotSampling() {}

void ChemPotSampling::init(ParticleContainer* /* particleContainer */, DomainDecompBase* domainDecomp, Domain* domain) {

    _globalBoxLength[0] = domain->getGlobalLength(0);
    _globalBoxLength[1] = domain->getGlobalLength(1);
    _globalBoxLength[2] = domain->getGlobalLength(2);

    _numBinsGlobal = static_cast<uint16_t>(_globalBoxLength[1]/_binwidth);
    if (_globalBoxLength[1]/_binwidth != static_cast<float>(_numBinsGlobal)) {
        global_log->error() << "[ChemPotSampling] Can not divide domain without remainder! Change binwidth" << std::endl;
        Simulation::exit(-1);
    }
    _slabVolume = _globalBoxLength[0]*_globalBoxLength[2]*_binwidth;

    if (_slabVolume < 1e-12) {
        global_log->error() << "[ChemPotSampling] Slab volume too small!" << std::endl;
        Simulation::exit(-1);
    }

    _chemPotSum.local.resize(_numBinsGlobal);
    _chemPotSum.global.resize(_numBinsGlobal);

    _countNTest.local.resize(_numBinsGlobal);
    _countNTest.global.resize(_numBinsGlobal);

    _countSamples.resize(_numBinsGlobal);

    _temperatureSumGlobal.resize(_numBinsGlobal);
    _temperatureWithDriftSumGlobal.resize(_numBinsGlobal);
    _numMoleculesSumGlobal.resize(_numBinsGlobal);

    // TODO: CAN CELL PROCESSOR CHANGE DURING SIMULATION???
    _cellProcessor = _simulation.getCellProcessor();
    // TODO: CAN PP HANDLER CHANGE DURING SIMULATION???
    _particlePairsHandler = new ParticlePairs2PotForceAdapter(*domain);
    // MolID is maximum possible number minus rank to prevent duplicate IDs
    // Always insert molecule of first component
    const unsigned long molID = std::numeric_limits<unsigned long>::max() - static_cast<unsigned long>(domainDecomp->getRank());
    _mTest = Molecule(molID, &_simulation.getEnsemble()->getComponents()->at(0));

    resetVectors();
}

void ChemPotSampling::readXML(XMLfileUnits& xmlconfig) {

    xmlconfig.getNodeValue("binwidth", _binwidth);  // Default: 1.0
    xmlconfig.getNodeValue("lattice", _lattice);  // Default: true
    xmlconfig.getNodeValue("factorNumTest", _factorNumTest);  // Default: 4.0
    xmlconfig.getNodeValue("start", _startSampling);  // Default: 0
    xmlconfig.getNodeValue("writefrequency", _writeFrequency);  // Default: 10000
    xmlconfig.getNodeValue("samplefrequency", _samplefrequency);  // Default: 50
    xmlconfig.getNodeValue("stop", _stopSampling);  // Default: 1000000000

    string insMethod;
    if (_lattice) {
        insMethod = "in a lattice";
    } else {
        insMethod = "randomly";
    }

    global_log->info() << "[ChemPotSampling] Start:WriteFreq:Stop: " << _startSampling << " : " << _writeFrequency << " : " << _stopSampling << std::endl;
    global_log->info() << "[ChemPotSampling] Binwidth: " << _binwidth << " ; Sample frequency: " << _samplefrequency << std::endl;
    global_log->info() << "[ChemPotSampling] " << _factorNumTest << " * numParticles will be inserted " << insMethod << std::endl;

    if (_samplefrequency > _writeFrequency) {
        global_log->warning() << "[ChemPotSampling] Sample frequency is greater than write frequency! " << std::endl;
    }
}

// Needs to be called when halo cells are still existing
void ChemPotSampling::afterForces(ParticleContainer* particleContainer, DomainDecompBase* domainDecomp, unsigned long simstep) {

    // Sampling starts after _startSampling and is conducted up to _stopSampling
    if ((simstep <= _startSampling) or (simstep > _stopSampling)) {
        return;
    }

    // Sample only every _samplefrequency
	if ( (simstep - _startSampling) % _samplefrequency != 0 ) {
        return;
    }

    // Do not write or sample data directly after (re)start in the first step
	if ( simstep == global_simulation->getNumInitTimesteps() ) {
        return;
    }
    
    std::array<double, 3> regionLowCorner;
    std::array<double, 3> regionHighCorner;
    std::array<double, 3> regionSize;

    CommVar<std::vector<double>> ekin2;
    std::array<CommVar<std::vector<double>>,3> velocity; // Local and global variable containing summed up velocity in all three directions and for all bins
    CommVar<std::vector<unsigned long>> numMols;

    for (unsigned short d = 0; d < 3; d++) {
        regionLowCorner[d] = particleContainer->getBoundingBoxMin(d);
        regionHighCorner[d] = particleContainer->getBoundingBoxMax(d);
        regionSize[d] = regionHighCorner[d] - regionLowCorner[d];
    }

    ekin2.local.resize(_numBinsGlobal);
    ekin2.global.resize(_numBinsGlobal);

    numMols.local.resize(_numBinsGlobal);
    numMols.global.resize(_numBinsGlobal);

    std::fill(ekin2.local.begin(), ekin2.local.end(), 0.0);
    std::fill(numMols.local.begin(), numMols.local.end(), 0ul);
    std::fill(ekin2.global.begin(), ekin2.global.end(), 0.0);
    std::fill(numMols.global.begin(), numMols.global.end(), 0ul);

    for (unsigned short d = 0; d < 3; d++) {
        velocity[d].local.resize(_numBinsGlobal);
        velocity[d].global.resize(_numBinsGlobal);
        std::fill(velocity[d].local.begin(), velocity[d].local.end(), 0.0);
        std::fill(velocity[d].global.begin(), velocity[d].global.end(), 0.0);
    }

    // Sample temperature, velocity (for drift calculation) and number of molecules
    for (auto pit = particleContainer->iterator(ParticleIterator::ONLY_INNER_AND_BOUNDARY); pit.isValid(); ++pit) {
        double ry = pit->r(1);
        uint16_t index = std::min(_numBinsGlobal, static_cast<uint16_t>(ry/_binwidth));  // Index of bin

        numMols.local.at(index) += 1;
        ekin2.local.at(index) += pit->U_trans_2() + pit->U_rot_2();

        velocity[0].local.at(index) += pit->v(0);
        velocity[1].local.at(index) += pit->v(1);
        velocity[2].local.at(index) += pit->v(2);
    }

#ifdef ENABLE_MPI
	MPI_Allreduce(numMols.local.data(), numMols.global.data(), _numBinsGlobal, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(ekin2.local.data(), ekin2.global.data(), _numBinsGlobal, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(velocity[0].local.data(), velocity[0].global.data(), _numBinsGlobal, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(velocity[1].local.data(), velocity[1].global.data(), _numBinsGlobal, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(velocity[2].local.data(), velocity[2].global.data(), _numBinsGlobal, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
#else
	for (unsigned int i = 0; i < _numBinsGlobal; i++) {
		numMols.global.at(i) = numMols.local.at(i);
        ekin2.global.at(i) = ekin2.local.at(i);
        velocity[0].global.at(i) = velocity[0].local.at(i);
        velocity[1].global.at(i) = velocity[1].local.at(i);
        velocity[2].global.at(i) = velocity[2].local.at(i);
	}
#endif

    // Calculate drift and temperature
    std::vector<double> temperatureStep(_numBinsGlobal, 0.0);
    for (uint16_t i = 0; i < _numBinsGlobal; i++) {
        double veloDrift2 = velocity[0].global.at(i) * velocity[0].global.at(i)
                                   + velocity[1].global.at(i) * velocity[1].global.at(i)
                                   + velocity[2].global.at(i) * velocity[2].global.at(i);
        if (numMols.global.at(i) > 0) {
            const double ekin2_T = ekin2.global.at(i) - veloDrift2/numMols.global.at(i); // Kinetic energy without drift
            temperatureStep.at(i) = ekin2_T/(numMols.global.at(i)*3.0);
            _temperatureSumGlobal.at(i) += temperatureStep.at(i);
            _temperatureWithDriftSumGlobal.at(i) += ekin2.global.at(i)/(numMols.global.at(i)*3.0);
            _numMoleculesSumGlobal.at(i) += numMols.global.at(i);
            _countSamples.at(i)++;
        }
    }

    // Calculate number of test particles per bin
    std::vector<double> dX(_numBinsGlobal, 0.0);
    std::vector<double> dY(_numBinsGlobal, 0.0);
    std::vector<double> dZ(_numBinsGlobal, 0.0);
    std::vector<unsigned long> nX(_numBinsGlobal, 0ul);
    std::vector<unsigned long> nY(_numBinsGlobal, 0ul);
    std::vector<unsigned long> nZ(_numBinsGlobal, 0ul);
    unsigned long nTestGlobal {0ul};

    for (uint16_t i = 0; i < _numBinsGlobal; i++) {
        // Make sure, number of test particles is never zero and number of test particles per direction is at least 1
        const unsigned long nTest = std::max(1ul,static_cast<unsigned long>(_factorNumTest*numMols.global.at(i)));

        nY.at(i) = std::max(1.0,std::pow((nTest*_binwidth*_binwidth)/(_globalBoxLength[0]*_globalBoxLength[2]),(1./3.)));
        dY.at(i) = std::min(static_cast<float>(_binwidth/nY.at(i)), static_cast<float>(0.5*regionSize[1]));

        nX.at(i) = std::max(1.0,std::pow((nTest*_globalBoxLength[0]*_globalBoxLength[0])/(_binwidth*_globalBoxLength[2]),(1./3.)));
        dX.at(i) = std::min(static_cast<float>(_globalBoxLength[0]/nX.at(i)), static_cast<float>(0.5*regionSize[0]));

        nZ.at(i) = std::max(1ul,nTest/(nX.at(i)*nY.at(i)));
        dZ.at(i) = std::min(static_cast<float>(_globalBoxLength[2]/nZ.at(i)), static_cast<float>(0.5*regionSize[2]));
        
        nTestGlobal += nTest;
    }

    if (_lattice) {
        // Insert particles in lattice structure and sample chem. pot.

        // Index of bin in which the left region boundary (y-dir) is in; std::min if particle position is precisely at right boundary
        const uint16_t idxStart = std::min(_numBinsGlobal, static_cast<uint16_t>(regionLowCorner[1]/_binwidth));
        double rY = regionLowCorner[1]+0.5*dY.at(idxStart);
        while (rY < regionHighCorner[1]) {
            const uint16_t index = std::min(_numBinsGlobal, static_cast<uint16_t>(rY/_binwidth));  // Index of bin

            double rX = regionLowCorner[0]+0.5*dX.at(idxStart);
            while (rX < regionHighCorner[0]) {

                double rZ = regionLowCorner[2]+0.5*dZ.at(idxStart);
                while (rZ < regionHighCorner[2]) {
                    _mTest.setr(0,rX);
                    _mTest.setr(1,rY);
                    _mTest.setr(2,rZ);
                    const double deltaUpot = particleContainer->getEnergy(_particlePairsHandler, &_mTest, *_cellProcessor);
                    if (temperatureStep.at(index) > 1e-9) {
                        double chemPot = exp(-deltaUpot/temperatureStep.at(index));
                        if (std::isfinite(chemPot)) {
                            _chemPotSum.local.at(index) += chemPot;
                            _countNTest.local.at(index)++;
#ifndef NDEBUG
                            std::cout << "[ChemPotSampling] Rank " << domainDecomp->getRank() << " : Inserting molecule at x,y,z = "
                                      << _mTest.r(0) << " , " << _mTest.r(1) << " , " << _mTest.r(2)
                                      << " ; chemPot = " << chemPot << " ; dU = " << deltaUpot << " ; T = " << temperatureStep.at(index) << " ; index = " << index << std::endl;
#endif
                        }
                    }
                    rZ += dZ.at(index);
                }
                rX += dX.at(index);
            }
            rY += dY.at(index);
        }
    } else {
        // Random insertion
        // NOTE: This differs from the lattice method as it does not take the local density into account
        Random* rnd = new Random();

        const float domainShare = (regionSize[0]*regionSize[1]*regionSize[2])/(_globalBoxLength[0]*_globalBoxLength[1]*_globalBoxLength[2]);  // Share of volume of present rank from whole domain
        const unsigned long nTest = static_cast<unsigned long>(domainShare*nTestGlobal);

#if defined(_OPENMP)
        #pragma omp parallel
#endif
        for (unsigned long i = 0; i < nTest; i++) {
            const double rX = regionLowCorner[0] + rnd->rnd()*regionSize[0];
            const double rY = regionLowCorner[1] + rnd->rnd()*regionSize[1];
            const double rZ = regionLowCorner[2] + rnd->rnd()*regionSize[2];
            const uint16_t index = std::min(_numBinsGlobal, static_cast<uint16_t>(rY/_binwidth));  // Index of bin
            _mTest.setr(0,rX);
            _mTest.setr(1,rY);
            _mTest.setr(2,rZ);
            const double deltaUpot = particleContainer->getEnergy(_particlePairsHandler, &_mTest, *_cellProcessor);
            if (temperatureStep.at(index) > 1e-9) {
                double chemPot = exp(-deltaUpot/temperatureStep.at(index));
                if (std::isfinite(chemPot)) {
#if defined(_OPENMP)
                    #pragma omp parallel
#endif
                    {
                    _chemPotSum.local.at(index) += chemPot;
                    _countNTest.local.at(index)++;
#ifndef NDEBUG
                    std::cout << "[ChemPotSampling] Rank " << domainDecomp->getRank() << " : Inserting molecule at x,y,z = "
                              << _mTest.r(0) << " , " << _mTest.r(1) << " , " << _mTest.r(2)
                              << " ; chemPot = " << chemPot << " ; dU = " << deltaUpot << " ; T = " << temperatureStep.at(index) << " ; index = " << index << std::endl;
#endif
                    }
                }
            }
        }
    }

    // Write out data every _writeFrequency step
    if ( (simstep - _startSampling) % _writeFrequency != 0 ) {
        return;
    }

#ifdef ENABLE_MPI
	MPI_Reduce(_chemPotSum.local.data(), _chemPotSum.global.data(), _numBinsGlobal, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(_countNTest.local.data(), _countNTest.global.data(), _numBinsGlobal, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
#else
	for (unsigned int i = 0; i < _numBinsGlobal; i++) {
        _chemPotSum.global.at(i) = _chemPotSum.local.at(i);
        _countNTest.global.at(i) = _countNTest.local.at(i);
	}
#endif

    if (domainDecomp->getRank() == 0) {
        // Write output file
        std::stringstream ss;
        ss << std::setw(9) << std::setfill('0') << simstep;
        const std::string fname = "ChemPotSampling_TS"+ss.str()+".dat";
        std::ofstream ofs;
        ofs.open(fname, std::ios::out);
        ofs << setw(24) << "pos"                // Bin position
            << setw(24) << "numParts"           // Average number of molecules in bin per step
            << setw(24) << "density"            // Density
            << setw(24) << "temperature"        // Temperature without drift (i.e. "real" temperature)
            << setw(24) << "temp_with_drift"    // Temperature with drift
            << setw(24) << "numParts_test"      // Number of inserted test particles per sample step
            << setw(24) << "chemPot_res"        // Chemical potential as known as mu_tilde (equals the ms2 value)
            << std::endl;
        for (uint16_t i = 0; i < _numBinsGlobal; i++) {
            double numMolsPerStep {0.0}; // Not an int as particles change bin during simulation
            double T {0.0};
            double TDrift {0.0};
            unsigned long numTest {0ul};
            double chemPot {0.0};
            if ((_chemPotSum.global.at(i) > 0.0) and (_countNTest.global.at(i) > 0ul) and (_countSamples.at(i) > 0ul)) {
                numMolsPerStep = static_cast<double>(_numMoleculesSumGlobal.at(i))/_countSamples.at(i);
                T = _temperatureSumGlobal.at(i)/_countSamples.at(i);
                TDrift = _temperatureWithDriftSumGlobal.at(i)/_countSamples.at(i);
                numTest = _countNTest.global.at(i)/_countSamples.at(i);
                chemPot = -log(_chemPotSum.global.at(i)/_countNTest.global.at(i)) + log(numMolsPerStep/_slabVolume);
            }
            ofs << std::setw(24) << std::scientific << std::setprecision(std::numeric_limits<double>::digits10) << (i+0.5)*_binwidth;
            ofs << std::setw(24) << std::scientific << std::setprecision(std::numeric_limits<double>::digits10) << numMolsPerStep;
            ofs << std::setw(24) << std::scientific << std::setprecision(std::numeric_limits<double>::digits10) << numMolsPerStep/_slabVolume;
            ofs << std::setw(24) << std::scientific << std::setprecision(std::numeric_limits<double>::digits10) << T;
            ofs << std::setw(24) << std::scientific << std::setprecision(std::numeric_limits<double>::digits10) << TDrift;
            ofs << std::setw(24) << std::scientific << std::setprecision(std::numeric_limits<double>::digits10) << numTest;
            ofs << std::setw(24) << std::scientific << std::setprecision(std::numeric_limits<double>::digits10) << chemPot << std::endl;
        }
        ofs.close();
    }

    // Reset vectors to zero
    resetVectors();
}


// Fill vectors with zeros
void ChemPotSampling::resetVectors() {
    std::fill(_chemPotSum.local.begin(), _chemPotSum.local.end(), 0.0f);
    std::fill(_chemPotSum.global.begin(), _chemPotSum.global.end(), 0.0f);

    std::fill(_temperatureSumGlobal.begin(), _temperatureSumGlobal.end(), 0.0f);
    std::fill(_temperatureWithDriftSumGlobal.begin(), _temperatureWithDriftSumGlobal.end(), 0.0f);
    std::fill(_numMoleculesSumGlobal.begin(), _numMoleculesSumGlobal.end(), 0ul);

    std::fill(_countNTest.local.begin(), _countNTest.local.end(), 0ul);
    std::fill(_countNTest.global.begin(), _countNTest.global.end(), 0ul);

    std::fill(_countSamples.begin(), _countSamples.end(), 0ul);
}
