/* CWBoon */

#ifdef YADE_POTENTIAL_BLOCKS
#pragma once
#include<pkg/dem/PotentialBlock.hpp>
#include<pkg/dem/PotentialBlock2AABB.hpp>


#include <vector>
#include <pkg/common/PeriodicEngines.hpp>

#include <stdio.h>



class Buoyancy: public PeriodicEngine{	

  	public:
		double getWaterVolume(const PotentialBlock* cm1,const State* state1, const double waterElevation, const Vector3r waterNormal);
		double getSignedArea(const Vector3r pt1,const Vector3r pt2, const Vector3r pt3);
		double getDet(const Eigen::MatrixXd A);
  		virtual void action(void);
  	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Buoyancy,PeriodicEngine,"Engine recording potential blocks as surfaces into files with given periodicity.",
		((Real,waterLevel,0.0,,"size of contact point"))
		((Real,density,2395.0,,"size of contact point"))
		((Vector3r,waterNormal,Vector3r(0.0,0.0,0.0) ,,"size of contact point"))
		,
		,
  	);


};
REGISTER_SERIALIZABLE(Buoyancy);

#endif // YADE_POTENTIAL_BLOCKS
