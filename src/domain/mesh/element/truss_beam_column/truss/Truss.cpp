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

// $Revision: 1.23 $
// $Date: 2003/08/13 23:52:23 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/truss/Truss.cpp,v $


// File: ~/element/truss/Truss.C
//
// Written: fmk
// Created: 07/98
// Revision: A
//
// Description: This file contains the implementation for the XC::Truss class.
//
// What: "@(#) Truss.C, revA"

#include "domain/mesh/element/truss_beam_column/truss/Truss.h"
#include <domain/mesh/element/utils/Information.h>

#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/uniaxial/CableMaterial.h>
#include <domain/load/ElementalLoad.h>
#include "domain/load/beam_loads/TrussStrainLoad.h"
#include <utility/matrix/Matrix.h>
#include <utility/recorder/response/ElementResponse.h>

#include "domain/component/Parameter.h"
#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Free the material pointer.
void XC::Truss::free_material(void)
  {
    if(theMaterial)
      {
        delete theMaterial;
        theMaterial= nullptr;
      }
  }

//! @brief Free the load sensitivity.
void XC::Truss::free_load_sens(void)
  {
    if(theLoadSens)
      {
        delete theLoadSens;
        theLoadSens= nullptr;
      }
  }

//! @brief Free the material pointer and the load sensitivity.
void XC::Truss::free_mem(void)
  {
    free_material();
    free_load_sens();
  }

//! @brief Assign the material.
void XC::Truss::set_material(const UniaxialMaterial &mat)
  {
    free_material();
    // get a copy of the material and check we obtained a valid copy
    theMaterial= mat.getCopy();
    if(!theMaterial)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; FATAL truss - " << getTag()
		  << " failed to get a copy of material with tag "
	          << mat.getTag() << std::endl;
        exit(-1);
      }
  }

//! @brief Assign the load sensitivity vector.
void XC::Truss::set_load_sens(const Vector &v)
  {
    free_load_sens();
    // get a copy of the material and check we obtained a valid copy
    theLoadSens= new Vector(v);
    if(!theLoadSens)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; truss " << getTag()
		  << " failed to get a copy of vector: " << v << std::endl;
        exit(-1);
      }
  }

//! @brief Nullify the pointers to node, the direction cosines and los sensibility parameters.
void XC::Truss::initialize(void)
  {
    TrussBase::initialize();
// AddingSensitivity:BEGIN /////////////////////////////////////
    parameterID = 0;
// AddingSensitivity:END //////////////////////////////////////
  }

//! @brief constructor:
//!  responsible for allocating the necessary space needed by each object
//!  and storing the tags of the truss end nodes.
XC::Truss::Truss(int tag,int dim,int Nd1, int Nd2, UniaxialMaterial &theMat,double a)
  :TrussBase(ELE_TAG_Truss,tag,dim,Nd1,Nd2), theMaterial(nullptr),A(a),theLoadSens(nullptr)
  {
    set_material(theMat);
    initialize();
  }

//! @brief constructor:
//!  responsible for allocating the necessary space needed by each object
//!  and storing the tags of the truss end nodes.
XC::Truss::Truss(int tag,int dim,const Material *ptr_mat)
  :TrussBase(ELE_TAG_Truss,tag,dim), theMaterial(nullptr), A(0.0),theLoadSens(nullptr)
  {
    UniaxialMaterial *tmp= cast_material<UniaxialMaterial>(ptr_mat);
    if(tmp)
      set_material(*tmp);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; not a suitable material." << std::endl;
    initialize();
  }


//! @brief constructor:
//!   invoked by a FEM_ObjectBroker - blank object that recvSelf needs
//!   to be invoked upon
XC::Truss::Truss(void)
  :TrussBase(ELE_TAG_Truss), theMaterial(nullptr), A(0.0),theLoadSens(nullptr)
  { initialize(); }

//! @brief Copy constructor.
XC::Truss::Truss(const Truss &other)
  :TrussBase(other), theMaterial(nullptr), A(other.A),theLoadSens(nullptr)
  {
    if(other.theMaterial)
      set_material(*other.theMaterial);
    if(other.theLoadSens)
      set_load_sens(*other.theLoadSens);
  }

//! @brief Assignment operator.
XC::Truss &XC::Truss::operator=(const Truss &other)
  {
    TrussBase::operator=(other);
    if(other.theMaterial)
      set_material(*other.theMaterial);
    A= other.A;
    if(other.theLoadSens)
      set_load_sens(*other.theLoadSens);
    return *this;
  }


//! @brief Virtual constructor.
XC::Element* XC::Truss::getCopy(void) const
  { return new Truss(*this); }

//!  destructor
//!     delete must be invoked on any objects created by the object
//!     and on the matertial object.
XC::Truss::~Truss(void)
  { free_mem(); }

//! @brief Set a link to the enclosing Domain and to set the node pointers.
//!    also determines the number of dof associated
//!    with the truss element, we set matrix and vector pointers,
//!    allocate space for t matrix, determine the length
//!    and set the transformation matrix.
void XC::Truss::setDomain(Domain *theDomain)
  {
    TrussBase::setDomain(theDomain);
    if(!theDomain)
      {
        L= 0;
        return;
      }

    // if can't find both - send a warning message
    if((theNodes[0] == 0) || (theNodes[1] == 0))
      {
        // fill this in so don't segment fault later
        numDOF = 2;
        theMatrix = &trussM2;
        theVector = &trussV2;
        return;
      }

    // now determine the number of dof and the dimesnion
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();

    // if differing dof at the ends - print a warning message
    if(dofNd1 != dofNd2)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING: nodes " << theNodes[0]->getTag()
                  << " and " <<  theNodes[1]->getTag()
                  << " have differing dof at ends for truss "
		  << this->getTag() << std::endl;

        // fill this in so don't segment fault later
        numDOF = 2;
        theMatrix = &trussM2;
        theVector = &trussV2;
        return;
      }

    setup_matrix_vector_ptrs(dofNd1);

    alloc_load(numDOF);

    // now determine the length, cosines and fill in the transformation
    setup_L_cos_dir();
    CableMaterial *ptrCableMaterial= dynamic_cast<CableMaterial *>(theMaterial);
    if(ptrCableMaterial)
      ptrCableMaterial->setLength(L);
  }

//! @brief Commits truss state.
//!
//! Return the result of invoking commitState()
//! on the base class and on it's associated
//! UniaxialMaterial object.
int XC::Truss::commitState()
  {
    int retVal = 0;
    // call element commitState to do any base class stuff
    if((retVal = this->TrussBase::commitState()) != 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed in base class";
    retVal = theMaterial->commitState();
    return retVal;
  }

//! @brief Returns to the last committed state.
//!
//! Return the result of invoking revertToLastCommit()
//! on it's base class an on it's associated UniaxialMaterial object.
int XC::Truss::revertToLastCommit()
  {
    int retVal= 0;
    // DON'T call element revertToLastCommit because is a pure virtual method.
    // if((retVal = this->TrussBase::revertToLastCommit()) != 0)
    //   std::cerr << getClassName() << "::" << __FUNCTION__
    // 		<< "; failed in base class";
    retVal = theMaterial->revertToLastCommit();
    return retVal;
  }

//! @brief Returns to the initial state.
//!
//! Return the result of invoking revertToStart()
//! on it's base class an on it's associated UniaxialMaterial object.
int XC::Truss::revertToStart()
  {
    int retVal= 0;
    // call element revertToLastCommit to do any base class stuff
    if((retVal = this->TrussBase::revertToStart()) != 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed in base class";
    retVal = theMaterial->revertToStart();
    return retVal;
  }

//! @brief Computes current strain from the trial displacements of the nodes.
int XC::Truss::update(void)
  {
    const double strain= this->computeCurrentStrain();
    const double rate= this->computeCurrentStrainRate();
    return theMaterial->setTrialStrain(strain, rate);
  }

//! @brief Returns the tangent stiffness matrix.
const XC::Matrix &XC::Truss::getTangentStiff(void) const
  {
    if(L == 0.0)
      { // - problem in setDomain() no further warnings
        theMatrix->Zero();
        return *theMatrix;
      }

    double E = theMaterial->getTangent();

    // come back later and redo this if too slow
    Matrix &stiff= *theMatrix;

    int numDOF2 = numDOF/2;
    double temp;
    double EAoverL = E*A/L;
    for(int i = 0; i < getNumDIM(); i++)
      {
        for(int j = 0; j < getNumDIM(); j++)
          {
            temp = cosX[i]*cosX[j]*EAoverL;
            stiff(i,j) = temp;
            stiff(i+numDOF2,j) = -temp;
            stiff(i,j+numDOF2) = -temp;
            stiff(i+numDOF2,j+numDOF2) = temp;
          }
      }
    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::Truss::getInitialStiff(void) const
  {
    if(L == 0.0)
      { // - problem in setDomain() no further warnings
        theMatrix->Zero();
        return *theMatrix;
      }

    const double E = theMaterial->getInitialTangent();

    // come back later and redo this if too slow
    Matrix &stiff= *theMatrix;

    int numDOF2 = numDOF/2;
    double temp;
    double EAoverL = E*A/L;
    for(int i = 0; i < getNumDIM(); i++)
      {
	for(int j = 0; j < getNumDIM(); j++)
	  {
	    temp = cosX[i]*cosX[j]*EAoverL;
	    stiff(i,j) = temp;
	    stiff(i+numDOF2,j) = -temp;
	    stiff(i,j+numDOF2) = -temp;
	    stiff(i+numDOF2,j+numDOF2) = temp;
	  }
      }
    if(isDead())
      (*theMatrix)*=dead_srf;
    return *theMatrix;
  }

//! @brief Returns the damping matrix.
const XC::Matrix &XC::Truss::getDamp(void) const
  {
    if(L == 0.0)
      { // - problem in setDomain() no further warnings
        theMatrix->Zero();
        return *theMatrix;
      }

    double eta = theMaterial->getDampTangent();

    // come back later and redo this if too slow
    Matrix &damp = *theMatrix;

    int numDOF2 = numDOF/2;
    double temp;
    double etaAoverL = eta*A/L;
    for(int i = 0; i < getNumDIM(); i++)
      {
        for(int j = 0; j < getNumDIM(); j++)
          {
            temp = cosX[i]*cosX[j]*etaAoverL;
            damp(i,j) = temp;
            damp(i+numDOF2,j) = -temp;
            damp(i,j+numDOF2) = -temp;
            damp(i+numDOF2,j+numDOF2) = temp;
          }
      }
    if(isDead())
      damp*=dead_srf;
    return damp;
  }

//! @brief Return a pointer to the element material.
const XC::Material *XC::Truss::getMaterial(void) const
  { return theMaterial; }

//! @brief Return a pointer to the element material.
XC::Material *XC::Truss::getMaterial(void)
  { return theMaterial; }

//! @brief Set the material density.
void XC::Truss::setRho(const double &r)
  { theMaterial->setRho(r); }

//! @brief Returns the material density.
double XC::Truss::getRho(void) const
  {
    double retval= 0.0;
    if(theMaterial)
      { retval= theMaterial->getRho(); }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; material not defined yet." << std::endl;
    return retval;
  }

//! @brief Returns the element mass per unit length.
double XC::Truss::getLinearRho(void) const
  { return getSectionArea()*getRho(); }

//! @brief Returns the mass matrix.
const XC::Matrix &XC::Truss::getMass(void) const
  {
    // zero the matrix
    Matrix &mass= *theMatrix;
    mass.Zero();

    const double rho= getLinearRho();
    // check for quick return
    if(L == 0.0 || rho == 0.0)
      { // - problem in setDomain() no further warnings
        return mass;
      }

    double M = 0.5*rho*L;
    int numDOF2 = numDOF/2;
    for(int i = 0; i < getNumDIM(); i++)
      {
        mass(i,i) = M;
        mass(i+numDOF2,i+numDOF2) = M;
      }
    if(isDead())
      mass*=dead_srf;
    return mass;
  }

//! @brief Zeroes loads on element.
void XC::Truss::zeroLoad(void)
  {
    TrussBase::zeroLoad();
    theMaterial->setInitialStrain(0.0); //Removes initial strains.
    return;
  }

//! @brief Adds a load.
int XC::Truss::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::clog << getClassName() << "::" << __FUNCTION__
                << "; Warning, load over inactive element: "
                << getTag() << std::endl;
    else
      {
        if(TrussStrainLoad *trsLoad= dynamic_cast<TrussStrainLoad *>(theLoad))
          {
            const double &e1= trsLoad->E1()*loadFactor;
            const double &e2= trsLoad->E2()*loadFactor;
            double ezero= theMaterial->getInitialStrain();
            ezero+= (e2+e1)/2;
            theMaterial->setInitialStrain(ezero);
          }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; load type unknown for truss with tag: "
		      << this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

//! @brief Adds inertia forces.
int XC::Truss::addInertiaLoadToUnbalance(const Vector &accel)
  {
    const double rho= getLinearRho();
    // check for a quick return
    if(L == 0.0 || rho == 0.0)
        return 0;

    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);

    int nodalDOF = numDOF/2;

    const double M = 0.5*rho*L;
    // want to add ( - fact * M R * accel ) to unbalance
    for(int i=0; i<getNumDIM(); i++)
      {
        double val1 = Raccel1(i);
        double val2 = Raccel2(i);

        // perform - fact * M*(R * accel) // remember M a diagonal matrix
        val1*= -M;
        val2*= -M;

        (*getLoad())(i) += val1;
        (*getLoad())(i+nodalDOF) += val2;
      }

    return 0;
  }


int XC::Truss::addInertiaLoadSensitivityToUnbalance(const XC::Vector &accel, bool somethingRandomInMotions)
  {
    if(!theLoadSens)
      { set_load_sens(Vector(numDOF)); }
    else
      { theLoadSens->Zero(); }
    const double rho= getLinearRho();

  if(somethingRandomInMotions)
    {

    // check for a quick return
    if(L == 0.0 || rho == 0.0)
      return 0;

    // get R * accel from the nodes
    const XC::Vector &Raccel1 = theNodes[0]->getRV(accel);
    const XC::Vector &Raccel2 = theNodes[1]->getRV(accel);

    int nodalDOF = numDOF/2;

#ifdef _G3DEBUG
    if(nodalDOF != Raccel1.Size() || nodalDOF != Raccel2.Size()) {
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; matrix and vector sizes are incompatible.\n";
      return -1;
    }
#endif

        double M  = 0.5*rho*L;
    // want to add ( - fact * M R * accel ) to unbalance
    for(int i=0; i<getNumDIM(); i++) {
      double val1 = Raccel1(i);
      double val2 = Raccel2(i);

      // perform - fact * M*(R * accel) // remember M a diagonal matrix
      val1 *= M;
      val2 *= M;

      (*theLoadSens)(i) = val1;
      (*theLoadSens)(i+nodalDOF) = val2;
    }
  }
  else {

    // check for a quick return
    if(L == 0.0 || rho == 0.0)
      return 0;

    // get R * accel from the nodes
    const XC::Vector &Raccel1 = theNodes[0]->getRV(accel);
    const XC::Vector &Raccel2 = theNodes[1]->getRV(accel);

    int nodalDOF = numDOF/2;

#ifdef _G3DEBUG
    if(nodalDOF != Raccel1.Size() || nodalDOF != Raccel2.Size()) {
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; marix and vector sizes are incompatible.\n";
      return -1;
    }
#endif

    double massDerivative = 0.0;
    if(parameterID == 2) {
      massDerivative = 0.5*L;
    }

    // want to add ( - fact * M R * accel ) to unbalance
    for(int i=0; i<getNumDIM(); i++) {
      double val1 = Raccel1(i);
      double val2 = Raccel2(i);

      // perform - fact * M*(R * accel) // remember M a diagonal matrix

      val1 *= massDerivative;
      val2 *= massDerivative;

      (*theLoadSens)(i) = val1;
      (*theLoadSens)(i+nodalDOF) = val2;
    }
  }
  return 0;
}

//! @brief Returns the reaction of the element.
const XC::Vector &XC::Truss::getResistingForce(void) const
  {
    if(L == 0.0)
      { // - problem in setDomain() no further warnings
        theVector->Zero();
        return *theVector;
      }

    // R = Ku - Pext
    // Ku = F * transformation
    const double force = A*theMaterial->getStress();
    int numDOF2 = numDOF/2;
    double temp;
    for(int i = 0; i < getNumDIM(); i++)
      {
        temp = cosX[i]*force;
        (*theVector)(i) = -temp;
        (*theVector)(i+numDOF2) = temp;
      }

    // subtract external load:  Ku - P
    (*theVector)-= *getLoad();

    if(isDead())
      (*theVector)*=dead_srf;
    return *theVector;
  }

//! @brief Returns the reaction of the element includin inertia forces.
const XC::Vector &XC::Truss::getResistingForceIncInertia(void) const
  {
    this->getResistingForce();

    const double rho= getLinearRho();
    // now incluof the mass portion
    if(L != 0.0 && rho != 0.0)
      {

        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();

        const int numDOF2 = numDOF/2;
        const double M = 0.5*rho*L;
        for(int i = 0; i < getNumDIM(); i++)
          {
            (*theVector)(i) += M*accel1(i);
            (*theVector)(i+numDOF2) += M*accel2(i);
          }

        // add the damping forces if rayleigh damping
        if(!rayFactors.nullValues())
          (*theVector)+= this->getRayleighDampingForces();
      }
    else
      {
        // add the damping forces if rayleigh damping
        if(!rayFactors.nullKValues())
          (*theVector) += this->getRayleighDampingForces();
      }
    if(isDead())
      (*theVector)*=dead_srf; //XXX It's applied 2 times on getResistingForce: fix it.
    return *theVector;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::Truss::getDbTagData(void) const
  {
    static DbTagData retval(28);
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::Truss::sendData(Communicator &comm)
  {
    int res= TrussBase::sendData(comm);
    res+= comm.sendBrokedPtr(theMaterial,getDbTagData(),BrokedPtrCommMetaData(20,21,22));
    res+= comm.sendDouble(A,getDbTagData(),CommMetaData(23));
    res+= comm.sendInt(parameterID,getDbTagData(),CommMetaData(24));
    res+= comm.sendVectorPtr(theLoadSens,getDbTagData(),ArrayCommMetaData(25,26,27)); 
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::Truss::recvData(const Communicator &comm)
  {
    int res= TrussBase::recvData(comm);
    theMaterial= comm.getBrokedMaterial(theMaterial,getDbTagData(),BrokedPtrCommMetaData(20,21,22));
    res+= comm.receiveDouble(A,getDbTagData(),CommMetaData(23));
    res+= comm.receiveInt(parameterID,getDbTagData(),CommMetaData(24));
    theLoadSens= comm.receiveVectorPtr(theLoadSens,getDbTagData(),ArrayCommMetaData(25,26,27));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::Truss::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(28);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::Truss::recvSelf(const Communicator &comm)
  {
    inicComm(28);
    
    const int dbTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << getClassName() << "::" << __FUNCTION__
		     << "; failed to receive data.\n";
      }
    return res;
  }

void XC::Truss::Print(std::ostream &s, int flag) const
  {
    // compute the strain and axial force in the member
    const double strain = theMaterial->getStrain();
    const double force = A * theMaterial->getStress();

    if(flag == 0)
      { // print everything
        s << "Element: " << this->getTag();
        s << " type: Truss  iNode: " << theNodes.getTagNode(0);
        s << " jNode: " << theNodes.getTagNode(1);

        s << " \n\t strain: " << strain;
        s << " axial load: " <<  force;
        if(L != 0.0)
          {
            int numDOF2 = numDOF/2;
            double temp;
            for(int i = 0; i < getNumDIM(); i++)
              {
                temp = cosX[i]*force;
                (*theVector)(i) = -temp;
                (*theVector)(i+numDOF2) = temp;
              }
            s << " \n\t unbalanced load: " << *theVector;
          }
        s << " \t Material: " << *theMaterial;
        s << std::endl;
      }
    else
     if(flag == 1)
       {
         s << this->getTag() << "  " << strain << "  ";
         s << force << std::endl;
       }
  }

//! @brief Return the axial internal force.
double XC::Truss::getAxialForce(void) const
  {
    double stress= theMaterial->getStress();
    if(isDead())
      stress*=dead_srf;
    return A*stress;
  }

//! @brief Return the element initial strain.
double XC::Truss::getInitialStrain(void) const
  {
    return theMaterial->getInitialStrain();
  }

double XC::Truss::computeCurrentStrain(void) const
  {
    // NOTE method will not be called if L == 0

    // determine the strain
    const Vector &disp1 = theNodes[0]->getTrialDisp();
    const Vector &disp2 = theNodes[1]->getTrialDisp();

    double dLength = 0.0;
    for(int i = 0; i < getNumDIM(); i++)
      dLength += (disp2(i)-disp1(i))*cosX[i];

    // this method should never be called with L == 0
    return dLength/L;
  }

double XC::Truss::computeCurrentStrainRate(void) const
  {
    // NOTE method will not be called if L == 0

    // determine the strain
    const XC::Vector &vel1 = theNodes[0]->getTrialVel();
    const XC::Vector &vel2 = theNodes[1]->getTrialVel();

    double dLength = 0.0;
    for(int i = 0; i < getNumDIM(); i++)
      {
        dLength += (vel2(i)-vel1(i))*cosX[i];
      }

    // this method should never be called with L == 0
    return dLength/L;
  }

XC::Response *XC::Truss::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    //
    // we compare argv[0] for known response types for the XC::Truss
    //

    if(argv[0] == "force" || argv[0] == "forces" || argv[0] == "axialForce")
      return new ElementResponse(this, 1, 0.0);

    else if(argv[0] == "defo" || argv[0] == "deformations" ||
           argv[0] == "deformation")
      return new ElementResponse(this, 2, 0.0);
    // tangent stiffness matrix
    else if(argv[0] == "stiff")
      return new ElementResponse(this, 3, *theMatrix);
    // a material quantity
    else if(argv[0] == "material" || argv[0] == "-material")
      return  setMaterialResponse(theMaterial,argv,1,eleInfo);
    else
      return 0;
  }

int XC::Truss::getResponse(int responseID, Information &eleInfo)
  {
    switch (responseID)
      {
      case 1:
        return eleInfo.setDouble(A * theMaterial->getStress());
      case 2:
        return eleInfo.setDouble(L * theMaterial->getStrain());
      case 3:
        return eleInfo.setMatrix(this->getTangentStiff());
      default:
        return 0;
      }
  }

// AddingSensitivity:BEGIN ///////////////////////////////////
int XC::Truss::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv.size() < 1)
        return -1;

    // Cross sectional area of the truss itself
    if(argv[0] == "A")
      return param.addObject(1, this);
    // Mass density (per unit volume) of the truss itself
    else if(argv[0] == "rho")
      return param.addObject(1, this);
    // a material parameter
    else if(argv[0] == "-material" || argv[0] == "material")
      {
        int ok =  setMaterialParameter(theMaterial,argv,1,param);
        if(ok < 0)
          return -1;
        else
         return ok + 100;
      }
    // otherwise parameter is unknown for the XC::Truss class
    else
      return -1;
  }

int XC::Truss::updateParameter(int parameterID, Information &info)
  {
    switch (parameterID)
      {
      case -1:
        return -1;
      case 1:
        this->A = info.theDouble;
        return 0;
//       case 2: // rho parameter belongs to the material.
//         this->rho= info.theDouble;
//         return 0;
      default:
        if(parameterID >= 100)
          return theMaterial->updateParameter(parameterID-100, info);
        else
          return -1;
      }
  }
int XC::Truss::activateParameter(int passedParameterID)
  {
    parameterID = passedParameterID;

    // The identifier needs to be passed "downwards" also when it's zero
    if(passedParameterID == 0 || passedParameterID == 1 || passedParameterID == 2)
      { theMaterial->activateParameter(0); }
    // If the identifier is non-zero and the parameter belongs to the material
    else if( passedParameterID > 100)
      { theMaterial->activateParameter(passedParameterID-100); }
    return 0;
  }


const XC::Matrix &XC::Truss::getKiSensitivity(int gradNumber)
  {
    Matrix &stiff = *theMatrix;
    stiff.Zero();

    if(parameterID == 0)
      {}
    else if(parameterID == 1)
      {
    // If cross sectional area is random
    double E = theMaterial->getInitialTangent();

    int numDOF2 = numDOF/2;
    double temp;
    double EoverL = E/L;
    for(int i = 0; i < getNumDIM(); i++) {
      for(int j = 0; j < getNumDIM(); j++) {
        temp = cosX[i]*cosX[j]*EoverL;
        stiff(i,j) = temp;
        stiff(i+numDOF2,j) = -temp;
        stiff(i,j+numDOF2) = -temp;
        stiff(i+numDOF2,j+numDOF2) = temp;
      }
    }
  }
  else if(parameterID == 2)
    {
      // Nothing here when 'rho' is random
    }
  else
    {
      double Esens = theMaterial->getInitialTangentSensitivity(gradNumber);

      int numDOF2 = numDOF/2;
      double temp;
      double EAoverL = Esens*A/L;
      for(int i = 0; i < getNumDIM(); i++)
        {
          for(int j = 0; j < getNumDIM(); j++)
            {
              temp = cosX[i]*cosX[j]*EAoverL;
              stiff(i,j) = temp;
              stiff(i+numDOF2,j) = -temp;
              stiff(i,j+numDOF2) = -temp;
              stiff(i+numDOF2,j+numDOF2) = temp;
            }
        }
    }
  return stiff;
  }

const XC::Matrix &XC::Truss::getMassSensitivity(int gradNumber)
  {
    Matrix &mass = *theMatrix;
    mass.Zero();

    if(parameterID == 2)
      {
        double massDerivative = 0.5*L;

        int numDOF2 = numDOF/2;
        for(int i = 0; i < getNumDIM(); i++)
          {
            mass(i,i) = massDerivative;
            mass(i+numDOF2,i+numDOF2) = massDerivative;
          }
      }
    return mass;
  }

const XC::Vector &XC::Truss::getResistingForceSensitivity(int gradNumber)
  {
    theVector->Zero();

    // Initial declarations
    int i;
    double stressSensitivity, temp1, temp2;

    // Make sure the material is up to date
    double strain = this->computeCurrentStrain();
    double rate = this->computeCurrentStrainRate();
    theMaterial->setTrialStrain(strain,rate);

    // Contribution from material
    stressSensitivity = theMaterial->getStressSensitivity(gradNumber,true);

    // Check if a nodal coordinate is random
    double dcosXdh[3];
    dcosXdh[0] = 0.0;
    dcosXdh[1] = 0.0;
    dcosXdh[2] = 0.0;

    int nodeParameterID0 = theNodes[0]->getCrdsSensitivity();
    int nodeParameterID1 = theNodes[1]->getCrdsSensitivity();
    if(nodeParameterID0 != 0 || nodeParameterID1 != 0) {

      double dx = L*cosX[0];
      double dy = L*cosX[1];
      //double dz = L*cosX[2];

        // Compute derivative of transformation matrix (assume 4 dofs)
        if(nodeParameterID0 == 1) { // here x1 is random
            temp1 = (-L+dx*dx/L)/(L*L);
            temp2 = dx*dy/(L*L*L);
            //dtdh(0) = -temp1;
            //dtdh(1) = -temp2;
            //dtdh(2) = temp1;
            //dtdh(3) = temp2;
            dcosXdh[0] = temp1;
            dcosXdh[1] = temp2;
            dcosXdh[2] = 0.0;
        }
        if(nodeParameterID0 == 2) { // here y1 is random
            temp1 = (-L+dy*dy/L)/(L*L);
            temp2 = dx*dy/(L*L*L);
            //dtdh(0) = -temp2;
            //dtdh(1) = -temp1;
            //dtdh(2) = temp2;
            //dtdh(3) = temp1;
            dcosXdh[0] = temp2;
            dcosXdh[1] = temp1;
            dcosXdh[2] = 0.0;
        }
        if(nodeParameterID1 == 1) { // here x2 is random
            temp1 = (L-dx*dx/L)/(L*L);
            temp2 = -dx*dy/(L*L*L);
            //dtdh(0) = -temp1;
            //dtdh(1) = -temp2;
            //dtdh(2) = temp1;
            //dtdh(3) = temp2;
            dcosXdh[0] = temp1;
            dcosXdh[1] = temp2;
            dcosXdh[2] = 0.0;
        }
        if(nodeParameterID1 == 2) { // here y2 is random
            temp1 = (L-dy*dy/L)/(L*L);
            temp2 = -dx*dy/(L*L*L);
            //dtdh(0) = -temp2;
            //dtdh(1) = -temp1;
            //dtdh(2) = temp2;
            //dtdh(3) = temp1;
            dcosXdh[0] = temp2;
            dcosXdh[1] = temp1;
            dcosXdh[2] = 0.0;
        }

        const XC::Vector &disp1 = theNodes[0]->getTrialDisp();
        const XC::Vector &disp2 = theNodes[1]->getTrialDisp();
        double dLengthDerivative = 0.0;
        for(i = 0; i < getNumDIM(); i++) {
            dLengthDerivative += (disp2(i)-disp1(i))*dcosXdh[i];
        }

        double materialTangent = theMaterial->getTangent();
        double strainSensitivity= 0.0;

        if(nodeParameterID0 == 1) {        // here x1 is random
            strainSensitivity = (dLengthDerivative*L+strain*dx)/(L*L);
        }
        if(nodeParameterID0 == 2) {    // here y1 is random
            strainSensitivity = (dLengthDerivative*L+strain*dy)/(L*L);
        }
        if(nodeParameterID1 == 1) {        // here x2 is random
            strainSensitivity = (dLengthDerivative*L-strain*dx)/(L*L);
        }
        if(nodeParameterID1 == 2) {    // here y2 is random
            strainSensitivity = (dLengthDerivative*L-strain*dy)/(L*L);
        }
        stressSensitivity += materialTangent * strainSensitivity;
    }


    // Compute sensitivity depending on 'parameter'
    double stress = theMaterial->getStress();
    int numDOF2 = numDOF/2;
    double temp;
    if(parameterID == 1) {            // Cross-sectional area
      for(i = 0; i < getNumDIM(); i++) {
        temp = (stress + A*stressSensitivity)*cosX[i];
        (*theVector)(i) = -temp;
        (*theVector)(i+numDOF2) = temp;
      }
    }
    else {        // Density, material parameter or nodal coordinate
      for(i = 0; i < getNumDIM(); i++) {
        temp = A*(stressSensitivity*cosX[i] + stress*dcosXdh[i]);
        (*theVector)(i) = -temp;
        (*theVector)(i+numDOF2) = temp;
      }
    }

    // subtract external load sensitivity
    if(!theLoadSens)
      set_load_sens(Vector(numDOF));

    (*theVector)-= *theLoadSens;

    return *theVector;
  }

int XC::Truss::commitSensitivity(int gradNumber, int numGrads)
  {
    // Initial declarations
    int i;
    double strainSensitivity, temp1, temp2;

    // Displacement difference between the two ends
    double strain = this->computeCurrentStrain();
    double dLength = strain*L;

    // Displacement sensitivity difference between the two ends
    double sens1;
    double sens2;
    double dSensitivity = 0.0;
    for(i=0; i<getNumDIM(); i++){
      sens1 = theNodes[0]->getDispSensitivity(i+1, gradNumber);
      sens2 = theNodes[1]->getDispSensitivity(i+1, gradNumber);
      dSensitivity += (sens2-sens1)*cosX[i];
    }

    strainSensitivity = dSensitivity/L;

    // Check if a nodal coordinate is random
    int nodeParameterID0 = theNodes[0]->getCrdsSensitivity();
    int nodeParameterID1 = theNodes[1]->getCrdsSensitivity();
    if(nodeParameterID0 != 0 || nodeParameterID1 != 0) {

      double dx = L*cosX[0];
      double dy = L*cosX[1];
      //double dz = L*cosX[2];

        // Compute derivative of transformation matrix (assume 4 dofs)
        double dcosXdh[3];

        if(nodeParameterID0 == 1) { // here x1 is random
            temp1 = (-L+dx*dx/L)/(L*L);
            temp2 = dx*dy/(L*L*L);
            //dtdh(0) = -temp1;
            //dtdh(1) = -temp2;
            //dtdh(2) = temp1;
            //dtdh(3) = temp2;
            dcosXdh[0] = temp1;
            dcosXdh[1] = temp2;
            dcosXdh[2] = 0.0;
        }
        if(nodeParameterID0 == 2) { // here y1 is random
            temp1 = (-L+dy*dy/L)/(L*L);
            temp2 = dx*dy/(L*L*L);
            //dtdh(0) = -temp2;
            //dtdh(1) = -temp1;
            //dtdh(2) = temp2;
            //dtdh(3) = temp1;
            dcosXdh[0] = temp2;
            dcosXdh[1] = temp1;
            dcosXdh[2] = 0.0;
        }

        if(nodeParameterID1 == 1) { // here x2 is random
            temp1 = (L-dx*dx/L)/(L*L);
            temp2 = -dx*dy/(L*L*L);
            //dtdh(0) = -temp1;
            //dtdh(1) = -temp2;
            //dtdh(2) = temp1;
            //dtdh(3) = temp2;
            dcosXdh[0] = temp1;
            dcosXdh[1] = temp2;
            dcosXdh[2] = 0.0;
        }
        if(nodeParameterID1 == 2) { // here y2 is random
            temp1 = (L-dy*dy/L)/(L*L);
            temp2 = -dx*dy/(L*L*L);
            //dtdh(0) = -temp2;
            //dtdh(1) = -temp1;
            //dtdh(2) = temp2;
            //dtdh(3) = temp1;
            dcosXdh[0] = temp2;
            dcosXdh[1] = temp1;
            dcosXdh[2] = 0.0;
        }

        const XC::Vector &disp1 = theNodes[0]->getTrialDisp();
        const XC::Vector &disp2 = theNodes[1]->getTrialDisp();
        double dLengthDerivative = 0.0;
        for(i = 0; i < getNumDIM(); i++){
            dLengthDerivative += (disp2(i)-disp1(i))*dcosXdh[i];
        }

        strainSensitivity += dLengthDerivative/L;

        if(nodeParameterID0 == 1) {        // here x1 is random
            strainSensitivity += dLength/(L*L*L)*dx;
        }
        if(nodeParameterID0 == 2) {    // here y1 is random
            strainSensitivity += dLength/(L*L*L)*dy;
        }
        if(nodeParameterID1 == 1) {        // here x2 is random
            strainSensitivity -= dLength/(L*L*L)*dx;
        }
        if(nodeParameterID1 == 2) {    // here y2 is random
            strainSensitivity -= dLength/(L*L*L)*dy;
        }
    }

    // Pass it down to the material
    theMaterial->commitSensitivity(strainSensitivity, gradNumber, numGrads);

   return 0;
  }

// AddingSensitivity:END /////////////////////////////////////////////
