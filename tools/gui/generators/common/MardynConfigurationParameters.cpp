/*
 * MardynConfigurationParameters.cpp
 *
 * @Date: 03.08.2011
 * @Author: eckhardw
 */

#include "MardynConfigurationParameters.h"
#include "MardynConfiguration.h"
#include "Parameters/ParameterWithDoubleValue.h"
#include "Parameters/ParameterWithChoice.h"
#include "Parameters/ParameterWithStringValue.h"
#include "Parameters/ParameterWithBool.h"
#include "Tokenize.h"
#include "../MDGenerator.h"

#include <vector>
#include <iostream>


MardynConfigurationParameters::MardynConfigurationParameters(const MardynConfiguration& other)
: ParameterCollection("ConfigurationParameters", "Mardyn/ls1 Configuration", "Mardyn/ls1 Configuration",
		Parameter::BUTTON, false, false) {

	addParameter(new ParameterWithStringValue("ConfigurationParameters.ScenarioName", "Scenario name", "Name of the scenario (determines the base name of the output files)",
			Parameter::LINE_EDIT, false, other.getScenarioName()));

	std::vector<string> choices;
	choices.push_back("xml");
	choices.push_back("legacy ls1");
	addParameter(new ParameterWithChoice("ConfigurationParameters.fileformat", "Configuration File Format",
			"The format of the configuration file (xml not yet supported)", Parameter::COMBOBOX, false, choices,
			other.getOutputFormat() == MardynConfiguration::XML ? choices[0] : choices[1]));

	addParameter(new ParameterWithDoubleValue("ConfigurationParameters.timesteplength", "Length of timestep [fs]", "Length of timestep",
			Parameter::LINE_EDIT, false, other.getTimestepLength() / MDGenerator::fs_2_mardyn));

	addParameter(new ParameterWithDoubleValue("ConfigurationParameters.cutoffradius", "Cutoff radius [Angstrom]", "Cutoff Radius, i.e. cell length",
				Parameter::LINE_EDIT, false, other.getCutoffRadius() / MDGenerator::angstroem_2_atomicUnitLength));

	addParameter(new ParameterWithDoubleValue("ConfigurationParameters.LJcutoffradius", "LJ Cutoff radius [Angstrom]", "Lennard-Jones Cutoff Radius, i.e. cell length",
				Parameter::LINE_EDIT, false, other.getLJCutoffRadius() / MDGenerator::angstroem_2_atomicUnitLength));
	addParameter(new ParameterWithBool("ConfigurationParameters.principalAxisTrafo", "Perform principal axis transformation", "Perform a principal axis transformation for each component\nwhen writing the Mardyn phasespace file.",
				Parameter::CHECKBOX, false, other.performPrincipalAxisTransformation()));
}


MardynConfigurationParameters::~MardynConfigurationParameters() {
}


void MardynConfigurationParameters::setParameterValue(MardynConfiguration& config, const Parameter* parameter, const std::string valueName) {
	if (valueName == "ScenarioName") {
		string scenarioName = dynamic_cast<const ParameterWithStringValue*>(parameter)->getStringValue();
		config.setScenarioName(scenarioName);
	} else if (valueName == "fileformat") {
		string choice = dynamic_cast<const ParameterWithChoice*>(parameter)->getStringValue();
		if (choice == "xml") {
			config.setOutputFormat(MardynConfiguration::XML);
			std::cout << "Set XML!" << endl;
		} else if (choice == "legacy ls1") {
			config.setOutputFormat(MardynConfiguration::LEGACY);
			std::cout << "Set LEGACY!" << endl;
		} else {
			std::cout << "Invalid Choice for Parameter " << parameter->getNameId() << std::endl;
		}
	} else if (valueName == "timesteplength") {
		double timestepLength = dynamic_cast<const ParameterWithDoubleValue*>(parameter)->getValue();
		config.setTimestepLength(timestepLength * MDGenerator::fs_2_mardyn);
	} else if (valueName == "cutoffradius") {
		double cutoffRadius = dynamic_cast<const ParameterWithDoubleValue*>(parameter)->getValue();
		config.setCutoffRadius(cutoffRadius * MDGenerator::angstroem_2_atomicUnitLength);
	} else if (valueName == "LJcutoffradius") {
		double cutoffRadius = dynamic_cast<const ParameterWithDoubleValue*>(parameter)->getValue();
		config.setLJCutoffRadius(cutoffRadius * MDGenerator::angstroem_2_atomicUnitLength);
	} else if (valueName == "principalAxisTrafo") {
		bool performTrafo = dynamic_cast<const ParameterWithBool*>(parameter)->getValue();
		config.setPerformPrincipalAxisTransformation(performTrafo);
	} else {
		std::cout << "Invalid Parameter in ConfigurationParameters::setParameter(): " << endl;
		parameter->print();
	}
}
