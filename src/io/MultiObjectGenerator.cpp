#include "io/MultiObjectGenerator.h"

#include "Domain.h"
#include "Simulation.h"
#include "ensemble/EnsembleBase.h"
#include "molecules/Molecule.h"
#include "particleContainer/ParticleContainer.h"
#include "parallel/DomainDecompBase.h"
#include "utils/Logger.h"
#include "utils/xmlfileUnits.h"
#include "molecules/MoleculeIdPool.h"
#include "utils/generator/GridFiller.h"
#include "utils/generator/VelocityAssignerBase.h"
#include "utils/generator/EqualVelocityAssigner.h"
#include "utils/generator/MaxwellVelocityAssigner.h"
#include "utils/generator/ObjectFactory.h"

#include <cmath>
#include <limits>
#include <map>
#include <random>
#include <string>

#ifdef ENABLE_MPI
#include <mpi.h>
#endif

using Log::global_log;
using namespace std;


void MultiObjectGenerator::readXML(XMLfileUnits& xmlconfig) {
	if(xmlconfig.changecurrentnode("velocityAssigner")) {
		std::string defaultVelocityAssignerName;
		xmlconfig.getNodeValue("@type", defaultVelocityAssignerName);
		if(defaultVelocityAssignerName == "EqualVelocityDistribution") {
			_defaultVelocityAssigner = new EqualVelocityAssigner();
		} else if(defaultVelocityAssignerName == "MaxwellVelocityDistribution") {
			_defaultVelocityAssigner = new MaxwellVelocityAssigner();
		}
		xmlconfig.changecurrentnode("..");
	}

	XMLfile::Query query = xmlconfig.query("objectgenerator");
	global_log->info() << "Number of sub-objectgenerators: " << query.card() << endl;
	string oldpath = xmlconfig.getcurrentnodepath();
	for( auto generatorIter = query.begin(); generatorIter; ++generatorIter ) {
		xmlconfig.changecurrentnode(generatorIter);
		_generators.push_back(new GridFiller);
		_generators.back()->readXML(xmlconfig);

		if(xmlconfig.changecurrentnode("object")) {
			std::string object_type;
			xmlconfig.getNodeValue("@type", object_type);
			ObjectFactory object_factory;
			global_log->debug() << "Obj name: " << object_type << endl;
			Object *object = object_factory.create(object_type);
			if(object == nullptr) {
				global_log->error() << "Unknown object type: " << object_type << endl;
			}
			global_log->debug() << "Created object of type: " << object->getName() << endl;
			object->readXML(xmlconfig);
			_generators.back()->setObject(object);
			xmlconfig.changecurrentnode("..");
		}
	}
	xmlconfig.changecurrentnode(oldpath);
}


long unsigned int MultiObjectGenerator::readPhaseSpace(ParticleContainer* particleContainer, list<ChemicalPotential>* lmu,
		Domain* domain, DomainDecompBase* domainDecomp) {
	unsigned long numMolecules = 0;

	Ensemble* ensemble = _simulation.getEnsemble();
	double bBoxMin[3];
	double bBoxMax[3];
	domainDecomp->getBoundingBoxMinMax(domain, bBoxMin, bBoxMax);
	MoleculeIdPool moleculeIdPool(std::numeric_limits<unsigned long>::max(), domainDecomp->getNumProcs(), domainDecomp->getRank());

	_defaultVelocityAssigner->setTemperature(ensemble->T());
	for(auto generator : _generators) {
		Molecule molecule;
		generator->setBoudingBox(bBoxMin, bBoxMax);
		generator->init();
		while(generator->getMolecule(&molecule) > 0) {
			molecule.setid(moleculeIdPool.getNewMoleculeId());
			_defaultVelocityAssigner->assignVelocity(&molecule);
			Quaternion q(1.0, 0., 0., 0.); /* orientation of molecules has to be set to a value other than 0,0,0,0! */
			molecule.setq(q);
			bool inserted = particleContainer->addParticle(molecule);
			if(inserted){
				numMolecules++;
			}
		}
	}
	global_log->debug() << "Number of locally inserted molecules: " << numMolecules << endl;
	particleContainer->updateMoleculeCaches();
	unsigned long globalNumMolecules = numMolecules;
#ifdef ENABLE_MPI
	MPI_Allreduce(MPI_IN_PLACE, &globalNumMolecules, 1, MPI_UNSIGNED_LONG, MPI_SUM, domainDecomp->getCommunicator());
#endif
	global_log->info() << "Number of inserted molecules: " << numMolecules << endl;
	//! @todo Get rid of the domain class calls at this place here...
	domain->setGlobalTemperature(ensemble->T());
	domain->setglobalNumMolecules(globalNumMolecules);
	domain->setglobalRho(numMolecules / ensemble->V() );
	return numMolecules;
}
