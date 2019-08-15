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

// $Revision: 1.2 $
// $Date: 2009/11/03 23:12:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/FlatSliderSimple2d.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the implementation of the
// FlatSliderSimple2d class.

#include "FlatSliderSimple2d.h"

#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/recorder/response/ElementResponse.h"
#include "frictionModel/FrictionModel.h"
#include "material/uniaxial/UniaxialMaterial.h"

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cstring>


// initialize the class wide variables
XC::Matrix XC::FlatSliderSimple2d::theMatrix(6,6);
XC::Vector XC::FlatSliderSimple2d::theVector(6);

XC::FlatSliderSimple2d::FlatSliderSimple2d(int tag, int Nd1, int Nd2,
					   FrictionModel &thefrnmdl, double _uy,const std::vector<UniaxialMaterial *> &materials,
    const Vector _y, const Vector _x, double m, int maxiter, double _tol)
  : FrictionElementBase(tag, ELE_TAG_FlatSliderSimple2d,Nd1,Nd2,3,thefrnmdl,UniaxialMatPhysicalProperties(materials),_uy,_x,_y,m,maxiter,tol),
    ubPlastic(0.0), ubPlasticC(0.0)
  {
    load.reset(6);
    assert(materials.size()==2);
    // initialize initial stiffness matrix
    kbInit.Zero();
    kbInit(0,0) = physicalProperties[0]->getInitialTangent();
    kbInit(1,1) = kbInit(0,0)*DBL_EPSILON;
    kbInit(2,2) = physicalProperties[1]->getInitialTangent();
    
    // initialize other variables
    revertToStart();
  }


XC::FlatSliderSimple2d::FlatSliderSimple2d()
  : FrictionElementBase(ELE_TAG_FlatSliderSimple2d,3),
    ubPlastic(0.0), ubPlasticC(0.0)
  {load.reset(6);}


int XC::FlatSliderSimple2d::getNumDOF() 
  { return 6; }


void XC::FlatSliderSimple2d::setDomain(Domain *theDomain)
  {
    FrictionElementBase::setDomain(theDomain);
	
    // now determine the number of dof and the dimension    
    const int dofNd1 = theNodes[0]->getNumberDOF();
    const int dofNd2 = theNodes[1]->getNumberDOF();	
	
    // if differing dof at the ends - print a warning message
    if(dofNd1 != 3)
      {
	std::cerr << "FlatSliderSimple2d::setDomain() - node 1: "
		<< " has incorrect number of DOF (not 3)\n";
	return;
      }
    if(dofNd2 != 3)
      {
	std::cerr << "FlatSliderSimple2d::setDomain() - node 2: "
		<< " has incorrect number of DOF (not 3)\n";
	return;
      }
	
    // set up the transformation matrix for orientation
    this->setUp();
  }


int XC::FlatSliderSimple2d::commitState()
  {
    int errCode = 0;
      
    ubPlasticC = ubPlastic;// commit trial history variables
    errCode += theFrnMdl->commitState();// commit friction model
    
    errCode += physicalProperties.commitState();// commit material models
    return errCode;
  }


int XC::FlatSliderSimple2d::revertToLastCommit()
  {
    int errCode = 0;
    
    errCode += theFrnMdl->revertToLastCommit();// revert friction model    
    errCode += physicalProperties.revertToLastCommit();// revert material models
    return errCode;
  }


int XC::FlatSliderSimple2d::revertToStart()
{     
    int errCode = 0;
    
    // reset trial history variables
    ub.Zero();
    ubPlastic = 0.0;
    qb.Zero();
    
    // reset committed history variables
    ubPlasticC = 0.0;
    
    // reset stiffness matrix in basic system
    kb = kbInit;
    
    // revert friction model
    errCode += theFrnMdl->revertToStart();
    
    
    errCode += physicalProperties.revertToStart();// revert material models
    return errCode;
  }


int XC::FlatSliderSimple2d::update()
{
    // get global trial displacements and velocities
    const Vector &dsp1 = theNodes[0]->getTrialDisp();
    const Vector &dsp2 = theNodes[1]->getTrialDisp();
    const Vector &vel1 = theNodes[0]->getTrialVel();
    const Vector &vel2 = theNodes[1]->getTrialVel();
    
    static Vector ug(6), ugdot(6), uldot(6), ubdot(3);
    for (int i=0; i<3; i++)  {
        ug(i)   = dsp1(i);  ugdot(i)   = vel1(i);
        ug(i+3) = dsp2(i);  ugdot(i+3) = vel2(i);
    }
    
    // transform response from the global to the local system
    ul = Tgl*ug;
    uldot = Tgl*ugdot;
    
    // transform response from the local to the basic system
    ub = Tlb*ul;
    ubdot = Tlb*uldot;
    
    // get absolute velocity
    double ubdotAbs = ubdot(1);
    
    // 1) get axial force and stiffness in basic x-direction
    double ub0Old = physicalProperties[0]->getStrain();
    physicalProperties[0]->setTrialStrain(ub(0),ubdot(0));
    qb(0) = physicalProperties[0]->getStress();
    kb(0,0) = physicalProperties[0]->getTangent();
    
    // check for uplift
    if (qb(0) >= 0.0)  {
        kb = kbInit;
        if (qb(0) > 0.0)  {
            physicalProperties[0]->setTrialStrain(ub0Old,0.0);
            kb(0,0) *= DBL_EPSILON;
        }
        qb.Zero();
        return 0;
    }
    
    // 2) calculate shear force and stiffness in basic y-direction
    int iter = 0;
    double qb1Old = 0.0;
    do  {
        // save old shear force
        qb1Old = qb(1);
        
        // get normal and friction (yield) forces
        double N = -qb(0) - qb(1)*ul(2);
        theFrnMdl->setTrial(N, ubdotAbs);
        double qYield = (theFrnMdl->getFrictionForce());
        
        // get initial stiffness of hysteretic component
        double k0 = qYield/uy;
        
        // get trial shear force of hysteretic component
        double qTrial = k0*(ub(1) - ubPlasticC);
        
        // compute yield criterion of hysteretic component
        double qTrialNorm = fabs(qTrial);
        double Y = qTrialNorm - qYield;
        
        // elastic step -> no updates required
        if (Y <= 0.0)  {
            // set shear force
            qb(1) = qTrial - N*ul(2);
            // set tangent stiffness
            kb(1,1) = k0;
        }
        // plastic step -> return mapping
        else  {
            // compute consistency parameter
            double dGamma = Y/k0;
            // update plastic displacement
            ubPlastic = ubPlasticC + dGamma*qTrial/qTrialNorm;
            // set shear force
            qb(1) = qYield*qTrial/qTrialNorm - N*ul(2);
            // set tangent stiffness
            kb(1,1) = 0.0;
        }
        iter++;
    } while ((fabs(qb(1)-qb1Old) >= tol) && (iter <= maxIter));
    
    // issue warning if iteration did not converge
    if (iter >= maxIter)  {
        std::cerr << "WARNING: XC::FlatSliderSimple2d::update() - did not find the shear force after "
            << iter << " iterations and norm: " << fabs(qb(1)-qb1Old) << std::endl;
        return -1;
    }
    
    // 3) get moment and stiffness in basic z-direction
    physicalProperties[1]->setTrialStrain(ub(2),ubdot(2));
    qb(2) = physicalProperties[1]->getStress();
    kb(2,2) = physicalProperties[1]->getTangent();
    
    return 0;
}


const XC::Matrix &XC::FlatSliderSimple2d::getTangentStiff()
{
    // zero the matrix
    theMatrix.Zero();
    
    // transform from basic to local system
    static Matrix kl(6,6);
    kl.addMatrixTripleProduct(0.0, Tlb, kb, 1.0);
    
    // add geometric stiffness to local stiffness
    kl(2,1) -= 1.0*qb(0);
    kl(2,4) += 1.0*qb(0);
    //kl(5,1) -= 0.0*qb(0);
    //kl(5,4) += 0.0*qb(0);
    
    // transform from local to global system
    theMatrix.addMatrixTripleProduct(0.0, Tgl, kl, 1.0);
    
    return theMatrix;
}


const XC::Matrix &XC::FlatSliderSimple2d::getInitialStiff()
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


const XC::Matrix &XC::FlatSliderSimple2d::getMass()
{
	// zero the matrix
    theMatrix.Zero();
    
	// check for quick return
	if (mass == 0.0)  {
		return theMatrix;
	}
    
	double m = 0.5*mass;
	for (int i=0; i<2; i++)  {
		theMatrix(i,i)     = m;
		theMatrix(i+3,i+3) = m;
	}
	
    return theMatrix; 
}

int XC::FlatSliderSimple2d::addLoad(ElementalLoad *theLoad, double loadFactor)
{  
	std::cerr <<"XC::FlatSliderSimple2d::addLoad() - "
		<< "load type unknown for element: "
		<< this->getTag() << std::endl;

	return -1;
}


int XC::FlatSliderSimple2d::addInertiaLoadToUnbalance(const Vector &accel)
{
	// check for quick return
	if (mass == 0.0)  {
		return 0;
	}    
    
	// get R * accel from the nodes
	const Vector &Raccel1 = theNodes[0]->getRV(accel);
	const Vector &Raccel2 = theNodes[1]->getRV(accel);
	
	if (3 != Raccel1.Size() || 3 != Raccel2.Size())  {
		std::cerr << "XC::FlatSliderSimple2d::addInertiaLoadToUnbalance() - "
			<< "matrix and vector sizes are incompatible\n";
		return -1;
	}
    
	// want to add ( - fact * M R * accel ) to unbalance
	// take advantage of lumped mass matrix
	double m = 0.5*mass;
    for (int i=0; i<2; i++)  {
        load(i)   -= m * Raccel1(i);
        load(i+3) -= m * Raccel2(i);
    }
    
	return 0;
}


const XC::Vector& XC::FlatSliderSimple2d::getResistingForce()
{
    // zero the residual
    theVector.Zero();
    
    // determine resisting forces in local system
    static Vector ql(6);
    ql = Tlb^qb;
    
    // add P-Delta moments to local forces
    double MpDelta = qb(0)*(ul(4)-ul(1));
    ql(2) += 1.0*MpDelta;
    //ql(5) += 0.0*MpDelta;
    
    // determine resisting forces in global system
    theVector = Tgl^ql;
    
    // subtract external load
    theVector.addVector(1.0, load, -1.0);
    
    return theVector;
}


const XC::Vector& XC::FlatSliderSimple2d::getResistingForceIncInertia()
  {	
    theVector = this->getResistingForce();
	
    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
      theVector += this->getRayleighDampingForces();
    
    // now include the mass portion
    if(mass != 0.0)
      {
	const Vector &accel1 = theNodes[0]->getTrialAccel();
	const Vector &accel2 = theNodes[1]->getTrialAccel();    
		
	const double m = 0.5*mass;
	for(int i=0; i<2; i++)
          {
	    theVector(i)+= m * accel1(i);
	    theVector(i+3)+= m * accel2(i);
          }
      }
    return theVector;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::FlatSliderSimple2d::sendData(CommParameters &cp)
  {
    int res= FrictionElementBase::sendData(cp);
    res+= cp.sendDoubles(ubPlastic,ubPlasticC,getDbTagData(),CommMetaData(19));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::FlatSliderSimple2d::recvData(const CommParameters &cp)
  {
    int res= FrictionElementBase::recvData(cp);
    res+= cp.receiveDoubles(ubPlastic,ubPlasticC,getDbTagData(),CommMetaData(19));
    return res;
  }


int XC::FlatSliderSimple2d::sendSelf(CommParameters &cp)
  {
    inicComm(20);

    int res= sendData(cp);

    const int dataTag= getDbTag();
    res += cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "ZeroLength::sendSelf -- failed to send ID data\n";
    return res;
  }


int XC::FlatSliderSimple2d::recvSelf(const CommParameters &cp)
  {
    inicComm(20);

    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ZeroLength::recvSelf -- failed to receive ID data\n";
    else
      res+= recvData(cp);
    return res;
  }


void XC::FlatSliderSimple2d::Print(std::ostream &s, int flag)
{
    if (flag == 0)  {
        // print everything
		s << "Element: " << this->getTag(); 
		//s << "  type: FlatSliderSimple2d  iNode: " << connectedExternalNodes(0);
		//s << "  jNode: " << connectedExternalNodes(1) << std::endl;
        s << "  FrictionModel: " << theFrnMdl->getTag() << std::endl;
        s << "  uy: " << uy << std::endl;
        s << "  Material ux: " << physicalProperties[0]->getTag() << std::endl;
        s << "  Material rz: " << physicalProperties[1]->getTag() << std::endl;
        s << "  mass: " << mass << "  maxIter: " << maxIter << "  tol: " << tol << std::endl;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << std::endl;
    } else if (flag == 1)  {
		// does nothing
    }
}


XC::Response *XC::FlatSliderSimple2d::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    Response *theResponse = 0;

//     output.tag("ElementOutput");
//     output.attr("eleType","FlatSliderSimple2d");
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
    
//     output.endTag(); // ElementOutput
    
    return theResponse;
}


int XC::FlatSliderSimple2d::getResponse(int responseID, Information &eleInfo)
{
    double MpDelta;

	switch (responseID)  {
	case 1:  // global forces
        return eleInfo.setVector(this->getResistingForce());
        
	case 2:  // local forces
        theVector.Zero();
        // determine resisting forces in local system
        theVector = Tlb^qb;
        // add P-Delta moments
        MpDelta = qb(0)*(ul(4)-ul(1));
        theVector(2) += 1.0*MpDelta;
        //theVector(5) += 0.0*MpDelta;
        
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
void XC::FlatSliderSimple2d::setUp()
{
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();	
    Vector xp = end2Crd - end1Crd;
    L = xp.Norm();
    
    if (L > DBL_EPSILON)  {
        if (x.Size() == 0)  {
            x.resize(3);
            x(0) = xp(0);  x(1) = xp(1);  x(2) = 0.0;
            y.resize(3);
            y(0) = -x(1);  y(1) = x(0);  y(2) = 0.0;
        } else  {
            std::cerr << "WARNING XC::FlatSliderSimple2d::setUp() - " 
                << "element: " << this->getTag() << std::endl
                << "ignoring nodes and using specified "
                << "local x vector to determine orientation\n";
        }
    }
    // check that vectors for orientation are of correct size
    if (x.Size() != 3 || y.Size() != 3)  {
        std::cerr << "XC::FlatSliderSimple2d::setUp() - "
            << "element: " << this->getTag() << std::endl
            << "incorrect dimension of orientation vectors\n";
        exit(-1);
    }
    
    // establish orientation of element for the transformation matrix
    // z = x cross y
    Vector z(3);
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
        std::cerr << "XC::FlatSliderSimple2d::setUp() - "
            << "element: " << this->getTag() << std::endl
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
    Tlb(1,5) = -L;    
}


double XC::FlatSliderSimple2d::sgn(double x)
  { 
    if(x > 0)
      return 1.0;
    else if (x < 0)
      return -1.0;
    else
      return 0.0;
  }
