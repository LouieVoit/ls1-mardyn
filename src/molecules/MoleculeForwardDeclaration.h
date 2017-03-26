/*
 * MoleculeForwardDeclaration.h
 *
 *  Created on: Jan 28, 2017
 *      Author: tchipevn
 */

#ifndef SRC_MOLECULES_MOLECULEFORWARDDECLARATION_H_
#define SRC_MOLECULES_MOLECULEFORWARDDECLARATION_H_

#ifndef MARDYN_WR
	class FullMolecule;
	typedef FullMolecule Molecule;
#else
	class Molecule_WR;
	typedef Molecule_WR Molecule;
#endif

#endif /* SRC_MOLECULES_MOLECULEFORWARDDECLARATION_H_ */
