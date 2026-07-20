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

// $Revision: 1.1 $
// $Date: 2009/11/03 23:13:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/SingleFPSimple2d.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the implementation of the
// SingleFPSimple2d class.

#include "SingleFPSimple2d.h"

#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/recorder/response/ElementResponse.h"
#include "frictionModel/FrictionModel.h"
#include "material/uniaxial/UniaxialMaterial.h"
#include "domain/component/Parameter.h"

// initialize the class wide variables
XC::Matrix XC::SingleFPSimple2d::theMatrix(6,6);
XC::Vector XC::SingleFPSimple2d::theVector(6);

//! @brief Initialize initial stiffness matrix.
void XC::SingleFPSimple2d::initializeStiffnessMatrix(void)
  {
    kbInit.Zero();
    kbInit(0,0) = physicalProperties[0]->getInitialTangent();
    kbInit(1,1) = this->kInit();
    kbInit(2,2) = physicalProperties[1]->getInitialTangent();
  }

//! @brief Initialize element state after changint the value of its materials,
//! or the k0 member.
void XC::SingleFPSimple2d::initialize(void)
  {
    this->numDOF= 6;
    //! Check the material vector size.
    assert(materials.size()==2);
    // Initialize initial stiffness matrix.
    this->initializeStiffnessMatrix();
    
    // Initialize other variables.
    revertToStart();
  }

//! @brief Constructor.
XC::SingleFPSimple2d::SingleFPSimple2d(int tag, int Nd1, int Nd2,
				       const FrictionModel &thefrnmdl,
				       const double &reff,const double &kinit,
				       const std::vector<UniaxialMaterial *> &materials,
				       const Vector &_y, const Vector &_x,
				       const double &sDistI, const int &addRay,
				       int vert, const double &m,
				       const int &maxiter, const double &_tol,
				       const double &kfactuplift)
    : SimpleBearingBase(tag, ELE_TAG_SingleFPSimple2d,Nd1,Nd2,3,
			FrictionModels(thefrnmdl), kinit,
			materials, _y, _x,
			sDistI, addRay,
			m,
			maxiter,
			_tol,
			kfactuplift),
    Reff(reff), inclVertDisp(vert), ubPlastic(0.0), ubPlasticC(0.0)
  {
    this->initialize();
    load.reset(this->numDOF);
  }

//! @brief Default constructor.
XC::SingleFPSimple2d::SingleFPSimple2d()
  : SimpleBearingBase(ELE_TAG_SingleFPSimple2d,3),
    Reff(0.0), inclVertDisp(0), ubPlastic(0.0), ubPlasticC(0.0)
  {
    this->numDOF= 6;
    load.reset(this->numDOF);
  }

void XC::SingleFPSimple2d::setDomain(Domain *theDomain)
  {
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

	// length from center of dish to pivot point
	// Reff = R - h;
      }
  }   	 


int XC::SingleFPSimple2d::commitState()
  {
    ubPlasticC = ubPlastic; // commit trial history variables
    int errCode= this->frictionModels.commitState(); // commit friction model
    errCode+= physicalProperties.commitState(); // commit material models
    errCode+= this->SimpleBearingBase::commitState(); // commit the base class
    return errCode;
  }


int XC::SingleFPSimple2d::revertToLastCommit()
  {
    // DON'T call Element::revertToLastCommit() because
    // is a pure virtual method.
    int errCode= this->frictionModels.revertToLastCommit(); // revert friction model
    errCode+= physicalProperties.revertToLastCommit();// revert material models
    return errCode;
  }


int XC::SingleFPSimple2d::revertToStart()
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


int XC::SingleFPSimple2d::update(void)
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
    const double ubdotAbs = sqrt(pow(ubdot(1)/Reff*ub(1),2) + pow(ubdot(1),2));
    
    // 1) get axial force and stiffness in basic x-direction
    const double ub0Old = physicalProperties[0]->getStrain();
    if(inclVertDisp == 0)
      {
        physicalProperties[0]->setTrialStrain(ub(0), ubdot(0));
      }
    else
      {
        double ubVert = Reff - sqrt(pow(Reff,2) - pow(ub(1),2));
        physicalProperties[0]->setTrialStrain(ub(0)-ubVert, ubdot(0));
      }
    qb(0) = physicalProperties[0]->getStress();
    kb(0,0) = physicalProperties[0]->getTangent();
    
    // check for uplift
    if(qb(0) >= 0.0)
      {
        kb = kbInit;
        if(qb(0) > 0.0)
	  {
            physicalProperties[0]->setTrialStrain(ub0Old, 0.0);
            //kb = DBL_EPSILON*kbInit;
            kb = kFactUplift*kbInit;
            // update plastic displacement
            ubPlastic = ub(1);
            //opserr << "WARNING: SingleFPSimple2d::update() - element: "
            //    << this->getTag() << " - uplift encountered, scaling "
            //    << "stiffness matrix by: " << kFactUplift << endln;
	  }
        qb.Zero();
        return 0;
      }
    
    // 2) calculate shear force and stiffness in basic y-direction
    int iter = 0;
    double qb1Old = 0.0;
    FrictionModel *theFrnMdl= this->frictionModels[0];
    do  {
        // save old shear force
        qb1Old = qb(1);
        
        // get normal and friction (yield) forces
        const double N = -qb(0) + qb(1)/Reff*ub(1) - qb(1)*ul(2);
        theFrnMdl->setTrial(N, ubdotAbs);
        const double qYield = (theFrnMdl->getFrictionForce());
        
        // get stiffness of elastic component
        const double k2 = N/Reff;
        
        // get initial stiffness of hysteretic component
        const double k0 = this->kInit() - k2;
        
        // get trial shear force of hysteretic component
        const double qTrial = k0*(ub(1) - ubPlasticC);
        
        // compute yield criterion of hysteretic component
        const double qTrialNorm = fabs(qTrial);
        const double Y = qTrialNorm - qYield;
        
        // elastic step -> no updates required
        if (Y <= 0.0)
	  {
            // set shear force
            qb(1) = qTrial + k2*ub(1) - N*ul(2);
            // set tangent stiffness
            kb(1,1) = this->kInit();
	  }
        // plastic step -> return mapping
        else
	  {
            // compute consistency parameter
            const double dGamma = Y/k0;
            // update plastic displacement
            ubPlastic = ubPlasticC + dGamma*qTrial/qTrialNorm;
            // set shear force
            qb(1) = qYield*qTrial/qTrialNorm + k2*ub(1) - N*ul(2);
            // set tangent stiffness
            kb(1,1) = k2;
	  }
        iter++;
    } while ((fabs(qb(1)-qb1Old) >= tol) && (iter <= maxIter));
    
    // issue warning if iteration did not converge
    if(iter >= maxIter)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "WARNING: did not find the shear force after "
                  << iter
		  << " iterations and norm: "
		  << fabs(qb(1)-qb1Old) << std::endl;
        return -1;
      }
    
    // 3) get moment and stiffness in basic z-direction
    physicalProperties[1]->setTrialStrain(ub(2),ubdot(2));
    qb(2) = physicalProperties[1]->getStress();
    kb(2,2) = physicalProperties[1]->getTangent();
    
    return 0;
  }


const XC::Matrix& XC::SingleFPSimple2d::getTangentStiff(void) const
  {
    // zero the matrix
    theMatrix.Zero();
    
    // transform from basic to local system
    static Matrix kl(6,6);
    kl.addMatrixTripleProduct(0.0, Tlb, kb, 1.0);
    
    // add geometric stiffness to local stiffness
    kl(2,1) -= qb(0);
    kl(2,4) += qb(0);
    const double kGeo = qb(0)*(1.0 - shearDistI)*L;
    kl(2,5) -= kGeo;
    kl(5,5) += kGeo;
    
    // transform from local to global system
    theMatrix.addMatrixTripleProduct(0.0, Tgl, kl, 1.0);
    
    return theMatrix;
  }


const XC::Matrix& XC::SingleFPSimple2d::getInitialStiff(void) const
  {
    // zero the matrix
    theMatrix.Zero();
    
    // transform from basic to local system
    static Matrix kl(6,6);
    kl.addMatrixTripleProduct(0.0, Tlb, kbInit, 1.0);
    
    // transform from local to global system
    theMatrix.addMatrixTripleProduct(0.0, Tgl, kl, 1.0);
    
    return theMatrix;
  }

const XC::Matrix &XC::SingleFPSimple2d::getDamp() const
  {
    // zero the matrix
    theMatrix.Zero();
    
    // call base class to setup Rayleigh damping
    double factThis = 0.0;
    if (addRayleigh == 1)
      {
        theMatrix = this->Element::getDamp();
        factThis = 1.0;
      }
    
    // now add damping tangent from materials
    static Matrix cb(3,3);
    cb.Zero();
    cb(0,0) = physicalProperties[0]->getDampTangent();
    cb(2,2) = physicalProperties[1]->getDampTangent();
    
    // transform from basic to local system
    static Matrix cl(6,6);
    cl.addMatrixTripleProduct(0.0, Tlb, cb, 1.0);
    
    // transform from local to global system and add to cg
    theMatrix.addMatrixTripleProduct(factThis, Tgl, cl, 1.0);
    
    return theMatrix;
  }

const XC::Matrix& XC::SingleFPSimple2d::getMass(void) const
  {
	// zero the matrix
    theMatrix.Zero();

    // check for quick return
    if (mass == 0.0)
      { return theMatrix; }    

    const double m = 0.5*mass;
    for (int i=0; i<2; i++)
      {
	theMatrix(i,i)     = m;
	theMatrix(i+3,i+3) = m;
      }
	
    return theMatrix; 
  }


int XC::SingleFPSimple2d::addLoad(ElementalLoad *theLoad, double loadFactor)
  {  
    std::cerr << getClassName() << "::" << __FUNCTION__
	      <<"; load type unknown for element: "
	      << this->getTag() << std::endl;
    return -1;
  }


int XC::SingleFPSimple2d::addInertiaLoadToUnbalance(const Vector &accel)
  {
    // check for quick return
    if(mass == 0.0)
      {	return 0; }    

    // get R * accel from the nodes
    const Vector &Raccel1 = theNodes[0]->getRV(accel);
    const Vector &Raccel2 = theNodes[1]->getRV(accel);
	
    if (3 != Raccel1.Size() || 3 != Raccel2.Size())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; matrix and vector sizes are incompatible\n";
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

//! @brief Return the element resisting force.
const XC::Vector& XC::SingleFPSimple2d::getResistingForce(void) const
  {
    // zero the residual
    theVector.Zero();
    
    // determine resisting forces in local system
    static Vector ql(6);
    ql.addMatrixTransposeVector(0.0, Tlb, qb, 1.0);
    
    // add P-Delta moments to local forces
    double MpDelta1 = qb(0)*(ul(4)-ul(1));
    ql(2) += MpDelta1;
    double MpDelta2 = qb(0)*(1.0 - shearDistI)*L*ul(5);
    ql(2) -= MpDelta2;
    ql(5) += MpDelta2;
    
    // determine resisting forces in global system
    theVector.addMatrixTransposeVector(0.0, Tgl, ql, 1.0);
        
    return theVector;
  }

//! @brief Return the element resisting force including inertia effects.
const XC::Vector& XC::SingleFPSimple2d::getResistingForceIncInertia(void) const
  {	
    theVector = this->getResistingForce();
	
    // subtract external load
    theVector.addVector(1.0, load, -1.0);
    
    // add the damping forces if rayleigh damping
    if (addRayleigh == 1)
      {
	if(!rayFactors.nullValues())
	  theVector += this->getRayleighDampingForces();
      }
    
    // now include the mass portion
    if(mass != 0.0)
      {
	const Vector &accel1 = theNodes[0]->getTrialAccel();
	const Vector &accel2 = theNodes[1]->getTrialAccel();    
		
	const double m = 0.5*mass;
	for (int i=0; i<2; i++)
	  {
	    theVector(i)+= m * accel1(i);
	    theVector(i+3) += m * accel2(i);
	  }
      }
    return theVector;
  }

//! @brief Send members through the communicator argument.
int XC::SingleFPSimple2d::sendData(Communicator &comm)
  {
    int res= SimpleBearingBase::sendData(comm);
    res+= comm.sendDoubles(ubPlastic,ubPlasticC, Reff,getDbTagData(),CommMetaData(21));
    res+= comm.sendInt(inclVertDisp, getDbTagData(),CommMetaData(22));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::SingleFPSimple2d::recvData(const Communicator &comm)
  {
    int res= SimpleBearingBase::recvData(comm);
    res+= comm.receiveDoubles(ubPlastic,ubPlasticC, Reff,getDbTagData(),CommMetaData(21));
    res+= comm.receiveInt(inclVertDisp, getDbTagData(),CommMetaData(22));
    return res;
  }

int XC::SingleFPSimple2d::sendSelf(Communicator &comm)
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

int XC::SingleFPSimple2d::recvSelf(const Communicator &comm)
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


void XC::SingleFPSimple2d::Print(std::ostream &s, int flag) const
  {
    if (flag == 0)
      {
        // print everything
	s << "Element: " << this->getTag()
          << "  type: RJWatsonEQS2d  iNode: " << theNodes[0]->getTag()
          << "  jNode: " << theNodes[1]->getTag() << std::endl;
	frictionModels.Print(s, flag);
        s << std::endl 
          << "  Reff: " << Reff << "  kInit: " << this->kInit() << std::endl
          << "  Material ux: " << physicalProperties[0]->getTag() << std::endl
          << "  Material rz: " << physicalProperties[1]->getTag() << std::endl
	  << "  shearDistI: " << shearDistI << "  addRayleigh: " << addRayleigh 
          << "  mass: " << mass
	  << "  maxIter: " << maxIter << "  tol: " << tol << std::endl;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << std::endl;
      }
    else if (flag == 1)
      {
		// does nothing
      }
  }


XC::Response* XC::SingleFPSimple2d::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    Response *theResponse= nullptr;

//     output.tag("ElementOutput");
//     output.attr("eleType","SingleFPSimple2d");
//     output.attr("eleTag",this->getTag());
//     output.attr("node1",connectedExternalNodes[0]);
//     output.attr("node2",connectedExternalNodes[1]);

//     // global forces
//     if (strcmp(argv[0],"force") == 0 ||
//         strcmp(argv[0],"forces") == 0 ||
//         strcmp(argv[0],"globalForce") == 0 ||
//         strcmp(argv[0],"globalForces") == 0)
//     {
//         output.tag("ResponseType","Px_1");
//         output.tag("ResponseType","Py_1");
//         output.tag("ResponseType","Mz_1");
//         output.tag("ResponseType","Px_2");
//         output.tag("ResponseType","Py_2");
//         output.tag("ResponseType","Mz_2");

//         theResponse = new ElementResponse(this, 1, theVector);
//     }
//     // local forces
//     else if (strcmp(argv[0],"localForce") == 0 ||
//         strcmp(argv[0],"localForces") == 0)
//     {
//         output.tag("ResponseType","N_1");
//         output.tag("ResponseType","V_1");
//         output.tag("ResponseType","M_1");
//         output.tag("ResponseType","N_2");
//         output.tag("ResponseType","V_2");
//         output.tag("ResponseType","M_2");

//         theResponse = new ElementResponse(this, 2, theVector);
//     }
//     // basic forces
//     else if (strcmp(argv[0],"basicForce") == 0 ||
//         strcmp(argv[0],"basicForces") == 0)
//     {
//         output.tag("ResponseType","qb1");
//         output.tag("ResponseType","qb2");
//         output.tag("ResponseType","qb3");

//         theResponse = new ElementResponse(this, 3, Vector(3));
//     }
// 	// local displacements
//     else if (strcmp(argv[0],"localDisplacement") == 0 ||
//         strcmp(argv[0],"localDisplacements") == 0)
//     {
//         output.tag("ResponseType","ux_1");
//         output.tag("ResponseType","uy_1");
//         output.tag("ResponseType","rz_1");
//         output.tag("ResponseType","ux_2");
//         output.tag("ResponseType","uy_2");
//         output.tag("ResponseType","rz_2");
        
//         theResponse = new ElementResponse(this, 4, theVector);
//     }
// 	// basic displacements
//     else if (strcmp(argv[0],"deformation") == 0 ||
//         strcmp(argv[0],"deformations") == 0 || 
//         strcmp(argv[0],"basicDeformation") == 0 ||
//         strcmp(argv[0],"basicDeformations") == 0 ||
//         strcmp(argv[0],"basicDisplacement") == 0 ||
//         strcmp(argv[0],"basicDisplacements") == 0)
//     {
//         output.tag("ResponseType","ub1");
//         output.tag("ResponseType","ub2");
//         output.tag("ResponseType","ub3");

//         theResponse = new ElementResponse(this, 5, Vector(3));
//     }
//     // material output
//     else if (strcmp(argv[0],"material") == 0)  {
//         if (argc > 2)  {
//             int matNum = atoi(argv[1]);
//             if (matNum >= 1 && matNum <= 2)
//                 theResponse =  physicalProperties[matNum-1]->setResponse(&argv[2], argc-2, output);
//         }
//     }
//     // friction model output
//     else if (strcmp(argv[0],"frictionModel") == 0 || strcmp(argv[0],"frnMdl") == 0 ||
//         strcmp(argv[0],"frictionMdl") == 0 || strcmp(argv[0],"frnModel") == 0)  {
//             if (argc > 1)
//                 theResponse = theFrnMdl->setResponse(&argv[1], argc-1, output);
//     }

//     output.endTag(); // ElementOutput

    return theResponse;
}


int XC::SingleFPSimple2d::getResponse(int responseID, Information &eleInfo)
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
        theVector(2) += MpDelta1;
        MpDelta2 = qb(0)*(1.0 - shearDistI)*L*ul(5);
        theVector(2) -= MpDelta2;
        theVector(5) += MpDelta2;
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
void XC::SingleFPSimple2d::setUp()
  {
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();	
    const Vector xp = end2Crd - end1Crd;
    this->L = xp.Norm();
    
    if (L > DBL_EPSILON)
      {
        if (x.Size() == 0)
	  {
            x.resize(3);
            x(0) = xp(0);  x(1) = xp(1);  x(2) = 0.0;
            y.resize(3);
            y(0) = -x(1);  y(1) = x(0);  y(2) = 0.0;
	  }
	else if(onP0) 
	  {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING element: " << this->getTag() << std::endl
		      << "ignoring nodes and using specified "
		      << "local x vector to determine orientation\n";
	  }
      }
    // check that vectors for orientation are of correct size
    if (x.Size() != 3 || y.Size() != 3)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; element: " << this->getTag() << std::endl
		  << "incorrect dimension of orientation vectors\n";
        exit(-1);
      }
    
    // establish orientation of element for the transformation matrix
    // z = x cross yp
    Vector z(3);
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
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; element: " << this->getTag() << std::endl
		  << "invalid orientation vectors\n";
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

int XC::SingleFPSimple2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if (argv.size() < 1)
      return -1;

    if((argv[0]=="R") || (argv[0]=="Reff"))
      {
	param.setValue(Reff);
	return param.addObject(1,this);
      }
    else
      {
	FrictionModel *theFrnMdl= this->frictionModels[0];
	return theFrnMdl->setParameter(argv, param);
      }
  }

int XC::SingleFPSimple2d::updateParameter(int parameterID, Information &info)
  {
    switch(parameterID)
      {
      case 1:
	Reff = info.theDouble;
	return 0;
      default:
	return -1;
      }
  }
