// 2019 © William Chèvremont <william.chevremont@univ-grenoble-alpes.fr>

#include "LubricationWithPotential.hpp"

YADE_PLUGIN((Law2_ScGeom_PotentialLubricationPhys))

bool Law2_ScGeom_PotentialLubricationPhys::go(shared_ptr<IGeom>&, shared_ptr<IPhys>&, Interaction*)
{
    return true;
}

CREATE_LOGGER(Law2_ScGeom_PotentialLubricationPhys);
