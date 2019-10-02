// 2019 © William Chèvremont <william.chevremont@univ-grenoble-alpes.fr>

#include "LubricationWithPotential.hpp"

YADE_PLUGIN((Law2_ScGeom_PotentialLubricationPhys)(GenericPotential)(CundallStrackPotential))

bool Law2_ScGeom_PotentialLubricationPhys::go(shared_ptr<IGeom>& iGeom, shared_ptr<IPhys>& iPhys, Interaction* interaction)
{
    // Physic & Geometry
    LubricationPhys* phys=static_cast<LubricationPhys*>(iPhys.get());
    ScGeom* geom=static_cast<ScGeom*>(iGeom.get());
    
    if(!phys || !geom) {
        LOG_ERROR("Wrong physics and/or geometry!");
        return false;
    }

    // geometric parameters
    Real a((geom->radius1+geom->radius2)/2.);
    
    // End-of Interaction condition
    if(-geom->penetrationDepth > MaxDist*a) { return false; }
    
    // inititalization
	if(phys->u == -1. ) {
        phys->u = -geom->penetrationDepth;
        phys->delta = std::log(phys->u);
    }

    // Normal part
    if(!solve_normalForce(-geom->penetrationDepth/a,scene->dt*a*phys->kn/(phys->nun*3./2.),*phys)) { LOG_ERROR("Unable to determine normal forces. MAYDAY MAYDAY MAYDAY!"); return false; }
    
    potential->applyPotential(phys->u, *phys, geom->normal); // Set contactForce, potentialForce, contact.
    phys->normalLubricationForce = phys->kn*a*phys->prevDotU*geom->normal; // From implicit formulation. Prevent computing divisions.
    phys->normalForce = phys->kn*(-geom->penetrationDepth-phys->u)*geom->normal; // From regularization expression.
	
    // Get bodies properties
    Body::id_t id1 = interaction->getId1();
    Body::id_t id2 = interaction->getId2();
    const shared_ptr<Body> b1 = Body::byId(id1,scene);
    const shared_ptr<Body> b2 = Body::byId(id2,scene);
    State* s1 = b1->state.get();
    State* s2 = b2->state.get();
    
	// Shear and torques
	Vector3r C1 = Vector3r::Zero();
	Vector3r C2 = Vector3r::Zero();
    computeShearForceAndTorques_log(phys, geom, s1, s2, C1, C2);
	
    // Apply!
    scene->forces.addForce(id1,phys->normalForce+phys->shearForce);
    scene->forces.addTorque(id1,C1);

    scene->forces.addForce(id2,-(phys->normalForce+phys->shearForce));
    scene->forces.addTorque(id2,C2);
    
    return true;
}

CREATE_LOGGER(Law2_ScGeom_PotentialLubricationPhys);

bool Law2_ScGeom_PotentialLubricationPhys::solve_normalForce(Real const& un, Real const& dt, LubricationPhys & phys)
{
    // Init
    Real const& pDelta(phys.delta);
    Real const& a(phys.a);
    Real const ga(phys.kn*a);
    Real d1(pDelta - 1.), d2(pDelta + 1.),d;
    
    auto objf = [&,this](Real delta) -> Real { return potential->potential(a*std::exp(delta),phys)/ga + (1.-std::exp(pDelta - delta))/dt - un + std::exp(delta); };
    Real F1(objf(d1)),F2(objf(d2)),F;
    
    // Seek to interval containing the zero
    Real inc = (F1 < 0.) ? 1. : -1;
	inc = (F1 < F2) ? inc : -inc;
    
    while(F1*F2 >= 0 && std::isfinite(F1) && std::isfinite(F2)) {
        LOG_TRACE("d1="<<d1<<" d2="<<d2<<" F1="<<F1<<" F2="<<F2);
        d1 += inc;
        d2 += inc;
        F1 = objf(d1);
        F2 = objf(d2);
    }
    
    if(!std::isfinite(F1) || !std::isfinite(F2)) {
        // Reset and search other way
        LOG_DEBUG("Wrong direction");
        d1 = pDelta - 1.;
        d2 = pDelta + 1.;
        F1 = objf(d1);
        F2 = objf(d2);
        inc = -inc;
        
        while(F1*F2 >= 0 && std::isfinite(F1) && std::isfinite(F2)) {
            LOG_TRACE("d1="<<d1<<" d2="<<d2<<" F1="<<F1<<" F2="<<F2);
            d1 += inc;
            d2 += inc;
            F1 = objf(d1);
            F2 = objf(d2);
        }
    }
    
    if(!std::isfinite(F1) || !std::isfinite(F2)) {
        LOG_ERROR("Unable to find a start point. Abandon. d1="<<d1<<" d2="<<d2<<" F1="<<F1<<" F2="<<F2);
        return false;
    }
    
    // Iterate to find a solution
    uint i(MaxIter);
    do {
        if(F1*F2 >= 0) {
            LOG_ERROR("Boundaries have the same sign. Algorithm FAIL.");
            return false;
        }
        
        d = (d1 + d2)/2.;
        F = objf(d);
        
        if(!std::isfinite(F)) {
            LOG_ERROR("Objective function return non-real value. Abandon. d="<<d<<" F="<<F);
            return false;
        }
        
        if(std::abs(F) < SolutionTol) break;
        
        if(F*F1 < 0) {
            d2 = d;
            F2 = F;
        } else {
            d1 = d;
            F1 = F;
        }        
    } while (--i);
    
    // Apply
    Real up = std::exp(d);
    phys.delta = d;
    phys.u = a*std::exp(d);
    phys.prevDotU = un - up - potential->potential(phys.u,phys)/ga; // dotu'/u'
    
    return true;
}

Real GenericPotential::potential(Real const& , LubricationPhys const&)
{
    return 0;
}

void GenericPotential::applyPotential(Real const&, LubricationPhys& phys, Vector3r const&)
{
    phys.normalContactForce = Vector3r::Zero();
    phys.normalPotentialForce = Vector3r::Zero();
    phys.contact = false;
}

CREATE_LOGGER(GenericPotential);

Real CundallStrackPotential::potential(Real const& u, LubricationPhys const& phys)
{
    return std::min(0.,-alpha*phys.kn*phys.a*(phys.eps*phys.a-u));
}

void CundallStrackPotential::applyPotential(Real const& u, LubricationPhys& phys, Vector3r const& n)
{
    phys.contact = u < phys.eps*phys.a;
    phys.normalContactForce = (phys.contact) ? Vector3r(-alpha*phys.kn*phys.a*(phys.eps*phys.a-u)*n) : Vector3r::Zero();
    phys.normalPotentialForce = Vector3r::Zero();
}

CREATE_LOGGER(CundallStrackPotential);
