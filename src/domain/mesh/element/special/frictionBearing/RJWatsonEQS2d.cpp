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
// RJWatsonEQS2d class.

#include "RJWatsonEQS2d.h"

#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/recorder/response/ElementResponse.h"
#include "material/uniaxial/UniaxialMaterial.h"

// initialize the class wide variables
XC::Matrix XC::RJWatsonEQS2d::theMatrix(6,6);
XC::Vector XC::RJWatsonEQS2d::theVector(6);

//! @brief Initialize initial stiffness matrix.
void XC::RJWatsonEQS2d::initializeStiffnessMatrix(void)
  {
    kbInit.Zero();
    kbInit(0,0)= physicalProperties[0]->getInitialTangent();
    kbInit(1,1)= this->k0 + physicalProperties[1]->getInitialTangent();
    kbInit(2,2)= physicalProperties[2]->getInitialTangent();
  }

//! @brief Initialize element state after changint the value of its materials,
//! or the k0 member.
void XC::RJWatsonEQS2d::initialize(void)
  {
    //! Check the material vector size.
    assert(materials.size()==2);
    // Initialize initial stiffness matrix.
    this->initializeStiffnessMatrix();
    
    // Initialize other variables.
    revertToStart();
  }

//! @brief Constructor.
XC::RJWatsonEQS2d::RJWatsonEQS2d(int tag, int Nd1, int Nd2,
				 const FrictionModel &thefrnmdl, double kInit,
				 const std::vector<UniaxialMaterial *> &mats,

				 const Vector &_y, const Vector &_x,
				 const double &sdI, const int &addRay,
				 const double &m,
				 const int &maxiter,
				 const double &_tol,
				 const double &kfactuplift)
    : SimpleBearingBase(tag, ELE_TAG_RJWatsonEQS2d,Nd1,Nd2,3,
			FrictionModels(thefrnmdl), kInit,
			mats, _y, _x,
			sdI, addRay,
			m,
			maxiter,
			_tol,
			kfactuplift),
    ubPlastic(0.0), ubPlasticC(0.0)
  {
    load.reset(6);
    this->initialize();
  }

//! @brief Default constructor.
XC::RJWatsonEQS2d::RJWatsonEQS2d()
  : SimpleBearingBase(ELE_TAG_RJWatsonEQS2d,3),
    ubPlastic(0.0), ubPlasticC(0.0)
  {
    load.reset(6);
  }


void XC::RJWatsonEQS2d::setDomain(Domain *theDomain)
  {
    // check Domain is not null - invoked when object removed from a domain
    if(theDomain)
      {
	// call the base class method
	SimpleBearingBase::setDomain(theDomain);

	// now determine the number of dof and the dimension
	const int dofNd1 = theNodes[0]->getNumberDOF();
	const int dofNd2 = theNodes[1]->getNumberDOF();	

	// if differing dof at the ends - print a warning message
	if(dofNd1 != 3)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; node 1: "
		      << theNodes[0]->getTag()
		      << " has incorrect number of DOF (not 3)."
		      << std::endl;
	  }
	if (dofNd2 != 3)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; node 2: "
		      << theNodes[1]->getTag()
		      << " has incorrect number of DOF (not 3)."
		      << std::endl;
	  }
	// set up the transformation matrix for orientation
	this->setUp();
      }
  }

int XC::RJWatsonEQS2d::commitState()
  {
    // commit the base class
    int errCode = this->SimpleBearingBase::commitState();
    
    ubPlasticC = ubPlastic; // commit trial history variables
    errCode += this->frictionModels.commitState(); // commit friction model
    errCode+= physicalProperties.commitState(); // commit material models
    return errCode;
  }


int XC::RJWatsonEQS2d::revertToLastCommit()
  {
    // DON'T call Element::revertToLastCommit() because
    // is a pure virtual method.
    int errCode= this->frictionModels.revertToLastCommit(); // revert friction model
    errCode+= physicalProperties.revertToLastCommit();// revert material models
    return errCode;
  }


int XC::RJWatsonEQS2d::revertToStart()
  {
    int errCode= SimpleBearingBase::revertToStart(); // reset parent object.
    
    // reset trial history variables
    ub.Zero();
    ubPlastic = 0.0;
    qb.Zero();
    
    ubPlasticC = 0.0; // reset committed history variables.
    kb = this->kbInit; // reset stiffness matrix in basic system.
    errCode += this->frictionModels.revertToStart(); // revert friction model.
    errCode+= physicalProperties.revertToStart(); // revert material models.
    
    return errCode;
  }


int XC::RJWatsonEQS2d::update()
  {
    // get global trial displacements and velocities
    const Vector &dsp1 = theNodes[0]->getTrialDisp();
    const Vector &dsp2 = theNodes[1]->getTrialDisp();
    const Vector &vel1 = theNodes[0]->getTrialVel();
    const Vector &vel2 = theNodes[1]->getTrialVel();
    
    static Vector ug(6), ugdot(6), uldot(6), ubdot(3);
    for (int i=0; i<3; i++)
      {
        ug(i)   = dsp1(i);  ugdot(i)   = vel1(i);
        ug(i+3) = dsp2(i);  ugdot(i+3) = vel2(i);
      }
    
    // transform response from the global to the local system
    ul.addMatrixVector(0.0, Tgl, ug, 1.0);
    uldot.addMatrixVector(0.0, Tgl, ugdot, 1.0);
    
    // transform response from the local to the basic system
    ub.addMatrixVector(0.0, Tlb, ul, 1.0);
    ubdot.addMatrixVector(0.0, Tlb, uldot, 1.0);
    
    // get absolute velocity
    const double ubdotAbs = fabs(ubdot(1));
    
    // 1) get axial force and stiffness in basic x-direction
    const double ub0Old = physicalProperties[0]->getStrain();
    physicalProperties[0]->setTrialStrain(ub(0), ubdot(0));
    qb(0) = physicalProperties[0]->getStress();
    kb(0,0) = physicalProperties[0]->getTangent();
    
    // check for uplift
    if(qb(0) >= 0.0)
      {
        physicalProperties[1]->setTrialStrain(ub(1), ubdot(1));
        // update plastic displacements
        ubPlastic = ub(1);
        // set basic forces
        qb.Zero();
        qb(1) = physicalProperties[1]->getStress();
        // set tangent stiffnesses
        kb = kbInit;
        if (qb(0) > 0.0)
	  {
            physicalProperties[0]->setTrialStrain(ub0Old, 0.0);
            kb = kFactUplift*kbInit;  // kb = DBL_EPSILON*kbInit;
            kb(1,1) = physicalProperties[1]->getTangent();
            //std::cerr << "WARNING: RJWatsonEQS3d::update() - element: "
            //    << this->getTag() << " - uplift encountered, scaling "
            //    << "stiffness matrix by: " << kFactUplift << endln;
	  }
        return 0;
      }

    // 2a) calculate sliding shear forces and stiffnesses in basic y-direction
    int iter = 0;
    double qb1Old = 0.0;
    qb(1) -= physicalProperties[1]->getStress();
    FrictionModel *theFrnMdl= this->frictionModels[0];
    do
      {
	// save old shear force
	iter++;
	qb1Old = qb(1);
        
        // get normal and friction (yield) forces
        double N = -qb(0) - qb(1)*ul(5);
        N = N > 0.0 ? N : 0.0;  // can not be negative
        theFrnMdl->setTrial(N, ubdotAbs);
        const double qYield = (theFrnMdl->getFrictionForce());
        
        // get trial shear force of hysteretic component
        const double qTrial = k0*(ub(1) - ubPlasticC);
        
        // compute yield criterion of hysteretic component
        const double qTrialNorm = fabs(qTrial);
        const double Y = qTrialNorm - qYield;
        
        // elastic step -> no updates required
        if (Y <= 0.0)
	  {
            // set shear force
            qb(1) = qTrial - N*ul(5);
            // set tangent stiffness
            kb(1,1) = k0;
	  }
        // plastic step -> return mapping
        else
	  {
            // compute consistency parameter
            const double dGamma = Y/k0;
            // update plastic displacement
            ubPlastic = ubPlasticC + dGamma*qTrial/qTrialNorm;
            // set shear force
            qb(1) = qYield*qTrial/qTrialNorm - N*ul(5);
            // set tangent stiffness
            kb(1,1) = 0.0;
	  }
      }
    while ((fabs(qb(1)-qb1Old) >= tol) && (iter <= maxIter));
    
    // issue warning if iteration did not converge
    if(iter >= maxIter)
      {
        std::cerr << "WARNING: XC::RJWatsonEQS2d::update() - element: "
		  << this->getTag() << " - did not find the shear force after "
		  << iter
		  << " iterations and norm: " << fabs(qb(1)-qb1Old) << "."
		  << std::endl;
        return -1;
      }
    
    // 2b) add MER-spring contributions to shear forces in basic y-direction
    physicalProperties[1]->setTrialStrain(ub(1), ubdot(1));
    qb(1) += physicalProperties[1]->getStress();
    kb(1,1) += physicalProperties[1]->getTangent();
    
    // 3) get moment and stiffness in basic z-direction
    physicalProperties[2]->setTrialStrain(ub(2), ubdot(2));
    qb(2) = physicalProperties[2]->getStress();
    kb(2,2) = physicalProperties[2]->getTangent();
    
    return 0;
  }


const XC::Matrix& XC::RJWatsonEQS2d::getTangentStiff() const
  {
    // zero the matrix
    theMatrix.Zero();
    
    // transform from basic to local system
    static Matrix kl(6,6);
    kl.addMatrixTripleProduct(0.0, Tlb, kb, 1.0);
    
    // add geometric stiffness to local stiffness
    kl(5,1) -= qb(0);
    kl(5,4) += qb(0);
    double kGeo = qb(0)*shearDistI*L;
    kl(2,2) += kGeo;
    kl(5,2) -= kGeo;
    
    // transform from local to global system
    theMatrix.addMatrixTripleProduct(0.0, Tgl, kl, 1.0);
    
    return theMatrix;
  }


const XC::Matrix& XC::RJWatsonEQS2d::getInitialStiff() const
  {
    // zero the matrix
    theMatrix.Zero();
    
    // transform from basic to local system
    static Matrix klInit(6,6);
    klInit.addMatrixTripleProduct(0.0, Tlb, kbInit, 1.0);
    
    // transform from local to global system
    theMatrix.addMatrixTripleProduct(0.0, Tgl, klInit, 1.0);
    
    return theMatrix;
  }  


const XC::Matrix& XC::RJWatsonEQS2d::getDamp() const
  {
    // zero the matrix
    theMatrix.Zero();
    
    // call base class to setup Rayleigh damping
    double factThis = 0.0;
    if(addRayleigh == 1)
      {
        theMatrix = this->SimpleBearingBase::getDamp();
        factThis = 1.0;
      }
    
    // now add damping tangent from materials
    static Matrix cb(3,3);
    cb.Zero();
    cb(0,0) = physicalProperties[0]->getDampTangent();
    cb(1,1) = physicalProperties[1]->getDampTangent();
    cb(2,2) = physicalProperties[2]->getDampTangent();
    
    // transform from basic to local system
    static Matrix cl(6,6);
    cl.addMatrixTripleProduct(0.0, Tlb, cb, 1.0);
    
    // transform from local to global system and add to cg
    theMatrix.addMatrixTripleProduct(factThis, Tgl, cl, 1.0);
    
    return theMatrix;
  }


const XC::Matrix& XC::RJWatsonEQS2d::getMass() const
  {
    // zero the matrix
    theMatrix.Zero();
    
    // check for quick return
    if(mass == 0.0)
      {
        return theMatrix;
      }
    
    const double m = 0.5*mass;
    for (int i=0; i<2; i++)
      {
        theMatrix(i,i)     = m;
        theMatrix(i+3,i+3) = m;
      }
    
    return theMatrix; 
  }


int XC::RJWatsonEQS2d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    std::cerr <<"XC::RJWatsonEQS2d::addLoad() - "
	      << "load type unknown for element: "
	      << this->getTag() << "."
	      << std::endl;
    
    return -1;
  }


int XC::RJWatsonEQS2d::addInertiaLoadToUnbalance(const Vector &accel)
  {
    // check for quick return
    if (mass == 0.0)
      {
        return 0;
      }    
    
    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
    
    if (3 != Raccel1.Size() || 3 != Raccel2.Size())
      {
        std::cerr << "XC::RJWatsonEQS2d::addInertiaLoadToUnbalance() - "
		  << "matrix and vector sizes are incompatible."
		  << std::endl;
        return -1;
      }
    
    // want to add ( - fact * M R * accel ) to unbalance
    // take advantage of lumped mass matrix
    const double m = 0.5*mass;
    for (int i=0; i<2; i++)
      {
        load(i)   -= m * Raccel1(i);
        load(i+3) -= m * Raccel2(i);
      }
    
    return 0;
  }


const XC::Vector& XC::RJWatsonEQS2d::getResistingForce() const
  {
    // zero the residual
    theVector.Zero();
    
    // determine resisting forces in local system
    static Vector ql(6);
    ql.addMatrixTransposeVector(0.0, Tlb, qb, 1.0);
    
    // add P-Delta moments to local forces
    const double MpDelta1 = qb(0)*(ul(4)-ul(1));
    ql(5) += MpDelta1;
    const double MpDelta2 = qb(0)*shearDistI*L*ul(2);
    ql(2) += MpDelta2;
    ql(5) -= MpDelta2;
    
    // determine resisting forces in global system
    theVector.addMatrixTransposeVector(0.0, Tgl, ql, 1.0);
    
    return theVector;
  }


const XC::Vector& XC::RJWatsonEQS2d::getResistingForceIncInertia() const
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
    if (mass != 0.0)  {
        const Vector &accel1 = theNodes[0]->getTrialAccel();
        const Vector &accel2 = theNodes[1]->getTrialAccel();
        
        double m = 0.5*mass;
        for (int i=0; i<2; i++)  {
            theVector(i)   += m * accel1(i);
            theVector(i+3) += m * accel2(i);
        }
    }
    
    return theVector;
  }

//! @brief Send members through the communicator argument.
int XC::RJWatsonEQS2d::sendData(Communicator &comm)
  {
    int res= SimpleBearingBase::sendData(comm);
    res+= comm.sendDoubles(ubPlastic,ubPlasticC, getDbTagData(),CommMetaData(21));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::RJWatsonEQS2d::recvData(const Communicator &comm)
  {
    int res= SimpleBearingBase::recvData(comm);
    res+= comm.receiveDoubles(ubPlastic,ubPlasticC, getDbTagData(),CommMetaData(21));
    return res;
  }

int XC::RJWatsonEQS2d::sendSelf(Communicator &comm)
  {
    inicComm(22);

    int res= sendData(comm);

    const int dataTag= getDbTag();
    res += comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send ID data\n";
    return res;
  }

int XC::RJWatsonEQS2d::recvSelf(const Communicator &comm)
  {
    inicComm(22);

    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ID data\n";
    else
      res+= recvData(comm);
    return res;
  }


void XC::RJWatsonEQS2d::Print(std::ostream &s, int flag) const
  {
    if (flag == 0)
      {
        // print everything
        s << "Element: " << this->getTag()
          << "  type: RJWatsonEQS2d  iNode: " << theNodes[0]->getTag()
          << "  jNode: " << theNodes[1]->getTag() << std::endl;
	frictionModels.Print(s, flag);
        s << std::endl
          << "  k0: " << k0 << std::endl
          << "  Material ux: " << physicalProperties[0]->getTag() << std::endl
          << "  Material uy: " << physicalProperties[1]->getTag() << std::endl
          << "  Material rz: " << physicalProperties[2]->getTag() << std::endl
          << "  shearDistI: " << shearDistI << "  addRayleigh: "
	  << addRayleigh << "  mass: " << mass << std::endl
          << "  maxIter: " << maxIter << "  tol: " << tol << std::endl;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << std::endl;
      }
  }


XC::Response* XC::RJWatsonEQS2d::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    Response *theResponse = nullptr;

    /*
    output.tag("ElementOutput");
    output.attr("eleType","RJWatsonEQS2d");
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
        output.tag("ResponseType","Mz_1");
        output.tag("ResponseType","Px_2");
        output.tag("ResponseType","Py_2");
        output.tag("ResponseType","Mz_2");
        
        theResponse = new ElementResponse(this, 1, theVector);
    }
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 ||
        strcmp(argv[0],"localForces") == 0)
    {
        output.tag("ResponseType","N_1");
        output.tag("ResponseType","V_1");
        output.tag("ResponseType","M_1");
        output.tag("ResponseType","N_2");
        output.tag("ResponseType","V_2");
        output.tag("ResponseType","M_2");
        
        theResponse = new ElementResponse(this, 2, theVector);
    }
    // basic forces
    else if (strcmp(argv[0],"basicForce") == 0 ||
        strcmp(argv[0],"basicForces") == 0)
    {
        output.tag("ResponseType","qb1");
        output.tag("ResponseType","qb2");
        output.tag("ResponseType","qb3");
        
        theResponse = new ElementResponse(this, 3, Vector(3));
    }
    // local displacements
    else if (strcmp(argv[0],"localDisplacement") == 0 ||
        strcmp(argv[0],"localDisplacements") == 0)
    {
        output.tag("ResponseType","ux_1");
        output.tag("ResponseType","uy_1");
        output.tag("ResponseType","rz_1");
        output.tag("ResponseType","ux_2");
        output.tag("ResponseType","uy_2");
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
        
        theResponse = new ElementResponse(this, 5, Vector(3));
    }
    // material output
    else if (strcmp(argv[0],"material") == 0)  {
        if (argc > 2)  {
            int matNum = atoi(argv[1]);
            if (matNum >= 1 && matNum <= 3)
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


int XC::RJWatsonEQS2d::getResponse(int responseID, Information &eleInfo)
  {
    double MpDelta1, MpDelta2;
    
    switch (responseID)
      {
      case 1:  // global forces
        return eleInfo.setVector(this->getResistingForce());
        
      case 2:  // local forces
        theVector.Zero();
        // determine resisting forces in local system
        theVector.addMatrixTransposeVector(0.0, Tlb, qb, 1.0);
        // add P-Delta moments
        MpDelta1 = qb(0)*(ul(4)-ul(1));
        theVector(5) += MpDelta1;
        MpDelta2 = qb(0)*shearDistI*L*ul(2);
        theVector(2) += MpDelta2;
        theVector(5) -= MpDelta2;
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


// establish the external nodes and set up the transformation matrix for orientation
void XC::RJWatsonEQS2d::setUp()
  {
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();	
    const Vector xp = end2Crd - end1Crd;
    L = xp.Norm();
    
    if (L > DBL_EPSILON)  {
        if (x.Size() == 0)  {
            x.resize(3);
            x(0) = xp(0);  x(1) = xp(1);  x(2) = 0.0;
            y.resize(3);
            y(0) = -x(1);  y(1) = x(0);  y(2) = 0.0;
        } else if (onP0)  {
            std::cerr << "WARNING XC::RJWatsonEQS2d::setUp() - " 
                << "element: " << this->getTag()
                << " - ignoring nodes and using specified "
                << "local x vector to determine orientation."
		  << std::endl;
        }
    }
    // check that vectors for orientation are of correct size
    if (x.Size() != 3 || y.Size() != 3)  {
        std::cerr << "XC::RJWatsonEQS2d::setUp() - "
            << "element: " << this->getTag()
            << " - incorrect dimension of orientation vectors."
		  << std::endl;
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
    double xn = x.Norm();
    double yn = y.Norm();
    double zn = z.Norm();
    
    // check valid x and y vectors, i.e. not parallel and of zero length
    if (xn == 0 || yn == 0 || zn == 0)  {
        std::cerr << "XC::RJWatsonEQS2d::setUp() - "
            << "element: " << this->getTag()
            << " - invalid orientation vectors."
		  << std::endl;
        exit(-1);
    }
    
    // create transformation matrix from global to local system
    Tgl.Zero();
    Tgl(0,0) = Tgl(3,3) = x(0)/xn;
    Tgl(0,1) = Tgl(3,4) = x(1)/xn;
    Tgl(1,0) = Tgl(4,3) = y(0)/yn;
    Tgl(1,1) = Tgl(4,4) = y(1)/yn;
    Tgl(2,2) = Tgl(5,5) = z(2)/zn;
    
    // create transformation matrix from local to basic system (linear)
    Tlb.Zero();
    Tlb(0,0) = Tlb(1,1) = Tlb(2,2) = -1.0;
    Tlb(0,3) = Tlb(1,4) = Tlb(2,5) = 1.0;
    Tlb(1,2) = -shearDistI*L;
    Tlb(1,5) = -(1.0 - shearDistI)*L;
  } 
