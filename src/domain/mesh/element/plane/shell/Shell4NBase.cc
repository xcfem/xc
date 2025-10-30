//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
//Shell4NBase.cpp

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "Shell4NBase.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableVector.h"
#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "preprocessor/multi_block_topology/aux_meshing.h"
#include "domain/mesh/node/Node.h"
#include "material/section/SectionForceDeformation.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/utils/coordTransformation/R3vectors.h"
#include "domain/mesh/element/utils/coordTransformation/ShellCrdTransf3dBase.h"
#include "domain/load/volumetric/SelfWeight.h"



#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/actor/MovableMatrix.h"
#include "utility/actor/actor/MovableVectors.h"


#include "utility/geom/d2/Polygon3d.h"
#include "domain/load/plane/ShellRawLoad.h"
#include "domain/load/plane/ShellUniformLoad.h"
#include "domain/load/plane/ShellStrainLoad.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/prep_handlers/LoadHandler.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "domain/mesh/element/utils/gauss_models/GaussModel.h"
#include "utility/utils/misc_utils/colormod.h"


//static data
XC::Matrix XC::Shell4NBase::stiff(24,24);
XC::Vector XC::Shell4NBase::resid(24);
XC::Matrix XC::Shell4NBase::mass(24,24);

//! @brief Releases memory.
void XC::Shell4NBase::free_mem(void)
  {
    if(theCoordTransf)
      {
        delete theCoordTransf;
        theCoordTransf= nullptr;
      }
  }

//! @brief Allocates memory.
void XC::Shell4NBase::alloc(const ShellCrdTransf3dBase *crdTransf)
  {
    free_mem();
    if(crdTransf)
      {
        theCoordTransf= crdTransf->getCopy();
	theCoordTransf->set_owner(this);
      }
  }

//! @brief Constructor
XC::Shell4NBase::Shell4NBase(int classTag, const ShellCrdTransf3dBase *crdTransf)
  : QuadBase4N<SectionFDPhysicalProperties>(0,classTag, SectionFDPhysicalProperties(4)),
  theCoordTransf(nullptr), applyLoad(0), p0()
  {
    alloc(crdTransf);
    appliedB[0]= 0.0;
    appliedB[1]= 0.0;
    appliedB[2]= 0.0;
  }

//! @brief Constructor.
//!
//! @param tag: element identifier.
//! @param classTag: class identifier.
//! @param ptr_mat: pointer to the material object.
//! @param crdTransf: coordinate transformation.
XC::Shell4NBase::Shell4NBase(int tag, int classTag,const SectionForceDeformation *ptr_mat, const ShellCrdTransf3dBase *crdTransf)
  : QuadBase4N<SectionFDPhysicalProperties>(tag,classTag,SectionFDPhysicalProperties(4,ptr_mat)),
  theCoordTransf(nullptr), applyLoad(0), p0()
  {
    alloc(crdTransf);
    appliedB[0]= 0.0;
    appliedB[1]= 0.0;
    appliedB[2]= 0.0;
  }

//! @brief Constructor
XC::Shell4NBase::Shell4NBase(int tag, int classTag,int node1,int node2,int node3,int node4,const SectionFDPhysicalProperties &physProp, const ShellCrdTransf3dBase *crdTransf)
  : QuadBase4N<SectionFDPhysicalProperties>(tag,classTag,physProp),
  theCoordTransf(nullptr), applyLoad(0), p0()
  {
    theNodes.set_id_nodes(node1,node2,node3,node4);
    alloc(crdTransf);
    theCoordTransf->initialize(theNodes);
    appliedB[0]= 0.0;
    appliedB[1]= 0.0;
    appliedB[2]= 0.0;
  }

//! @brief Copy constructor.
XC::Shell4NBase::Shell4NBase(const Shell4NBase &other)
  : QuadBase4N<SectionFDPhysicalProperties>(other), theCoordTransf(nullptr), applyLoad(other.applyLoad), p0(other.p0)
  {
    alloc(other.theCoordTransf);
    appliedB[0]= other.appliedB[0];
    appliedB[1]= other.appliedB[1];
    appliedB[2]= other.appliedB[2];
  }

//! @brief Assignment operator.
XC::Shell4NBase &XC::Shell4NBase::operator=(const Shell4NBase &other)
  {
    QuadBase4N<SectionFDPhysicalProperties>::operator=(other);
    alloc(other.theCoordTransf);
    applyLoad= other.applyLoad;
    appliedB[0]= other.appliedB[0];
    appliedB[1]= other.appliedB[1];
    appliedB[2]= other.appliedB[2];
    p0= other.p0;
    return *this;
  }

//! @brief Destructor.
XC::Shell4NBase::~Shell4NBase(void)
  { free_mem(); }

//! @brief Defines a load over the element from a vector of nodal loads
//! in local coordinates.
//!
//! @param nLoads: loads on each element node.
XC::ElementalLoad *XC::Shell4NBase::vector3dRawLoadLocal(const std::vector<Vector> &nLoads)
  {
    ShellRawLoad *retval= nullptr;
    Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        MapLoadPatterns &lPatterns= preprocessor->getLoadHandler().getLoadPatterns();
        static ID eTags(1);
        eTags[0]= getTag(); //Load for this element.
        const int &loadTag= lPatterns.getCurrentElementLoadTag(); //Load identifier.

        const size_t sz= nLoads.size();
	const size_t nn= getNumExternalNodes();
        if(sz==nn)
          {
            LoadPattern *lp= lPatterns.getCurrentLoadPatternPtr();
            if(lp)
              {
                retval= new ShellRawLoad(loadTag,nLoads,eTags);
                lp->addElementalLoad(retval);
                lPatterns.setCurrentElementLoadTag(loadTag+1);
              }
            else
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                        << " there is no current load pattern."
                        << " Load ignored."
			<< Color::def << std::endl; 
          }
        else
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                    << "; a vector of dimension " << nn
	            << " was expected."
		    << Color::def << std::endl;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; modeler not defined."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Defines a load over the element from a vector of nodal loads
//! in global coordinates.
//!
//! @param nLoads: loads on each element node.
XC::ElementalLoad *XC::Shell4NBase::vector3dRawLoadGlobal(const std::vector<Vector> &nLoads)
  {
    ElementalLoad *retval= nullptr;
    const size_t sz= nLoads.size();
    const size_t nn= getNumExternalNodes();
    if(sz==nn)
      {
        assert(theCoordTransf);
	std::vector<Vector> tmp(nn);
        // Transform loads to local coordinates.
	for(size_t i= 0;i<nn;i++)
	  {
	    const Vector v= nLoads[i];
            const Vector vTrf= theCoordTransf->getVectorLocalCoordFromGlobal(v);
	    tmp[i]= vTrf;
	  }
        retval= vector3dRawLoadLocal(tmp);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                << "; a vector of dimension " << nn
                << " was expected."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Defines a load over the element from a vector in local coordinates.
//!
//! @param v: vector that defines the load values.
XC::ElementalLoad *XC::Shell4NBase::vector3dUniformLoadLocal(const Vector &v)
  {
    ShellUniformLoad *retval= nullptr;
    Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        MapLoadPatterns &lPatterns= preprocessor->getLoadHandler().getLoadPatterns();
        static ID eTags(1);
        eTags[0]= getTag(); //Load for this element.
        const int &loadTag= lPatterns.getCurrentElementLoadTag(); //Load identifier.

        const size_t sz= v.Size();
        if(sz>2)
          {
            LoadPattern *lp= lPatterns.getCurrentLoadPatternPtr();
            if(lp)
              {
                retval= new ShellUniformLoad(loadTag,v,eTags);
                lp->addElementalLoad(retval);
                lPatterns.setCurrentElementLoadTag(loadTag+1);
              }
            else
	      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                        << " there is no current load pattern."
                        << " Load ignored."
			<< Color::def << std::endl; 
          }
        else
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                    << "; a vector of dimension 3 was expected."
		    << Color::def << std::endl;
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; modeler not defined."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Defines a load over the element from a vector in global coordinates. 
//!
//! @param v: vector that defines the load values.
XC::ElementalLoad *XC::Shell4NBase::vector3dUniformLoadGlobal(const Vector &v)
  {
    ElementalLoad *retval= nullptr;
    const size_t sz= v.Size();
    if(sz>2)
      {
        assert(theCoordTransf);
        const Vector vTrf= theCoordTransf->getVectorLocalCoordFromGlobal(v);
        retval= vector3dUniformLoadLocal(vTrf);
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; a vector of dimension 3 was expected."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Defines a strain load on this element.
//! 
XC::ElementalLoad *XC::Shell4NBase::strainLoad(const Matrix &strains)
  {
    ElementalLoad *retval= nullptr;
    Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        MapLoadPatterns &lPatterns= preprocessor->getLoadHandler().getLoadPatterns();
        static ID eTags(1);
        eTags[0]= getTag(); //Load for this element.
        const int &loadTag= lPatterns.getCurrentElementLoadTag(); //Load identifier.
        LoadPattern *lp= lPatterns.getCurrentLoadPatternPtr();
        if(lp)
          {
            ShellStrainLoad *tmp= new ShellStrainLoad(loadTag,eTags);
            if(tmp)
              {
		tmp->setStrains(strains);
                tmp->set_owner(this);
                lp->addElementalLoad(tmp);
		retval= tmp;
              }
            else
              std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; can't create load."
			<< Color::def << std::endl;
          }
        else
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; there is no current load pattern."
                    << " Load ignored."
		    << Color::def << std::endl; 
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; mdeler not defined."
		<< Color::def << std::endl;
    return retval;
  }


//! @brief return number of dofs
int XC::Shell4NBase::getNumDOF(void) const
  { return 24; }



//! @brief Update state variables.
int XC::Shell4NBase::update(void)
  {
    theCoordTransf->update();
    return QuadBase4N<SectionFDPhysicalProperties>::update();
  }

//! @brief return stiffness matrix
const XC::Matrix &XC::Shell4NBase::getTangentStiff(void) const
  {
    theCoordTransf->update();

    const int tang_flag= 1; //get the tangent
    formResidAndTangent(tang_flag); //do tangent and residual here
    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }

//! @brief return mass matrix
const XC::Matrix& XC::Shell4NBase::getMass(void) const
  {
    int tangFlag= 1;
    formInertiaTerms(tangFlag);
    if(isDead())
      mass*=dead_srf;
    return mass;
  }

//! @brief Return the element Gauss points.
const XC::GaussModel &XC::Shell4NBase::getGaussModel(void) const
  { return gauss_model_quad4; }


//! @brie Return a pointer to the i-th section of the element.
const XC::SectionForceDeformation *XC::Shell4NBase::getSectionPtr(const size_t &i) const
  {
    const SectionForceDeformation *retval= nullptr;
    const size_t sz= this->physicalProperties.size();
    if(sz>i)
      retval= this->physicalProperties[i];
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; index " << i << " out of range: (0,"
	        << sz << ")."
		<< Color::def << std::endl;    
    return retval;
  }

//! @brief Sets loads to zero.
void XC::Shell4NBase::zeroLoad(void)
  {
    QuadBase4N<SectionFDPhysicalProperties>::zeroLoad();
    applyLoad = 0;
    appliedB[0] = 0.0;
    appliedB[1] = 0.0;
    appliedB[2] = 0.0;
    p0.zero();
  }

//! @brief Applies on the element the load being passed as parameter.
int XC::Shell4NBase::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__ 
                << "; load over inactive element: "
                << getTag()
		<< Color::def << std::endl;
    else
      {
        if(SelfWeight *shellLoad= dynamic_cast<SelfWeight *>(theLoad))
	  {
	    // added compatability with selfWeight class implemented
	    // for all continuum elements, C.McGann, U.W.
	    applyLoad = 1;
	    appliedB[0] += loadFactor*shellLoad->getXFact();
	    appliedB[1] += loadFactor*shellLoad->getYFact();
	    appliedB[2] += loadFactor*shellLoad->getZFact();
	  }
        else if(ShellMecLoad *shellMecLoad= dynamic_cast<ShellMecLoad *>(theLoad))
          {
   	    computeTributaryAreas();
            const std::vector<double> areas= getTributaryAreas();
            shellMecLoad->addReactionsInBasicSystem(areas,loadFactor,p0); // Accumulate reactions in basic system
          }	
        else
          return QuadBase4N<SectionFDPhysicalProperties>::addLoad(theLoad,loadFactor);
      }
    return 0;
  }

//! @brief Creates the inertia load that corresponds to the
//! acceleration argument.
//!
//! @param accel: acceleration vector.
XC::ElementalLoad *XC::Shell4NBase::createInertiaLoad(const Vector &accel)
  {
    ElementalLoad *retval= nullptr;
    const bool haveRho= this->physicalProperties.haveRho();
    if(haveRho)
      {
	const int dim= std::min(accel.Size(),6);
	// Create 6 DOF acceleration vector.
	Vector tmpAccel(6);
	tmpAccel.Zero();
        for(int i= 0; i<dim;i++)
	  tmpAccel(i)= accel(i);
	// Create 4x6= 24 rows acceleration vector.
        Vector nodeAccel(24);
	nodeAccel.Zero();
	for(int i= 0; i<4; i++)
	  for(int j= 0; j<6;j++)
	    {
	      const int k= 6*i+j;
	      nodeAccel(k)= tmpAccel(j);
	    }        
	const int tangFlag= 1;
	formInertiaTerms(tangFlag);
        Vector force(24);
	force.addMatrixVector(1.0, mass, nodeAccel, -1.0);//= -mass*nodeAccel;
	// Extract nodal loads.
	std::vector<Vector> nLoads(4);
	for(int i=0;i<4;i++)
	  {
	    Vector nLoad(6);
	    for(int j= 0; j<6;j++)
	       {
 	         const int k= 6*i+j;
	         nLoad(j)+= force(k);
	       }
	    nLoads[i]= nLoad;
	  }
        retval= vector3dRawLoadGlobal(nLoads);
      }
    return retval;
  }

//! @brief get residual
const XC::Vector &XC::Shell4NBase::getResistingForce(void) const
  {
    theCoordTransf->update();

    const int tang_flag= 0; //don't get the tangent
    formResidAndTangent(tang_flag);
    // subtract external loads
    if(!load.isEmpty())
      resid-= load;
    resid+= theCoordTransf->getGlobalResistingForce(p0.getVector());

    if(isDead())
      resid*=dead_srf;
    return resid;
  }

//! @brief get residual with inertia terms
const XC::Vector &XC::Shell4NBase::getResistingForceIncInertia(void) const
  {
    static Vector res(24);
    res= getResistingForce();

    formInertiaTerms(0);

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      res+= this->getRayleighDampingForces();

    return res;
  }

//! @brief Add to the unbalance vector the inertial load
//! corresponding to the acceleration argument.
//!
//! @param accel: acceleration vector.
int XC::Shell4NBase::addInertiaLoadToUnbalance(const Vector &accel)
  {
    int retval= 0;

    const bool haveRho= physicalProperties.haveRho();
    if(haveRho)
      {
	const int tangFlag= 1;
	formInertiaTerms(tangFlag);

	int count= 0;
	for(int i=0;i<4;i++)
	  {
	    const Vector &Raccel= theNodes[i]->getRV(accel);
	    //XXX ¿change the basis of Raccel to the local system?
	    for(int j=0;j<6;j++)
	      resid(count++)= Raccel(i);
	  }

	if(load.isEmpty())
	  load.reset(24);
	load.addMatrixVector(1.0, mass, resid, -1.0);
      }
    return retval;
  }

double XC::Shell4NBase::getMeanInternalForce(const std::string &code) const
  { return physicalProperties.getMeanInternalForce(code); }

double XC::Shell4NBase::getMeanInternalDeformation(const std::string &code) const
  { return physicalProperties.getMeanInternalDeformation(code); }

//! @brief Consuma la coordinate transformation de acuerdo con el estado actual.
int XC::Shell4NBase::commitState(void)
  {
    int retval= theCoordTransf->commitState();
    retval+= QuadBase4N<SectionFDPhysicalProperties>::commitState();
    return retval;
  }

//! @brief Returns to the last committed state.
int XC::Shell4NBase::revertToLastCommit(void)
  {
    int retval= theCoordTransf->revertToLastCommit();
    retval+= QuadBase4N<SectionFDPhysicalProperties>::revertToLastCommit();
    return retval;
  }

//! @brief Return the initial state.
int XC::Shell4NBase::revertToStart(void)
  {
    int retval= theCoordTransf->revertToStart();
    retval+= QuadBase4N<SectionFDPhysicalProperties>::revertToStart();
    return retval;
  }

//! @brief Returns interpolation factors for a material point.
//! @param pos: natural coordinates of the material point.
XC::Vector XC::Shell4NBase::getInterpolationFactors(const ParticlePos3d &pos) const
  {
    static const int numberOfNodes= 4;
    static const int nShape= 3;
    static const int shpIndex= nShape-1;

    double xsj;  // determinant of the jacobian matrix
    static double shp[nShape][numberOfNodes]; //storage for shape functions values.
    Vector retval(numberOfNodes);


    //compute basis vectors and local nodal coordinates
    //theCoordTransf->update( );


    double sx[2][2]; //inverse jacobian matrix (not used here).
    shape2d(pos.r_coordinate(), pos.s_coordinate(), xl, shp, xsj, sx);

    //node loop to extract factors
    retval.Zero( );
    for(int j= 0; j < numberOfNodes; j++ )
      retval[j]= shp[shpIndex][j]; // last row for each node.

    return retval;
  }

//! @brief Return the cartesian coordinates of the argument.
//!
//! @param p: position in natural coordinates.
Pos3d XC::Shell4NBase::getCartesianCoordinates(const ParticlePos2d &p, bool initialGeometry) const
  {
    Pos3d retval;
    //Values of the shape functions.
    static const int numberOfNodes= 4;
    static const int nShape= 3;
    double xsj;  // determinant of the jacobian matrix
    double sx[2][2]; //inverse jacobian matrix.
    static double shp[nShape][numberOfNodes];  //shape functions at point p
    shape2d(p.r_coordinate(), p.s_coordinate(), xl, shp, xsj, sx);
    const double N1= shp[nShape-1][0];
    const double N2= shp[nShape-1][1];
    const double N3= shp[nShape-1][2];
    const double N4= shp[nShape-1][3];
    //Node positions
    double factor= 1.0;
    if(initialGeometry)
      factor= 0.0;
    const Pos3d n1Pos= theNodes[0]->getCurrentPosition3d(factor);
    const Pos3d n2Pos= theNodes[1]->getCurrentPosition3d(factor);
    const Pos3d n3Pos= theNodes[2]->getCurrentPosition3d(factor);
    const Pos3d n4Pos= theNodes[3]->getCurrentPosition3d(factor);
    const double x= N1*n1Pos.x()+N2*n2Pos.x()+N3*n3Pos.x()+N4*n4Pos.x();
    const double y= N1*n1Pos.y()+N2*n2Pos.y()+N3*n3Pos.y()+N4*n4Pos.y();
    const double z= N1*n1Pos.z()+N2*n2Pos.z()+N3*n3Pos.z()+N4*n4Pos.z();
    retval= Pos3d(x,y,z);
    return retval;
  }

//! @brief Return the cartesian coordinates of the argument.
//!
//! @param p: position in natural coordinates.
Pos3d XC::Shell4NBase::getCartesianCoordinates(const ParticlePos3d &p, bool initialGeometry) const
  {
    const ParticlePos2d p2d(p.r_coordinate(), p.s_coordinate());
    return getCartesianCoordinates(p2d, initialGeometry);
  }

//! @brief Returns interpolation factors for a material point.
XC::Vector XC::Shell4NBase::getInterpolationFactors(const Pos3d &pos) const
  { return getInterpolationFactors(getNaturalCoordinates(pos)); }

//! @brief Return the local coordinates that correspond to the i-th node.
XC::ParticlePos3d XC::Shell4NBase::getLocalCoordinatesOfNode(const int &i) const
  { return ParticlePos3d(xl[0][i],xl[1][i],0.0); }

//! @brief Return the natural coordinates that correspond to the argument.
XC::ParticlePos3d XC::Shell4NBase::getNaturalCoordinates(const Pos3d &p, bool initialGeometry) const
  { return theCoordTransf->getNaturalCoordinates(p,xl); }

//! @brief Returns interpolated displacements for a material point.
XC::Vector XC::Shell4NBase::getInterpolatedDisplacements(const ParticlePos3d &pos) const
  {
    //  six(6) nodal dof's ordered :
    //
    //    -        - 
    //   |    u1    |   <---plate membrane
    //   |    u2    |
    //   |----------|
    //   |  w= u3  |   <---plate bending
    //   |  theta1  | 
    //   |  theta2  | 
    //   |----------|
    //   |  theta3  |   <---drill 
    //    -        -  


    static const int ndf= 6;
    static const int numberOfNodes= 4;
    Vector displacement(ndf);


    //compute basis vectors and local nodal coordinates
    //theCoordTransf->update( );


    const Vector factors= getInterpolationFactors(pos);

    //node loop to compute displacements
    displacement.Zero();
    for(int j= 0; j < numberOfNodes; j++)
      //displacement += ( factor[j] * theNodes[j]->getTrialAccel() );
      displacement.addVector(1.0, theCoordTransf->getBasicTrialAccel(j), factors[j] );

    return displacement;
  }

//! @brief Returns interpolattion factors for a material point.
XC::Vector XC::Shell4NBase::getInterpolatedDisplacements(const Pos3d &pos) const
  { return getInterpolatedDisplacements(getNaturalCoordinates(pos)); }

//! @brief Form inertia terms
void XC::Shell4NBase::formInertiaTerms( int tangFlag ) const
  {

    //translational mass only
    //rotational inertia terms are neglected
    static const int ndf= 6;
    static const int numberOfNodes= 4;
    static const int numberGauss= 4;
    static const int nShape= 3;
    static const int massIndex= nShape - 1;

    double xsj;  // determinant of the jacobian matrix
    double dvol; //volume element
    static double shp[nShape][numberOfNodes];  //shape functions at a gauss point
    static Vector momentum(ndf);


    double sx[2][2]; //inverse jacobian matrix.


    //zero mass
    mass.Zero( );

    //compute basis vectors and local nodal coordinates
    //theCoordTransf->update( );


    //gauss loop
    for(int i= 0; i < numberGauss; i++ )
      {
        //get shape functions
        const GaussPoint &gp= getGaussModel().getGaussPoints()[i];
        shape2d(gp.r_coordinate(), gp.s_coordinate(), xl, shp, xsj, sx);

        //volume element to also be saved
        dvol= gp.weight() * xsj;


        //node loop to compute accelerations
        momentum.Zero( );
        for(int j= 0; j < numberOfNodes; j++ )
          //momentum += ( shp[massIndex][j] * theNodes[j]->getTrialAccel() );
          momentum.addVector(1.0, theCoordTransf->getBasicTrialAccel(j), shp[massIndex][j] );

        //density on the Gauss point i.
        const double rhoH= physicalProperties[i]->getArealRho(); //mass per unit area.

        //multiply acceleration by density to form momentum
        momentum*= rhoH;


        //residual and tangent calculations node loops
	double temp, massJK;
        for(int j=0, jj=0; j<numberOfNodes; j++, jj+=ndf )
          {
            temp= shp[massIndex][j] * dvol;
            for(int p= 0;p<3;p++)
              resid(jj+p )+= (temp*momentum(p));

            if(tangFlag == 1 && rhoH != 0.0)
              {
               //multiply by density
               temp *= rhoH;

               for(int k=0, kk=0; k<numberOfNodes; k++, kk+=ndf )
                 {
                   massJK= temp * shp[massIndex][k];
                   //node-node translational mass
                   for(int p= 0;p<3;p++)
                     mass(jj+p,kk+p)+= massJK;
                   //we put a very small rotational mass
                   //(error is negligible) but not zero.
                   //This way we avoid singularity of M.
                   for(int p= 3;p<6;p++)
                     mass(jj+p,kk+p)+= massJK*1.0e-10;
                 } // end for k loop
              } // end if tang_flag
          } // end for j loop
      } //end for i gauss loop
  }

//! @brief compute local coordinates and basis
void XC::Shell4NBase::computeBasis(void)
  {
    theCoordTransf->initialize(theNodes);
    theCoordTransf->setup_nodal_local_coordinates();
  }

//! @brief shape function routine for MITC4 elements.
//! @param ss "s" natural coordinate of the point.
//! @param tt "t" natural coordinate of the point.
//! @param x[2][4]
//! @param shp[3][4] shape function values at the (ss,tt) point (value and two derivatives for each shape function).
//! @param xsj jacobian determinant.
//! @param sx jacobian inverse
void XC::Shell4NBase::shape2d(const double &ss, const double &tt,const double x[2][4],double shp[3][4],double &xsj, double sx[2][2])
  {

    static const double s[]= { -0.5,  0.5, 0.5, -0.5 };
    static const double t[]= { -0.5, -0.5, 0.5,  0.5 };

    static double xs[2][2]; // jacobian.

    for(int i= 0; i < 4; i++ )
      {
        shp[2][i]= ( 0.5 + s[i]*ss )*( 0.5 + t[i]*tt );
        shp[0][i]= s[i] * ( 0.5 + t[i]*tt );
        shp[1][i]= t[i] * ( 0.5 + s[i]*ss );
      } // end for i


    // Construct jacobian and its inverse

    for(int i= 0; i < 2; i++ )
      {
        for(int j= 0; j < 2; j++ )
          {
            xs[i][j]= 0.0;
            for(int k= 0;k<4;k++)
              xs[i][j]+=  x[i][k] * shp[j][k];
          } //end for j
      }  // end for i

    xsj= xs[0][0]*xs[1][1] - xs[0][1]*xs[1][0];

    //inverse jacobian
    double jinv= 1.0 / xsj;
    sx[0][0]=  xs[1][1] * jinv;
    sx[1][1]=  xs[0][0] * jinv;
    sx[0][1]= -xs[0][1] * jinv;
    sx[1][0]= -xs[1][0] * jinv;


    //form global derivatives
    double temp= 0.0;
    for(int i= 0; i < 4; i++ )
      {
        temp= shp[0][i]*sx[0][0] + shp[1][i]*sx[1][0];
        shp[1][i]= shp[0][i]*sx[0][1] + shp[1][i]*sx[1][1];
        shp[0][i]= temp;
      } // end for i
    return;
  }

//! @brief Send the coordinate transformation through the communicator argument.
int XC::Shell4NBase::sendCoordTransf(int posFlag,const int &posClassTag,const int &posDbTag,Communicator &comm)
  {
    int retval= 0;
    DbTagData &data= getDbTagData();
    if(!theCoordTransf)
      data.setDbTagDataPos(posFlag,1);
    else
      {
        data.setDbTagDataPos(posFlag,0);
        data.setDbTagDataPos(posClassTag,theCoordTransf->getClassTag());
        retval= comm.sendMovable(*theCoordTransf,data,CommMetaData(posDbTag));
      }
    return retval;
  }

//! @brief Receives the coordinate transformation through the communicator argument.
int XC::Shell4NBase::recvCoordTransf(int posFlag,const int &posClassTag,const int &posDbTag,const Communicator &comm)
  {
    int res= 0;
    DbTagData &data= getDbTagData();
    const int flag= data.getDbTagDataPos(posFlag);
    if(flag == 0)
      {
        if(!theCoordTransf)
          {
            theCoordTransf= comm.getNewShellCrdTransf3d(data.getDbTagDataPos(posClassTag));
            if(!theCoordTransf)
              std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; ran out of memory."
		        << Color::def << std::endl;
          }
        if(theCoordTransf)
          {
            res= comm.receiveMovable(*theCoordTransf,data,CommMetaData(posDbTag));
            theCoordTransf->revertToLastCommit();// Revert the crdtransf to its last committed state
            if(res<0)
              std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			<< "; failed to receive vector data."
		        << Color::def << std::endl;
          }
      }
    return res;
  }

//! @brief Send members through the communicator argument.
int XC::Shell4NBase::sendData(Communicator &comm)
  {
    int res= QuadBase4N<SectionFDPhysicalProperties>::sendData(comm);
    res+= comm.sendDoubles(xl[1][0],xl[1][1],xl[1][2],xl[1][3],getDbTagData(),CommMetaData(8));
    res+= comm.sendMatrix(Ki,getDbTagData(),CommMetaData(9));
    res+= sendCoordTransf(10,11,12,comm);
    res+= comm.sendInt(applyLoad,getDbTagData(),CommMetaData(13));
    res+= comm.sendDoubles(appliedB[0],appliedB[1],appliedB[2],getDbTagData(),CommMetaData(14));
    res+= p0.sendData(comm,getDbTagData(),CommMetaData(15));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::Shell4NBase::recvData(const Communicator &comm)
  {
    int res= QuadBase4N<SectionFDPhysicalProperties>::recvData(comm);
    res+= comm.receiveDoubles(xl[1][0],xl[1][1],xl[1][2],xl[1][3],getDbTagData(),CommMetaData(8));
    res+= comm.receiveMatrix(Ki,getDbTagData(),CommMetaData(9));
    res+= recvCoordTransf(10,11,12,comm);
    res+= comm.receiveInt(applyLoad,getDbTagData(),CommMetaData(13));
    res+= comm.receiveDoubles(appliedB[0],appliedB[1],appliedB[2],getDbTagData(),CommMetaData(14));
    res+= p0.receiveData(comm,getDbTagData(),CommMetaData(15));
    return res;
  }

//! @brief Returns a matrix with the axes of the element as matrix rows
//! [[x1,y1,z1],[x2,y2,z2],...·]
XC::Matrix XC::Shell4NBase::getLocalAxes(bool initialGeometry) const
  { return theCoordTransf->getLocalAxes(initialGeometry); }

//! @brief Returns a pointer to the coordinate transformation.
XC::ShellCrdTransf3dBase *XC::Shell4NBase::getCoordTransf(void)
  { return theCoordTransf; }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
const XC::ShellCrdTransf3dBase *XC::Shell4NBase::getCoordTransf(void) const
  { return theCoordTransf; }

//! @brief Element response.
XC::Response *XC::Shell4NBase::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    Response *theResponse= QuadBase4N<SectionFDPhysicalProperties>::setResponse(argv,eleInformation);
    if(!theResponse)
      {
	// output.tag("ElementOutput");
	// output.attr("eleType", "ShellNLDKGQ");
	// output.attr("eleTag",this->getTag());
	const int numNodes= this->getNumExternalNodes();
	static char nodeData[32];

	for(int i=0; i<numNodes; i++)
	  {
	    sprintf(nodeData,"node%d",i+1);
	    //output.attr(nodeData,nodes(i));
	  }

	if((argv[0]=="force") || (argv[0]=="forces") ||
	    (argv[0]=="globalForce") || (argv[0]=="globalForces"))
	  {
	    const Vector &force= this->getResistingForce();
	    int size= force.Size();
	    for(int i=0; i<size; i++)
	      {
		sprintf(nodeData,"P%d",i+1);
		//output.tag("ResponseType",nodeData);
	      }
	    theResponse= new ElementResponse(this, 1, this->getResistingForce());
	  }
	//output.endTag();
      }
    return theResponse;
  }

//! @brief Obtain information from an analysis.
int XC::Shell4NBase::getResponse(int responseID, Information &eleInfo)
  {
    int retval= QuadBase4N<SectionFDPhysicalProperties>::getResponse(responseID, eleInfo);
    if(retval==-1)
      {
	switch (responseID)
	  {
	  case 1: // global forces
	    retval= eleInfo.setVector(this->getResistingForce());
	    break;
	  default:
	    retval= -1;
	    break;
	  }
      }
    return retval;
  }

//! @brief print out element data
void XC::Shell4NBase::Print(std::ostream &s, int flag ) const
  {
    if(flag == -1)
      {
        int eleTag= this->getTag();
        s << "EL_QUAD4\t" << eleTag << "\t";
        s << eleTag << "\t" << 1;
        s  << "\t" << theNodes.getTagNode(0) << "\t" << theNodes.getTagNode(1);
        s  << "\t" << theNodes.getTagNode(2) << "\t" << theNodes.getTagNode(3) << "\t0.00";
        s << std::endl;
        s << "PROP_2D\t" << eleTag << "\t";
        s << eleTag << "\t" << 1;
        s  << "\t" << -1 << "\tSHELL\t1.0\0.0";
        s << std::endl;
      }
    else if(flag < -1)
      {
        int counter= (flag + 1) * -1;
        int eleTag= this->getTag();
        int i,j;
        for(i= 0;i < 4;i++)
          {
            const Vector &stress= physicalProperties[i]->getStressResultant();
            s << "STRESS\t" << eleTag << "\t" << counter << "\t" << i << "\tTOP";
            for(j=0; j<6; j++)
              s << "\t" << stress(j);
            s << std::endl;
          }
      }
    else
      {
        s << std::endl;
        s << getClassName() << " Four Node Shell \n"
          << "Element Number: " << this->getTag() << std::endl
          << "Node 1 : " << theNodes.getTagNode(0) << std::endl
          << "Node 2 : " << theNodes.getTagNode(1) << std::endl
          << "Node 3 : " << theNodes.getTagNode(2) << std::endl
          << "Node 4 : " << theNodes.getTagNode(3) << std::endl
	  << "Material Information : \n ";
        physicalProperties.Print( s, flag );
        s << std::endl;
      }
  }
