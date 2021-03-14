//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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

// $Revision: 1.9 $
// $Date: 2003/02/25 23:33:13 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/zeroLength/ZeroLengthSection.cpp,v $

// Written: MHS
// Created: Sept 2000
//
// Description: This file contains the implementation for the
// ZeroLengthSection class.

#include <domain/mesh/element/zeroLength/ZeroLengthSection.h>
#include <domain/mesh/element/utils/Information.h>

#include "material/section/PrismaticBarCrossSection.h"
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "utility/actor/actor/MovableMatrix.h"
#include <material/section/SectionForceDeformation.h>
#include <utility/recorder/response/ElementResponse.h>
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/coo_sys/Rect3d3dCooSys.h"

#include "material/section/ResponseId.h"
#include "utility/matrix/Matrix.h"
#include "utility/actor/actor/MatrixCommMetaData.h"

XC::Matrix XC::ZeroLengthSection::K6(6,6);
XC::Matrix XC::ZeroLengthSection::K12(12,12);

XC::Vector XC::ZeroLengthSection::P6(6);
XC::Vector XC::ZeroLengthSection::P12(12);

void XC::ZeroLengthSection::setup_section(const Material *sec)
  {
    free_mem();
    if(!sec)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; pointer to material is null." << std::endl;
    else
      {
        const SectionForceDeformation *ptr= dynamic_cast<const SectionForceDeformation *>(sec);
        if(!ptr)
          std::cerr << getClassName() << "::" << __FUNCTION__
                    << "; material type: '" << sec->getClassName()
	            << "' is not valid.\n";
        else
          {
            theSection = ptr->getCopy();// Obtain copy of section model
            if(!theSection)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; failed to get copy of section.\n";
                exit(-1);
              }
            // Get the section order
            order = ptr->getOrder();
          }
      }
  }

//! @brief Creates the A matrix and the vector v.
void XC::ZeroLengthSection::inicAv(void)
  {
    // Allocate transformation matrix
    A= Matrix(order, numDOF);
    // Allocate section deformation vector
    v= Vector(order);
  }

//! @brief invoke the destructor on any objects created by the object
//! that the object still holds a pointer to
void XC::ZeroLengthSection::free_mem(void)
  {
    if(theSection) delete theSection;
    theSection=nullptr;
  }

//! @brief Constructor.
//!
//! @param tag: element identifier.
//! @param dim: space dimension (1, 2 or 3).
//! @param Nd1: identifier of the first node.
//! @param Nd2: identifier of the second node.
//! @param x: Vector that defines the local x-axis.
//! @param yprime: Vector that defines the local x-y plane.
//! @param sec: SectionForceDeformation material for the element.
XC::ZeroLengthSection::ZeroLengthSection(int tag, int dim, int Nd1, int Nd2,
                                         const Vector &x, const Vector &yprime,
                                         SectionForceDeformation& sec)
  : Element0D(tag, ELE_TAG_ZeroLengthSection,Nd1,Nd2,dim,x,yprime),
    K(nullptr), P(nullptr), theSection(nullptr), order(0)
  { setup_section(&sec); }

//! @brief Constructor:
//!
//! @param tag: element identifier.
//! @param dim: space dimension (1, 2 or 3).
//! @param sec: SectionForceDeformation material for the element.
XC::ZeroLengthSection::ZeroLengthSection(int tag, int dim,const Material *sec)
  : Element0D(tag, ELE_TAG_ZeroLengthSection,0,0,dim),
    K(nullptr), P(nullptr), theSection(nullptr), order(0)
  {
    if(sec)
      setup_section(sec);
  }

//! @brief Copy constructor.
XC::ZeroLengthSection::ZeroLengthSection(const ZeroLengthSection &other)
  : Element0D(other), A(other.A), v(other.v), K(nullptr), P(nullptr), theSection(nullptr), order(0)
  {
    if(other.theSection)
      setup_section(other.theSection);
  }

//! @brief Assignment operator.
XC::ZeroLengthSection &XC::ZeroLengthSection::operator=(const ZeroLengthSection &other)
  {
    Element0D::operator=(other);
    if(other.theSection)
      setup_section(other.theSection);
    A= other.A;
    v= other.v;
    return *this;
  }

//! @brief Default constructor.
XC::ZeroLengthSection::ZeroLengthSection(void)
  : XC::Element0D(0, ELE_TAG_ZeroLengthSection,0,0,0),
    K(nullptr), P(nullptr), theSection(nullptr), order(0) {}

//! @brief Virtual constructor.
XC::Element *XC::ZeroLengthSection::getCopy(void) const
  { return new ZeroLengthSection(*this); }

//! @brief Destructor.
XC::ZeroLengthSection::~ZeroLengthSection(void)
  { free_mem(); }

// method: setDomain()
//! @brief Set the enclosing domain.
//!
//! Set a link to the enclosing Domain and to set the node pointers.
//! also determines the number of dof associated
//! with the XC::ZeroLengthSection element, we set matrix and vector pointers,
//! allocate space for t matrix and define it as the basic deformation-
//! displacement transformation matrix.
void XC::ZeroLengthSection::setDomain(Domain *theDomain)
  {
    Element0D::setDomain(theDomain);
    // check XC::Domain is not null - invoked when object removed from a domain

    // first set the node pointers
    int Nd1 = theNodes.getTagNode(0);
    int Nd2 = theNodes.getTagNode(1);

    // if can't find both - send a warning message
    if(theNodes[0] == nullptr || theNodes[1] == nullptr)
      {
        if(theNodes[0] == nullptr)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; Nd1: " << Nd1 << " does not exist in ";
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; Nd2: " << Nd2 << " does not exist in ";
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "model for ZeroLengthSection with id " << this->getTag()
		  << std::endl;
        return;
      }

    // now determine the number of dof and the dimension
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();

    // if differing dof at the ends - print a warning message
    if(dofNd1 != dofNd2)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; nodes " << Nd1 << " and " << Nd2
                  << " have differing dof at ends for ZeroLengthSection: "
		  << this->getTag() << std::endl;
        return;
      }

    numDOF = 2*dofNd1;

    if(numDOF != 6 && numDOF != 12)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; material supports only with 3 (2D) or 6 (3D)"
	        << " degrees of freedom by node.\n";

    // Set pointers to class wide objects
    if(numDOF == 6)
      {
        P= &P6;
        K= &K6;
      }
    else
      {
        P= &P12;
        K= &K12;
      }

    // Check that length is zero within tolerance
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    const Vector diff = end1Crd - end2Crd;
    double L  = diff.Norm();
    double v1 = end1Crd.Norm();
    double v2 = end2Crd.Norm();
    double vm;

    vm = (v1<v2) ? v2 : v1;

    if(L > LenTol*vm)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; Element " << this->getTag()
                << "has L= " << L << ", which is greater than the tolerance\n";

    // call the base class method
    XC::Element0D::setDomain(theDomain);

    setTransformation(); // Set up the A matrix
  }

//! @brief Update element state.
// MSN: added to allow error identification in setTrialSectionDeformation()
int XC::ZeroLengthSection::update()		
  {
    int retval= 0;
    // Compute section deformation vector
    this->computeSectionDefs();
    // Set trial section deformation
    if(theSection->setTrialSectionDeformation(v) < 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING! element: " << this->getTag()
		  << " failed in setTrialSectionDeformation.\n";
	retval= -1;
      }
    return retval;
  }


//! @brief Commit state of element by committing state of the section.
//! Return 0 if successful, !0 otherwise.
int XC::ZeroLengthSection::commitState()
  {
    int retVal=0;

    // call element commitState to do any base class stuff
    if((retVal = this->XC::Element::commitState()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed in base class.\n";
      }
    // Commit the section
    retVal += theSection->commitState();
    return retVal;
  }

//! @brief Revert state of element to last commit by reverting to last committed
//! state of the section. Return 0 if successful, !0 otherwise.
int XC::ZeroLengthSection::revertToLastCommit()
  {
    // DON'T call Element::revertToLastCommit() because
    // is a pure virtual method.
    // int retval= Element0D::revertToLastCommit();
    
    // Revert the section
    int retval= theSection->revertToLastCommit();
    return retval;
  }

//! @brief Revert state of element to initial state by reverting to initial state
//! of the section. Return 0 if successful, !0 otherwise.
int XC::ZeroLengthSection::revertToStart()
  {
    int retval= Element0D::revertToStart();
    // Revert the section to start
    retval+= theSection->revertToStart();
    return retval;
  }

const XC::Matrix &XC::ZeroLengthSection::getTangentStiff(void) const
  {
    // Compute section deformation vector
    // this->computeSectionDefs();	// MSN: commented out beause the method "update()" was added to the class

    // Set trial section deformation
    // theSection->setTrialSectionDeformation(*v);	// MSN: commented out beause the method "update()" was added to the class

    // Get section tangent stiffness, the element basic stiffness
    const Matrix &kb = theSection->getSectionTangent();

    // Compute element stiffness ... K = A^*kb*A
    K->addMatrixTripleProduct(0.0,A, kb, 1.0);

    if(isDead())
      (*K)*=dead_srf;
    return *K;
  }

//! @brief Return tangent stiffness matrix for element.  The element tangent
//! is computed from the section tangent matrix,
//! \f$k_b\f$, as \f$K_e = A^T k_b A\f$.
const XC::Matrix &XC::ZeroLengthSection::getInitialStiff(void) const
  {
    // Get section tangent stiffness, the element basic stiffness
    const Matrix &kb = theSection->getInitialTangent();
    // Compute element stiffness ... K = A^*kb*A
    K->addMatrixTripleProduct(0.0,A, kb, 1.0);
    if(isDead())
      (*K)*=dead_srf;
    return *K;
  }

//! @brief The element has no loads, so this operation has no effect and returns 0.
int XC::ZeroLengthSection::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; load type unknown for zero length section element with tag: "
              << this->getTag() << std::endl;
    return -1;
  }


//! @brief The element has no mass, so this operation has no effect and returns 0.
int XC::ZeroLengthSection::addInertiaLoadToUnbalance(const Vector &accel)
  {
    // does nothing as element has no mass yet!
    return 0;
  }

//! @brief Return resisting force vector for element.  The element resisting force
//! is computed from the section stress resultants, \f$s\f$, as \f$P_e = A^T s\f$.
//! The section stress resulant is obtained by calling getStressResultant().
const XC::Vector &XC::ZeroLengthSection::getResistingForce(void) const
  {
    // Compute section deformation vector
    // this->computeSectionDefs(); // MSN: commented out beause the method "update()" was added to the class

    // Set trial section deformation
    // theSection->setTrialSectionDeformation(v); // MSN: commented out beause the method "update()" was added to the class

    // Get section stress resultants, the element basic forces
    const Vector &q = theSection->getStressResultant();

    // Compute element resisting force ... P = A^*q
    P->addMatrixTransposeVector(0.0,A, q, 1.0);

    if(isDead())
      (*P)*=dead_srf;
    return *P;
  }

//! @brief Returns the result of getResistingForce() as there is no element mass.
const XC::Vector &XC::ZeroLengthSection::getResistingForceIncInertia(void) const
  {
    this->getResistingForce();

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullKValues())
      *P += this->getRayleighDampingForces();
    if(isDead())
      (*P)*=dead_srf;
    return *P;
  }

//! @brief Returns the direction vector of element strong axis
//! expressed in the local coordinate system.
XC::Vector XC::ZeroLengthSection::getVDirStrongAxisLocalCoord(void) const
  {
    Vector retval(3);
    const PrismaticBarCrossSection *sccBarra= dynamic_cast<const PrismaticBarCrossSection *>(theSection);
    if(sccBarra)
      {
        const Vector2d sectionStrongAxis= sccBarra->getVDirStrongAxis();
        retval(0)= 0.0; retval(1)= sectionStrongAxis.x(); retval(2)= sectionStrongAxis.y();
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; Element's section doesn't returns inertia axis."
		<< std::endl;
    return retval;
  }

//! @brief Returns the direction vector of element weak axis
//! expressed in the local coordinate system.
XC::Vector XC::ZeroLengthSection::getVDirWeakAxisLocalCoord(void) const
  {
    Vector retval(3);
    const PrismaticBarCrossSection *sccBarra= dynamic_cast<const PrismaticBarCrossSection *>(theSection);
    if(sccBarra)
      {
        const Vector2d sectionWeakAxis= sccBarra->getVDirWeakAxis();
        retval(0)= 0.0; retval(1)= sectionWeakAxis.x(); retval(2)= sectionWeakAxis.y();
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; element's section doesn't returns inertia axis."
		<< std::endl;
    return retval;
  }
//! @brief Returns the angle between element strong axis
//! and local XZ plane.
double XC::ZeroLengthSection::getStrongAxisAngle(void) const
  {
    Vector eF= getVDirStrongAxisLocalCoord();
    return atan2(eF(2),eF(1));
  }

//! @brief Returns the angle between element weak axis
//! and local XZ plane.
double XC::ZeroLengthSection::getWeakAxisAngle(void) const
  {
    Vector eD= getVDirWeakAxisLocalCoord();
    return atan2(eD(2),eD(1));
  }

//! @brief Returns the direction vector of element strong axis
//! expressed in the global coordinate system.
const XC::Vector &XC::ZeroLengthSection::getVDirStrongAxisGlobalCoord(bool initialGeometry) const
  {
    const Rect3d3dCooSys coo_sys= getCooSys(initialGeometry);
    const Vector eF= getVDirStrongAxisLocalCoord();
    const Vector3d v= coo_sys.GetCooGlobales(Vector3d(eF[0],eF[1],eF[2]));
    static Vector retval(3);
    retval[0]= v.x(); retval[1]= v.y(); retval[2]= v.z();
    return retval;
  }

//! @brief Returns the direction vector of element weak axis
//! expressed in the global coordinate system.
const XC::Vector &XC::ZeroLengthSection::getVDirWeakAxisGlobalCoord(bool initialGeometry) const
  {
    const Rect3d3dCooSys coo_sys= getCooSys(initialGeometry);
    const Vector eD= getVDirWeakAxisLocalCoord();
    const Vector3d v= coo_sys.GetCooGlobales(Vector3d(eD[0],eD[1],eD[2]));
    static Vector retval(3);
    retval[0]= v.x(); retval[1]= v.y(); retval[2]= v.z();
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::ZeroLengthSection::sendData(Communicator &comm)
  {

    int res= Element0D::sendData(comm);
    res+= comm.sendInt(order,getDbTagData(),CommMetaData(9));
    res+= comm.sendMatrix(A,getDbTagData(),CommMetaData(10));
    res+= comm.sendVector(v,getDbTagData(),CommMetaData(11));
    res+= comm.sendMatrixPtr(K,getDbTagData(),MatrixCommMetaData(17,18,19,20));
    res+= comm.sendVectorPtr(P,getDbTagData(),ArrayCommMetaData(21,22,23));
    res+= comm.sendBrokedPtr(theSection,getDbTagData(),BrokedPtrCommMetaData(24,25,26));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ZeroLengthSection::recvData(const Communicator &comm)
  {
    int res= Element0D::recvData(comm);
    res+= comm.receiveInt(order,getDbTagData(),CommMetaData(9));
    res+= comm.receiveMatrix(A,getDbTagData(),CommMetaData(10));
    res+= comm.receiveVector(v,getDbTagData(),CommMetaData(14));
    K= comm.receiveMatrixPtr(K,getDbTagData(),MatrixCommMetaData(17,18,19,20));
    P= comm.receiveVectorPtr(P,getDbTagData(),ArrayCommMetaData(21,22,23));
    theSection= comm.getBrokedMaterial(theSection,getDbTagData(),BrokedPtrCommMetaData(24,25,26));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::ZeroLengthSection::sendSelf(Communicator &comm)
  {
    inicComm(27);

    int res= sendData(comm);

    const int dataTag= getDbTag();
    res += comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send ID data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::ZeroLengthSection::recvSelf(const Communicator &comm)
  {
    inicComm(27);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief Prints the element node tags and section model to the stream {em s}.
void XC::ZeroLengthSection::Print(std::ostream &s, int flag) const
  {
    s << "ZeroLengthSection, tag: " << this->getTag() << std::endl;
    s << "\tConnected Nodes: " << theNodes << std::endl;
    s << "\tSection, tag: " << theSection->getTag() << std::endl;
  }

XC::Response* XC::ZeroLengthSection::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    // element forces
    if(argv[0] == "force" || argv[0] == "forces")
      return new ElementResponse(this, 1, *P);
    // element stiffness matrix
    else if(argv[0] == "stiff" || argv[0] == "stiffness")
      return new ElementResponse(this, 2, *K);
    else if(argv[0] == "defo" || argv[0] == "deformations" ||
                argv[0] == "deformation")
      { return new ElementResponse(this, 3, Vector(order)); }
    else if(argv[0] == "section")
      return setMaterialResponse(theSection,argv,1,eleInformation);
    else
      return 0;
  }

int XC::ZeroLengthSection::getResponse(int responseID, Information &eleInfo)
  {
    switch(responseID)
      {
      case 1:
        return eleInfo.setVector(this->getResistingForce());
      case 2:
        return eleInfo.setMatrix(this->getTangentStiff());
      case 3:
        this->computeSectionDefs();
        return eleInfo.setVector(v);
      default:
        return -1;
      }
  }

// Private methods

// Set basic deformation-displacement transformation matrix for section
void XC::ZeroLengthSection::setTransformation(void)
  {
    inicAv();

    // Get the section code
    const ID &code= theSection->getType();

    // Set a reference to make the syntax nicer
    Matrix &tran= A;
    tran.Zero();

    // Loop over the section code
    for(int i= 0;i<order;i++)
      {
        // Fill in row i of A based on section code
        switch(code(i))
          {
          // The in-plane transformations
          case SECTION_RESPONSE_MZ:
            if(numDOF == 6)
              {
                tran(i,3) = 0.0;
                tran(i,4) = 0.0;
                tran(i,5) = transformation(2,2);
              }
            else if(numDOF == 12)
              {
                tran(i,9) = transformation(2,0);
                tran(i,10) = transformation(2,1);
                tran(i,11) = transformation(2,2);
              }
            break;
          case SECTION_RESPONSE_P:
            if(numDOF == 6)
              {
                tran(i,3) = transformation(0,0);
                tran(i,4) = transformation(0,1);
                tran(i,5) = 0.0;
              }
            else if(numDOF == 12)
              {
                tran(i,6) = transformation(0,0);
                tran(i,7) = transformation(0,1);
                tran(i,8) = transformation(0,2);
              }
            break;
          case SECTION_RESPONSE_VY:
            if(numDOF == 6)
              {
                tran(i,3) = transformation(1,0);
                tran(i,4) = transformation(1,1);
                tran(i,5) = 0.0;
              }
            else if(numDOF == 12)
              {
                tran(i,6) = transformation(1,0);
                tran(i,7) = transformation(1,1);
                tran(i,8) = transformation(1,2);
              }
            break;
          // The out-of-plane transformations
          case SECTION_RESPONSE_MY:
            if(numDOF == 12)
              {
                tran(i,9) = transformation(1,0);
                tran(i,10) = transformation(1,1);
                tran(i,11) = transformation(1,2);
              }
            break;
          case SECTION_RESPONSE_VZ:
            if(numDOF == 12)
              {
                tran(i,6) = transformation(2,0);
                tran(i,7) = transformation(2,1);
                tran(i,8) = transformation(2,2);
              }
            break;
          case SECTION_RESPONSE_T:
            if(numDOF == 12)
              {
                tran(i,9) = transformation(0,0);
                tran(i,10) = transformation(0,1);
                tran(i,11) = transformation(0,2);
              }
            break;
          default:
            break;
          }
        // Fill in first half of transformation matrix with negative sign
        for(int j=0;j<numDOF/2;j++)
          tran(i,j) = -tran(i,j+numDOF/2);
      }
  }

//! @brief Set up the transformation matrix for orientation
//! @param x: vector components in global coordinates defining local x-axis (vector x)
//! @param yp: vector components in global coordinates defining vector yp which lies in the local x-y plane for the element. The local z-axis is defined by the cross product between the vectors x and yp
void XC::ZeroLengthSection::setUpVectors(const Vector &x, const Vector &yp)
  {
    Element0D::setUpVectors(x,yp);
    setTransformation(); // Set up the A matrix
  }

void XC::ZeroLengthSection::computeSectionDefs(void) const
  {
    // Get nodal displacements
    const Vector &u1 = theNodes[0]->getTrialDisp();
    const Vector &u2 = theNodes[1]->getTrialDisp();

    // Compute differential displacements
    const Vector diff = u2 - u1;

    // Set some references to make the syntax nicer
    Vector &def= v;
    const Matrix &tran= A;

    def.Zero();

    // Compute element basic deformations ... v = A*(u2-u1)
    for(int i= 0;i<order;i++)
      for(int j= 0;j<numDOF/2;j++)
        def(i)+= -diff(j)*tran(i,j);
  }

const XC::Vector &XC::ZeroLengthSection::getResistingForceSensitivity(int gradIndex)
  {
    // Compute section deformation vector
    // this->computeSectionDefs();	// MSN: commented out beause the method "update()" was added to the class

    // Set trial section deformation
    // theSection->setTrialSectionDeformation(*v);	// MSN: commented out beause the method "update()" was added to the class

    // Get section stress resultants, the element basic forces
    const Vector &dqdh = theSection->getStressResultantSensitivity(gradIndex, true);

    // Compute element resisting force ... P = A^*q
    P->addMatrixTransposeVector(0.0, A, dqdh, 1.0);

    return *P;
  }
    
int XC::ZeroLengthSection::commitSensitivity(int gradIndex, int numGrads)
  {
    // Get nodal displacement sensitivity
    Vector diff(numDOF/2);
    for(int i = 0; i < numDOF/2; i++)
      {
        diff(i) = theNodes[1]->getDispSensitivity(i+1,gradIndex) - theNodes[0]->getDispSensitivity(i+1,gradIndex);
      }

    // Set some references to make the syntax nicer
    Vector &dedh = v;
    const Matrix &tran = A;

    dedh.Zero();

    // Compute element basic deformations ... v = A*(u2-u1)
    for (int i = 0; i < order; i++)
      for (int j = 0; j < numDOF/2; j++)
	dedh(i) += -diff(j)*tran(i,j);
    return theSection->commitSensitivity(dedh, gradIndex, numGrads);
  }
