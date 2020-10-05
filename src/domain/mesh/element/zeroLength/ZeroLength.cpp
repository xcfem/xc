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

// $Revision: 1.16 $
// $Date: 2003/02/25 23:33:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/zeroLength/ZeroLength.cpp,v $


// File: ~/element/ZeroLength/ZeroLength.C
//
// Written: GLF
// Created: 12/99
// Revision: A
//
// Description: This file contains the implementation for the ZeroLength class.
//
// What: "@(#) ZeroLength.C, revA"

#include "domain/mesh/element/zeroLength/ZeroLength.h"
#include <domain/mesh/element/utils/Information.h>

#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <material/uniaxial/UniaxialMaterial.h>

#include <cmath>
#include <cstdlib>
#include <cstring>

#include <utility/recorder/response/ElementResponse.h>

#include "preprocessor/Preprocessor.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include "utility/actor/actor/MatrixCommMetaData.h"

// initialise the class wide variables
XC::Matrix XC::ZeroLength::ZeroLengthM2(2,2);
XC::Matrix XC::ZeroLength::ZeroLengthM4(4,4);
XC::Matrix XC::ZeroLength::ZeroLengthM6(6,6);
XC::Matrix XC::ZeroLength::ZeroLengthM12(12,12);
XC::Vector XC::ZeroLength::ZeroLengthV2(2);
XC::Vector XC::ZeroLength::ZeroLengthV4(4);
XC::Vector XC::ZeroLength::ZeroLengthV6(6);
XC::Vector XC::ZeroLength::ZeroLengthV12(12);

//!  @brief Constructor.
//!
//! Construct element with one unidirectional material (numMaterials1d=1)
//! The force-deformation relationship for the element is given by a
//! pointer \p theMaterial to a {\bf UniaxialMaterial} model acting in
//! local \p direction.
//! The local \p direction is 1, 2, 3, for translation in the local x, y, z
//! axes or 4, 5, 6 for rotation about the local x, y, z axes. 
//!
//! @param tag: element identifier.
//! @param dim: space dimension (1, 2 or 3).
//! @param Nd1: identifier of the first node.
//! @param Nd2: identifier of the second node.
//! @param x: Vector that defines the local x-axis.
//! @param yp: Vector that defines the local x-y plane.
//! @param theMat: uniaxial material for the element.
//! @param direction: local direction on which the material works.
XC::ZeroLength::ZeroLength(int tag,int dim,int Nd1, int Nd2,const Vector &x, const Vector &yp,UniaxialMaterial &theMat, int direction)
  : Element0D(tag,ELE_TAG_ZeroLength,Nd1,Nd2,dim,x,yp),
    theMatrix(nullptr), theVector(nullptr), theMaterial1d(this,theMat,direction)
    {}


//! @brief Constructor.
//!
//! Construct element with one unidirectional material (numMaterials1d=1)
//! The force-deformation relationship for the element is given by a
//! pointer \p theMaterial to a {\bf UniaxialMaterial} model acting in
//! local \p direction.
//! The local \p direction is 1, 2, 3, for translation in the local x, y, z
//! axes or 4, 5, 6 for rotation about the local x, y, z axes. 
//!
//! @param tag: element identifier.
//! @param dim: space dimension (1, 2 or 3).
//! @param ptr_mat: uniaxial material for the element.
//! @param direction: local direction on which the material works.
XC::ZeroLength::ZeroLength(int tag,int dim,const Material *ptr_mat,int direction)
  :Element0D(tag,ELE_TAG_ZeroLength,0,0,dim),
   theMatrix(nullptr), theVector(nullptr),
   theMaterial1d(this,cast_material<UniaxialMaterial>(ptr_mat),direction) {}

//! @brief Construct element with multiple unidirectional materials
//!
//! Construct a ZeroLength element with \p tag.
//! The force-deformation relationship is given by the {\em n1dMat} pointers
//! \p theMaterial to {\bf UniaxialMaterial} models. 
//! \p direction is an {\bf ID}  of length {\em n1dMat} that gives
//! the local direction for each corresponding entry in the array \p theMaterial
//! The local \p direction is 1, 2, 3, for translation in the local x, y, z
//! axes or 4, 5, 6  for rotation about the local x, y, z axes.
//!
//! @param tag: element identifier.
//! @param dim: space dimension (1, 2 or 3).
//! @param Nd1: identifier of the first node.
//! @param Nd2: identifier of the second node.
//! @param x: Vector that defines the local x-axis.
//! @param yp: Vector that defines the local x-y plane.
//! @param theMat: material container.
//! @param direction: direction container.
XC::ZeroLength::ZeroLength(int tag,int dim,int Nd1, int Nd2,const Vector& x, const Vector& yp,const DqUniaxialMaterial &theMat,const ID& direction )
  :Element0D(tag,ELE_TAG_ZeroLength,Nd1,Nd2,dim,x,yp), theMatrix(nullptr), theVector(nullptr), theMaterial1d(this,theMat,direction) {}


//! @brief Default constructor:
XC::ZeroLength::ZeroLength(void)
  :Element0D(0,ELE_TAG_ZeroLength,0,0,0),theMatrix(nullptr), theVector(nullptr),
   theMaterial1d(this) {}

void XC::ZeroLength::setMaterial(const int &dir,const std::string &nmbMat)
  {
    Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        const MaterialHandler &material_handler= getPreprocessor()->getMaterialHandler();
        const Material *ptr_mat= material_handler.find_ptr(nmbMat);
        if(ptr_mat)
          {
            const UniaxialMaterial *tmp= dynamic_cast<const UniaxialMaterial *>(ptr_mat);
            if(tmp)
              theMaterial1d.push_back(dir,tmp);
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
                        << "material identified by: '" << nmbMat
                        << "' is not an uniaxial material.\n";
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
                    << "material identified by: '" << nmbMat
                    << "' not found.\n";
        if(theMaterial1d.size() > 0 )
          setTran1d(elemType, theMaterial1d.size());
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to preprocessor." << std::endl;
      }
  }

void XC::ZeroLength::setMaterials(const std::deque<int> &dirs,const std::vector<std::string> &nmbMats)
  {
    const size_t n= nmbMats.size();
    if(n!= dirs.size())
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; error in number of materials; number of directions: "
	      << dirs.size()
              << " number of materials: " << n << std::endl;
    Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        const MaterialHandler &material_handler= getPreprocessor()->getMaterialHandler();
        for(size_t i= 0;i<n;i++)
          {
            const Material *ptr_mat= material_handler.find_ptr(nmbMats[i]);
            if(ptr_mat)
              {
                const UniaxialMaterial *tmp= dynamic_cast<const UniaxialMaterial *>(ptr_mat);
                if(tmp)
                  theMaterial1d.push_back(dirs[i],tmp);
                else
	      std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
                            << "el material de code: '" << nmbMats[i]
                            << "' no corresponde a un material uniaxial.\n";
              }
            else
              std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
                        << "material identified by : '" << nmbMats[i]
                        << "' not found.\n";
          }
        if(theMaterial1d.size() > 0 )
          setTran1d(elemType, theMaterial1d.size());
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; null pointer to preprocessor." << std::endl;
      }
  }

//! @brief Virtual constructor.
XC::Element* XC::ZeroLength::getCopy(void) const
  { return new ZeroLength(*this); }

//! @brief Destructor:
//!  delete must be invoked on any objects created by the object
//!  and on the matertial object.
XC::ZeroLength::~ZeroLength(void)
  {
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to

    // invoke destructors on material objects
    theMaterial1d.clear();
  }

//! @brief Sets the element type and matrix dimensions from
//! element dimension and the number of DOF of the connected nodes.
void XC::ZeroLength::setUpType(const size_t &numDOFsNodes)
  {
    if(dimension == 1 && numDOFsNodes == 1)
      {
        numDOF = 2;
        theMatrix = &ZeroLengthM2;
        theVector = &ZeroLengthV2;
        elemType  = D1N2;
      }
    else if(dimension == 2 && numDOFsNodes == 2)
      {
        numDOF = 4;
        theMatrix = &ZeroLengthM4;
        theVector = &ZeroLengthV4;
        elemType  = D2N4;
      }
    else if(dimension == 2 && numDOFsNodes == 3)
      {
        numDOF = 6;
        theMatrix = &ZeroLengthM6;
        theVector = &ZeroLengthV6;
        elemType  = D2N6;
      }
    else if(dimension == 3 && numDOFsNodes == 3)
      {
        numDOF = 6;
        theMatrix = &ZeroLengthM6;
        theVector = &ZeroLengthV6;
        elemType  = D3N6;
      }
    else if(dimension == 3 && numDOFsNodes == 6)
      {
        numDOF = 12;
        theMatrix = &ZeroLengthM12;
        theVector = &ZeroLengthV12;
        elemType  = D3N12;
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING cannot handle " << dimension
                  << " dofs at nodes in " << numDOFsNodes << " d problem\n";
        return;
      }
  }

//! @brief Set the elemento domain.
//!
//! to set a link to the enclosing Domain and to set the node pointers.
//! also determines the number of dof associated
//! with the ZeroLength element, we set matrix and vector pointers,
//! allocate space for t matrix and define it as the basic deformation-
//! displacement transformation matrix.
void XC::ZeroLength::setDomain(Domain *theDomain)
  {
    // call the base class method
    Element0D::setDomain(theDomain);

    // set default values for error conditions
    numDOF = 2;
    theMatrix = &ZeroLengthM2;
    theVector = &ZeroLengthV2;

    // now determine the number of dof and the dimension
    const int dofNd1 = theNodes[0]->getNumberDOF();
    const int dofNd2 = theNodes[1]->getNumberDOF();

    // if differing dof at the ends - print a warning message
    if(dofNd1 != dofNd2)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING nodes " << theNodes[0]->getTag() << " and "
                  << theNodes[1]->getTag()
		  << "have differing dof at ends for ZeroLength "
                  << this->getTag() << std::endl;
        return;
      }

    // Check that length is zero within tolerance
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    Vector diff = end1Crd - end2Crd;
    double L  = diff.Norm();
    double v1 = end1Crd.Norm();
    double v2 = end2Crd.Norm();
    const double vm= (v1<v2) ? v2 : v1;
    const double lentol_vm= LenTol*vm;


    if(L > lentol_vm)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; WARNING element " 
                << this->getTag() << " has L= " << L
                << ", which is greater than the tolerance: "
		<< lentol_vm << std::endl;


    // set the number of dof for element and set matrix and vector pointer
    setUpType(dofNd1);

    // create the basic deformation-displacement transformation matrix for the element
    // for 1d materials (uniaxial materials)
    if(theMaterial1d.size() > 0 )
      setTran1d(elemType,theMaterial1d.size());
  }

//! @brief Commit state of element.
//! 
//! Commit state of element by committing state of materials.
//! Return 0 if successful, !0 otherwise.
int XC::ZeroLength::commitState(void)
  {
    int code=0;

    // call element commitState to do any base class stuff
    if((code = Element0D::commitState()) != 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed in base class";
    // commit 1d materials
    theMaterial1d.commitState();
    return code;
  }

//! Revert state of element to last commit.
//! 
//! Revert state of element to last commit by reverting to last committed
//! state of materials. Return 0 if successful, !0 otherwise.
int XC::ZeroLength::revertToLastCommit(void)
  { return theMaterial1d.revertToLastCommit(); }

//! Revert state of element to initial.
//! 
//! Revert state of element to initial state by reverting to initial state
//! its materials. Return 0 if successful, !0 otherwise.
int XC::ZeroLength::revertToStart(void)
  { return theMaterial1d.revertToStart(); }


int XC::ZeroLength::update(void)
  {
    // get trial displacements and take difference
    if(!theNodes[0])
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; node 0 of element: " << getTag()
		<< " is not set." << std::endl;
    if(!theNodes[1])
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; node 1 of element: "
		<< getTag() << " is not set." << std::endl;

    int ret = 0;
    if(theNodes[0] && theNodes[1])
      {
        const Vector& disp1= theNodes[0]->getTrialDisp();
        const Vector& disp2= theNodes[1]->getTrialDisp();
        Vector  diff  = disp2-disp1;
        const Vector& vel1= theNodes[0]->getTrialVel();
        const Vector& vel2= theNodes[1]->getTrialVel();
        Vector  diffv = vel2-vel1;

        // loop over 1d materials

        //    Matrix& tran = t1d;
        double strain;
        double strainRate;
        for(size_t mat=0; mat<theMaterial1d.size(); mat++)
          {
            // compute strain and rate; set as current trial for material
            strain     = this->computeCurrentStrain1d(mat,diff );
            strainRate = this->computeCurrentStrain1d(mat,diffv);
            ret += theMaterial1d[mat]->setTrialStrain(strain,strainRate);
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; cannot update element: " << getTag() << std::endl;
    return ret;
  }

//! @brief Return tangent stiffness matrix for element.
const XC::Matrix &XC::ZeroLength::getTangentStiff(void) const
  {
    double E;

    // stiff is a reference to the matrix holding the stiffness matrix
    Matrix& stiff = *theMatrix;

    // zero stiffness matrix
    stiff.Zero();

    // loop over 1d materials

    const Matrix &tran= t1d;
    for(size_t mat=0; mat<theMaterial1d.size(); mat++)
      {

        // get tangent for material
        E = theMaterial1d[mat]->getTangent();

        // compute contribution of material to tangent matrix
        for(int i=0; i<numDOF; i++)
          for(int j=0; j<i+1; j++)
            stiff(i,j) +=  tran(mat,i) * E * tran(mat,j);

      }

    // end loop over 1d materials

    // complete symmetric stiffness matrix
    for(int i=0; i<numDOF; i++)
      for(int j=0; j<i; j++)
        stiff(j,i) = stiff(i,j);
    if(isDead())
      stiff*=dead_srf;
    return stiff;
  }

//! @brief Return initial stiffness matrix.
const XC::Matrix &XC::ZeroLength::getInitialStiff(void) const
  {
    double E= 0.0;

    // stiff is a reference to the matrix holding the stiffness matrix
    Matrix& stiff = *theMatrix;

    // zero stiffness matrix
    stiff.Zero();

    // loop over 1d materials

    const Matrix &tran= t1d;
    for(size_t mat=0; mat<theMaterial1d.size(); mat++)
      {
        // get tangent for material
        E = theMaterial1d[mat]->getInitialTangent();

        // compute contribution of material to tangent matrix
        for(int i=0; i<numDOF; i++)
          for(int j=0; j<i+1; j++)
            stiff(i,j) +=  tran(mat,i) * E * tran(mat,j);
      }

    // end loop over 1d materials

    // complete symmetric stiffness matrix
    for(int i=0; i<numDOF; i++)
      for(int j=0; j<i; j++)
        stiff(j,i) = stiff(i,j);

    if(isDead())
      stiff*= dead_srf;
    return stiff;
  }


//! @brief Return the element damping matrix.
const XC::Matrix &XC::ZeroLength::getDamp(void) const
  {
    double eta;

    // damp is a reference to the matrix holding the damping matrix
    Matrix& damp = *theMatrix;

    // zero stiffness matrix
    damp.Zero();

    // loop over 1d materials
    const Matrix &tran= t1d;
    for(size_t mat=0; mat<theMaterial1d.size(); mat++)
      {
        // get tangent for material
        eta = theMaterial1d[mat]->getDampTangent();

        // compute contribution of material to tangent matrix
        for(int i=0; i<numDOF; i++)
          for(int j=0; j<i+1; j++)
            damp(i,j) +=  tran(mat,i) * eta * tran(mat,j);
      } // end loop over 1d materials

    // complete symmetric stiffness matrix
    for(int i=0; i<numDOF; i++)
        for(int j=0; j<i; j++)
            damp(j,i) = damp(i,j);

    if(isDead())
      damp*=dead_srf;
    return damp;
  }

//! @brief Return a zero mass matrix.
const XC::Matrix &XC::ZeroLength::getMass(void) const
  {
    // no mass
    theMatrix->Zero();
    return *theMatrix;
  }


//! @brief The element has no loads, so this operation has no effect
//! and returns 0.
int XC::ZeroLength::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; load type unknown for ZeroLength with tag: "
	      << this->getTag() << std::endl;
    return -1;
  }

//! @brief The element has no mass, so this operation has no effect and
//! returns 0.
int XC::ZeroLength::addInertiaLoadToUnbalance(const Vector &accel)
  {
    // does nothing as element has no mass yet!
    return 0;
  }

//! @brief Return the element internal forces.
XC::Vector XC::ZeroLength::getInternalForces(void) const
  {
    const size_t sz= theMaterial1d.size();
    Vector retval(sz);

    // loop over 1d materials
    for(size_t mat=0; mat<sz; mat++)
      {
        // get resisting force for material
        retval(mat)= theMaterial1d[mat]->getStress();
      } // end loop over 1d materials
    return retval;
  }

//! @brief Return resisting force vector.
const XC::Vector &XC::ZeroLength::getResistingForce(void) const
  {
    // zero the residual
    theVector->Zero();

    const Vector internalForces= getInternalForces();
    const size_t sz= internalForces.Size();
    // loop over 1d materials
    for(size_t mat=0; mat<sz; mat++)
      {
        const double &force= internalForces[mat];
        // compute residual due to resisting force
        for(int i=0; i<numDOF; i++)
            (*theVector)(i)+= t1d(mat,i) * force;
      } // end loop over 1d materials
    if(isDead())
      (*theVector)*=dead_srf;
    return *theVector;
  }

//! @brief Return resisting force vector with inertia included.
const XC::Vector &XC::ZeroLength::getResistingForceIncInertia(void) const
  {
    // there is no mass, so return
    return this->getResistingForce();
  }

//! @brief Extrapolate from Gauss points to nodes.
XC::Matrix XC::ZeroLength::getExtrapolatedValues(const Matrix &values) const
  {
    const size_t sz= values.noRows();
    Matrix retval(2,sz); // Two nodes and sz values per node.
    const Matrix &e_matrix= getExtrapolationMatrix();
    std::cout << "values: " << values << std::endl;
    for(size_t i= 0;i<sz;i++)
      {
	retval(0,i)= e_matrix(0,0)*values(i,0);
	retval(1,i)= e_matrix(1,0)*values(i,0);
      }
    return retval;
  }

//! @brief Return a python list with the values of the argument property
//! at element nodes.
//!
//! When the property requested its located at the integration point this
//! function is responsible of the extrapolation of values from
//! Gauss points to nodes.
//! @param code: identifier of the requested value.
//! @param silent: if true don't complain about non-existen property.
boost::python::list XC::ZeroLength::getValuesAtNodes(const std::string &code, bool silent) const
  {
    boost::python::list retval;
    if(code=="strain")
      {
	const Matrix elementStrains= theMaterial1d.getGeneralizedStrains();
	const Matrix strainsAtNodes= getExtrapolatedValues(elementStrains);
	const size_t nRows= strainsAtNodes.noRows();
	for(size_t i= 0;i<nRows;i++)
	  {
	    Vector strainAtNode= strainsAtNodes.getRow(i);
	    retval.append(strainAtNode);
	  }
      }
    else if(code=="stress")
      {
	const Matrix elementStresses= theMaterial1d.getGeneralizedStresses();
	const Matrix stressesAtNodes= getExtrapolatedValues(elementStresses);
	const size_t nRows= stressesAtNodes.noRows();
	for(size_t i= 0;i<nRows;i++)
	  {
	    Vector stressAtNode= stressesAtNodes.getRow(i);
	    retval.append(stressAtNode);
	  }
      }
    else
      retval= Element0D::getValuesAtNodes(code, silent); 
    return retval;
  }

//! @brief Send members through the communicator argument.
int XC::ZeroLength::sendData(Communicator &comm)
  {
    int res= Element0D::sendData(comm);
    res+= comm.sendInt(elemType,getDbTagData(),CommMetaData(9));
    res+= comm.sendMatrixPtr(theMatrix,getDbTagData(),MatrixCommMetaData(10,11,12,13));
    res+= comm.sendVectorPtr(theVector,getDbTagData(),ArrayCommMetaData(14,15,16));
    res+= comm.sendMovable(theMaterial1d,getDbTagData(),CommMetaData(17));
    res+= comm.sendMatrix(t1d,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ZeroLength::recvData(const Communicator &comm)
  {
    int res= Element0D::recvData(comm);
    int et= elemType;
    res+= comm.receiveInt(et,getDbTagData(),CommMetaData(9));
    elemType= Etype(et);
    theMatrix= comm.receiveMatrixPtr(theMatrix,getDbTagData(),MatrixCommMetaData(10,11,12,13));
    theVector= comm.receiveVectorPtr(theVector,getDbTagData(),ArrayCommMetaData(14,15,16));
    res+= comm.receiveMovable(theMaterial1d,getDbTagData(),CommMetaData(17));
    res+= comm.receiveMatrix(t1d,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::ZeroLength::sendSelf(Communicator &comm)
  {
    inicComm(22);

    int res= sendData(comm);

    const int dataTag= getDbTag();
    res += comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send ID data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::ZeroLength::recvSelf(const Communicator &comm)
  {
    inicComm(22);

    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ID data.\n";
    else
      res+= recvData(comm);
    return res;
  }

//! @brief Print information about element.
void XC::ZeroLength::Print(std::ostream &s, int flag) const
  {
    // compute the strain and axial force in the member
    double strain=0.0;
    double force =0.0;

    for(int i=0; i<numDOF; i++)
      (*theVector)(i) = t1d(0,i)*force;

    if(flag == 0)
      { // print everything
        s << "Element: " << this->getTag();
        s << " type: ZeroLength  iNode: " << theNodes.getTagNode(0);
        s << " jNode: " << theNodes.getTagNode(1) << std::endl;
        for(size_t j = 0; j < theMaterial1d.size(); j++)
          {
            s << "\tMaterial1d, tag: " << theMaterial1d[j]->getTag()
              << ", dir: " << theMaterial1d.getDir(j) << std::endl;
            s << *(theMaterial1d[j]);
          }
      }
    else if(flag == 1)
      { s << this->getTag() << "  " << strain << "  ";  }
  }

//! Set response quantities as "force", "deformation", "material", or "stiff".
//! Return response ID  or -1 if error.  Currently, only the one uniaxial
//! material can be set.
XC::Response *XC::ZeroLength::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    if(argv[0] == "force" || argv[0] == "forces")
       return new ElementResponse(this, 1, Vector(theMaterial1d.size()));
    else if(argv[0] == "defo" || argv[0] == "deformations" ||
                argv[0] == "deformation")
       return new ElementResponse(this, 2, Vector(theMaterial1d.size()));
    else if((argv[0] == "defoANDforce") ||
             (argv[0] == "deformationANDforces") ||
             (argv[0] == "deformationsANDforces"))
       return new ElementResponse(this, 4, Vector(2*theMaterial1d.size()));
    else if(argv[0] == "stiff") // tangent stiffness matrix
       return new ElementResponse(this, 3, Matrix(theMaterial1d.size(),theMaterial1d.size()));
    else if(argv[0] == "material")
      {
        if(argv.size() <= 2)
          return 0;
        size_t matNum = atoi(argv[1]);
        if(matNum < 1 || matNum > theMaterial1d.size())
          return 0;
        else
          return setMaterialResponse(theMaterial1d[matNum-1],argv,2,eleInformation);
      }
    else
      return 0;
  }

//! @brief Get response information for \p responseID.  Return 0 if successful,
//! -1 otherwise.
int XC::ZeroLength::getResponse(int responseID, Information &eleInformation)
  {
    const Vector& disp1 = theNodes[0]->getTrialDisp();
    const Vector& disp2 = theNodes[1]->getTrialDisp();
    const Vector  diff  = disp2-disp1;

    switch (responseID)
      {
      case -1:
        return -1;
      case 1:
        if(eleInformation.theVector != 0)
          {
            for(size_t i = 0; i < theMaterial1d.size(); i++)
              (*(eleInformation.theVector))(i) = theMaterial1d[i]->getStress();
          }
         return 0;
      case 2:
        if(eleInformation.theVector != 0)
          {
            for(size_t i = 0; i < theMaterial1d.size(); i++)
              (*(eleInformation.theVector))(i) = theMaterial1d[i]->getStrain();
          }
        return 0;
      case 4:
        if(eleInformation.theVector != 0)
          {
            for(size_t i = 0; i < theMaterial1d.size(); i++)
              {
                (*(eleInformation.theVector))(i) = theMaterial1d[i]->getStrain();
                (*(eleInformation.theVector))(i+theMaterial1d.size()) = theMaterial1d[i]->getStress();
              }
          }
        return 0;
      case 3:
        if(eleInformation.theMatrix != 0)
          {
            for(size_t i = 0; i < theMaterial1d.size(); i++)
              (*(eleInformation.theMatrix))(i,i) = theMaterial1d[i]->getTangent();
          }
        return 0;
      default:
        return -1;
      }
  }


// Private methods


// Check that direction is in the range of 0 to 5
void XC::ZeroLength::checkDirection( ID &dir ) const
  {
    for(int i=0; i<dir.Size(); i++)
      if(dir(i) < 0 || dir(i) > 5 )
        {
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << ";  incorrect direction "
		    << dir(i) << " is set to 0\n";
          dir(i) = 0;
        }
  }

std::string XC::ZeroLength::getElementType(void) const
  {
    std::string retval;
    switch(elemType)
      {
      case D1N2:
        retval= "D1N2";
        break;
      case D2N4:
        retval= "D2N4";
        break;
      case D2N6:
        retval= "D2N6";
        break;
      case D3N6:
        retval= "D3N6";
        break;
      case D3N12:
        retval= "D3N12";
        break;
      default:
        retval= "unknown";
        break;
      } // end switch
    return retval;
  }

//! @brief Set basic deformation-displacement transformation matrix for 1d
//! uniaxial materials
void XC::ZeroLength::setTran1d(Etype elemType,int numMat)
  {
    if(numDOF>0)
      {
        enum Dtype { TRANS, ROTATE };

        // Create 1d transformation matrix
        t1d= Matrix(numMat,numDOF);

        // Use reference for convenience and zero matrix.
        Matrix& tran= t1d;
        tran.Zero();

        // loop over materials, setting row in tran for each material depending on dimensionality of element
        int indx,dir;
        Dtype dirType;
        for(int i=0;i<numMat;i++)
          {
            dir= theMaterial1d.getDir(i); //! direction 0 to 5;
            indx= dir % 3; //! direction 0, 1, 2 for axis of translation or rotation

            // set direction type to translation or rotation
            dirType= (dir<3) ? TRANS : ROTATE;

            // now switch on dimensionality of element
            switch(elemType)
              {
              case D1N2:
                if(dirType == TRANS)
                  tran(i,1) = transformation(indx,0);
                break;
              case D2N4:
                if(dirType == TRANS)
                  {
                    tran(i,2) = transformation(indx,0);
                    tran(i,3) = transformation(indx,1);
                  }
                break;
              case D2N6:
                if(dirType == TRANS)
                  {
                    tran(i,3) = transformation(indx,0);
                    tran(i,4) = transformation(indx,1);
                    tran(i,5) = 0.0;
                  }
                else if(dirType == ROTATE)
                  {
                    tran(i,3) = 0.0;
                    tran(i,4) = 0.0;
                    tran(i,5) = transformation(indx,2);
                  }
                break;

              case D3N6:
                if(dirType == TRANS)
                  {
                    tran(i,3) = transformation(indx,0);
                    tran(i,4) = transformation(indx,1);
                    tran(i,5) = transformation(indx,2);
                  }
                break;
              case D3N12:
                if(dirType == TRANS)
                  {
                    tran(i,6)  = transformation(indx,0);
                    tran(i,7)  = transformation(indx,1);
                    tran(i,8)  = transformation(indx,2);
                    tran(i,9)  = 0.0;
                    tran(i,10) = 0.0;
                    tran(i,11) = 0.0;
                  }
                else if(dirType == ROTATE)
                  {
                    tran(i,6)  = 0.0;
                    tran(i,7)  = 0.0;
                    tran(i,8)  = 0.0;
                    tran(i,9)  = transformation(indx,0);
                    tran(i,10) = transformation(indx,1);
                    tran(i,11) = transformation(indx,2);
                  }
                break;
              } // end switch

            // fill in first half of transformation matrix with
            // negative sign

            for(int j=0; j < numDOF/2; j++ )
                tran(i,j) = -tran(i,j+numDOF/2);
        } // end loop over 1d materials
      } //if numDOF>0
  }


//! @brief Compute current strain for 1d material mat dispDiff are the
//! displacements of node 2 minus those of node 1
double XC::ZeroLength::computeCurrentStrain1d(int mat,const XC::Vector& dispDiff ) const
  {
    double strain = 0.0;
    for(int i=0; i<numDOF/2; i++)
      { strain+= -dispDiff(i) * t1d(mat,i); }
    return strain;
  }

void XC::ZeroLength::setUp(int Nd1, int Nd2,const Vector &x,const Vector &y)
  {
    Element0D::setUp(Nd1,Nd2, x, y);
    if(theMaterial1d.size() > 0 )
      setTran1d(elemType, theMaterial1d.size());
  }

void XC::ZeroLength::updateDir(const XC::Vector& x, const XC::Vector& y)
  { setUp(theNodes.getTagNode(0), theNodes.getTagNode(1), x, y); }
