#ifndef MIXINGRULE_BASE_H_
#define MIXINGRULE_BASE_H_

#include <string>

#include "utils/Logger.h"
#include "utils/mardyn_assert.h"
#include "utils/xmlfileUnits.h"

class MixingRuleBase {
public:
	MixingRuleBase(){}
	virtual ~MixingRuleBase(){}

	virtual void readXML(XMLfileUnits& xmlconfig) {
		int cid1;
		int cid2;
		xmlconfig.getNodeValue("@cid1", cid1);
		xmlconfig.getNodeValue("@cid2", cid2);

		if (cid1 == cid2) {
			Log::global_log->error() << "Mixing rules: cid1 and cid2 must not be the same but are both " << cid1 << std::endl;
			mardyn_exit(1);
		} else if ((cid1 <= 0) or (cid2 <= 0)) {
			Log::global_log->error() << "Mixing rules: cid1 and cid2 must be greater than zero" << std::endl;
			mardyn_exit(1);
		// Symmetry for mixing rules is assumed
		// cid1 must be less than cid2
		} else if (cid1 > cid2) {
			_cid1 = cid2 - 1;  // component id - 1 to convert to internal format starting with 0
			_cid2 = cid1 - 1;
		} else {
			_cid1 = cid1 - 1;
			_cid2 = cid2 - 1;
		}
	}

	int getCid1() const { return _cid1; }
	int getCid2() const { return _cid2; }

	void setCid1(int cid1) { _cid1 = cid1; }  // Internal cid starting with 0
	void setCid2(int cid2) { _cid2 = cid2; }  // Internal cid starting with 0

	virtual std::string getType() const = 0;

	const std::vector<double> &getParameters() const { return _parameters; };
	void setParameters(std::vector<double> params) { _parameters = params; };

	// Parameters of mixing rule
	// e.g. for LB it contains (eta, xi)
	std::vector<double> _parameters;

private:
	// Internal cids starting with 0
	int _cid1{};
	int _cid2{};
};

#endif /* MIXINGRULE_BASE_H_ */
