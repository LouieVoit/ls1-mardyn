/*
 * FlopRateWriter.h
 *
 *  Created on: 7 Feburary 2018
 *      Author: kruegener
 */

#ifndef SRC_UTILS_TESTPLUGIN_H_
#define SRC_UTILS_TESTPLUGIN_H_

#include "utils/PluginBase.h"

class TestPlugin: public PluginBase {
public:
    TestPlugin() {}
    ~TestPlugin() {}

    //! @brief will be called at the beginning of the simulation
    //!
    //! Some OutputPlugins will need some initial things to be done before
    //! the output can start, e.g. opening some files. This method will
    //! be called once at the beginning of the simulation (see Simulation.cpp)
    void init(ParticleContainer* particleContainer,
                    DomainDecompBase* domainDecomp, Domain* domain) {
        global_log->debug()  << "[TESTPLUGIN] TESTPLUGIN INIT" << std::endl;
    }

    void readXML(XMLfileUnits& xmlconfig) {
        global_log -> debug() << "[TESTPLUGIN] READING XML" << endl;
    }

    /** @brief Method beforeForces will be called before forcefields have been applied
            *
            * make pure Virtual ?
    */

    void beforeForces(
            ParticleContainer* particleContainer, DomainDecompBase* domainDecomp,
            unsigned long simstep
    ) {
        global_log -> debug() << "[TESTPLUGIN] BEFORE FORCES" << endl;
    }

    /** @brief Method afterForces will be called after forcefields have been applied
     *
     * make pure Virtual ?
     */
    void afterForces(
            ParticleContainer* particleContainer, DomainDecompBase* domainDecomp,
            unsigned long simstep
    ) {
        global_log->debug()  << "[TESTPLUGIN] TESTPLUGIN AFTER FORCES" << endl;
    }

    /** @brief Method endStep will be called at the end of each time step.
     *
     * This method will be called every time step passing the simstep as an additional parameter.
     * It can be used e.g. to write per time step data to a file or perform additional computations.
     * @param particleContainer  particle container storing the (local) molecules
     * @param domainDecomp       domain decomposition in use
     * @param domain
     */
    void endStep(
            ParticleContainer* particleContainer, DomainDecompBase* domainDecomp,
            Domain* domain, unsigned long simstep,
            std::list<ChemicalPotential>* lmu,
            std::map<unsigned, CavityEnsemble>* mcav
    ) {
        global_log->debug()  << "[TESTPLUGIN] ENDSTEP" << endl;
    }

    /** @brief Method finalOutput will be called at the end of the simulation
     *
     * This method will be called once at the end of the simulation.
     * It can be used e.g. to closing output files or writing final statistics.
     * @param particleContainer  particle container storing the (local) molecules
     * @param domainDecomp       domain decomposition in use
     * @param domain
     */
    void finish(ParticleContainer* particleContainer,
                        DomainDecompBase* domainDecomp, Domain* domain) {
        global_log->debug()  << "[TESTPLUGIN] FINISHING" << endl;
    }

    /** @brief return the name of the plugin */
    std::string getPluginName()  {
        global_log->debug()  << "[TESTPLUGIN] GETTING NAME" << endl;
        return "TestPlugin";}
    static PluginBase* createInstance() {
        global_log->debug()  << "[TESTPLUGIN] CREATE INSTANCE" << endl;
        return new TestPlugin(); }

private:

    std::ofstream _fileStream;
    unsigned long _writeFrequency;
    std::string _outputPrefix;
};

#endif /* SRC_UTILS_TESTPLUGIN_H_ */
