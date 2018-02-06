#include "VCP1CLJRMM.h"

#include "particleContainer/adapter/CellDataSoARMM.h"
#include "molecules/Molecule.h"
#include "particleContainer/ParticleCell.h"
#include "Domain.h"
#include "utils/Logger.h"
#include "ensemble/EnsembleBase.h"
#include "integrators/Integrator.h"
#include "Simulation.h"
#include "particleContainer/adapter/vectorization/MaskGatherChooser.h"

#include <algorithm>

VCP1CLJRMM::VCP1CLJRMM(Domain& domain, double cutoffRadius, double LJcutoffRadius) :
	CellProcessor(cutoffRadius, LJcutoffRadius), _domain(domain), _eps24(), _sig2(), _shift6(), _dtInvm(0.0), _upot6lj(0.0), _virial(0.0) {
#if VCP_VEC_TYPE==VCP_NOVEC
	global_log->info() << "VCP1CLJRMM: using no intrinsics." << std::endl;
#elif VCP_VEC_TYPE==VCP_VEC_SSE3
	global_log->info() << "VCP1CLJRMM: using SSE3 intrinsics." << std::endl;
#elif VCP_VEC_TYPE==VCP_VEC_AVX
	global_log->info() << "VCP1CLJRMM: using AVX intrinsics." << std::endl;
#elif VCP_VEC_TYPE==VCP_VEC_AVX2
	global_log->info() << "VCP1CLJRMM: using AVX2 intrinsics." << std::endl;
#elif (VCP_VEC_TYPE==VCP_VEC_KNC) || (VCP_VEC_TYPE==VCP_VEC_KNC_GATHER)
	global_log->info() << "VCP1CLJRMM: using KNC intrinsics." << std::endl;
#elif (VCP_VEC_TYPE==VCP_VEC_KNL) || (VCP_VEC_TYPE==VCP_VEC_KNL_GATHER)
	global_log->info() << "VCP1CLJRMM: using KNL intrinsics." << std::endl;
#endif

	const Component& componentZero = _simulation.getEnsemble()->getComponents()->front();
	const unsigned int componentZeroID = componentZero.ID();
	ParaStrm & p = _domain.getComp2Params()(componentZeroID, componentZeroID);
	p.reset_read();
	double e, sig, shi;
	p >> e; p >> sig; p >> shi;
	_eps24 = static_cast<vcp_real_calc>(e);
	_sig2 = static_cast<vcp_real_calc>(sig);
	_shift6 = static_cast<vcp_real_calc>(shi);

	if (global_simulation != nullptr and global_simulation->getIntegrator() != nullptr) {
		double dt = global_simulation->getIntegrator()->getTimestepLength();
		_dtInvm = dt / componentZero.m();
	} else {
		global_log->info() << "VCP1CLJRMM: initialize dtInv2m via setter method necessary." << endl;
	}

	// initialize thread data
	_numThreads = mardyn_get_max_threads();
	global_log->info() << "VCP1CLJRMM: allocate data for "
			<< _numThreads << " threads." << std::endl;
	_threadData.resize(_numThreads);

	#if defined(_OPENMP)
	#pragma omp parallel
	#endif
	{
		VCP1CLJRMMThreadData * myown = new VCP1CLJRMMThreadData();
		const int myid = mardyn_get_thread_num();
		_threadData[myid] = myown;
	} // end pragma omp parallel
}

VCP1CLJRMM::~VCP1CLJRMM() {
	#if defined(_OPENMP)
	#pragma omp parallel
	#endif
	{
		const int myid = mardyn_get_thread_num();
		delete _threadData[myid];
	}
}

void VCP1CLJRMM::initTraversal() {
	mardyn_assert(_dtInvm != static_cast<vcp_real_calc>(0.0));

	#if defined(_OPENMP)
	#pragma omp master
	#endif
	{
		_upot6lj = 0.0;
		_virial = 0.0;
	} // end pragma omp master

	global_log->debug() << "VCP1CLJRMM::initTraversal()." << std::endl;
}

void VCP1CLJRMM::processCellPair(ParticleCell& cell1, ParticleCell& cell2, bool sumAll) {
	mardyn_assert(&cell1 != &cell2);
	ParticleCellRMM & cellRMM1 = downcastCellReferenceRMM(cell1);
	ParticleCellRMM & cellRMM2 = downcastCellReferenceRMM(cell2);

	CellDataSoARMM& soa1 = cellRMM1.getCellDataSoA();
	CellDataSoARMM& soa2 = cellRMM2.getCellDataSoA();
	const bool c1Halo = cellRMM1.isHaloCell();
	const bool c2Halo = cellRMM2.isHaloCell();

	// this variable determines whether
	// _calcPairs(soa1, soa2) or _calcPairs(soa2, soa1)
	// is more efficient
	const bool calc_soa1_soa2 = (soa1.getMolNum() <= soa2.getMolNum());

	// if one cell is empty, or both cells are Halo, skip
	if (soa1.getMolNum() == 0 or soa2.getMolNum() == 0 or (c1Halo and c2Halo)) {
		return;
	}

	// Macroscopic conditions:
	// if none of the cells is halo, then compute
	// if one of them is halo:
	// 		if full_c1-index < full_c2-index, then compute
	// 		else, then don't compute
	// This saves the Molecule::isLessThan checks
	// and works similar to the "Half-Shell" scheme

	const bool ApplyCutoff = true; 
	
        if(sumAll) { // sumAll
		const bool CalculateMacroscopic = true; // is now always set to true
        
		if (calc_soa1_soa2) {
		    _calculatePairs<CellPairPolicy_<ApplyCutoff>, CalculateMacroscopic, MaskGatherC>(soa1, soa2);
		} else {
		    _calculatePairs<CellPairPolicy_<ApplyCutoff>, CalculateMacroscopic, MaskGatherC>(soa2, soa1);
		}
	} else { // sumHalf
		if ((not c1Halo and not c2Halo) or						// no cell is halo or
				(cellRMM1.getCellIndex() < cellRMM2.getCellIndex())) 		// one of them is halo, but cellRMM1.index < cellRMM2.index
		{
			const bool CalculateMacroscopic = true;

			if (calc_soa1_soa2) {
				_calculatePairs<CellPairPolicy_<ApplyCutoff>, CalculateMacroscopic, MaskGatherC>(soa1, soa2);
			} else {
				_calculatePairs<CellPairPolicy_<ApplyCutoff>, CalculateMacroscopic, MaskGatherC>(soa2, soa1);
			}

		} else {
			mardyn_assert(c1Halo != c2Halo);							// one of them is halo and
			mardyn_assert(not (cellRMM1.getCellIndex() < cellRMM2.getCellIndex()));

			const bool CalculateMacroscopic = false;

			if (calc_soa1_soa2) {
				_calculatePairs<CellPairPolicy_<ApplyCutoff>, CalculateMacroscopic, MaskGatherC>(soa1, soa2);
			} else {
				_calculatePairs<CellPairPolicy_<ApplyCutoff>, CalculateMacroscopic, MaskGatherC>(soa2, soa1);
			}
		}
	}
}


void VCP1CLJRMM::processCell(ParticleCell& cell) {
	ParticleCellRMM & cellRMM = downcastCellReferenceRMM(cell);

	CellDataSoARMM& soa = cellRMM.getCellDataSoA();
	if (cellRMM.isHaloCell() or soa.getMolNum() < 2) {
		return;
	}
	const bool CalculateMacroscopic = true;
	const bool ApplyCutoff = true;
	_calculatePairs<SingleCellPolicy_<ApplyCutoff>, CalculateMacroscopic, MaskGatherC>(soa, soa);
}

void VCP1CLJRMM::endTraversal() {
	vcp_real_calc glob_upot6lj = 0.0;
	vcp_real_calc glob_virial = 0.0;

	#if defined(_OPENMP)
	#pragma omp parallel reduction(+:glob_upot6lj, glob_virial)
	#endif
	{
		const int tid = mardyn_get_thread_num();

		// reduce vectors and clear local variable
		vcp_real_calc thread_upot = 0.0, thread_virial = 0.0;

		load_hSum_Store_Clear(&thread_upot, _threadData[tid]->_upot6ljV);
		load_hSum_Store_Clear(&thread_virial, _threadData[tid]->_virialV);

		// add to global sum
		glob_upot6lj += thread_upot;
		glob_virial += thread_virial;
	} // end pragma omp parallel reduction

	_upot6lj = glob_upot6lj;
	_virial = glob_virial;
	_domain.setLocalVirial(_virial /*+ 3.0 * _myRF*/);
	_domain.setLocalUpot(_upot6lj / 6.0 /*+ _upotXpoles + _myRF*/);
}

const RealCalcVec one = RealCalcVec::set1(1.0);

template<bool calculateMacroscopic>
vcp_inline void VCP1CLJRMM::_loopBodyLJ(
	const RealCalcVec& c_dx, const RealCalcVec& c_dy, const RealCalcVec& c_dz, const RealCalcVec& c_r2,
	RealCalcVec& f_x, RealCalcVec& f_y, RealCalcVec& f_z,
	RealCalcVec& sum_upot6lj, RealCalcVec& sum_virial,
	const MaskCalcVec& forceMask,
	const RealCalcVec& eps_24, const RealCalcVec& sig2,
	const RealCalcVec& shift6)
{
#if 0
	const RealCalcVec r2_inv_unmasked = one / c_r2;
	const RealCalcVec r2_inv = RealCalcVec::apply_mask(r2_inv_unmasked, forceMask);
#else
	const RealCalcVec r2_inv = RealCalcVec::fastReciprocal_mask(c_r2, forceMask);
#endif


	const RealCalcVec lj2 = sig2 * r2_inv;//1FP (scale)
	const RealCalcVec lj4 = lj2 * lj2;//1FP (scale)
	const RealCalcVec lj6 = lj4 * lj2;//1FP (scale)
	const RealCalcVec lj12 = lj6 * lj6;//1FP (scale)
	const RealCalcVec lj12m6 = lj12 - lj6;//1FP (scale)

	const RealCalcVec eps24r2inv = eps_24 * r2_inv;//1FP (scale)
	const RealCalcVec lj12lj12m6 = lj12 + lj12m6;//1FP (scale)
	const RealCalcVec scale = eps24r2inv * lj12lj12m6;//1FP (scale)

	f_x = c_dx * scale;//1FP (apply scale)
	f_y = c_dy * scale;//1FP (apply scale)
	f_z = c_dz * scale;//1FP (apply scale)

	// Check if we have to add the macroscopic values up
	if (calculateMacroscopic) {

		const RealCalcVec upot_sh = RealCalcVec::fmadd(eps_24, lj12m6, shift6); //2 FP upot				//shift6 is not masked -> we have to mask upot_shifted
		const RealCalcVec upot_masked = RealCalcVec::apply_mask(upot_sh, forceMask); //mask it

		sum_upot6lj = sum_upot6lj + upot_masked;//1FP (sum macro)

		sum_virial = sum_virial + c_dx * f_x + c_dy * f_y + c_dz * f_z;//1 FP (sum macro) + 5 FP (virial)
	}
}

template<class ForcePolicy, bool CalculateMacroscopic, class MaskGatherChooser>
vcp_inline void VCP1CLJRMM::_calculatePairs(CellDataSoARMM& soa1, CellDataSoARMM& soa2) {

	const int tid = mardyn_get_thread_num();
	VCP1CLJRMMThreadData &my_threadData = *_threadData[tid];

	// initialize dist lookups
	my_threadData._centers_dist_lookup.resize_zero_shrink(soa2.getMolNum(), true, false);

	// Pointer for molecules
	const vcp_real_calc * const soa1_mol_pos_x = soa1.r_xBegin();
	const vcp_real_calc * const soa1_mol_pos_y = soa1.r_yBegin();
	const vcp_real_calc * const soa1_mol_pos_z = soa1.r_zBegin();

	      vcp_real_calc * const soa1_mol_vel_x = soa1.v_xBegin();
	      vcp_real_calc * const soa1_mol_vel_y = soa1.v_yBegin();
	      vcp_real_calc * const soa1_mol_vel_z = soa1.v_zBegin();

	const vcp_real_calc * const soa2_mol_pos_x = soa2.r_xBegin();
	const vcp_real_calc * const soa2_mol_pos_y = soa2.r_yBegin();
	const vcp_real_calc * const soa2_mol_pos_z = soa2.r_zBegin();

	      vcp_real_calc * const soa2_mol_vel_x = soa2.v_xBegin();
	      vcp_real_calc * const soa2_mol_vel_y = soa2.v_yBegin();
	      vcp_real_calc * const soa2_mol_vel_z = soa2.v_zBegin();

	vcp_lookupOrMask_single* const soa2_ljc_dist_lookup = my_threadData._ljc_dist_lookup;

	RealCalcVec sum_upot6lj = RealCalcVec::zero();
	RealCalcVec sum_virial = RealCalcVec::zero();

	const RealCalcVec rc2 = RealCalcVec::set1(_LJCutoffRadiusSquare);
	const RealCalcVec eps24 = RealCalcVec::set1(_eps24);
	const RealCalcVec sig2 = RealCalcVec::set1(_sig2);
	const RealCalcVec shift6 = RealCalcVec::set1(_shift6);
	const RealCalcVec dtInv2m = RealCalcVec::set1(_dtInvm);

	const size_t end_ljc_j = vcp_floor_to_vec_size(soa2.getMolNum());
	const size_t end_ljc_j_longloop = vcp_ceil_to_vec_size(soa2.getMolNum());//this is ceil _ljc_num, VCP_VEC_SIZE

#if not (VCP_VEC_TYPE == VCP_VEC_KNC_GATHER or VCP_VEC_TYPE == VCP_VEC_KNL_GATHER)

	const size_t soa1_mol_num = soa1.getMolNum();
	for (size_t i = 0; i < soa1_mol_num; ++i) {
		size_t j = ForcePolicy :: InitJ(i);
		MaskCalcVec initJ_mask = ForcePolicy :: InitJ_Mask(i);

		const RealCalcVec m1_r_x = RealCalcVec::broadcast(soa1_mol_pos_x + i);
		const RealCalcVec m1_r_y = RealCalcVec::broadcast(soa1_mol_pos_y + i);
		const RealCalcVec m1_r_z = RealCalcVec::broadcast(soa1_mol_pos_z + i);

		RealCalcVec sum_fx1 = RealCalcVec::zero();
		RealCalcVec sum_fy1 = RealCalcVec::zero();
		RealCalcVec sum_fz1 = RealCalcVec::zero();

		for (; j < end_ljc_j; j += VCP_VEC_SIZE) {
			const RealCalcVec m2_r_x = RealCalcVec::aligned_load(soa2_mol_pos_x + j);
			const RealCalcVec m2_r_y = RealCalcVec::aligned_load(soa2_mol_pos_y + j);
			const RealCalcVec m2_r_z = RealCalcVec::aligned_load(soa2_mol_pos_z + j);

			const RealCalcVec m_dx = m1_r_x - m2_r_x;
			const RealCalcVec m_dy = m1_r_y - m2_r_y;
			const RealCalcVec m_dz = m1_r_z - m2_r_z;

			const RealCalcVec m_r2 = RealCalcVec::scal_prod(m_dx, m_dy, m_dz, m_dx, m_dy, m_dz);

			const MaskCalcVec forceMask = ForcePolicy::GetForceMask(m_r2, rc2, initJ_mask);

			if (MaskGatherChooser::computeLoop(forceMask)) {
				RealCalcVec fx, fy, fz;
				_loopBodyLJ<CalculateMacroscopic>(m_dx, m_dy, m_dz, m_r2, fx, fy, fz, sum_upot6lj, sum_virial, forceMask, eps24, sig2, shift6);

				vcp_simd_load_fnmadd_store<MaskGatherChooser>(soa2_mol_vel_x, j, fx, dtInv2m, forceMask);
				vcp_simd_load_fnmadd_store<MaskGatherChooser>(soa2_mol_vel_y, j, fy, dtInv2m, forceMask);
				vcp_simd_load_fnmadd_store<MaskGatherChooser>(soa2_mol_vel_z, j, fz, dtInv2m, forceMask);

				sum_fx1 = sum_fx1 + fx;
				sum_fy1 = sum_fy1 + fy;
				sum_fz1 = sum_fz1 + fz;
			}
		}
		const MaskCalcVec remainderMask = vcp_simd_getRemainderMask(soa2.getMolNum());
		if (remainderMask.movemask())
		{
			const RealCalcVec m2_r_x = RealCalcVec::aligned_load_mask(soa2_mol_pos_x + j, remainderMask);
			const RealCalcVec m2_r_y = RealCalcVec::aligned_load_mask(soa2_mol_pos_y + j, remainderMask);
			const RealCalcVec m2_r_z = RealCalcVec::aligned_load_mask(soa2_mol_pos_z + j, remainderMask);

			const RealCalcVec m_dx = m1_r_x - m2_r_x;
			const RealCalcVec m_dy = m1_r_y - m2_r_y;
			const RealCalcVec m_dz = m1_r_z - m2_r_z;

			const RealCalcVec m_r2 = RealCalcVec::scal_prod(m_dx, m_dy, m_dz, m_dx, m_dy, m_dz);

			const MaskCalcVec forceMask = remainderMask and ForcePolicy::GetForceMask(m_r2, rc2, initJ_mask);//AND remainderMask -> set unimportant ones to zero.

			if (MaskGatherChooser::computeLoop(forceMask)) {
				RealCalcVec fx, fy, fz;
				_loopBodyLJ<CalculateMacroscopic>(m_dx, m_dy, m_dz, m_r2, fx, fy, fz, sum_upot6lj, sum_virial, forceMask, eps24, sig2, shift6);

				vcp_simd_load_fnmadd_store<MaskGatherChooser>(soa2_mol_vel_x, j, fx, dtInv2m, forceMask);
				vcp_simd_load_fnmadd_store<MaskGatherChooser>(soa2_mol_vel_y, j, fy, dtInv2m, forceMask);
				vcp_simd_load_fnmadd_store<MaskGatherChooser>(soa2_mol_vel_z, j, fz, dtInv2m, forceMask);

				sum_fx1 = sum_fx1 + fx;
				sum_fy1 = sum_fy1 + fy;
				sum_fz1 = sum_fz1 + fz;
			}
		}

		hSum_Add_Store(soa1_mol_vel_x + i, sum_fx1 * dtInv2m);
		hSum_Add_Store(soa1_mol_vel_y + i, sum_fy1 * dtInv2m);
		hSum_Add_Store(soa1_mol_vel_z + i, sum_fz1 * dtInv2m);
	}

	sum_upot6lj.aligned_load_add_store(&my_threadData._upot6ljV[0]);
	sum_virial.aligned_load_add_store(&my_threadData._virialV[0]);

#else
#pragma message "TODO: RMM Mode is not implemented yet for KNC/KNL."
#endif
}
