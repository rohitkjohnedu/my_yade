// 2019 © William Chèvremont <william.chevremont@univ-grenoble-alpes.fr>


#include "Lubrication.hpp"

class Law2_ScGeom_PotentialLubricationPhys: public Law2_ScGeom_ImplicitLubricationPhys{
        public:
            bool go(shared_ptr<IGeom>& iGeom, shared_ptr<IPhys>& iPhys, Interaction* interaction);
            FUNCTOR2D(GenericSpheresContact,LubricationPhys);
		
            YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_PotentialLubricationPhys,
			Law2_ScGeom_ImplicitLubricationPhys,
			"Material law for lubrication + generic potential between two spheres. The \"generic potential\" is all forces derived from potential in the system, including contact. This material law will solve the system with lubrication and the provided potential.",
			// ATTR
			
			,// CTOR
			,// PY
            );
            DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_PotentialLubricationPhys);

