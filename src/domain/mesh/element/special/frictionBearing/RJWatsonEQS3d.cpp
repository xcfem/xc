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

// Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
// Created: 12/13
// Revision: A
//
// Description: This file contains the implementation of the
// RJWatsonEQS3d class.

#include "RJWatsonEQS3d.h"

#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/recorder/response/ElementResponse.h"
#include "frictionModel/FrictionModel.h"
#include "material/uniaxial/UniaxialMaterial.h"

// initialize the class wide variables
XC::Matrix XC::RJWatsonEQS3d::theMatrix(12,12);
XC::Vector XC::RJWatsonEQS3d::theVector(12);

//! @brief Initialize initial stiffness matrix.
void XC::RJWatsonEQS3d::initializeStiffnessMatrix(void)
  {
    kbInit.Zero();
    kbInit(0,0) = physicalProperties[0]->getInitialTangent();
    kbInit(1,1) = k0 + physicalProperties[1]->getInitialTangent();
    kbInit(2,2) = k0 + physicalProperties[2]->getInitialTangent();
    kbInit(3,3) = physicalProperties[3]->getInitialTangent();
    kbInit(4,4) = physicalProperties[4]->getInitialTangent();
    kbInit(5,5) = physicalProperties[5]->getInitialTangent();
  }

//! @brief Initialize element state after changint the value of its materials,
//! or the k0 member.
void XC::RJWatsonEQS3d::initialize(void)
  {
    //! Check the material vector size.
    assert(materials.size()==6);
    // Initialize initial stiffness matrix.
    this->initializeStiffnessMatrix();
    
    // Initialize other variables.
    this->revertToStart();
  }

//! @brief Constructor.
XC::RJWatsonEQS3d::RJWatsonEQS3d(int tag, int Nd1, int Nd2,
				 const FrictionModel &thefrnmdl, double kInit,
				 const std::vector<UniaxialMaterial *> &mats,

				 const Vector &_y, const Vector &_x,
				 const double &sdI, const int &addRay,
				 const double &m,
				 const int &maxiter,
				 const double &_tol,
				 const double &kfactuplift)
  : SimpleBearingBase(tag, ELE_TAG_RJWatsonEQS3d, Nd1, Nd2, 6,
		      FrictionModels(thefrnmdl), kInit,
		      mats, _y, _x,
		      sdI, addRay,
		      m,
		      maxiter,
		      _tol,
		      kfactuplift),
    ubPlastic(2), ubPlasticC(2)

  {
    load.reset(12);
    this->initialize();
  }

//! @brief Default constructor.
XC::RJWatsonEQS3d::RJWatsonEQS3d()
  : SimpleBearingBase(ELE_TAG_RJWatsonEQS3d, 6),
    ubPlastic(2), ubPlasticC(2)
  { }


void XC::RJWatsonEQS3d::setDomain(Domain *theDomain)
  {
    // check Domain is not null - invoked when object removed from a domain
    if(theDomain)
      {
	SimpleBearingBase::setDomain(theDomain);
	
	// now determine the number of dof and the dimension
	const int dofNd1 = theNodes[0]->getNumberDOF();
	const int dofNd2 = theNodes[1]->getNumberDOF();

	// if differing dof at the ends - print a warning message
	if(dofNd1 != 6)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; node 1: "
		      << theNodes[0]->getTag()
		      << " has incorrect number of DOF (not 6)."
		      << std::endl;
	    return;
	  }
	if(dofNd2 != 6)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; node 2: "
		      << theNodes[1]->getTag()
		      << " has incorrect number of DOF (not 6)."
		      << std::endl;
	    return;
	  }

	// set up the transformation matrix for orientation
	this->setUp();
      }
  }


int XC::RJWatsonEQS3d::commitState()
  {
    int errCode= this->SimpleBearingBase::commitState(); // commit the base class.
    ubPlasticC = ubPlastic; // commit trial history variables.
    errCode+= this->frictionModels.commitState(); // commit friction model
    
    errCode+= physicalProperties.commitState();// commit material models
    return errCode;
  }


int XC::RJWatsonEQS3d::revertToLastCommit()
  {
    // DON'T call Element::revertToLastCommit() because
    // is a pure virtual method.
    int errCode = this->frictionModels.revertToLastCommit();// revert friction model
    errCode+= physicalProperties.revertToLastCommit();// revert material models
    return errCode;
  }


int XC::RJWatsonEQS3d::revertToStart()
  {
    int errCode= SimpleBearingBase::revertToStart();
    
    // reset trial history variables
    ub.Zero();
    ubPlastic.Zero();
    qb.Zero();
    
    // reset committed history variables
    ubPlasticC.Zero();
    
    // reset stiffness matrix in basic system
    kb= this->kbInit;
    
    // revert friction model
    errCode += this->frictionModels.revertToStart();
    
    errCode+= physicalProperties.revertToStart();// revert material models
    
    return errCode;
  }


int XC::RJWatsonEQS3d::update()
  {
    // get global trial displacements and velocities
    const Vector &dsp1 = theNodes[0]->getTrialDisp();
    const Vector &dsp2 = theNodes[1]->getTrialDisp();
    const Vector &vel1 = theNodes[0]->getTrialVel();
    const Vector &vel2 = theNodes[1]->getTrialVel();
    
    static Vector ug(12), ugdot(12), uldot(12), ubdot(6);
    for (int i=0; i<6; i++)
      {
        ug(i)   = dsp1(i);  ugdot(i)   = vel1(i);
        ug(i+6) = dsp2(i);  ugdot(i+6) = vel2(i);
    }
    
    // transform response from the global to the local system
    ul.addMatrixVector(0.0, Tgl, ug, 1.0);
    uldot.addMatrixVector(0.0, Tgl, ugdot, 1.0);
    
    // transform response from the local to the basic system
    ub.addMatrixVector(0.0, Tlb, ul, 1.0);
    ubdot.addMatrixVector(0.0, Tlb, uldot, 1.0);
    
    // get absolute velocity
    const double ubdotAbs = sqrt(pow(ubdot(1),2) + pow(ubdot(2),2));
    
    // 1) get axial force and stiffness in basic x-direction
    const double ub0Old = physicalProperties[0]->getStrain();
    physicalProperties[0]->setTrialStrain(ub(0), ubdot(0));
    qb(0) = physicalProperties[0]->getStress();
    kb(0,0) = physicalProperties[0]->getTangent();
    
    // check for uplift
    if (qb(0) >= 0.0)
      {
        physicalProperties[1]->setTrialStrain(ub(1), ubdot(1));
        physicalProperties[2]->setTrialStrain(ub(2), ubdot(2));
        // update plastic displacements
        ubPlastic(0) = ub(1);
        ubPlastic(1) = ub(2);
        // set basic forces
        qb.Zero();
        qb(1) = physicalProperties[1]->getStress();
        qb(2) = physicalProperties[2]->getStress();
        // set tangent stiffnesses
        kb = kbInit;
        if (qb(0) > 0.0)
	  {
            physicalProperties[0]->setTrialStrain(ub0Old, 0.0);
            kb = kFactUplift*kbInit;  // kb = DBL_EPSILON*kbInit;
            kb(1,1) = physicalProperties[1]->getTangent();
            kb(2,2) = physicalProperties[2]->getTangent();
            //std::cerr << "WARNING: XC::RJWatsonEQS3d::update() - element: "
            //    << this->getTag() << " - uplift encountered, scaling "
            //    << "stiffness matrix by: " << kFactUplift << std::endl;
	  }
        return 0;
      }
    
    // 2a) calculate sliding shear forces and stiffnesses in basic y- and z-direction
    int iter = 0;
    Vector qbOld(2);
    qb(1) -= physicalProperties[1]->getStress();
    qb(2) -= physicalProperties[2]->getStress();
    FrictionModel *theFrnMdl= this->frictionModels[0];
    do
      {
        // save old shear forces
        iter++;
        qbOld(0) = qb(1);
        qbOld(1) = qb(2);
        
        // get normal and friction (yield) forces
        double N = -qb(0) - qb(1)*ul(11) + qb(2)*ul(10);
        N = N > 0.0 ? N : 0.0;  // can not be negative
        theFrnMdl->setTrial(N, ubdotAbs);
        const double qYield = (theFrnMdl->getFrictionForce());
        
        // get trial shear forces of hysteretic component
        Vector qTrial(2);
        qTrial(0) = k0*(ub(1) - ubPlasticC(0));
        qTrial(1) = k0*(ub(2) - ubPlasticC(1));
        
        // compute yield criterion of hysteretic component
        const double qTrialNorm = qTrial.Norm();
        const double Y = qTrialNorm - qYield;
        
        // elastic step -> no updates required
        if (Y <= 0.0)
	  {
            // set shear forces
            qb(1) = qTrial(0) - N*ul(11);
            qb(2) = qTrial(1) + N*ul(10);
            // set tangent stiffnesses
            kb(1,1) = kb(2,2) = k0;
            kb(1,2) = kb(2,1) = 0.0;
	  }
        // plastic step -> return mapping
        else
	  {
            // compute consistency parameters
            const double dGamma = Y/k0;
            // update plastic displacements
            ubPlastic(0) = ubPlasticC(0) + dGamma*qTrial(0)/qTrialNorm;
            ubPlastic(1) = ubPlasticC(1) + dGamma*qTrial(1)/qTrialNorm;
            // set shear forces
            qb(1) = qYield*qTrial(0)/qTrialNorm - N*ul(11);
            qb(2) = qYield*qTrial(1)/qTrialNorm + N*ul(10);
            // set tangent stiffnesses
            const double D = pow(qTrialNorm,3);
            kb(1,1) = qYield*k0*qTrial(1)*qTrial(1)/D;
            kb(1,2) = kb(2,1) = -qYield*k0*qTrial(1)*qTrial(0)/D;
            kb(2,2) = qYield*k0*qTrial(0)*qTrial(0)/D;
	  }
      }
    while ((sqrt(pow(qb(1)-qbOld(0),2)+pow(qb(2)-qbOld(1),2)) >= tol) && (iter <= maxIter));
    
    // issue warning if iteration did not converge
    if (iter >= maxIter)
      {
        std::cerr << "WARNING: XC::RJWatsonEQS3d::update() - element: "
            << this->getTag() << " - did not find the shear force after "
            << iter << " iterations and norm: "
            << sqrt(pow(qb(1)-qbOld(0),2)+pow(qb(2)-qbOld(1),2)) << ".\n";
        return -1;
      }
    
    // 2b) add MER-spring contributions to shear forces in basic y- and z-direction
    physicalProperties[1]->setTrialStrain(ub(1), ubdot(1));
    physicalProperties[2]->setTrialStrain(ub(2), ubdot(2));
    qb(1) += physicalProperties[1]->getStress();
    qb(2) += physicalProperties[2]->getStress();
    kb(1,1) += physicalProperties[1]->getTangent();
    kb(2,2) += physicalProperties[2]->getTangent();
    
    // 3) get moment and stiffness in basic x-direction
    physicalProperties[3]->setTrialStrain(ub(3), ubdot(3));
    qb(3) = physicalProperties[3]->getStress();
    kb(3,3) = physicalProperties[3]->getTangent();
    
    // 4) get moment and stiffness in basic y-direction
    physicalProperties[4]->setTrialStrain(ub(4), ubdot(4));
    qb(4) = physicalProperties[4]->getStress();
    kb(4,4) = physicalProperties[4]->getTangent();
    
    // 5) get moment and stiffness in basic z-direction
    physicalProperties[5]->setTrialStrain(ub(5), ubdot(5));
    qb(5) = physicalProperties[5]->getStress();
    kb(5,5) = physicalProperties[5]->getTangent();
    
    return 0;
  }


const XC::Matrix& XC::RJWatsonEQS3d::getTangentStiff() const
  {
    // zero the matrix
    theMatrix.Zero();
    
    // transform from basic to local system
    static Matrix kl(12,12);
    kl.addMatrixTripleProduct(0.0, Tlb, kb, 1.0);
    
    // add geometric stiffness to local stiffness
    const double Ls = shearDistI*L;
    // add P-Delta moment stiffness terms
    kl(11,1) -= qb(0);
    kl(11,7) += qb(0);
    kl(5,5)  += qb(0)*Ls;
    kl(11,5) -= qb(0)*Ls;
    kl(10,2) += qb(0);
    kl(10,8) -= qb(0);
    kl(4,4)  += qb(0)*Ls;
    kl(10,4) -= qb(0)*Ls;
    // add V-Delta torsion stiffness terms
    kl(9,1)  += qb(2);
    kl(9,2)  -= qb(1);
    kl(9,7)  -= qb(2);
    kl(9,8)  += qb(1);
    kl(3,4)  -= qb(1)*Ls;
    kl(3,5)  -= qb(2)*Ls;
    kl(9,4)  += qb(1)*Ls;
    kl(9,5)  += qb(2)*Ls;
    
    // transform from local to global system
    theMatrix.addMatrixTripleProduct(0.0, Tgl, kl, 1.0);
    
    return theMatrix;
  }


const XC::Matrix& XC::RJWatsonEQS3d::getInitialStiff() const
  {
    // zero the matrix
    theMatrix.Zero();
    
    // transform from basic to local system
    static Matrix klInit(12,12);
    klInit.addMatrixTripleProduct(0.0, Tlb, kbInit, 1.0);
    
    // transform from local to global system
    theMatrix.addMatrixTripleProduct(0.0, Tgl, klInit, 1.0);
    
    return theMatrix;
  }


const XC::Matrix& XC::RJWatsonEQS3d::getDamp() const
  {
    // zero the matrix
    theMatrix.Zero();
    
    // call base class to setup Rayleigh damping
    double factThis = 0.0;
    if (addRayleigh == 1)
      {
        theMatrix = this->SimpleBearingBase::getDamp();
        factThis = 1.0;
      }
    
    // now add damping tangent from materials
    static Matrix cb(6,6);
    cb.Zero();
    cb(0,0) = physicalProperties[0]->getDampTangent();
    cb(1,1) = physicalProperties[1]->getDampTangent();
    cb(2,2) = physicalProperties[2]->getDampTangent();
    cb(3,3) = physicalProperties[3]->getDampTangent();
    cb(4,4) = physicalProperties[4]->getDampTangent();
    cb(5,5) = physicalProperties[5]->getDampTangent();
    
    // transform from basic to local system
    static Matrix cl(12,12);
    cl.addMatrixTripleProduct(0.0, Tlb, cb, 1.0);
    
    // transform from local to global system and add to cg
    theMatrix.addMatrixTripleProduct(factThis, Tgl, cl, 1.0);
    
    return theMatrix;
  }


const XC::Matrix& XC::RJWatsonEQS3d::getMass() const
  {
    // zero the matrix
    theMatrix.Zero();
    
    // check for quick return
    if (mass == 0.0)
      {
        return theMatrix;
      }    
    
    const double m = 0.5*mass;
    for (int i=0; i<3; i++)
      {
        theMatrix(i,i)     = m;
        theMatrix(i+6,i+6) = m;
      }
    
    return theMatrix; 
  }


int XC::RJWatsonEQS3d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr <<"XC::RJWatsonEQS3d::addLoad() - "
	      << "load type unknown for element: "
	      << this->getTag() << "."
              << std::endl;
    
    return -1;
  }


int XC::RJWatsonEQS3d::addInertiaLoadToUnbalance(const Vector &accel)
  {
    // check for quick return
    if (mass == 0.0)
      {
        return 0;
      }
    
    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    
    if (6 != Raccel1.Size() || 6 != Raccel2.Size())
      {
        std::cerr << "XC::RJWatsonEQS3d::addInertiaLoadToUnbalance() - "
		  << "matrix and vector sizes are incompatible.\n";
        return -1;
      }
    
    // want to add ( - fact * M R * accel ) to unbalance
    // take advantage of lumped mass matrix
    const double m = 0.5*mass;
    for (int i=0; i<3; i++)
      {
        load(i)   -= m * Raccel1(i);
        load(i+6) -= m * Raccel2(i);
      }
    
    return 0;
  }


const XC::Vector &XC::RJWatsonEQS3d::getResistingForce() const
  {
    // zero the residual
    theVector.Zero();
    
    // determine resisting forces in local system
    static Vector ql(12);
    ql.addMatrixTransposeVector(0.0, Tlb, qb, 1.0);
    
    // add P-Delta moments to local forces
    const double MpDelta1 = qb(0)*(ul(7)-ul(1));
    ql(11) += MpDelta1;
    const double MpDelta2 = qb(0)*shearDistI*L*ul(5);
    ql(5)  += MpDelta2;
    ql(11) -= MpDelta2;
    const double MpDelta3 = qb(0)*(ul(8)-ul(2));
    ql(10) -= MpDelta3;
    const double MpDelta4 = qb(0)*shearDistI*L*ul(4);
    ql(4)  += MpDelta4;
    ql(10) -= MpDelta4;
    
    // add V-Delta torsion to local forces
    const double Vdelta1 = qb(1)*(ul(8)-ul(2)) - qb(2)*(ul(7)-ul(1));
    ql(9) += Vdelta1;
    const double Vdelta2 = shearDistI*L*(qb(1)*ul(4) + qb(2)*ul(5));
    ql(3) -= Vdelta2;
    ql(9) += Vdelta2;
    
    // determine resisting forces in global system
    theVector.addMatrixTransposeVector(0.0, Tgl, ql, 1.0);
    
    return theVector;
  }


const XC::Vector &XC::RJWatsonEQS3d::getResistingForceIncInertia() const
  {
    // this already includes damping forces from materials
    theVector = this->getResistingForce();
    
    // subtract external load
    theVector.addVector(1.0, load, -1.0);
    
    // add the damping forces from rayleigh damping
    if (addRayleigh == 1)
      {
 	if(!rayFactors.nullValues())
	  theVector.addVector(1.0, this->getRayleighDampingForces(), 1.0);
      }
    
    // add inertia forces from element mass
    if(mass != 0.0)
      {
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();
        
        const double m = 0.5*mass;
        for (int i=0; i<3; i++)
	  {
            theVector(i)   += m * accel1(i);
            theVector(i+6) += m * accel2(i);
	  }
      } 
    return theVector;
  }

//! @brief Send members through the communicator argument.
int XC::RJWatsonEQS3d::sendData(Communicator &comm)
  {
    int res= SimpleBearingBase::sendData(comm);
    res+= comm.sendVector(ubPlastic,getDbTagData(),CommMetaData(21));
    res+= comm.sendVector(ubPlasticC,getDbTagData(),CommMetaData(22));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::RJWatsonEQS3d::recvData(const Communicator &comm)
  {
    int res= SimpleBearingBase::recvData(comm);
    res+= comm.receiveVector(ubPlastic,getDbTagData(),CommMetaData(21));
    res+= comm.receiveVector(ubPlasticC,getDbTagData(),CommMetaData(22));
    return res;
  }

int XC::RJWatsonEQS3d::sendSelf(Communicator &comm)
  {
    inicComm(23);

    int res= sendData(comm);

    const int dataTag= getDbTag();
    res += comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send ID data\n";
    return res;
  }

int XC::RJWatsonEQS3d::recvSelf(const Communicator &comm)
  {
    inicComm(23);

    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ID data\n";
    else
      res+= recvData(comm);
    return res;
  }


void XC::RJWatsonEQS3d::Print(std::ostream &s, int flag) const
  {
    if (flag == 0)
      {
        // print everything
        s << "Element: " << this->getTag() 
          << "  type: RJWatsonEQS3d  iNode: " << theNodes[0]->getTag()
          << "  jNode: " << theNodes[1]->getTag() << std::endl;
	frictionModels.Print(s, flag);
        s << std::endl
	  << " k0: " << k0 << std::endl
          << "  Material ux: " << physicalProperties[0]->getTag() << std::endl
          << "  Material uy: " << physicalProperties[1]->getTag() << std::endl
          << "  Material uz: " << physicalProperties[2]->getTag() << std::endl
          << "  Material rx: " << physicalProperties[3]->getTag() << std::endl
          << "  Material ry: " << physicalProperties[4]->getTag() << std::endl
          << "  Material rz: " << physicalProperties[5]->getTag() << std::endl
          << "  shearDistI: " << shearDistI << "  addRayleigh: "
            << addRayleigh << "  mass: " << mass << std::endl
          << "  maxIter: " << maxIter << "  tol: " << tol << std::endl;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << std::endl;
      }
  }


XC::Response* XC::RJWatsonEQS3d::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    Response *theResponse= nullptr;
    /*
    output.tag("ElementOutput");
    output.attr("eleType","RJWatsonEQS3d");
    output.attr("eleTag",this->getTag());
    output.attr("node1",connectedExternalNodes[0]);
    output.attr("node2",connectedExternalNodes[1]);
    
    // global forces
    if (strcmp(argv[0],"force") == 0 ||
        strcmp(argv[0],"forces") == 0 ||
        strcmp(argv[0],"globalForce") == 0 ||
        strcmp(argv[0],"globalForces") == 0)
    {
        output.tag("ResponseType","Px_1");
        output.tag("ResponseType","Py_1");
        output.tag("ResponseType","Pz_1");
        output.tag("ResponseType","Mx_1");
        output.tag("ResponseType","My_1");
        output.tag("ResponseType","Mz_1");
        output.tag("ResponseType","Px_2");
        output.tag("ResponseType","Py_2");
        output.tag("ResponseType","Pz_2");
        output.tag("ResponseType","Mx_2");
        output.tag("ResponseType","My_2");
        output.tag("ResponseType","Mz_2");
        
        theResponse = new ElementResponse(this, 1, theVector);
    }
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 ||
        strcmp(argv[0],"localForces") == 0)
    {
        output.tag("ResponseType","N_ 1");
        output.tag("ResponseType","Vy_1");
        output.tag("ResponseType","Vz_1");
        output.tag("ResponseType","T_1");
        output.tag("ResponseType","My_1");
        output.tag("ResponseType","Tz_1");
        output.tag("ResponseType","N_2");
        output.tag("ResponseType","Py_2");
        output.tag("ResponseType","Pz_2");
        output.tag("ResponseType","T_2");
        output.tag("ResponseType","My_2");
        output.tag("ResponseType","Mz_2");
        
        theResponse = new ElementResponse(this, 2, theVector);
    }
    // basic forces
    else if (strcmp(argv[0],"basicForce") == 0 ||
        strcmp(argv[0],"basicForces") == 0)
    {
        output.tag("ResponseType","qb1");
        output.tag("ResponseType","qb2");
        output.tag("ResponseType","qb3");
        output.tag("ResponseType","qb4");
        output.tag("ResponseType","qb5");
        output.tag("ResponseType","qb6");
        
        theResponse = new ElementResponse(this, 3, Vector(6));
    }
    // local displacements
    else if (strcmp(argv[0],"localDisplacement") == 0 ||
        strcmp(argv[0],"localDisplacements") == 0)
    {
        output.tag("ResponseType","ux_1");
        output.tag("ResponseType","uy_1");
        output.tag("ResponseType","uz_1");
        output.tag("ResponseType","rx_1");
        output.tag("ResponseType","ry_1");
        output.tag("ResponseType","rz_1");
        output.tag("ResponseType","ux_2");
        output.tag("ResponseType","uy_2");
        output.tag("ResponseType","uz_2");
        output.tag("ResponseType","rx_2");
        output.tag("ResponseType","ry_2");
        output.tag("ResponseType","rz_2");
        
        theResponse = new ElementResponse(this, 4, theVector);
    }
    // basic displacements
    else if (strcmp(argv[0],"deformation") == 0 ||
        strcmp(argv[0],"deformations") == 0 || 
        strcmp(argv[0],"basicDeformation") == 0 ||
        strcmp(argv[0],"basicDeformations") == 0 ||
        strcmp(argv[0],"basicDisplacement") == 0 ||
        strcmp(argv[0],"basicDisplacements") == 0)
    {
        output.tag("ResponseType","ub1");
        output.tag("ResponseType","ub2");
        output.tag("ResponseType","ub3");
        output.tag("ResponseType","ub4");
        output.tag("ResponseType","ub5");
        output.tag("ResponseType","ub6");
        
        theResponse = new ElementResponse(this, 5, Vector(6));
    }
    // material output
    else if (strcmp(argv[0],"material") == 0)  {
        if (argc > 2)  {
            int matNum = atoi(argv[1]);
            if (matNum >= 1 && matNum <= 6)
                theResponse = physicalProperties[matNum-1]->setResponse(&argv[2], argc-2, output);
        }
    }
    // friction model output
    else if (strcmp(argv[0],"frictionModel") == 0 || strcmp(argv[0],"frnMdl") == 0 ||
        strcmp(argv[0],"frictionMdl") == 0 || strcmp(argv[0],"frnModel") == 0)  {
            if (argc > 1)
                theResponse = theFrnMdl->setResponse(&argv[1], argc-1, output);
    }
    
    output.endTag(); // ElementOutput
    */
    return theResponse;
  }


int XC::RJWatsonEQS3d::getResponse(int responseID, Information &eleInfo)
  {
    double MpDelta1, MpDelta2, MpDelta3, MpDelta4, Vdelta1, Vdelta2;
    
    switch (responseID)
      {
      case 1:  // global forces
	return eleInfo.setVector(this->getResistingForce());
        
      case 2:  // local forces
	theVector.Zero();
	// determine resisting forces in local system
	theVector.addMatrixTransposeVector(0.0, Tlb, qb, 1.0);
	// add P-Delta moments
	MpDelta1 = qb(0)*(ul(7)-ul(1));
	theVector(11) += MpDelta1;
	MpDelta2 = qb(0)*shearDistI*L*ul(5);
	theVector(5)  += MpDelta2;
	theVector(11) -= MpDelta2;
	MpDelta3 = qb(0)*(ul(8)-ul(2));
	theVector(10) -= MpDelta3;
	MpDelta4 = qb(0)*shearDistI*L*ul(4);
	theVector(4)  += MpDelta4;
	theVector(10) -= MpDelta4;
	// add V-Delta torsion
	Vdelta1 = qb(1)*(ul(8)-ul(2)) - qb(2)*(ul(7)-ul(1));
	theVector(9)  += Vdelta1;
	Vdelta2 = shearDistI*L*(qb(1)*ul(4) + qb(2)*ul(5));
	theVector(3)  -= Vdelta2;
	theVector(9)  += Vdelta2;
	return eleInfo.setVector(theVector);
        
      case 3:  // basic forces
	return eleInfo.setVector(qb);
        
      case 4:  // local displacements
	return eleInfo.setVector(ul);
        
      case 5:  // basic displacements
        return eleInfo.setVector(ub);
        
      default:
        return -1;
      }
  }


// Establish the external nodes and set up the transformation matrix for orientation
void XC::RJWatsonEQS3d::setUp()
  {
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    const Vector xp= end2Crd - end1Crd;
    L = xp.Norm();
    
    if (L > DBL_EPSILON)
      {
        if (x.Size() == 0)
	  {
            x.resize(3);
            x = xp;
	  }
	else if (onP0)
	  {
            std::cerr << "WARNING XC::RJWatsonEQS3d::setUp() - "
		      << "element: " << this->getTag()
		      << " - ignoring nodes and using specified "
		      << "local x vector to determine orientation.\n";
	  }
      }
    // check that vectors for orientation are of correct size
    if(x.Size() != 3 || y.Size() != 3)
      {
        std::cerr << "XC::RJWatsonEQS3d::setUp() - "
		  << "element: " << this->getTag()
		  << " - incorrect dimension of orientation vectors.\n";
        exit(-1);
      }
    
    // establish orientation of element for the transformation matrix
    // z = x cross y
    static Vector z(3);
    z(0) = x(1)*y(2) - x(2)*y(1);
    z(1) = x(2)*y(0) - x(0)*y(2);
    z(2) = x(0)*y(1) - x(1)*y(0);
    
    // y = z cross x
    y(0) = z(1)*x(2) - z(2)*x(1);
    y(1) = z(2)*x(0) - z(0)*x(2);
    y(2) = z(0)*x(1) - z(1)*x(0);
    
    // compute length(norm) of vectors
    const double xn = x.Norm();
    const double yn = y.Norm();
    const double zn = z.Norm();
    
    // check valid x and y vectors, i.e. not parallel and of zero length
    if (xn == 0 || yn == 0 || zn == 0)
      {
        std::cerr << "XC::RJWatsonEQS3d::setUp() - "
            << "element: " << this->getTag()
            << " - invalid orientation vectors.\n";
        exit(-1);
      }
    
    // create transformation matrix from global to local system
    Tgl.Zero();
    Tgl(0,0) = Tgl(3,3) = Tgl(6,6) = Tgl(9,9)   = x(0)/xn;
    Tgl(0,1) = Tgl(3,4) = Tgl(6,7) = Tgl(9,10)  = x(1)/xn;
    Tgl(0,2) = Tgl(3,5) = Tgl(6,8) = Tgl(9,11)  = x(2)/xn;
    Tgl(1,0) = Tgl(4,3) = Tgl(7,6) = Tgl(10,9)  = y(0)/yn;
    Tgl(1,1) = Tgl(4,4) = Tgl(7,7) = Tgl(10,10) = y(1)/yn;
    Tgl(1,2) = Tgl(4,5) = Tgl(7,8) = Tgl(10,11) = y(2)/yn;
    Tgl(2,0) = Tgl(5,3) = Tgl(8,6) = Tgl(11,9)  = z(0)/zn;
    Tgl(2,1) = Tgl(5,4) = Tgl(8,7) = Tgl(11,10) = z(1)/zn;
    Tgl(2,2) = Tgl(5,5) = Tgl(8,8) = Tgl(11,11) = z(2)/zn;
    
    // create transformation matrix from local to basic system (linear)
    Tlb.Zero();
    Tlb(0,0) = Tlb(1,1) = Tlb(2,2) = Tlb(3,3) = Tlb(4,4) = Tlb(5,5) = -1.0;
    Tlb(0,6) = Tlb(1,7) = Tlb(2,8) = Tlb(3,9) = Tlb(4,10) = Tlb(5,11) = 1.0;
    Tlb(1,5) = -shearDistI*L;
    Tlb(1,11) = -(1.0 - shearDistI)*L;
    Tlb(2,4) = -Tlb(1,5);
    Tlb(2,10) = -Tlb(1,11);
  } 
