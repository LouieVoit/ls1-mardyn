/*
 * Adios2Writer.cpp
 *
 *  Created on: April 2019
 *      Author: Tobias Rau
 */

#include "Adios2Writer.h"
#include "particleContainer/ParticleContainer.h"
#include "Domain.h"
#include "Simulation.h"
#include "parallel/DomainDecompBase.h"
#include "utils/mardyn_assert.h"
#include "utils/xmlfile.h"

using Log::global_log;

void Adios2Writer::init(ParticleContainer* particleContainer,
        DomainDecompBase* domainDecomp, Domain* domain) {

        // set variable name to write
		vars["molecule_id"] = std::vector<uint64_t>();
		vars["component_id"] = std::vector<uint64_t>();
        vars["rx"] = std::vector<double>();
        vars["ry"] = std::vector<double>();
        vars["rz"] = std::vector<double>();
        vars["vx"] = std::vector<double>();
        vars["vy"] = std::vector<double>();
        vars["vz"] = std::vector<double>();
        vars["qw"] = std::vector<double>();
        vars["qx"] = std::vector<double>();
        vars["qy"] = std::vector<double>();
        vars["qz"] = std::vector<double>();
        vars["Lx"] = std::vector<double>();
        vars["Ly"] = std::vector<double>();
        vars["Lz"] = std::vector<double>();

};

void Adios2Writer::readXML(XMLfileUnits& xmlconfig) {
	using std::endl;
	_outputfile = "mardyn.bp";
	xmlconfig.getNodeValue("outputfile", _outputfile);
	global_log->info() << "[Adios2Writer] Outputfile: " << _outputfile << endl;
	_adios2enginetype = "BP4";
	xmlconfig.getNodeValue("adios2enginetype", _adios2enginetype);
	global_log->info() << "[Adios2Writer] Adios2 engine type: " << _adios2enginetype << endl;
	_writefrequency = 50000;
	xmlconfig.getNodeValue("writefrequency", _writefrequency);
	global_log->info() << "[Adios2Writer] write frequency: " << _writefrequency << endl;

	xmlconfig.changecurrentnode("/");
	xmlconfig.printXML(_xmlstream);
	
	if (!inst) initAdios2();
};

void Adios2Writer::initAdios2() {
  auto& domainDecomp = _simulation.domainDecomposition();

  try {
    //get adios2 instance
	    inst = std::make_shared<adios2::ADIOS>((MPI_Comm) MPI_COMM_WORLD);

        // declare io as output
        io = std::make_shared<adios2::IO>(inst->DeclareIO("Output"));

        // use bp engine
        io->SetEngine(_adios2enginetype);

        if (!engine) {
            global_log->info() << "[Adios2Writer]: Opening File for writing." << _outputfile.c_str() << std::endl;
            engine = std::make_shared<adios2::Engine>(io->Open(_outputfile, adios2::Mode::Write));
        }
  }
    catch (std::invalid_argument& e) {
        global_log->fatal() 
                << "Invalid argument exception, STOPPING PROGRAM from rank" 
                << domainDecomp.getRank() 
                << ": " << e.what() << std::endl;
	mardyn_exit(1);
    } catch (std::ios_base::failure& e) {
        global_log->fatal() 
                << "IO System base failure exception, STOPPING PROGRAM from rank " 
                << domainDecomp.getRank() 
                << ": " << e.what() << std::endl;
	mardyn_exit(1);
    } catch (std::exception& e) {
        global_log->fatal() 
                << "Exception, STOPPING PROGRAM from rank" 
                << domainDecomp.getRank() 
                << ": " << e.what() << std::endl;
	mardyn_exit(1);
    }
    global_log->info() << "[AdiosWriter]: Init complete." << std::endl;
};

void Adios2Writer::beforeEventNewTimestep(
        ParticleContainer* particleContainer, DomainDecompBase* domainDecomp,
        unsigned long simstep) {
    global_log->info() << "[AdiosWriter]: beforeEventNewTimestep." << std::endl;

};

void Adios2Writer::beforeForces(
        ParticleContainer* particleContainer, DomainDecompBase* domainDecomp,
        unsigned long simstep
) {
    global_log->info() << "[AdiosWriter]: beforeForces." << std::endl;

};

void Adios2Writer::afterForces(
        ParticleContainer* particleContainer, DomainDecompBase* domainDecomp,
        unsigned long simstep
) {
    global_log->info() << "[AdiosWriter]: afterForces." << std::endl;

};

void Adios2Writer::endStep(
        ParticleContainer* particleContainer, DomainDecompBase* domainDecomp,
        Domain* domain, unsigned long simstep) {

    if (simstep % _writefrequency != 0) return;
  
    // for all outputs
    size_t localNumParticles = particleContainer->getNumberOfParticles();
    size_t const globalNumParticles = domain->getglobalNumMolecules();
    int const numProcs = domainDecomp->getNumProcs();
	int const rank = domainDecomp->getRank();

    std::vector<uint64_t> m_id;
    m_id.reserve(localNumParticles);

    std::vector<uint32_t> comp_id;
    comp_id.reserve(localNumParticles);

    for (auto m = particleContainer->iterator(ParticleIterator::ONLY_INNER_AND_BOUNDARY); m.isValid(); ++m) {
	    std::get<std::vector<uint64_t>>(vars["molecule_id"]).emplace_back(m->getID());
		std::get<std::vector<uint64_t>>(vars["component_id"]).emplace_back(m->componentid());
		std::get<std::vector<double>>(vars["rx"]).emplace_back(m->r(0));
		std::get<std::vector<double>>(vars["ry"]).emplace_back(m->r(1));
		std::get<std::vector<double>>(vars["rz"]).emplace_back(m->r(2));
        std::get<std::vector<double>>(vars["vx"]).emplace_back(m->v(0));
        std::get<std::vector<double>>(vars["vy"]).emplace_back(m->v(1));
        std::get<std::vector<double>>(vars["vz"]).emplace_back(m->v(2));
        auto q = m->q();
        std::get<std::vector<double>>(vars["qw"]).emplace_back(q.qw());
        std::get<std::vector<double>>(vars["qx"]).emplace_back(q.qx());
        std::get<std::vector<double>>(vars["qy"]).emplace_back(q.qy());
        std::get<std::vector<double>>(vars["qz"]).emplace_back(q.qz());
        std::get<std::vector<double>>(vars["Lx"]).emplace_back(m->D(0));
        std::get<std::vector<double>>(vars["Ly"]).emplace_back(m->D(1));
        std::get<std::vector<double>>(vars["Lz"]).emplace_back(m->D(2));

        m_id.emplace_back(m->getID());
        comp_id.emplace_back(m->componentid());
	}
  
    global_log->set_mpi_output_all();

    // gather offsets
    global_log->info() << "numProcs: " << numProcs 
            << " localNumParticles: " << localNumParticles
            << " domainDecomp->getRank(): " << domainDecomp->getRank() << std::endl;

    uint64_t offset = 0;
	MPI_Exscan(&localNumParticles, &offset, 1, MPI_UINT64_T, MPI_SUM, MPI_COMM_WORLD);

	global_log->info() << "[AdiosWriter]: localNumParticles " << localNumParticles << std::endl;
	global_log->info() << "[AdiosWriter]: Offset " << offset << std::endl;

    std::array<double,6> global_box{0, 0,0, domain->getGlobalLength(0), domain->getGlobalLength(1), domain->getGlobalLength(2)};
    std::array<double,6> local_box;
    domainDecomp->getBoundingBoxMinMax(domain, &local_box[0], &local_box[3]);
    
    global_log->info() << "Local Box: " << local_box[0] << " "<< local_box[1]<< " " << local_box[2] << " "<< local_box[3] << " "<< local_box[4]<< " " << local_box[5] <<  std::endl;
    try {
	    engine->BeginStep();
	    io->RemoveAllVariables();
	    for (auto& [variableName,variableContainer] : vars) {
			global_log->info() << "[AdiosWriter]: Defining Variables " << variableName << std::endl;

      		if (std::holds_alternative<std::vector<double>>(variableContainer)) {
		        adios2::Variable<double> adios2Var = io->DefineVariable<double>(variableName, {globalNumParticles},
		                    {offset},
		                    {localNumParticles}, adios2::ConstantDims);

				// ready for write; transfer data to adios2
				engine->Put<double>(adios2Var, std::get<std::vector<double>>(variableContainer).data());
			} else {
				adios2::Variable<uint64_t> adios2Var =
					io->DefineVariable<uint64_t>(variableName, {globalNumParticles}, {offset},
																			   {localNumParticles}, adios2::ConstantDims);
				// ready for write; transfer data to adios2
				engine->Put<uint64_t>(adios2Var, std::get<std::vector<uint64_t>>(variableContainer).data());
			}
	    }

	    // global box
	    if(domainDecomp->getRank() == 0) {
	    adios2::Variable<double> adios2_global_box =
	        io->DefineVariable<double>("global_box", {6}, {0}, {6}, adios2::ConstantDims);

	    global_log->info() << "[AdiosWriter]: Putting Variables" << std::endl;
	    if (!adios2_global_box) {
	        global_log->error() << "[AdiosWriter]: Could not create variable: global_box" << std::endl;
	        return;
	    }
	    engine->Put<double>(adios2_global_box, global_box.data());
	    }

	    //local box
	    adios2::Variable<double> adios2_local_box =
		io->DefineVariable<double>("local_box", {},
		            {},
		            {6});

		if (!adios2_local_box) {
		    global_log->error() << "[AdiosWriter]: Could not create variable: local_box" << std::endl;
		    return;
		}
		engine->Put<double>(adios2_local_box, local_box.data());

        // offsets
		adios2::Variable<uint64_t> adios2_offset = io->DefineVariable<uint64_t>(
			"offsets", {static_cast<size_t>(numProcs)}, {static_cast<size_t>(rank)}, {1}, adios2::ConstantDims);
		engine->Put<uint64_t>(adios2_offset, offset);

    	
	    //simulation time
	    current_time = _simulation.getSimulationTime();
	    if(domainDecomp->getRank() == 0) {
	        adios2::Variable<double> adios2_simulationtime =
	            io->DefineVariable<double>("simulationtime");
	        if (!adios2_simulationtime) {
	            global_log->error() << "[AdiosWriter]: Could not create variable: simulationtime" << std::endl;
	            return;
	        }             
	        engine->Put<double>(adios2_simulationtime, current_time);

            // write number of procs
			adios2::Variable<int> adios2_numprocs = io->DefineVariable<int>("numProcs");
			engine->Put<int>(adios2_numprocs, numProcs);

	    	// write config
	    	adios2::Attribute<std::string> adios2_ls1config = io->DefineAttribute<std::string>("config", _xmlstream.str());

	    	auto const components= _simulation.getEnsemble()->getComponents();
			for (auto& component : *components) {
				std::string component_id = "component_" + std::to_string(component.ID());
				std::vector<std::array<double, 3>> adios_lj_centers;
				std::vector<double> adios_sigmas;
				std::vector<double> adios_mass;
				std::vector<double> adios_epsilon;
				auto lj_centers = component.ljcenters();
				for (auto& lj_center : lj_centers) {
					adios_lj_centers.emplace_back(lj_center.r());
					adios_sigmas.emplace_back(lj_center.sigma());
					adios_mass.emplace_back(lj_center.m());
					adios_epsilon.emplace_back(lj_center.eps());
				}
				
				adios2::Attribute<double> adios2_site_pos = io->DefineAttribute<double>(
					component_id + "_centers", adios_lj_centers[0].data(), adios_lj_centers.size() * 3);
				adios2::Attribute<double> adios2_site_sigma =
					io->DefineAttribute<double>(component_id + "_sigma", adios_sigmas.data(), adios_sigmas.size());
				adios2::Attribute<double> adios2_site_mass=
					io->DefineAttribute<double>(component_id + "_mass", adios_mass.data(), adios_mass.size());
				adios2::Attribute<double> adios2_site_epsilon= io->DefineAttribute<double>(component_id + "_epsilon", adios_epsilon.data(), adios_epsilon.size());
				adios2::Attribute<std::string> adios2_site_name =
					io->DefineAttribute<std::string>(component_id + "_name", std::string(component.getName()));
				
			}
	    }


	    // wait for completion of write
	    engine->EndStep();
	}
	catch (std::invalid_argument& e) {
		global_log->error() <<
			"[ADIOS2] Invalid argument exception, STOPPING PROGRAM";
		global_log->error() <<e.what();
	}
	catch (std::ios_base::failure& e) {
		global_log->error() <<
			"[ADIOS2] IO System base failure exception, STOPPING PROGRAM";
		global_log->error() <<e.what();
	}
	catch (std::exception& e) {
		global_log->error() <<"[ADIOS2] Exception, STOPPING PROGRAM";
		global_log->error() <<e.what();
	}
    global_log->info() << "[AdiosWriter]: endStep." << std::endl;
};

void Adios2Writer::finish(ParticleContainer* particleContainer,
        DomainDecompBase* domainDecomp, Domain* domain
) {
    engine->Close();
    global_log->info() << "[AdiosWriter]: finish." << std::endl;
}
