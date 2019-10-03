// 2019 © William Chèvremont <william.chevremont@univ-grenoble-alpes.fr>

#include "Lubrication.hpp"

class GenericPotential : public Serializable {
    public:
        /* This is where the magic happens.
         * This function needs to be reimplemented by childs class.
         * @param u distances between surfaces
         * @param a mean radius
         * @param phys Actual physics on which potential is computed
         * @return The total force from potential (contact + potential)
         */
        virtual Real potential(Real const& u, LubricationPhys const& phys) const;
        virtual void applyPotential(Real const& u, LubricationPhys& phys, Vector3r const& n);
//         YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(GenericPotential,Serializable,
//                                           "Generic class for potential representation in PotentialLubrication law. Don't do anything. If set as potential, the result will be a lubrication-only simulation.",
//                                           // ATTRS
//                                           , // CTOR
//                                           , // PY
//         );
        REGISTER_CLASS_AND_BASE(GenericPotential,Serializable);
        DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(GenericPotential);

class Law2_ScGeom_PotentialLubricationPhys: public Law2_ScGeom_ImplicitLubricationPhys{
        public:
            bool go(shared_ptr<IGeom>& iGeom, shared_ptr<IPhys>& iPhys, Interaction* interaction);
            
            /*
             * This function solve the lubricated interaction with provided potential. It set:
             * phys.delta = log(u/a) = log(u'),
             * phys.u = u,
             * phys.prevDotU = dotu'/u'
             * @param un dimentionless geometric distance (un/a)
             * @param dt dimentionless time step
             * @param a dimentionnal mean radius
             * @param phys Physics
             * @return false if something went wrong.
             */
            bool solve_normalForce(Real const& un, Real const& dt, LubricationPhys & phys);
            
            FUNCTOR2D(GenericSpheresContact,LubricationPhys);
		
            YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Law2_ScGeom_PotentialLubricationPhys,
			Law2_ScGeom_ImplicitLubricationPhys,
			"Material law for lubrication + potential between two spheres. The potential model include contact. This material law will solve the system with lubrication and the provided potential.",
			// ATTR
			((shared_ptr<GenericPotential>,potential, new GenericPotential(), ,"Physical potential force between spheres."))
			,// CTOR
			,// PY
            );
            DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(Law2_ScGeom_PotentialLubricationPhys);

class CundallStrackPotential : public GenericPotential {
    public:
        Real potential(Real const& u, LubricationPhys const& phys) const;
        void applyPotential(Real const& u, LubricationPhys& phys, Vector3r const& n);
        YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(CundallStrackPotential,GenericPotential,
                                          "Potential with only Cundall-and-Strack-like contact.",
                                          // ATTRS
                                          ((Real, alpha, 1, , "Bulk-to-roughness stiffness ratio"))
                                          , // CTOR
                                          , // PY
        );
        DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(CundallStrackPotential)

