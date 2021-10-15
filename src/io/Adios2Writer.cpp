/*
 * Adios2Writer.cpp
 *
 *  Created on: April 2021
 *      Author: Tobias Rau, Matthias Heinen, Patrick Gralka, Christoph Niethammer, Simon Homes
 */
#ifdef ENABLE_ADIOS2
#include "Adios2Writer.h"

#include <numeric>

#include "Domain.h"
#include "Simulation.h"
#include "parallel/DomainDecompBase.h"
#include "particleContainer/ParticleContainer.h"
#include "utils/mardyn_assert.h"
#include "utils/xmlfile.h"

using Log::global_log;

constexpr char const* mol_id_name = "molecule_id";
constexpr char const* comp_id_name = "component_id";
constexpr char const* rx_name = "rx";
constexpr char const* ry_name = "ry";
constexpr char const* rz_name = "rz";
constexpr char const* vx_name = "vx";
constexpr char const* vy_name = "vy";
constexpr char const* vz_name = "vz";
constexpr char const* qx_name = "qx";
constexpr char const* qy_name = "qy";
constexpr char const* qz_name = "qz";
constexpr char const* qw_name = "qw";
constexpr char const* Lx_name = "Lx";
constexpr char const* Ly_name = "Ly";
constexpr char const* Lz_name = "Lz";
constexpr char const* gbox_name = "global_box";
constexpr char const* lbox_name = "local_box";
constexpr char const* offsets_name = "offsets";
constexpr char const* simtime_name = "simulationtime";

void Adios2Writer::resetContainers() {
	// set variable name to write
	_vars[mol_id_name] = std::vector<uint64_t>();
	_vars[comp_id_name] = std::vector<uint64_t>();
	_vars[ry_name] = std::vector<double>();
	_vars[rx_name] = std::vector<double>();
	_vars[rz_name] = std::vector<double>();
	_vars[vx_name] = std::vector<double>();
	_vars[vy_name] = std::vector<double>();
	_vars[vz_name] = std::vector<double>();
	if (!_singleCenter) {
		_vars[qw_name] = std::vector<double>();
		_vars[qx_name] = std::vector<double>();
		_vars[qy_name] = std::vector<double>();
		_vars[qz_name] = std::vector<double>();
		_vars[Lx_name] = std::vector<double>();
		_vars[Ly_name] = std::vector<double>();
		_vars[Lz_name] = std::vector<double>();
	}
}

void Adios2Writer::clearContainers() {
	for (auto& [variableName, variableContainer] : _vars) {
		if (std::holds_alternative<std::vector<double>>(variableContainer)) {
			std::get<std::vector<double>>(variableContainer).clear();
		} else {
			std::get<std::vector<uint64_t>>(variableContainer).clear();
		}
	}
}

void Adios2Writer::defineVariables(uint64_t global, uint64_t offset, uint64_t local, int numProcs, int rank) {
	for (auto& [variableName, variableContainer] : _vars) {
		global_log->info() << "[Adios2Writer] Defining Variable " << variableName << std::endl;
		if (std::holds_alternative<std::vector<double>>(variableContainer)) {
			auto advar_double = _io->DefineVariable<double>(variableName, {global}, {offset}, {local}, adios2::ConstantDims);
			
			if (!_compressionOperator.Type().empty()) {
				if (_compression == "SZ" || _compression == "sz") {
#ifdef ADIOS2_HAVE_SZ
					advar_double.AddOperation(_compressionOperator,
										  {{adios2::ops::sz::key::accuracy, _compression_accuracy}});
#endif
				} else if (_compression == "ZFP" || _compression == "zfp") {
#ifdef ADIOS2_HAVE_ZFP
					advar_double.AddOperation(_compressionOperator,
											  {{adios2::ops::zfp::key::rate, _compression_rate}});
#endif
				}
			}

		} else {
			auto advar_uint64 = _io->DefineVariable<uint64_t>(variableName, {global}, {offset}, {local}, adios2::ConstantDims);

			if (!_compressionOperator.Type().empty()) {
				if (_compression == "SZ" || _compression == "sz") {
#ifdef ADIOS2_HAVE_SZ
					advar_uint64.AddOperation(_compressionOperator,
											  {{adios2::ops::sz::key::accuracy, _compression_accuracy}});
#endif
				} else if (_compression == "ZFP" || _compression == "zfp") {
#ifdef ADIOS2_HAVE_ZFP
					advar_uint64.AddOperation(_compressionOperator,
											  {{adios2::ops::zfp::key::rate, _compression_rate}});
#endif
				}
			}
		}
	}

	global_log->info() << "[Adios2Writer] Defining Variable " << gbox_name << std::endl;
	_io->DefineVariable<double>(gbox_name, {6}, {0}, {6}, adios2::ConstantDims);
	global_log->info() << "[Adios2Writer] Defining Variable " << lbox_name << std::endl;
	_io->DefineVariable<double>(lbox_name, {}, {}, {6}, adios2::ConstantDims);
	global_log->info() << "[Adios2Writer] Defining Variable " << offsets_name << std::endl;
	_io->DefineVariable<uint64_t>(offsets_name, {static_cast<size_t>(numProcs)}, {static_cast<size_t>(rank)}, {1},
								  adios2::ConstantDims);
	global_log->info() << "[Adios2Writer] Defining Variable " << simtime_name << std::endl;
	_io->DefineVariable<double>(simtime_name);
}

void Adios2Writer::init(ParticleContainer* particleContainer, DomainDecompBase* domainDecomp, Domain* domain) {
	resetContainers();
}

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
	_append_mode = "OFF";
	xmlconfig.getNodeValue("appendmode", _append_mode);
	global_log->info() << "[Adios2Writer] Append mode: " << _append_mode << endl;
	_compression = "none";
	xmlconfig.getNodeValue("compression", _compression);
	global_log->info() << "[Adios2Writer] compression type: " << _compression << endl;
	_compression_accuracy = "0.00001";
	xmlconfig.getNodeValue("compressionaccuracy", _compression_accuracy);
	global_log->info() << "[Adios2Writer] compression accuracy (SZ): " << _compression_accuracy << endl;
	_compression_rate = "8";
	xmlconfig.getNodeValue("compressionrate", _compression_rate);
	global_log->info() << "[Adios2Writer] compression rate (ZFP): " << _compression_rate << endl;
	
	xmlconfig.changecurrentnode("/");
	xmlconfig.printXML(_xmlstream);

	if (!_inst) initAdios2();
}

void Adios2Writer::testInit(std::vector<Component>& comps, const std::string outfile, const std::string adios2enginetype, const unsigned long writefrequency,
							const std::string compression, const std::string compression_accuracy,
							const std::string compression_rate) {
	using std::endl;
	_outputfile = outfile;
	global_log->info() << "[Adios2Writer] Outputfile: " << _outputfile << endl;
	_adios2enginetype = adios2enginetype;
	global_log->info() << "[Adios2Writer] Adios2 engine type: " << _adios2enginetype << endl;
	_writefrequency = writefrequency;
	global_log->info() << "[Adios2Writer] write frequency: " << _writefrequency << endl;
	_compression = compression;
	global_log->info() << "[Adios2Writer] compression type: " << _compression << endl;
	_compression_accuracy = compression_accuracy;
	global_log->info() << "[Adios2Writer] compression accuracy (SZ): " << _compression_accuracy << endl;
	_compression_rate = compression_rate;
	global_log->info() << "[Adios2Writer] compression rate (ZFP): " << _compression_rate << endl;
	_comps = comps;
	
	if (!_inst) initAdios2();
}

void Adios2Writer::initAdios2() {

	try {
		/* Set up ADIOS2 instance for output with provided ADIOS2 Engine type */
#ifdef ENABLE_MPI
		_inst = std::make_shared<adios2::ADIOS>((MPI_Comm)MPI_COMM_WORLD);
#else
		_inst = std::make_shared<adios2::ADIOS>();
#endif
		_io = std::make_shared<adios2::IO>(_inst->DeclareIO("Output"));
		_io->SetEngine(_adios2enginetype);

		if (!_engine) {
			global_log->info() << "[Adios2Writer] Opening File for writing." << _outputfile.c_str() << std::endl;
			if (_append_mode == "ON" || _append_mode == "on" || _append_mode == "TRUE" || _append_mode == "true") {
				_engine = std::make_shared<adios2::Engine>(_io->Open(_outputfile, adios2::Mode::Append));
			} else {
				_engine = std::make_shared<adios2::Engine>(_io->Open(_outputfile, adios2::Mode::Write));
			}
		}

		// add operations
		if (_compression == "SZ" || _compression == "sz") {
#ifdef ADIOS2_HAVE_SZ
			_compressionOperator = _inst->DefineOperator("szCompressor", adios2::ops::LossySZ);
#endif
		} else if (_compression == "ZFP" || _compression == "zfp") {
#ifdef ADIOS2_HAVE_ZFP
			_compressionOperator = _inst->DefineOperator("ZFPCompressor", adios2::ops::LossyZFP);
#endif
		}
		
		// Write information about this simulation using ADIOS2 attributes
		_io->DefineAttribute<std::string>("config", _xmlstream.str());
		auto& domainDecomp = _simulation.domainDecomposition();
		_io->DefineAttribute<int>("num_processes", domainDecomp.getNumProcs());

		/* Include number of components and component definitions */
		std::vector<Component>* components;
		if (_comps.empty()) {
			_io->DefineAttribute<int>("num_components", _simulation.getEnsemble()->getComponents()->size());
			components = _simulation.getEnsemble()->getComponents();
		} else {
			components = &_comps;
		}
		for (auto& component : *components) {
			// only write lj components (for now)
			if (!component.ljcenters().empty()) {
				auto sites = component.ljcenters();
				_singleCenter = _singleCenter && sites.size() <= 1;
				std::vector<std::string> component_elements_vec;
				component_elements_vec.reserve(sites.size());
				for (auto& site : sites) {
					component_elements_vec.emplace_back(site.getName());
				}
				string component_elements =
					std::accumulate(component_elements_vec.begin(), component_elements_vec.end(), std::string(),
									[](std::string& ss, std::string& s) { return ss.empty() ? s : ss + "," + s; });

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

				_io->DefineAttribute<double>(component_id + "_centers", adios_lj_centers[0].data(), adios_lj_centers.size() * 3);
				_io->DefineAttribute<double>(component_id + "_sigma", adios_sigmas.data(), adios_sigmas.size());
				_io->DefineAttribute<double>(component_id + "_mass", adios_mass.data(), adios_mass.size());
				_io->DefineAttribute<double>(component_id + "_epsilon", adios_epsilon.data(), adios_epsilon.size());
				_io->DefineAttribute<std::string>(component_id + "_name", std::string(component.getName()));
				_io->DefineAttribute<std::string>(component_id + "_element_names", component_elements);
			}
		}
		resetContainers();
	} catch (std::invalid_argument& e) {
		global_log->fatal() << "Invalid argument exception, STOPPING PROGRAM from rank: " << e.what() << std::endl;
		mardyn_exit(1);
	} catch (std::ios_base::failure& e) {
		global_log->fatal() << "IO System base failure exception, STOPPING PROGRAM from rank: " << e.what() << std::endl;
		mardyn_exit(1);
	} catch (std::exception& e) {
		global_log->fatal() << "Exception, STOPPING PROGRAM from rank: " << e.what()
							<< std::endl;
		mardyn_exit(1);
	}
	global_log->info() << "[Adios2Writer] Init complete." << std::endl;
}

void Adios2Writer::beforeEventNewTimestep(ParticleContainer* particleContainer, DomainDecompBase* domainDecomp,
										  unsigned long simstep) {
	global_log->debug() << "[Adios2Writer] beforeEventNewTimestep." << std::endl;
}

void Adios2Writer::beforeForces(ParticleContainer* particleContainer, DomainDecompBase* domainDecomp,
								unsigned long simstep) {
	global_log->debug() << "[Adios2Writer] beforeForces." << std::endl;
}

void Adios2Writer::afterForces(ParticleContainer* particleContainer, DomainDecompBase* domainDecomp,
							   unsigned long simstep) {
	global_log->debug() << "[Adios2Writer] afterForces." << std::endl;
}

void Adios2Writer::endStep(ParticleContainer* particleContainer, DomainDecompBase* domainDecomp, Domain* domain,
						   unsigned long simstep) {
	if (simstep % _writefrequency != 0) return;

	// for all outputs
	domain->updateglobalNumMolecules(particleContainer, domainDecomp);
	uint64_t localNumParticles = particleContainer->getNumberOfParticles();
	uint64_t globalNumParticles = domain->getglobalNumMolecules();
	auto numProcs = domainDecomp->getNumProcs();
	int const rank = domainDecomp->getRank();

	std::vector<uint64_t> m_id;
	m_id.reserve(localNumParticles);

	std::vector<uint32_t> comp_id;
	comp_id.reserve(localNumParticles);

	for (auto& [variableName, variableContainer] : _vars) {
		if (std::holds_alternative<std::vector<double>>(variableContainer)) {
			std::get<std::vector<double>>(variableContainer).reserve(localNumParticles);
		} else {
			std::get<std::vector<uint64_t>>(variableContainer).reserve(localNumParticles);
		}
	}

	for (auto m = particleContainer->iterator(ParticleIterator::ONLY_INNER_AND_BOUNDARY); m.isValid(); ++m) {
		std::get<std::vector<uint64_t>>(_vars[mol_id_name]).emplace_back(m->getID());
		std::get<std::vector<uint64_t>>(_vars[comp_id_name]).emplace_back(m->componentid());
		std::get<std::vector<double>>(_vars[rx_name]).emplace_back(m->r(0));
		std::get<std::vector<double>>(_vars[ry_name]).emplace_back(m->r(1));
		std::get<std::vector<double>>(_vars[rz_name]).emplace_back(m->r(2));
		std::get<std::vector<double>>(_vars[vx_name]).emplace_back(m->v(0));
		std::get<std::vector<double>>(_vars[vy_name]).emplace_back(m->v(1));
		std::get<std::vector<double>>(_vars[vz_name]).emplace_back(m->v(2));
		if (!_singleCenter) {
			auto q = m->q();
			std::get<std::vector<double>>(_vars[qw_name]).emplace_back(q.qw());
			std::get<std::vector<double>>(_vars[qx_name]).emplace_back(q.qx());
			std::get<std::vector<double>>(_vars[qy_name]).emplace_back(q.qy());
			std::get<std::vector<double>>(_vars[qz_name]).emplace_back(q.qz());
			std::get<std::vector<double>>(_vars[Lx_name]).emplace_back(m->D(0));
			std::get<std::vector<double>>(_vars[Ly_name]).emplace_back(m->D(1));
			std::get<std::vector<double>>(_vars[Lz_name]).emplace_back(m->D(2));
		}
		
		m_id.emplace_back(m->getID());
		comp_id.emplace_back(m->componentid());

	}

	// gather offsets
	global_log->debug() << "[Adios2Writer] numProcs: " << numProcs << std::endl;
	
	uint64_t offset = 0;
#ifdef ENABLE_MPI
	MPI_Exscan(&localNumParticles, &offset, 1, MPI_UINT64_T, MPI_SUM, MPI_COMM_WORLD);
	global_log->debug() << "[Adios2Writer] localNumParticles " << localNumParticles << std::endl;
	global_log->debug() << "[Adios2Writer] Offset " << offset << std::endl;
#endif

	std::array<double, 6> global_box{
		0, 0, 0, domain->getGlobalLength(0), domain->getGlobalLength(1), domain->getGlobalLength(2)};
	std::array<double, 6> local_box;
	domainDecomp->getBoundingBoxMinMax(domain, &local_box[0], &local_box[3]);

	global_log->debug() << "[Adios2Writer] Local Box: " << local_box[0] << " " << local_box[1] << " " << local_box[2]
					   << " " << local_box[3] << " " << local_box[4] << " " << local_box[5] << std::endl;
	try {
		_engine->BeginStep();
		_io->RemoveAllVariables();
		defineVariables(globalNumParticles, offset, localNumParticles, numProcs, rank);
		for (auto& [variableName, variableContainer] : _vars) {
			if (std::holds_alternative<std::vector<double>>(variableContainer)) {
				auto adios2Var = _io->InquireVariable<double>(variableName);
				_engine->Put<double>(adios2Var, std::get<std::vector<double>>(variableContainer).data());
			} else {
				auto adios2Var = _io->InquireVariable<uint64_t>(variableName);
				_engine->Put<uint64_t>(adios2Var, std::get<std::vector<uint64_t>>(variableContainer).data());
			}
		}

		// global box
		if (domainDecomp->getRank() == 0) {
			auto adios2_global_box = _io->InquireVariable<double>(gbox_name);

			global_log->debug() << "[Adios2Writer] Putting Variables" << std::endl;
			if (!adios2_global_box) {
				global_log->error() << "[Adios2Writer] Could not create variable: global_box" << std::endl;
				return;
			}
			_engine->Put<double>(adios2_global_box, global_box.data());
		}

		// local box
		auto adios2_local_box = _io->InquireVariable<double>(lbox_name);

		if (!adios2_local_box) {
			global_log->error() << "[Adios2Writer] Could not create variable: local_box" << std::endl;
			return;
		}
		_engine->Put<double>(adios2_local_box, local_box.data());

		// offsets
		auto adios2_offset = _io->InquireVariable<uint64_t>(offsets_name);
		_engine->Put<uint64_t>(adios2_offset, offset);

		// simulation time
		double current_time = _simulation.getSimulationTime();
		if (domainDecomp->getRank() == 0) {
			auto adios2_simulationtime = _io->InquireVariable<double>(simtime_name);
			if (!adios2_simulationtime) {
				global_log->error() << "[Adios2Writer] Could not create variable: simulationtime" << std::endl;
				return;
			}
			_engine->Put<double>(adios2_simulationtime, current_time);
		}

		// wait for completion of write
		_engine->EndStep();
		
		clearContainers();
	} catch (std::invalid_argument& e) {
		global_log->error() << "[Adios2Writer] Invalid argument exception, STOPPING PROGRAM";
		global_log->error() << e.what();
	} catch (std::ios_base::failure& e) {
		global_log->error() << "[Adios2Writer] IO System base failure exception, STOPPING PROGRAM";
		global_log->error() << e.what();
	} catch (std::exception& e) {
		global_log->error() << "[Adios2Writer] Exception, STOPPING PROGRAM";
		global_log->error() << e.what();
	}
	//global_log->set_mpi_output_root();
	global_log->info() << "[Adios2Writer] endStep." << std::endl;
}

void Adios2Writer::finish(ParticleContainer* particleContainer, DomainDecompBase* domainDecomp, Domain* domain) {
	_engine->Close();
	global_log->info() << "[Adios2Writer] finish." << std::endl;
}
#endif