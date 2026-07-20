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

// $Revision$
// $Date$
// $URL$

// TripleFrictionPendulum element
// Written by Nhan Dao, nhan.unr@gmail.com

#include "TripleFrictionPendulum.h"
#include "utility/recorder/response/ElementResponse.h"

// initialize the class wide variables
XC::Matrix XC::TripleFrictionPendulum::eleK(12,12);
XC::Matrix XC::TripleFrictionPendulum::eleKinit(12,12);
XC::Matrix XC::TripleFrictionPendulum::eleM(12,12);
XC::Matrix XC::TripleFrictionPendulum::eleD(12,12);
XC::Vector XC::TripleFrictionPendulum::eleR(12);


void XC::TripleFrictionPendulum::initialize()
  {
    // initialize constants
    v1Fact= 0.5;
    v3Fact= L2/(L2 - L1);
    v5Fact= L3/(L3 - L1);
    
    Gap2= 2.0*(L1/L3*Ubar3 + Ubar1);
    Gap4= Ubar2*(1 - L1/L2);
    Gap6= Ubar3*(1 - L1/L3);
    
    // initialize other variables
    this->revertToStart();
  }

//! @brief Cconstructor
XC::TripleFrictionPendulum::TripleFrictionPendulum(int tag,
    int Nd1, int Nd2,
    const FrictionModels &frnmdls, 
    const UniaxialMatPhysicalProperties &materials,
    double l1,
    double l2,
    double l3,
    double ubar1,
    double ubar2,
    double ubar3,
    double w,
    double uy,
    double kvt,
    double minFv,
    double tol)
  : Element0D(tag, ELE_TAG_TripleFrictionPendulum,Nd1,Nd2, 6),
    frictionModels(frnmdls), physicalProperties(materials),
    L1(l1), L2(l2), L3(l3), Ubar1(ubar1), Ubar2(ubar2), Ubar3(ubar3),
    W(w), Uy(uy), Kvt(kvt), MinFv(minFv), TOL(tol), Niter(20),
    K(2,2), Kpr(2,2), f(2), fpr(2),
    k12(2,2), k12pr(2,2), k34(2,2), k34pr(2,2), k56(2,2), k56pr(2,2),
    d1(2), d1pr(2), d3(2), d3pr(2), d5(2), d5pr(2), v1(2), v3(2), v5(2),
    ep1(2), ep1pr(2), ep3(2), ep3pr(2), ep5(2), ep5pr(2),
    q1(2), q1pr(2), q3(2), q3pr(2), q5(2), q5pr(2),
    ep1tmp(2), ep3tmp(2), ep5tmp(2), q1tmp(2), q3tmp(2), q5tmp(2)
  {
    
    // check friction model input
    if(frnmdls.size() != 3)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; number of friction models is: "
		      << frnmdls.size()
		      << "; it must be 3."
		      << std::endl;
        exit(-1);
      }
    
    // check material input
    if(materials.size() != 4)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; number of materials is: "
		      << materials.size()
		      << "; it must be 4."
		      << std::endl;
        exit(-1);
      }

    initialize();
  }


//! @brief Default constructor.
XC::TripleFrictionPendulum::TripleFrictionPendulum()
    : Element0D(0, ELE_TAG_TripleFrictionPendulum,0,0,6),
    L1(0.0), L2(0.0), L3(0.0), Ubar1(0.0), Ubar2(0.0), Ubar3(0.0),
    W(0.0), Uy(0.0), Kvt(0.0), MinFv(0.0), TOL(1E-6), Niter(20),
    K(2,2), Kpr(2,2), f(2), fpr(2),
    k12(2,2), k12pr(2,2), k34(2,2), k34pr(2,2), k56(2,2), k56pr(2,2),
    d1(2), d1pr(2), d3(2), d3pr(2), d5(2), d5pr(2), v1(2), v3(2), v5(2),
    ep1(2), ep1pr(2), ep3(2), ep3pr(2), ep5(2), ep5pr(2),
    q1(2), q1pr(2), q3(2), q3pr(2), q5(2), q5pr(2),
    ep1tmp(2), ep3tmp(2), ep5tmp(2), q1tmp(2), q3tmp(2), q5tmp(2)
  {
  }


//! @brief Virtual constructor.
XC::Element* XC::TripleFrictionPendulum::getCopy() const
  { return new TripleFrictionPendulum(*this); }

//! @brief Set a link to the enclosing Domain, ensure nodes exist in Domain
//!    and set pointers to these nodes, also determines the length and 
//!    transformation Matrix.
void XC::TripleFrictionPendulum::setDomain(Domain *theDomain)
  {
    // check Domain is not null - invoked when object removed from a domain
    if(theDomain)
      {
	Element0D::setDomain(theDomain);
	
	// ensure connected nodes have correct number of dof's
	const int dofNd1= theNodes[0]->getNumberDOF();
	const int dofNd2= theNodes[1]->getNumberDOF();	

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
      }
  }

int XC::TripleFrictionPendulum::commitState()
  {
    int errCode= this->Element0D::commitState(); // commit the base class.
    
    errCode+= this->frictionModels.commitState(); // commit friction model
    errCode+= physicalProperties.commitState();// commit material models
    
    // commit other history variables
    Wpr= Wcr;
    Fy1pr= Fy1; Fy3pr= Fy3; Fy5pr= Fy5;
    Kpr= K;
    fpr= f;
    k12pr= k12; k34pr= k34; k56pr= k56;
    d1pr= d1; d3pr= d3; d5pr= d5;
    ep1pr= ep1tmp; ep3pr= ep3tmp; ep5pr= ep5tmp;
    q1pr= q1tmp; q3pr= q3tmp; q5pr= q5tmp;
    
    return 0;
  }

int XC::TripleFrictionPendulum::revertToLastCommit()
  {
    // DON'T call Element::revertToLastCommit() because
    // is a pure virtual method.
    int errCode= this->frictionModels.revertToLastCommit();// revert friction model
    errCode+= physicalProperties.revertToLastCommit();// revert material models
    
    return 0;
  }

int XC::TripleFrictionPendulum::revertToStart()
  {
    int errCode= Element0D::revertToStart();
    Vector tmp1(2), tmp2(2), tmp3(2);
    
    Vel1Avg= Vel3Avg= Vel5Avg= 0.0;
    Fy1pr= Fy3pr= Fy5pr= 0.0;
    Wpr= Wcr= Wavg= W;
    
    // revert friction models and re-initialize
    errCode+= this->frictionModels.revertToStart();
    const size_t sz= this->frictionModels.size();
    for(size_t i=0; i<sz; i++)
      {
        this->frictionModels[i]->setTrial(Wavg, 0.0);
      }
    Fy1= this->frictionModels[0]->getFrictionCoeff();
    Fy3= this->frictionModels[1]->getFrictionCoeff();
    Fy5= this->frictionModels[2]->getFrictionCoeff();
    
    E1= E2= 3.0*Fy1/Uy;
    E3= E4= 3.0*Fy1/Uy;
    E5= E6= 3.0*Fy1/Uy;
    
    const double E1p= 1.0/(2.0*L1);
    const double E3p= 1.0/(L2 - L1);
    const double E5p= 1.0/(L3 - L1);
    
    H1= E1*E1p/(E1 - E1p);
    H3= E3*E3p/(E3 - E3p);
    H5= E5*E5p/(E5 - E5p);
    
    // revert material models
    errCode+= physicalProperties.revertToStart();// revert material models
    
    // initialize remaining variables
    Fvert= 0.0;
    Kvert= physicalProperties[0]->getInitialTangent();
    TorqX= 0.0;
    KrotX= physicalProperties[2]->getInitialTangent();
    TorqY= 0.0;
    KrotY= physicalProperties[3]->getInitialTangent();
    TorqZ= 0.0;
    KrotZ= physicalProperties[1]->getInitialTangent();
    Hisolator= 0.0;
    Dx= Dy= Dz= 0.0;
    
    // reset history variables
    d1pr.Zero();
    d3pr.Zero();
    d5pr.Zero();
    ep1pr.Zero();
    ep3pr.Zero();
    ep5pr.Zero();
    q1pr.Zero();
    q3pr.Zero();
    q5pr.Zero();
    fpr.Zero();
    
    BidirectionalPlastic(k12pr, tmp1, tmp2, tmp3, Fy1, E1, H1, ep1pr, q1pr, d1pr);
    BidirectionalPlastic(k34pr, tmp1, tmp2, tmp3, Fy3, E3, H3, ep3pr, q3pr, d3pr);
    BidirectionalPlastic(k56pr, tmp1, tmp2, tmp3, Fy5, E5, H5, ep5pr, q5pr, d5pr);
    StiffnessForm(Kpr, k12pr, k34pr, k56pr);
    
    return errCode;
  }

int XC::TripleFrictionPendulum::update()
  {
    // get current time
    Domain *theDomain= this->getDomain();
    const double time= theDomain->getCurrentTime();
    
    const Vector &duNd1= theNodes[0]->getIncrDisp();
    const Vector &duNd2= theNodes[1]->getIncrDisp();
    const Vector &utrialNd1= theNodes[0]->getTrialDisp();
    const Vector &utrialNd2= theNodes[1]->getTrialDisp();
    const Vector &vtrialNd1= theNodes[0]->getTrialVel();
    const Vector &vtrialNd2= theNodes[1]->getTrialVel();
    const Vector &uNd1= theNodes[0]->getDisp();
    const Vector &uNd2= theNodes[1]->getDisp();
    const Vector &end1Crd= theNodes[0]->getCrds();
    const Vector &end2Crd= theNodes[1]->getCrds();
    
    Vector u(2);
    u(0)= uNd2(0) - uNd1(0);  // converged displacement from previous step
    u(1)= uNd2(1) - uNd1(1);
    Vector utrial(2);
    Dx= utrial(0)= utrialNd2(0) - utrialNd1(0);  // trial displacement (target displacement)
    Dy= utrial(1)= utrialNd2(1) - utrialNd1(1);
    Dz= utrialNd2(2) - utrialNd1(2);
    
    Vector r(3), rdot(3);
    r(0)= utrialNd2(3) - utrialNd1(3);
    r(1)= utrialNd2(4) - utrialNd1(4);
    r(2)= utrialNd2(5) - utrialNd1(5);
    rdot(0)= vtrialNd2(3) - vtrialNd1(3);
    rdot(1)= vtrialNd2(4) - vtrialNd1(4);
    rdot(2)= vtrialNd2(5) - vtrialNd1(5);
    
    Vector dusub(2);
    dusub(0)= duNd2(0) - duNd1(0);  // incremental displacement
    dusub(1)= duNd2(1) - duNd1(1);
    
    // isolator height
    Hisolator= end2Crd(2) - end1Crd(2);
    
    // 1) get axial force and stiffness in vertical direction
    const double DzOld= physicalProperties[0]->getStrain();
    const double Vz= vtrialNd2(2) - vtrialNd1(2);
    physicalProperties[0]->setTrialStrain(Dz, Vz);
    Fvert= physicalProperties[0]->getStress();
    Kvert= physicalProperties[0]->getTangent();
    
    // vertical force for computing friction
    if(Fvert >= 0.0)
      {
        Kvert= physicalProperties[0]->getInitialTangent();
        if (Fvert > Kvert*DBL_EPSILON)
	  {
            physicalProperties[0]->setTrialStrain(DzOld, 0.0);
            Kvert= Kvt;
	  }
        Fvert= -MinFv;
      }
    Wcr= -Fvert;
    
    // 2) calculate shear forces and stiffnesses in horizontal direction
    const double Tol= dusub.Norm()*TOL;
    K= Kpr;
    f= fpr;
    k12= k12pr; k34= k34pr; k56= k56pr;
    d1= d1pr; d3= d3pr; d5= d5pr;
    ep1= ep1pr; ep3= ep3pr; ep5= ep5pr;
    q1= q1pr; q3= q3pr; q5= q5pr;
    ep1tmp= ep1pr; ep3tmp= ep3pr; ep5tmp= ep5pr;
    q1tmp= q1pr; q3tmp= q3pr; q5tmp= q5pr;
    Vector ErrDisp= dusub;
    
    // averaging vertical force
    Wavg= (Wpr + Wcr)/2.0;
    
    // get coefficients of friction
    double Fy1cr, Fy3cr, Fy5cr, dFy1, dFy3, dFy5;
    this->frictionModels[0]->setTrial(Wavg, v1Fact*Vel1Avg);
    this->frictionModels[1]->setTrial(Wavg, v3Fact*Vel3Avg);
    this->frictionModels[2]->setTrial(Wavg, v5Fact*Vel5Avg);
    Fy1cr= this->frictionModels[0]->getFrictionCoeff();
    Fy3cr= this->frictionModels[1]->getFrictionCoeff();
    Fy5cr= this->frictionModels[2]->getFrictionCoeff();
    
    dFy1= Fy1cr - Fy1pr; dFy3= Fy3cr - Fy3pr; dFy5= Fy5cr - Fy5pr;
    Fy1= Fy1pr; Fy3= Fy3pr; Fy5= Fy5pr;
    
    int nDiv= 0; int nWhileIter= 0;
    double TolOriginal= Tol;
    // get global timestep variable.
    const double ops_Dt= theDomain->getDt(); //time step
    if(ops_Dt==0.0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; time step cannot be zero"
	          << "use Domain::setDt(dt) to set a suitable value."
	          << std::endl;
	exit(-1);
      }
    while ((nDiv < 10) && (ErrDisp.Norm() > TolOriginal))
      {
        Fy1= Fy1 + dFy1; Fy3= Fy3 + dFy3; Fy5= Fy5 + dFy5;
        TFPElement(Conv, ep1tmp, ep3tmp, ep5tmp, q1tmp, q3tmp, q5tmp, K, f, k12, k34, k56, d1, d3, d5, ep1, ep3, ep5, q1, q3, q5, u, dusub, Fy1, Fy3, Fy5, E1, E3, E5, H1, H3, H5, E2, E4, E6, Gap2, Gap4, Gap6, Tol, Niter);
        if ((!Conv) && (nDiv < 7))
	  {
            u(0)= uNd2(0) - uNd1(0);
            u(1)= uNd2(1) - uNd1(1);
            dFy1 /= 2.0; dFy3 /= 2.0; dFy5 /= 2.0;
            Fy1= Fy1pr; Fy3= Fy3pr; Fy5= Fy5pr;
            K= Kpr;
            f= fpr;
            k12= k12pr; k34= k34pr; k56= k56pr;
            d1= d1pr; d3= d3pr; d5= d5pr;
            ep1= ep1pr; ep3= ep3pr; ep5= ep5pr;
            q1= q1pr; q3= q3pr; q5= q5pr;
            dusub /= 2.0;
            nDiv++;
            nWhileIter= 0;
	  }
	else
	  {
            if (nWhileIter >= pow(2.0, nDiv))
	      {
                break;
	      }
            ep1= ep1tmp; ep3= ep3tmp; ep5= ep5tmp;
            q1= q1tmp; q3= q3tmp; q5= q5tmp;
            u += dusub;
            ErrDisp(0)= utrial(0) - u(0);
            ErrDisp(1)= utrial(1) - u(1);
            nWhileIter++;
	  }
        v1= (1.0/ops_Dt)*(d1 - d1pr);
        v3= (1.0/ops_Dt)*(d3 - d3pr);
        v5= (1.0/ops_Dt)*(d5 - d5pr);
        Vel1Avg= v1.Norm();
        Vel3Avg= v3.Norm();
        Vel5Avg= v5.Norm();
      }
    if (nDiv == 10)
      {
        if ((!Conv) || (Tol < ErrDisp.Norm()))
	  {
            std::cerr << "Warning: isolator "
		      << this->getTag()
		      << " has not converged, ErrDisp= "
		      << ErrDisp << std::endl;
	  }
      }
    
    // 3) get moment and stiffness about vertical direction
    physicalProperties[1]->setTrialStrain(r(2), rdot(2));
    TorqZ= physicalProperties[1]->getStress();
    KrotZ= physicalProperties[1]->getTangent();
    
    // 4) get moment and stiffness about horizontal 1 direction
    physicalProperties[2]->setTrialStrain(r(0), rdot(0));
    TorqX= physicalProperties[2]->getStress();
    KrotX= physicalProperties[2]->getTangent();
    
    // 5) get moment and stiffness about horizontal 2 direction
    physicalProperties[3]->setTrialStrain(r(1), rdot(1));
    TorqY= physicalProperties[3]->getStress();
    KrotY= physicalProperties[3]->getTangent();
    
    return 0;
  }


const XC::Matrix &XC::TripleFrictionPendulum::getTangentStiff() const
  {
    Matrix a(2,12);
    Matrix aT(12,2);
    
    a.Zero();
    aT.Zero();
    a(0,0)= a(1,1)= -1;
    a(0,6)= a(1,7)=  1;
    aT(0,0)= aT(1,1)= -1;
    aT(6,0)= aT(7,1)=  1;
    eleK= aT*K*a;
    eleK *= -Fvert;
    
    eleK(2,2)= eleK(8,8)= Kvert;
    eleK(2,8)= eleK(8,2)= -Kvert;
    eleK(3,3)= eleK(9,9)= KrotX;
    eleK(3,9)= eleK(9,3)= -KrotX;
    eleK(4,4)= eleK(10,10)= KrotY;
    eleK(4,10)= eleK(10,4)= -KrotY;
    eleK(5,5)= eleK(11,11)= KrotZ;
    eleK(5,11)= eleK(11,5)= -KrotZ;
    
    return eleK;
  }


const XC::Matrix& XC::TripleFrictionPendulum::getInitialStiff() const
  {
    Matrix a(2,12);
    Matrix aT(12,2);
    Matrix Kinit(2,2);
    
    Kinit.Zero();
    Kinit(0,0)= Kinit(1,1)= E1/3.0;
    a.Zero();
    aT.Zero();
    a(0,0)= a(1,1)= -1;
    a(0,6)= a(1,7)=  1;
    aT(0,0)= aT(1,1)= -1;
    aT(6,0)= aT(7,1)=  1;
    eleKinit= aT*Kinit*a;
    eleKinit *= W;
    
    eleKinit(2,2)= eleKinit(8,8)= physicalProperties[0]->getInitialTangent();
    eleKinit(2,8)= eleKinit(8,2)= -eleKinit(2,2);
    eleKinit(3,3)= eleKinit(9,9)= physicalProperties[2]->getInitialTangent();
    eleKinit(3,9)= eleKinit(9,3)= -eleKinit(3,3);
    eleKinit(4,4)= eleKinit(10,10)= physicalProperties[3]->getInitialTangent();
    eleKinit(4,10)= eleKinit(10,4)= -eleKinit(4,4);
    eleKinit(5,5)= eleKinit(11,11)= physicalProperties[1]->getInitialTangent();
    eleKinit(5,11)= eleKinit(11,5)= -eleKinit(5,5);
    
    return eleKinit;
}


const XC::Matrix& XC::TripleFrictionPendulum::getDamp() const
{
    // zero the matrix
    eleD.Zero();
    
    // add damping tangent from materials
    eleD(2,2)= eleD(8,8)= physicalProperties[0]->getDampTangent();
    eleD(2,8)= eleD(8,2)= -eleD(2,2);
    eleD(3,3)= eleD(9,9)= physicalProperties[2]->getDampTangent();
    eleD(3,9)= eleD(9,3)= -eleD(3,3);
    eleD(4,4)= eleD(10,10)= physicalProperties[3]->getDampTangent();
    eleD(4,10)= eleD(10,4)= -eleD(4,4);
    eleD(5,5)= eleD(11,11)= physicalProperties[1]->getDampTangent();
    eleD(5,11)= eleD(11,5)= -eleD(5,5);
    
    return eleD;
}


const XC::Matrix& XC::TripleFrictionPendulum::getMass() const
  {
    eleM.Zero();
    
    return eleM;
  }

int XC::TripleFrictionPendulum::addLoad(ElementalLoad *theLoad, double loadFactor)
  {  
    std::cerr << getClassName() << "::" << __FUNCTION__
	      <<"; load type unknown for element: "
	      << this->getTag() << std::endl;

    return -1;
  }


int XC::TripleFrictionPendulum::addInertiaLoadToUnbalance(const Vector &accel)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      <<"; not implemented yet."
	      << std::endl;
    return 0;
  }

const XC::Vector& XC::TripleFrictionPendulum::getResistingForce() const
  {
    Matrix aT(12,2);
    aT.Zero();
    aT(0,0)= aT(1,1)= -1;
    aT(6,0)= aT(7,1)= 1;
    eleR= aT*f;
    eleR *= -Fvert;
    double Mx, My, Mz;
    Mx= -Fvert*Dy + eleR(7)*Hisolator;
    My= Fvert*Dx - eleR(6)*Hisolator;
    Mz= eleR(6)*Dy - eleR(7)*Dx;
    eleR(3)= eleR(9)= TorqX + Mx/2;
    eleR(4)= eleR(10)= TorqY + My/2;
    eleR(5)= eleR(11)= TorqZ + Mz/2;
    eleR(2)= -Fvert;
    eleR(8)= Fvert;
    
    return eleR;
  }

//! @brief Send members through the communicator argument.
int XC::TripleFrictionPendulum::sendData(Communicator &comm)
  {
    int res= Element0D::sendData(comm);
    res+= comm.sendMovable(frictionModels, getDbTagData(),CommMetaData(9));
    res+= comm.sendMovable(physicalProperties, getDbTagData(),CommMetaData(10));
    res+= comm.sendDoubles(L1, L2, L3, getDbTagData(),CommMetaData(11));
    res+= comm.sendDoubles(Ubar1, Ubar2, Ubar3, getDbTagData(),CommMetaData(12));
    res+= comm.sendDoubles(W, Uy, Kvt, MinFv, TOL, getDbTagData(),CommMetaData(13));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::TripleFrictionPendulum::recvData(const Communicator &comm)
  {
    int res= Element0D::recvData(comm);
    res+= comm.receiveMovable(frictionModels, getDbTagData(),CommMetaData(9));
    res+= comm.receiveMovable(physicalProperties, getDbTagData(),CommMetaData(10));
    res+= comm.receiveDoubles(L1, L2, L3, getDbTagData(),CommMetaData(11));
    res+= comm.receiveDoubles(Ubar1, Ubar2, Ubar3, getDbTagData(),CommMetaData(12));
    res+= comm.receiveDoubles(W, Uy, Kvt, MinFv, TOL, getDbTagData(),CommMetaData(13));
    this->initialize();
    return res;
  }

int XC::TripleFrictionPendulum::sendSelf(Communicator &comm)
  {
    inicComm(14);

    int res= sendData(comm);

    const int dataTag= getDbTag();
    res += comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send ID data\n";
    return res;
  }

int XC::TripleFrictionPendulum::recvSelf(const Communicator &comm)
  {
    inicComm(14);

    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ID data\n";
    else
      res+= recvData(comm);
    return res;
  }

//! @ Print stuff.
void XC::TripleFrictionPendulum::Print(std::ostream &s, int flag) const
  {
    if (flag == 0)
      {
        // print everything
        s << "Element: " << this->getTag() 
          << "  type: RJWatsonEQS2d  iNode: " << theNodes[0]->getTag()
          << "  jNode: " << theNodes[1]->getTag() << std::endl;
	frictionModels.Print(s, flag);
        s << std::endl
          << "  Materials: " << physicalProperties[0]->getTag() << ", "
          << physicalProperties[1]->getTag() << ", " << physicalProperties[2]->getTag()
          << ", " << physicalProperties[3]->getTag() << std::endl
          << "  L1: " << L1 << ", L2: " << L2 << ", L3: " << L3 << std::endl
          << "  d1: " << Ubar1 << ", d2: " << Ubar2 << ", d3: " << Ubar3 << std::endl
          << "  uy: " << Uy << ", kvt: " << Kvt << ",  minFv: " << MinFv << std::endl;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << std::endl;
    }
    
  }


XC::Response* XC::TripleFrictionPendulum::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
    Response *theResponse= nullptr;

    /*
    output.tag("ElementOutput");
    output.attr("eleType","TripleFrictionPendulum");
    output.attr("eleTag",this->getTag());
    output.attr("node1",externalNodes[0]);
    output.attr("node2",externalNodes[1]);
    
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
        
        theResponse= new ElementResponse(this, 1, eleR);
    }
    // local forces
    else if (strcmp(argv[0],"localForce") == 0 ||
        strcmp(argv[0],"localForces") == 0)
    {
        output.tag("ResponseType","N_1");
        output.tag("ResponseType","Vy_1");
        output.tag("ResponseType","Vz_1");
        output.tag("ResponseType","T_1");
        output.tag("ResponseType","My_1");
        output.tag("ResponseType","Mz_1");
        output.tag("ResponseType","N_2");
        output.tag("ResponseType","Vy_2");
        output.tag("ResponseType","Vz_2");
        output.tag("ResponseType","T_2");
        output.tag("ResponseType","My_2");
        output.tag("ResponseType","Mz_2");
        
        theResponse= new ElementResponse(this, 2, Vector(12));
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
        
        theResponse= new ElementResponse(this, 3, Vector(6));
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
        
        theResponse= new ElementResponse(this, 4, Vector(12));
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
        
        theResponse= new ElementResponse(this, 5, Vector(6));
    }
    // displacement components
    else if (strcmp(argv[0],"compDeformation") == 0 ||
        strcmp(argv[0],"compDeformations") == 0 ||
        strcmp(argv[0],"compDisplacement") == 0 ||
        strcmp(argv[0],"compDisplacements") == 0)
    {
        output.tag("ResponseType","d1x");
        output.tag("ResponseType","d1y");
        output.tag("ResponseType","d3x");
        output.tag("ResponseType","d3y");
        output.tag("ResponseType","d5x");
        output.tag("ResponseType","d5y");
        
        theResponse= new ElementResponse(this, 6, Vector(6));
    }
    // friction model output
    else if (strcmp(argv[0],"frictionModel") == 0 || strcmp(argv[0],"frnMdl") == 0 ||
        strcmp(argv[0],"frictionMdl") == 0 || strcmp(argv[0],"frnModel") == 0)  {
        if (argc > 2)  {
            int frnNum= atoi(argv[1]);
            if (frnNum >= 1 && frnNum <= 3)
                theResponse= theFrnMdls[frnNum-1]->setResponse(&argv[2], argc-2, output);
        }
    }
    // material output
    else if (strcmp(argv[0],"material") == 0)  {
        if (argc > 2)  {
            int matNum= atoi(argv[1]);
            if (matNum >= 1 && matNum <= 4)
                theResponse= physicalProperties[matNum-1]->setResponse(&argv[2], argc-2, output);
        }
    }
    
    output.endTag(); // ElementOutput
    */
    return theResponse;
  }


int XC::TripleFrictionPendulum::getResponse(int responseID, Information &eleInfo)
  {
    Vector locForce(12), locDisp(12), basForce(6), basDisp(6), cmpDisp(6);
    switch (responseID)
      {
      case 1:  // global forces
        return eleInfo.setVector(this->getResistingForce());
        
      case 2:  // local forces
        this->getResistingForce();
        locForce(0)= eleR(2);
        locForce(1)= eleR(0);
        locForce(2)= eleR(1);
        locForce(3)= eleR(5);
        locForce(4)= eleR(3);
        locForce(5)= eleR(4);
        locForce(6)= eleR(8);
        locForce(7)= eleR(6);
        locForce(8)= eleR(7);
        locForce(9)= eleR(11);
        locForce(10)= eleR(9);
        locForce(11)= eleR(10);
        return eleInfo.setVector(locForce);
        
      case 3:  // basic forces
        this->getResistingForce();
        basForce(0)= eleR(8);
        basForce(1)= eleR(6);
        basForce(2)= eleR(7);
        basForce(3)= eleR(11);
        basForce(4)= eleR(9);
        basForce(5)= eleR(10);
        return eleInfo.setVector(basForce);
        
      case 4:  // local displacements
        locDisp.Zero();
        return eleInfo.setVector(locDisp);
        
      case 5:  // basic displacements
        basDisp(0)= Dz;
        basDisp(1)= Dx;
        basDisp(2)= Dy;
        basDisp(3)= 0.0;
        basDisp(4)= 0.0;
        basDisp(5)= 0.0;
        return eleInfo.setVector(basDisp);
        
      case 6:  // displacement components
        cmpDisp(0)= d1(0);
        cmpDisp(1)= d1(1);
        cmpDisp(2)= d3(0);
        cmpDisp(3)= d3(1);
        cmpDisp(4)= d5(0);
        cmpDisp(5)= d5(1);
        return eleInfo.setVector(cmpDisp);
        
      default:
        return -1;
      }
  }


void XC::TripleFrictionPendulum::CircularElasticGap(Matrix &kj, Vector &fj, double Ej, double Gapj, Vector di)
{
    const double r= di.Norm();
    if (r == 0) {
        kj.Zero();
        fj.Zero();
    } else {
        double sn= di(1)/r;
        double cs= di(0)/r;
        
        if (r <= Gapj) {
            kj.Zero();
            fj.Zero();
        } else {
            kj(0,0)= Ej*(1 - Gapj/r*sn*sn);
            kj(0,1)= kj(1,0)= Ej*Gapj/r*sn*cs;
            kj(1,1)= Ej*(1 - Gapj/r*cs*cs);
            fj(0)= Ej*(r-Gapj)*cs;
            fj(1)= Ej*(r-Gapj)*sn;
        }
    }
}


void XC::TripleFrictionPendulum::BidirectionalPlastic(Matrix &ki, Vector &fi, Vector &epitmp, Vector &qitmp, double Fyi, double Ei, double Hi, Vector epi, Vector qi, Vector di)
{
    // This part was adapted to the source code of Bidirectional section in OpenSees
    Vector xsi;
    Vector ntmp(2);
    double normxsi;
    double fn;
    fi= Ei*(di - epi);  //trial stress
    xsi= fi - qi;
    normxsi= xsi.Norm();
    fn= normxsi - Fyi;  //yield function
    
    // Elastic step
    if (fn <= 0)
    {
        ki(0,0)= ki(1,1)= Ei;
        ki(1,0)= ki(0,1)= 0.0;
        epitmp= epi;
        qitmp= qi;
    }
    else {
        double dlam= fn/(Ei+Hi);
        double n1= xsi(0)/normxsi;
        double n2= xsi(1)/normxsi;
        double A= Ei*Ei/(Ei+Hi);
        double B= Ei*Ei*dlam/normxsi;
        double EB= Ei-B;
        double BA= B-A;
        ki(0,0)= EB + BA*n1*n1;
        ki(1,1)= EB + BA*n2*n2;
        ki(0,1)= ki(1,0)= BA*n1*n2;
        
        n1= n1*dlam;
        n2= n2*dlam;
        fi(0) -= Ei*n1;
        fi(1) -= Ei*n2;	
        ntmp(0)= n1;
        ntmp(1)= n2;
        epitmp= epi + ntmp;
        qitmp= qi + ntmp*Hi;
    }
}


void XC::TripleFrictionPendulum::Segment(Vector &epitmp, Vector &qitmp, bool &conv, Matrix &kij, Vector &di, Vector epi, Vector qi, Vector f, Vector df, double Fyi, double Ei, double Hi, double Ej, double Gapj, double Tol, int Niter)
{
    Vector dftmp= df;
    Vector dd;
    Matrix ki(2,2);
    Matrix kj(2,2);
    Vector fi(2);
    Vector fj(2);
    Vector fprime(2);
    Matrix invkij(2,2);
    
    kij.Invert(invkij);
    dd= invkij*dftmp;
    int iter= 1;
    epitmp= epi;
    qitmp= qi;
    
    bool enterloop= false;
    
    while (((dd.Norm() > 0.01*Tol) && (iter <= Niter)) || (!enterloop))
    {
        enterloop= true;
        iter++;
        di= di + dd;
        BidirectionalPlastic(ki, fi, epitmp, qitmp, Fyi, Ei, Hi, epi, qi, di);
        CircularElasticGap(kj, fj, Ej, Gapj, di);
        kij= ki + kj;
        fprime= fi + fj;
        dftmp= f + df - fprime;
        kij.Invert(invkij);
        dd= invkij*dftmp;
    }
    if (iter > Niter)
        conv= false;
    else
        conv= true;
}


void XC::TripleFrictionPendulum::TFPElement(bool &Conv, Vector &ep1tmp, Vector &ep3tmp, Vector &ep5tmp, Vector &q1tmp, Vector &q3tmp, Vector &q5tmp, Matrix &K, Vector &f, Matrix &k12, Matrix &k34, Matrix &k56, Vector &d1, Vector &d3, Vector &d5, Vector ep1, Vector ep3, Vector ep5, Vector q1, Vector q3, Vector q5, Vector u, Vector dusub, double Fy1, double Fy3, double Fy5, double E1, double E3, double E5, double H1, double H3, double H5, double E2, double E4, double E6, double Gap2, double Gap4, double Gap6, double Tol, int Niter)
{
    Vector df(2);
    Vector du(2);
    du= dusub;
    int iter= 1;
    bool conv= true;
    Vector uprime(2);
    
    Conv= true;
    ep1tmp= ep1;
    ep3tmp= ep3;
    ep5tmp= ep5;
    q1tmp= q1;
    q3tmp= q3;
    q5tmp= q5;
    
    bool enterloop= false;
    while (((du.Norm() > Tol) && (iter <= Niter) && Conv) || (!enterloop))
    {
        enterloop= true;
        iter++;
        df= K*du;
        Segment(ep1tmp, q1tmp, conv, k12, d1, ep1, q1, f, df, Fy1, E1, H1, E2, Gap2, Tol, Niter);
        if (!conv)
        {
            Conv= false;
            break;
        }
        Segment(ep3tmp, q3tmp, conv, k34, d3, ep3, q3, f, df, Fy3, E3, H3, E4, Gap4, Tol, Niter);
        if (!conv)
        {
            Conv= false;
            break;
        }
        Segment(ep5tmp, q5tmp, conv, k56, d5, ep5, q5, f, df, Fy5, E5, H5, E6, Gap6, Tol, Niter);
        if (!conv)
        {
            Conv= false;
            break;
        }
        f= f + df;
        
        uprime(0)= d1(0) + d3(0) + d5(0);
        uprime(1)= d1(1) + d3(1) + d5(1);
        du(0)= u(0) + dusub(0) - uprime(0);
        du(1)= u(1) + dusub(1) - uprime(1);
        StiffnessForm(K, k12, k34, k56);
    }
    if (iter > Niter)
        Conv= false;
}


void XC::TripleFrictionPendulum::StiffnessForm(Matrix &K, Matrix k12, Matrix k34, Matrix k56)
{
    Matrix K88(8,8);
    Matrix ktt(4,4);
    Matrix Ktmp1(4,4);
    Matrix Ktmp2(4,4);
    Matrix kot(4,4);
    Matrix kto(4,4);
    Matrix invktt(4,4);
    
    K88.Zero();	
    K88(0,0)= k12(0,0);
    K88(0,1)= K88(1,0)= k12(0,1);
    K88(0,4)= K88(4,0)= -k12(0,0);
    K88(0,5)= K88(5,0)= -k12(0,1);
    K88(1,1)= k12(1,1);
    K88(1,4)= K88(4,1)= -k12(0,1);
    K88(1,5)= K88(5,1)= -k12(1,1);
    K88(2,2)= k56(0,0);
    K88(2,3)= K88(3,2)= k56(0,1);
    K88(2,6)= K88(6,2)= -k56(0,0);
    K88(2,7)= K88(7,2)= -k56(0,1);
    K88(3,3)= k56(1,1);
    K88(3,6)= K88(6,3)= -k56(0,1);
    K88(3,7)= K88(7,3)= -k56(1,1);
    K88(4,4)= k12(0,0) + k34(0,0);
    K88(4,5)= K88(5,4)= k12(0,1) + k34(0,1);
    K88(4,6)= K88(6,4)= -k34(0,0);
    K88(4,7)= K88(7,4)= -k34(0,1);
    K88(5,5)= k12(1,1) + k34(1,1);
    K88(5,6)= K88(6,5)= -k34(0,1);
    K88(5,7)= K88(7,5)= -k34(1,1);
    K88(6,6)= k34(0,0) + k56(0,0);
    K88(6,7)= K88(7,6)= k34(0,1) + k56(0,1);
    K88(7,7)= k34(1,1) + k56(1,1);
    
    for (int i=0; i < 4; i++) {
        for (int j=0; j < 4; j++) {
            ktt(i,j)= K88(i+4,j+4);
            kot(i,j)= kto(j,i)= K88(i+4,j);
            Ktmp1(i,j)= K88(i,j);
        }
    }
    invktt.Zero();
    ktt.Invert(invktt);
    Ktmp2= Ktmp1 - kot*invktt*kto;
    for (int i=0; i<2; i++) {
        for (int j=0; j<2; j++) {
            K(i,j)= Ktmp2(i+2,j+2);
        }
    }
}
