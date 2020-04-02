/*************************************************************************
*  Copyright (C) 2019 by Robert Caulk <rob.caulk@gmail.com>              *
*  Copyright (C) 2019 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

//#ifndef FLOW_GUARD
//#define FLOW_GUARD

#ifdef PARTIALSAT
#include "FlowEngine_PartialSatClayEngineT.hpp"
#include <core/Body.hpp>
#include <core/Omega.hpp>
#include <core/PartialEngine.hpp>
#include <core/Scene.hpp>
#include <core/State.hpp>
#include <pkg/common/Dispatching.hpp>
#include <pkg/dem/ScGeom.hpp>
#include <Eigen/SparseLU>

//#include<vtkSmartPointer.h>
#include <lib/compatibility/VTKCompatibility.hpp>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkLine.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkQuad.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLUnstructuredGridWriter.h>
//#include<vtkDoubleArray.h>
//#include<vtkUnstructuredGrid.h>
#include <vtkPolyData.h>

#ifdef FLOW_ENGINE
//#include<pkg/pfv/FlowEngine.hpp>
#include "FlowEngine_FlowEngineT.hpp"
#include <lib/triangulation/FlowBoundingSphere.hpp>
#include <lib/triangulation/Network.hpp>
#include <lib/triangulation/Tesselation.h>
#include <pkg/dem/TesselationWrapper.hpp>
#endif

#ifdef LINSOLV
#include <cholmod.h>
#endif

namespace yade { // Cannot have #include directive inside.

class PartialSatCellInfo : public FlowCellInfo_PartialSatClayEngineT {
public:
	double saturation; //the saturation of single pore (will be used in quasi-static imbibition and dynamic flow)
	double porosity;
	//double solidLine [4][4];//the length of intersecting line between sphere and facet. [i][j] is for facet "i" and sphere (facetVertices)"[i][j]". Last component [i][3] for 1/sumLines in the facet "i" (used by chao).
	double    dsdp; // the change of saturation for given capillary pressure
	bool      crack;
	short int crackNum;
	Real      crackArea;
	bool      isExposed;
	Real      initialPorosity;
	Real      initialSaturation;
	double    Po;      // parameter for water retention curve, unique to cell
	double    lambdao; // parameter for water retention curve
	double    vSolids;
	double    equivalentBulkModulus;
	double    oldPressure;
	bool      clumped;
	bool      initiallyCracked; // flag to continue computing initial crack perm during simulation without needing displacements or broken bonds

	PartialSatCellInfo(void)
	{
		initialPorosity   = 1.0;
		initialSaturation = 0.01;
		saturation        = 0.01;
		porosity          = 1;
		dsdp              = 0;
		vSolids           = 0;
		crack             = false;
		crackNum          = 0;
		isExposed         = false; // flag for determining if a pore is exposed to atmosphere, which controls the pressure force calculations
		Po                = 1.5;
		lambdao           = 0.2;
		clumped           = false;
		blocked           = false;
		initiallyCracked  = false;
	}

	inline double& sat(void) { return saturation; }
};

class PartialSatVertexInfo : public FlowVertexInfo_PartialSatClayEngineT {
public:
	//same here if needed
};

typedef CGT::_Tesselation<CGT::TriangulationTypes<PartialSatVertexInfo, PartialSatCellInfo>> PartialSatTesselation;
#ifdef LINSOLV
#define PartialSatBoundingSphere CGT::PartialSatLinSolv<PartialSatTesselation>
//class PartialSatBoundingSphere; // : public CGT::FlowBoundingSphereLinSolv<PartialSatTesselation> {};
#endif

typedef TemplateFlowEngine_PartialSatClayEngineT<PartialSatCellInfo, PartialSatVertexInfo, PartialSatTesselation, PartialSatBoundingSphere>
        PartialSatClayEngineT;

REGISTER_SERIALIZABLE(PartialSatClayEngineT);
YADE_PLUGIN((PartialSatClayEngineT));
class PartialSatClayEngine : public PartialSatClayEngineT {
public:
	//typedef TemplateFlowEngine_FlowEngineT<FlowCellInfo_FlowEngineT,FlowVertexInfo_FlowEngineT> FlowEngineT;
	typedef PartialSatClayEngineT::Tesselation         Tesselation;
	typedef PartialSatClayEngineT::RTriangulation      RTriangulation;
	typedef PartialSatClayEngineT::FiniteCellsIterator FiniteCellsIterator;
	typedef PartialSatClayEngineT::CellHandle          CellHandle;
	typedef PartialSatClayEngineT::VertexHandle        VertexHandle;
	typedef std::vector<CellHandle>                    VectorCell;
	typedef typename VectorCell::iterator              VCellIterator;

public:
	//PartialSatClayEngineT* clayFlow;
	double dsdp(CellHandle& cell);
	void   initializeSaturations(FlowSolver& flow);
	void   setSaturationFromPcS(CellHandle& cell);
	void   setCellsDSDP(FlowSolver& flow);
	double getTotalVolume();
	void   updateSaturation(FlowSolver& flow);
	void   updateBoundarySaturation(FlowSolver& flow);
	//	void triangulate(FlowSolver& flow);
	//double diagonalSaturationContribution(CellHandle cell);
	//double RHSSaturationContribution(CellHandle cell);
	void collectParticleSuction(FlowSolver& flow);
	void swellParticles();
	void setOriginalParticleValues();
	//void resetOriginalParticleValues() {this->setOriginalParticleValues();}
	void setInitialPorosity(FlowSolver& flow);
	void updatePorosity(FlowSolver& flow);
	void computeEquivalentBulkModuli(FlowSolver& flow);
	//	void updatePcSParams();
	double laplaceDeviate(double mu, double b);
	double exponentialDeviate(double a, double b);
	double weibullDeviate(double lambda, double k);
	double vanGenuchten(CellHandle& cell, double pc);
	void   exposureRecursion(CellHandle cell, Real bndPressure);
	void   determineFracturePaths();
	void   createSphere(shared_ptr<Body>& body, Vector3r position, Real radius);
	void   insertMicroPores(const Real fracMicroPore);
	bool   findInscribedRadiusAndLocation(CellHandle& cell, std::vector<double>& coordAndRad);
	bool   checkSphereContainedInTet(CellHandle& cell, std::vector<double>& coordAndRad);
	void   setPorosityWithImageryGrid(string imageryFilePath, FlowSolver& flow);
	void   resetPoresVolumeSolids(FlowSolver& flow);

	void       blockLowPoroRegions(FlowSolver& flow);
	void       blockMineralCellRecursion(CellHandle cell, std::vector<Body::id_t>& clumpIds);
	void       addIncidentParticleIdsToClumpList(CellHandle cell, std::vector<Body::id_t>& clumpIds);
	Body::id_t clump(vector<Body::id_t> ids, unsigned int discretization);
	void       printPorosityToFile(string file);

	// fracture network
	//void crackCellsAbovePoroThreshold(FlowSolver& flow);
	void trickPermeability(Solver* flow);
	//void trickPermOnCrackedCells(FlowSolver& flow);
	void interpolateCrack(Tesselation& Tes, Tesselation& NewTes);
	void computeFracturePerm(RTriangulation::Facet_circulator& facet, Real aperture, RTriangulation::Finite_edges_iterator& edge);
	void circulateFacets(RTriangulation::Finite_edges_iterator& edge, Real aperture);
	void saveFractureNetworkVTK(string fileName);
	void savePermeabilityNetworkVTK(string fileName);
	void blockCellsAbovePoroThreshold(FlowSolver& flow);
	void blockIsolatedCells(FlowSolver& flow);
	void simulateConfinement();
	void removeForceOnVertices(RTriangulation::Facet_circulator& facet, RTriangulation::Finite_edges_iterator& ed_it);
	void circulateFacetstoRemoveForces(RTriangulation::Finite_edges_iterator& edge);
	void removeForcesOnBrokenBonds();
	//	void setPositionsBuffer(bool current);
	Real leakOffRate;
	Real averageAperture;
	Real averageFracturePermeability;
	Real maxAperture;
	Real crackArea;
	Real crackVolume;
	Real totalFractureArea;

	virtual void initializeVolumes(FlowSolver& flow);
	virtual void updateVolumes(FlowSolver& flow);
	virtual void buildTriangulation(double pZero, Solver& flow);
	virtual void initSolver(FlowSolver& flow);
	virtual void action();

	void reloadSolver(FlowSolver& flow) { this->initSolver(flow); }

	virtual ~PartialSatClayEngine();
	Real   getCrackArea() { return crackArea; }
	Real   getCrackVolume() { return crackVolume; }
	void   printPorosity(string file) { printPorosityToFile(file); }
	double getCellSaturation(Vector3r pos) { return solver->getCellSaturation(pos[0], pos[1], pos[2]); }
	//Real getCellVelocity(Vector3r pos){return solver->getCellVelocity(pos[0], pos[1], pos[2]);}
	std::vector<Real> getCellVelocity(Vector3r pos) { return solver->getCellVelocity(pos[0], pos[1], pos[2]); }
	Real              getCellPorosity(Vector3r pos) { return solver->getCellPorosity(pos[0], pos[1], pos[2]); }
	Real              getCellVolume(Vector3r pos) { return solver->getCellVolume(pos[0], pos[1], pos[2]); }
	bool              getCellCracked(Vector3r pos) { return solver->getCellCracked(pos[0], pos[1], pos[2]); }
	Real              getAverageSaturation() { return solver->getAverageSaturation(); }
	double            getTotalSpecimenVolume() { return getTotalVolume(); }
	CELL_SCALAR_GETTER(Real, .sat(), cellSaturation);
	CELL_SCALAR_SETTER(double, .sat(), setCellSaturation);

	//FlowEngineT* flow;

	//	PartialSatClayEngineT* flow;

	void saveUnsatVtk(const char* folder, bool withBoundaries);

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(
	        PartialSatClayEngine,
	        PartialSatClayEngineT,
	        "Engine designed to simulate the partial saturation of clay and associated swelling.",
	        ((double,
	          lmbda,
	          0.2,
	          ,
	          "Lambda parameter for Van Genuchten model. Free swelling 0.4. If porosity is distributed, this value becomes cell based."))(
	                (double, pAir, 0, , "Air pressure for calculation of capillary pressure (Pair - Pwater)"))(
	                (double,
	                 Po,
	                 1.5e6,
	                 ,
	                 "Po parameter for Van Genuchten model, Free swelling 0.04e6. If porosity is distributed, this value becomes cell based."))(
	                (double, a, 6.8, , "parameter a for evolution of Po as a function of porosity"))(
	                (double, b, -1.5, , "parameter b for evolution of lambda as a function of porosity"))(
	                (double,
	                 meanInitialPorosity,
	                 -1,
	                 ,
	                 " if not negative, activates stochastic distribution for porosity. mean value of porosity for specimen"))(
	                (double, maxPorosity, -1, , "max porosity found during stochastic poro distribution. used for evolution of porosity"))(
	                (double, lambdaWeibullShape, 6., , "shape of weibull distribution of the correction factor used for porosity distribution."))(
	                (double,
	                 kappaWeibullScale,
	                 1.,
	                 ,
	                 "scale of weibull dist, this is the mean correction factor multiplied by "
	                 ":yref:`meanInitialPorosity<PartialSatClayEngine.meanInitialPorosity>`"))(
	                (double, meanPoreSizeDiameter, 0.013e-6, , "mean pore size diameter, used for stochastic generation of porosity field"))(
	                (double, phiZero, 0.25, , "initial porosity of the specimen"))(
	                (double,
	                 nUnsatPerm,
	                 0,
	                 ,
	                 "n parameter for empirical relative saturation based permeability relationship. Off by default. n=5 in Mokni2016b"))(
	                (double, SrM, 0, , "residual saturation for empirical relative saturation based permeability relationship"))(
	                (double, SsM, 1., , "saturated saturation for empirical relative saturation based permeability relationship"))(
	                (Real,
	                 bIntrinsicPerm,
	                 0,
	                 ,
	                 "b parameter for dependency of intrinsic permeability on macroporosity Gens 2011. Not active if 0 (default). Mokni2016 uses 8"))(
	                (bool, useKozeny, false, , "use Kozeny for determining the permeability based on porosity (off by default)"))(
	                (bool, partialSatEngine, 1, , "Activates the partial sat clay engine"))(
	                (bool, allCellsFractured, 0, , "use to simulate all pores fractured for debugging purposes only"))(
	                (Real,
	                 manualCrackPerm,
	                 0,
	                 ,
	                 "If >0, it overrides the crack perm calculations (useful for setting cracked cells to extremely low perms to avoid fluid movement)"))(
	                (Real, permClamp, 0, , "If >0, it prevents any permeabilities from increasing beyond this value (useful in case of very close cells"))(
	                (Real, crackAreaFactor, 1, , "Factors the area used for crack geometry computations and capillary force removal inside cracks"))
	        //((bool,freezeRadii,0,,"use to stop swelling debugging purposes only"))
	        ((bool, crackModelActive, 0, , "Activates the parallel plate approximation model for facets connected to cohesionBroken edges"))(
	                (double, alpham, 2.6048e-08, , "alpha parameter for particle volumetric strain model MPa^-1"))(
	                (double, betam, 2.10206e-08, , "beta parameter for particle volumetric strain model MPa^-1"))(
	                (bool, particleSwelling, 1, , "set false to neglect particle swelling"))(
	                (bool, freeSwelling, 1, , "if true, boundary forces are computed with pAir pressure only"))(
	                (bool, forceConfinement, false, , "If true, all the boundary particles are locked in place to simulate perfect constant volume."))(
	                (double, totalVolChange, 0, , "tracks the total volumetric strain that occured in each step"))(
	                (double, matricSuctionRatio, 1, , "The ratio of matric:osmotic suction. Facet forces computed for matricSuction fraction only."))(
	                (double, residualAperture, 0., , "residual aperture of induced cracks"))(
	                (double, apertureFactor, 1., , "factor to consider tortuosity"))(
	                (bool, calcCrackArea, true, , "The amount of crack per pore is updated if calcCrackArea=True"))(
	                (double, microStructureE, 1e6, , "The amount of crack per pore is updated if calcCrackArea=True"))(
	                (double, microStructureNu, 0.3, , "The amount of crack per pore is updated if calcCrackArea=True"))(
	                (double, microStructurePhi, 18., , "The amount of crack per pore is updated if calcCrackArea=True"))(
	                (double, microStructureRho, 2600, , "The amount of crack per pore is updated if calcCrackArea=True"))(
	                (double, microStructureAdh, 6e6, , "Adhesion between microstructure particles"))(
	                (bool, swelling, true, , "turn just particle swelling off (for debug)"))(
	                (bool, suction, true, , "turn just particle suction off (for debug)"))(
	                (bool, volumes, true, , "turn just particle volumes off (for debug)"))(
	                (double,
	                 minMicroRadFrac,
	                 0.1,
	                 ,
	                 "Used during sphere insertion checks, if inscribed sphere contacts facet it cannot be reduced further than "
	                 "minMicroRadFrac*originalInscribedRadius"))((double, minPoroClamp, 0.1, , "min clamp for distribution of porosity"))(
	                (double, maxPoroClamp, 0.8, , "max clamp for distribution of porosity. Value over 0.8 messes with water retention curve"))(
	                (bool,
	                 freezePorosity,
	                 false,
	                 ,
	                 "useful for freezing porosity values during stage for reaching initial conditions where volume changes should not impact porosity"))(
	                (bool,
	                 resetVolumeSolids,
	                 false,
	                 ,
	                 "useful if genesis process was used to reach an initial condition. We don't want the volume changes that occured during geneis to "
	                 "affect porosity evolution."))((double, Ka, 101e3, , "bulk modulus of air used for equivalent compressibility model"))(
	                (double, Kw, 2.15e9, , "bulkmodulus of water used for equivalent compressibility model"))(
	                (double, Ks, 21.2e9, , "bulkmodulus of solid used for equivalent compressibility model"))(
	                (bool, useKeq, 0, , "use the equivalent bulkmodulus for pressure field"))(
	                (bool, constantPorosity, 0, , "use the meanInitialPorosity everywhere instead of random distribution"))(
	                (bool, resetOriginalParticleValues, 0, , "use to reset initial volume and radii values for particles."))(
	                (short int, totalCracks, 0, , "total discretely connected cracks."))(
	                (bool, onlyFractureExposedCracks, false, , "if true, only the exposed cracks have tricked permeability."))(
	                (bool, freezeSaturation, false, , "if true, saturation will not change in specimen."))(
	                (double, fracPorosity, 0.5, , "porosity value used for cracked cells"))(
	                (bool, displacementBasedCracks, false, , "fracture criteria will be based on displacement instead of broken bond status"))(
	                (bool,
	                 changeCrackSaturation,
	                 false,
	                 ,
	                 "if cell becomes cracked, its saturation is reduced to residual saturation (warning this is not conservative). Useful for reducing "
	                 "partial sat permeability in these cells."))
	        //((double,alphaBoundSat,0,,"holds boundary saturation for transfering between triangulations"))
	        // less useful items below
	        ((double, betaLaplaceShape, 0.23, , "shape of laplace distribution used for porosity distribution"))(
	                (double, alphaExpRate, 0.353, , "rate of exponential distribution for porosity distribution"))(
	                (double, betaExpRate, 0.353, , "rate of exponential distribution for porosity distribution"))(
	                (double, totalSpecimenVolume, 0, , "report the total specimen volume"))((int, crackedCellTotal, 0, , "total number of cracked cells"))(
	                (string, imageryFilePath, "none", , "path to the porosity grid extracted from imagery"))(
	                (Real,
	                 mineralPoro,
	                 0,
	                 ,
	                 "If >0, all cell with porosity below this threshold will be blocked from flow and any associated particles will be clumped together"))(
	                (Real,
	                 crackCellPoroThreshold,
	                 0,
	                 ,
	                 "If >0, any cell above this porosity will follow crack logic from the beginning. (~0.35 for pellet imagery)"))(
	                (Real,
	                 blockCellPoroThreshold,
	                 0,
	                 ,
	                 "If >0, any cell above this porosity will be blocked from the beginning (partially sat crack should not participate)."))(
	                (bool,
	                 blockIsoCells,
	                 true,
	                 ,
	                 "search for cells that might be surrounded by blocked (minerals or cracks) and block them to avoid numerical instabilities."))(
	                (bool,
	                 brokenBondsRemoveCapillaryforces,
	                 false,
	                 ,
	                 "if true, broken bonds will also remove any capillary forces associated with the area of the crack"))(
	                (bool,
	                 directlyModifySatFromPoro,
	                 false,
	                 ,
	                 "if true, changes in porosity are used to directly change porosity. Normally, the water retention curve is taking care of this on its "
	                 "own."))(
	                (Real,
	                 partialSatDT,
	                 0,
	                 ,
	                 "time step used for partial sat engine. The engine will only activate once every partialSatDT/scene->dt steps. Hydromechanical forces "
	                 "estimated and added as persistant forces to particles during non partial sat time steps.")),
	        /*PartialSatClayEngineT()*/,
	        solver = shared_ptr<FlowSolver>(new FlowSolver);
	        ,
	        .def("setCellSaturation",
	             &PartialSatClayEngine::setCellSaturation,
	             (boost::python::arg("id"), boost::python::arg("temperature")),
	             "set temperature in cell 'id'.")
	                .def("getCellSaturation",
	                     &PartialSatClayEngine::getCellSaturation,
	                     (boost::python::arg("pos")),
	                     "Measure cell saturation in position pos[0],pos[1],pos[2]")
	                .def("getCellVelocity",
	                     &PartialSatClayEngine::getCellVelocity,
	                     (boost::python::arg("pos")),
	                     "Get relative cell velocity at position pos[0] pos [1] pos[2].")
	                //.def("getBoundaryVel",&TemplateFlowEngine_@TEMPLATE_FLOW_NAME@::getBoundaryVel,(boost::python::arg("boundary")),"Get total avg cell velocity associated with boundary defined by its body id.")
	                .def("getCrackArea", &PartialSatClayEngine::getCrackArea, "get the total cracked area.")
	                .def("printPorosity",
	                     &PartialSatClayEngine::printPorosity,
	                     (boost::python::arg("file") = "./porosity"),
	                     "save the porosity of the cell network.")
	                .def("getCrackVolume", &PartialSatClayEngine::getCrackVolume, "get the total cracked volume.")
	                .def("getCellPorosity",
	                     &PartialSatClayEngine::getCellPorosity,
	                     (boost::python::arg("pos")),
	                     "Measure cell porosity in position pos[0],pos[1],pos[2].")
	                .def("getCellVolume",
	                     &PartialSatClayEngine::getCellVolume,
	                     (boost::python::arg("pos")),
	                     "Get cell volume in position pos[0],pos[1],pos[2].")
	                .def("getCellCracked",
	                     &PartialSatClayEngine::getCellCracked,
	                     (boost::python::arg("pos")),
	                     "Get cell cracked in position pos[0],pos[1],pos[2].")
	                //	.def("getCellSaturation",&TwoPhaseFlowEngine::getCellSaturation,"Get saturation of cell")
	                .def("getAverageSaturation", &PartialSatClayEngine::getAverageSaturation, "Get average saturation of entire specimen.")
	                .def("saveUnsatVtk",
	                     &PartialSatClayEngine::saveUnsatVtk,
	                     (boost::python::arg("folder") = "./VTK", boost::python::arg("withBoundaries") = false),
	                     "Save pressure and saturation field in vtk format. Specify a folder name for output. The cells adjacent to the bounding spheres "
	                     "are generated conditionally based on :yref:`withBoundaries` (not compatible with periodic boundaries)")
	                .def("saveFractureNetworkVTK",
	                     &PartialSatClayEngine::saveFractureNetworkVTK,
	                     (boost::python::arg("fileName") = "./VTK"),
	                     "Save fracturenetwork as connections between cell centers")
	                .def("savePermeabilityNetworkVTK",
	                     &PartialSatClayEngine::savePermeabilityNetworkVTK,
	                     (boost::python::arg("fileName") = "./VTK"),
	                     "Save permeability network as connections between cell centers")
	                .def("insertMicroPores",
	                     &PartialSatClayEngine::insertMicroPores,
	                     (boost::python::arg("fracMicroPores")),
	                     "run to inscribe spheres in a desired fraction of existing pores.")
	                .def("reloadSolver",
	                     &PartialSatClayEngine::reloadSolver,
	                     (boost::python::arg("solver") = solver),
	                     "use after reloading a partialSat simulation and before running next step")
	                //.def("resetOriginalParticleValues",&PartialSatClayEngine::resetOriginalParticleValues,"reset the initial valume and radii values for particles.")
	                .def("getTotalSpecimenVolume", &PartialSatClayEngine::getTotalSpecimenVolume, "get the total specimen volume"))
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(PartialSatClayEngine);
} //namespaceyade
#endif
