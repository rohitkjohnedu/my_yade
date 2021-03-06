#ifdef DEPREC_CODE

/*************************************************************************
*  Copyright (C) 2006 by Luc Scholtes                                    *
*  luc.scholtes@hmg.inpg.fr                                              *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SampleCapillaryPressureEngine.hpp"
#include <lib/base/Math.hpp>
#include <core/Omega.hpp>
#include <core/Scene.hpp>
#include <pkg/dem/FrictPhys.hpp>
#include <pkg/dem/Law2_ScGeom_CapillaryPhys_Capillarity.hpp>

namespace yade { // Cannot have #include directive inside.

YADE_PLUGIN((SampleCapillaryPressureEngine));
CREATE_LOGGER(SampleCapillaryPressureEngine);

SampleCapillaryPressureEngine::~SampleCapillaryPressureEngine() { }

void SampleCapillaryPressureEngine::updateParameters()
{
	UnbalancedForce = ComputeUnbalancedForce(scene);

	if (!Phase1 && UnbalancedForce <= StabilityCriterion && !pressureVariationActivated) {
		internalCompaction = false;
		Phase1             = true;
	}

	if (Phase1 && UnbalancedForce <= StabilityCriterion && !pressureVariationActivated) {
		Real S = meanStress;
		cerr << "Smoy = " << meanStress << endl;
		if ((S > (sigma_iso - (sigma_iso * SigmaPrecision))) && (S < (sigma_iso + (sigma_iso * SigmaPrecision)))) {
			string fileName = "../data/" + Phase1End + "_" + boost::lexical_cast<string>(scene->iter) + ".xml";
			cerr << "saving snapshot: " << fileName << " ...";
			Omega::instance().saveSimulation(fileName);
			pressureVariationActivated = true;
		}
	}
}

void SampleCapillaryPressureEngine::action()
{
	updateParameters();
	TriaxialStressController::action();
	if (pressureVariationActivated) {
		if (scene->iter % 100 == 0) cerr << "pressure variation!!" << endl;

		if ((Pressure >= 0) && (Pressure <= 1000000000)) Pressure += PressureVariation;
		capillaryCohesiveLaw->capillaryPressure = Pressure;

		capillaryCohesiveLaw->fusionDetection = fusionDetection;
		capillaryCohesiveLaw->binaryFusion    = binaryFusion;
	} else {
		capillaryCohesiveLaw->capillaryPressure = Pressure;
		capillaryCohesiveLaw->fusionDetection   = fusionDetection;
		capillaryCohesiveLaw->binaryFusion      = binaryFusion;
	}
	if (scene->iter % 100 == 0) cerr << "capillary pressure = " << Pressure << endl;
	capillaryCohesiveLaw->scene = scene;
	;
	capillaryCohesiveLaw->action();
	UnbalancedForce = ComputeUnbalancedForce(scene);
}

} // namespace yade

#endif //DEPREC CODE
