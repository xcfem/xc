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

// $Revision: 1.11 $
// $Date: 2003/03/12 19:20:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/truss/CorotTruss.cpp,v $

// Written: MHS
// Created: May 2001
//
// Description: This file contains the class implementation for XC::CorotTruss.

#include <domain/mesh/element/truss_beam_column/truss/CorotTruss.h>
#include <domain/mesh/element/utils/Information.h>

#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/uniaxial/CableMaterial.h>
#include <utility/recorder/response/ElementResponse.h>

#include "domain/load/beam_loads/TrussStrainLoad.h"
#include "utility/actor/actor/MovableVector.h"

// constructor:
//  responsible for allocating the necessary space needed by each object
//  and storing the tags of the XC::CorotTruss end nodes.
XC::CorotTruss::CorotTruss(int tag, int dim,int Nd1, int Nd2, UniaxialMaterial &theMat,double a)
  :CorotTrussBase(tag,ELE_TAG_CorotTruss,dim,Nd1,Nd2), theMaterial(nullptr), A(a)
  {
    // get a copy of the material and check we obtained a valid copy
    theMaterial = theMat.getCopy();
    if(theMaterial == 0)
      {
        std::cerr << "FATAL XC::CorotTruss::CorotTruss - " <<  tag <<
          "failed to get a copy of material with tag " << theMat.getTag() << std::endl;
        exit(-1);
      }
  }

XC::CorotTruss::CorotTruss(int tag,int dim,const Material *ptr_mat)
  :CorotTrussBase(tag,ELE_TAG_CorotTruss,dim,0,0), theMaterial(nullptr),A(0.0)
  { theMaterial= cast_material<UniaxialMaterial>(ptr_mat); }

// constructor:
//   invoked by a FEM_ObjectBroker - blank object that recvSelf needs
//   to be invoked upon
XC::CorotTruss::CorotTruss(void)
  :CorotTrussBase(0,ELE_TAG_CorotTruss,0,0,0), theMaterial(nullptr), A(0.0) {}

//! @brief Copy constructor.
XC::CorotTruss::CorotTruss(const CorotTruss &otro)
  : CorotTrussBase(otro), theMaterial(nullptr), A(otro.A)
  {
    if(otro.theMaterial)
      theMaterial= otro.theMaterial->getCopy();
  }

//! @brief Operador de asignación.
XC::CorotTruss &XC::CorotTruss::operator=(const CorotTruss &otro)
  {
    CorotTrussBase::operator=(otro);
    A= otro.A;
    if(theMaterial) delete theMaterial;
    theMaterial= nullptr;
    if(otro.theMaterial)
      theMaterial= otro.theMaterial->getCopy();
    return *this;
  }

//! @brief Virtual constructor.
XC::Element* XC::CorotTruss::getCopy(void) const
  { return new CorotTruss(*this); }

//! @brief Destructor.
//!     delete must be invoked on any objects created by the object
//!     and on the matertial object.
XC::CorotTruss::~CorotTruss()
  {
    // invoke the destructor on any objects created by the object
    // that the object still holds a pointer to
    if(theMaterial != 0)
      delete theMaterial;
  }

// method: setDomain()
//    to set a link to the enclosing XC::Domain and to set the node pointers.
//    also determines the number of dof associated
//    with the XC::CorotTruss element, we set matrix and vector pointers,
//    allocate space for t matrix, determine the length
//    and set the transformation matrix.
void XC::CorotTruss::setDomain(Domain *theDomain)
  {
    // check Domain is not null - invoked when object removed from a domain
    CorotTrussBase::setDomain(theDomain);
    if(!theDomain)
      {
        Lo= 0.0;
        Ln= 0.0;
        return;
      }

    // now determine the number of dof and the dimesnion
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();

    // if differing dof at the ends - print a warning message
    if(dofNd1 != dofNd2)
      {
        std::cerr << "WARNING XC::CorotTruss::setDomain(): nodes " << theNodes[0]->getTag() <<
        " and " << theNodes[1]->getTag() << "have differing dof at ends for XC::CorotTruss " << this->getTag() << std::endl;
        // fill this in so don't segment fault later
        numDOF = 6;
        return;
      }

    setup_matrix_vector_ptrs(dofNd1);

    // now determine the length, cosines and fill in the transformation
    // NOTE t = -t(every one else uses for residual calc)
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();

    // Determine global offsets
    double cosX[3];
    cosX[0] = 0.0;  cosX[1] = 0.0;  cosX[2] = 0.0;
    int i;
    for(i = 0; i < getNumDIM(); i++)
      { cosX[i] += end2Crd(i)-end1Crd(i); }

    // Set undeformed and initial length
    Lo= cosX[0]*cosX[0] + cosX[1]*cosX[1] + cosX[2]*cosX[2];
    Lo= sqrt(Lo);
    Ln= Lo;
    CableMaterial *ptrCableMaterial= dynamic_cast<CableMaterial *>(theMaterial);
    if(ptrCableMaterial)
      ptrCableMaterial->setLength(Ln);
   

    // Initial offsets
    d21[0] = Lo;
    d21[1] = 0.0;
    d21[2] = 0.0;

    // Set global orientation
    cosX[0] /= Lo;
    cosX[1] /= Lo;
    cosX[2] /= Lo;

    R(0,0) = cosX[0];
    R(0,1) = cosX[1];
    R(0,2) = cosX[2];

    // Element lies outside the YZ plane
    if(fabs(cosX[0]) > 0.0)
      {
        R(1,0) = -cosX[1];
        R(1,1) =  cosX[0];
        R(1,2) =  0.0;

        R(2,0) = -cosX[0]*cosX[2];
        R(2,1) = -cosX[1]*cosX[2];
        R(2,2) =  cosX[0]*cosX[0] + cosX[1]*cosX[1];
      } // Element is in the YZ plane
    else
      {
        R(1,0) =  0.0;
        R(1,1) = -cosX[2];
        R(1,2) =  cosX[1];

        R(2,0) =  1.0;
        R(2,1) =  0.0;
        R(2,2) =  0.0;
      }

    // Orthonormalize last two rows of R
    double norm;
    for(i = 1; i < 3; i++)
      {
        norm = sqrt(R(i,0)*R(i,0) + R(i,1)*R(i,1) + R(i,2)*R(i,2));
        R(i,0) /= norm;
        R(i,1) /= norm;
        R(i,2) /= norm;
      }
  }

int XC::CorotTruss::commitState(void)
  {
    int retVal = 0;
    // call element commitState to do any base class stuff
    if((retVal = this->CorotTrussBase::commitState()) != 0)
      {
        std::cerr << "XC::CorotTruss::commitState () - failed in base class\n";
      }
    retVal = theMaterial->commitState();
    return retVal;
  }

int XC::CorotTruss::revertToLastCommit()
  {
        // Revert the material
        return theMaterial->revertToLastCommit();
  }

int XC::CorotTruss::revertToStart()
  {
        // Revert the material to start
        return theMaterial->revertToStart();
  }

int XC::CorotTruss::update(void)
  {
    // Nodal displacements
    const XC::Vector &end1Disp = theNodes[0]->getTrialDisp();
    const XC::Vector &end2Disp = theNodes[1]->getTrialDisp();

    // Initial offsets
    d21[0] = Lo;
    d21[1] = 0.0;
    d21[2] = 0.0;

    // Update offsets in basic system due to nodal displacements
    for(int i = 0; i < getNumDIM(); i++)
      {
        d21[0] += R(0,i)*(end2Disp(i)-end1Disp(i));
        d21[1] += R(1,i)*(end2Disp(i)-end1Disp(i));
        d21[2] += R(2,i)*(end2Disp(i)-end1Disp(i));
      }

    // Compute new length
    Ln= d21[0]*d21[0] + d21[1]*d21[1] + d21[2]*d21[2];
    Ln= sqrt(Ln);

    CableMaterial *ptrCableMaterial= dynamic_cast<CableMaterial *>(theMaterial);
    if(ptrCableMaterial)
      ptrCableMaterial->setLength(Ln);

    // Compute engineering strain
    const double strain= (Ln-Lo)/Lo;

    // Set material trial strain
    return theMaterial->setTrialStrain(strain);
  }

const XC::Matrix &XC::CorotTruss::getTangentStiff(void) const
  {
    static Matrix kl(3,3);

    // Material stiffness
    //
    // Get material tangent
    double EA = A*theMaterial->getTangent();
    EA /= (Ln * Ln * Lo);

    int i,j;
    for(i = 0; i < 3; i++)
      for(j = 0; j < 3; j++)
        kl(i,j) = EA*d21[i]*d21[j];

    // Geometric stiffness
    //
    // Get material stress
    double q = A*theMaterial->getStress();
    double SA = q/(Ln*Ln*Ln);
    double SL = q/Ln;

    for(i = 0; i < 3; i++)
      {
        kl(i,i) += SL;
        for(j = 0; j < 3; j++)
          kl(i,j) -= SA*d21[i]*d21[j];
      }

    // Compute R'*kl*R
    static Matrix kg(3,3);
    kg.addMatrixTripleProduct(0.0, R, kl, 1.0);

    Matrix &K = *theMatrix;
    K.Zero();

    // Copy stiffness into appropriate blocks in element stiffness
    int numDOF2 = numDOF/2;
    for(i = 0; i < getNumDIM(); i++)
      {
        for(j = 0; j < getNumDIM(); j++)
          {
            K(i,j)                 =  kg(i,j);
            K(i,j+numDOF2)         = -kg(i,j);
            K(i+numDOF2,j)         = -kg(i,j);
            K(i+numDOF2,j+numDOF2) =  kg(i,j);
          }
      }
    if(isDead())
      (*theMatrix)*=dead_srf;
    return *theMatrix;
  }

const XC::Matrix &XC::CorotTruss::getInitialStiff(void) const
  {
    static XC::Matrix kl(3,3);

    // Material stiffness
    kl.Zero();
    kl(0,0)= A*theMaterial->getInitialTangent() / Lo;

    // Compute R'*kl*R
    static XC::Matrix kg(3,3);
    kg.addMatrixTripleProduct(0.0, R, kl, 1.0);

    Matrix &K = *theMatrix;
    K.Zero();

    // Copy stiffness into appropriate blocks in element stiffness
    int numDOF2 = numDOF/2;
    for(int i = 0; i < getNumDIM(); i++) {
        for(int j = 0; j < getNumDIM(); j++) {
            K(i,j)                 =  kg(i,j);
            K(i,j+numDOF2)         = -kg(i,j);
            K(i+numDOF2,j)         = -kg(i,j);
            K(i+numDOF2,j+numDOF2) =  kg(i,j);
        }
    }

    if(isDead())
      (*theMatrix)*=dead_srf;
    return *theMatrix;
  }

const XC::Material *XC::CorotTruss::getMaterial(void) const
  { return theMaterial; }
XC::Material *XC::CorotTruss::getMaterial(void)
  { return theMaterial; }
//! @brief Return the densidad del material.
double XC::CorotTruss::getRho(void) const
  { return theMaterial->getRho(); }

const XC::Matrix &XC::CorotTruss::getMass(void) const
  {
    Matrix &Mass = *theMatrix;
    Mass.Zero();

    const double rho= getRho();
    // check for XC::quick return
    if(Lo == 0.0 || rho == 0.0)
      return Mass;

    double M = 0.5*rho*Lo;
    int numDOF2 = numDOF/2;
    for(int i = 0; i < getNumDIM(); i++)
      {
        Mass(i,i)                 = M;
        Mass(i+numDOF2,i+numDOF2) = M;
      }
    if(isDead())
      (*theMatrix)*=dead_srf;
    return *theMatrix;
  }

//! @brief Adds a load.
int XC::CorotTruss::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(isDead())
      std::cerr << nombre_clase() 
                << "; load over inactive element: "
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
            std::cerr <<"CorotTruss::addLoad - load type unknown for truss with tag: " << this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }



int XC::CorotTruss::addInertiaLoadToUnbalance(const XC::Vector &accel)
  { return 0; }

double XC::CorotTruss::getAxil(void) const
  { return A*theMaterial->getStress(); }

const XC::Vector &XC::CorotTruss::getResistingForce(void) const
  {
    // Get material stress
    double SA= getAxil();
    SA /= Ln;

    static XC::Vector ql(3);

    ql(0) = d21[0]*SA;
    ql(1) = d21[1]*SA;
    ql(2) = d21[2]*SA;

    static XC::Vector qg(3);
    qg.addMatrixTransposeVector(0.0, R, ql, 1.0);

    Vector &P = *theVector;
    P.Zero();

    // Copy forces into appropriate places
    int numDOF2 = numDOF/2;
    for(int i = 0; i < getNumDIM(); i++)
      {
        P(i)         = -qg(i);
        P(i+numDOF2) =  qg(i);
      }
    if(isDead())
      (*theVector)*=dead_srf;
    return *theVector;
  }



const XC::Vector &XC::CorotTruss::getResistingForceIncInertia(void) const
  {
    Vector &P = *theVector;
    P = this->getResistingForce();

    const double rho= getRho();
    if(rho != 0.0)
      {
        const XC::Vector &accel1 = theNodes[0]->getTrialAccel();
        const XC::Vector &accel2 = theNodes[1]->getTrialAccel();

        double M = 0.5*rho*Lo;
        int numDOF2 = numDOF/2;
        for(int i = 0; i < getNumDIM(); i++)
           {
             P(i)+= M*accel1(i);
             P(i+numDOF2)+= M*accel2(i);
           }
      }

    // add the damping forces if rayleigh damping
    if(!rayFactors.Nulos())
      *theVector += this->getRayleighDampingForces();

    if(isDead())
      (*theVector)*=dead_srf; //XXX Se aplica 2 veces sobre getResistingForce: arreglar.
    return *theVector;
  }


void XC::CorotTruss::Print(std::ostream &s, int flag)
  {
    s << "\nCorotTruss, tag: " << this->getTag() << std::endl;
    s << "\tConnected Nodes: " << theNodes;
    s << "\tSection Area: " << A << std::endl;
    s << "\tUndeformed Length: " << Lo << std::endl;
    s << "\tCurrent Length: " << Ln << std::endl;
    s << "\tRotation matrix: " << std::endl;

    if(theMaterial)
      {
        s << "\tAxial Force: " << getAxil() << std::endl;
        s << "\tUniaxialMaterial, tag: " << theMaterial->getTag() << std::endl;
        theMaterial->Print(s,flag);
      }
  }

XC::Response *XC::CorotTruss::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
  {
    // force (axialForce)
    if(argv[0] == "force" || argv[0] == "forces" || argv[0] == "axialForce")
                return new XC::ElementResponse(this, 1, 0.0);

    else if(argv[0] == "defo" || argv[0] == "deformations" ||
                argv[0] == "deformation")
                return new XC::ElementResponse(this, 2, 0.0);

    // a material quantity
    else if(argv[0] == "material")
                return  setMaterialResponse(theMaterial,argv,1,eleInfo);

        else
                return 0;
  }

int XC::CorotTruss::getResponse(int responseID, Information &eleInfo)
  {
    switch (responseID)
      {
      case 1:
        return eleInfo.setDouble(getAxil());
      case 2:
        return eleInfo.setDouble(Lo * theMaterial->getStrain());
      default:
        return 0;
      }
  }

//! @brief Send members through the channel being passed as parameter.
int XC::CorotTruss::sendData(CommParameters &cp)
  {
    int res= CorotTrussBase::sendData(cp);
    res+= cp.sendBrokedPtr(theMaterial,getDbTagData(),BrokedPtrCommMetaData(18,19,20));
    res+= cp.sendDouble(A,getDbTagData(),CommMetaData(21));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::CorotTruss::recvData(const CommParameters &cp)
  {
    int res= CorotTrussBase::recvData(cp);
    theMaterial= cp.getBrokedMaterial(theMaterial,getDbTagData(),BrokedPtrCommMetaData(18,19,20));
    res+= cp.receiveDouble(A,getDbTagData(),CommMetaData(21));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::CorotTruss::sendSelf(CommParameters &cp)
  {
    inicComm(22);
    int res= sendData(cp);

    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "CorotTruss::sendSelf -- failed to send ID data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::CorotTruss::recvSelf(const CommParameters &cp)
  {
    inicComm(22);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "CorotTruss::recvSelf() - failed to recv ID data";
    else
      res+= recvData(cp);
    return res;
  }
