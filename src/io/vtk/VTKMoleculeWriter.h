/*
 * VTKMoleculeWriter.h
 *
 * @Date: 24.08.2010
 * @Author: Wolfgang Eckhardt
 */

#ifndef VTKWRITER_H_
#define VTKWRITER_H_

#include "io/OutputBase.h"
#include "io/vtk/VTKMoleculeWriterImplementation.h"

/**
 * This class is an implementation of the OutputBase for the VTK file format.
 *
 * @TODO Think about a way how to handle the setup of particleConainers, Domain, etc...
 *       Maybe some kind of factory?
 */
class VTKMoleculeWriter: public OutputBase {

private:

	unsigned int _writeFrequency;

	std::string _fileName;

public:
	VTKMoleculeWriter() :
			_writeFrequency(50), _fileName("") {
	}

	VTKMoleculeWriter(unsigned int frequency, std::string name):
		_writeFrequency(frequency), _fileName(name) {}

	virtual ~VTKMoleculeWriter() {}

	//! @todo document me!
	virtual void initOutput(ParticleContainer* particleContainer,
			DomainDecompBase* domainDecomp, Domain* domain);

	//! @todo document me!
	virtual void doOutput(
			ParticleContainer* particleContainer, DomainDecompBase* domainDecomp,
			Domain* domain, unsigned long simstep,
			std::list<ChemicalPotential>* lmu,
			std::map<unsigned, CavityEnsemble>* mcav
	);

	//! @todo document me!
	virtual void finishOutput(ParticleContainer* particleContainer,
			DomainDecompBase* domainDecomp, Domain* domain);

	static std::string getPluginName() {
		return std::string("VTKMoleculeWriter");
	}
	static OutputBase* createInstance() { return new VTKMoleculeWriter(); }

	void readXML(XMLfileUnits& xmlconfig);

private:
	void outputParallelVTKFile(unsigned int numProcs, unsigned long simstep,
			VTKMoleculeWriterImplementation& impl);

};

#endif /* VTKWRITER_H_ */
