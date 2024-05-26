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
                                                                       
// Created: Chris McGann, UW
//          December 2010
//
// Description: This file contains the implementation of the BeamContact2D class

#include "BeamContact2D.h"
#include "domain/component/Parameter.h"
#include "domain/mesh/node/Node.h"
#include "material/nD/uw_materials/ContactMaterial2D.h"
#include "utility/utils/misc_utils/colormod.h"

void XC::BeamContact2D::setup_contact_state(void) const
  {
    if(mIniContact == 0)
      {
	inContact= true;
	was_inContact= true;
	to_be_released= false;
	should_be_released= false;
	in_bounds= true;
      }
    else
      {
	inContact= false;
	was_inContact= false;
	to_be_released= false;
	should_be_released= false;
	in_bounds= true;
      }

    mGap= 0.0;
    mLambda= 0.0;
  }

void XC::BeamContact2D::update_contact_state(void)
  {
    was_inContact= (mGap < mGapTol);
    in_bounds= ((mXi > 0.000) && (mXi < 1.000));
    to_be_released= (should_be_released || !in_bounds);
    inContact= (was_inContact && !to_be_released && in_bounds);
  }

//! @brief Constructors
XC::BeamContact2D::BeamContact2D(int tag, int Nd1, int Nd2, int NdS, int NdL, ContactMaterial2D &theMat, double width, double tolG, double tolF, int cSwitch)
  : ElemWithMaterial<BC2D_NUM_NODE, Contact2D>(tag,ELE_TAG_BeamContact2D, Contact2D(1,theMat)), mLength(0.0), mRadius(width/2.0), mGapTol(tolG), mForceTol(tolF), mIniContact(cSwitch), 
    mTangentStiffness(BC2D_NUM_DOF, BC2D_NUM_DOF),
    mInternalForces(BC2D_NUM_DOF),
    mEye1(BC2D_NUM_DIM, BC2D_NUM_DIM),
    mEyeS(BC2D_NUM_DIM, BC2D_NUM_DIM),
    mg_xi(BC2D_NUM_DIM),
    mNormal(BC2D_NUM_DIM),
    mShape(4),
    mDshape(4),
    mBn(BC2D_NUM_DOF-2),
    mBs(BC2D_NUM_DOF-2),
    ma_1(BC2D_NUM_DIM),
    mb_1(BC2D_NUM_DIM),
    mc_1(BC2D_NUM_DIM),
    mIcrd_a(BC2D_NUM_DIM),
    mIcrd_b(BC2D_NUM_DIM),
    mIcrd_s(BC2D_NUM_DIM),
    mDcrd_a(BC2D_NUM_DIM),
    mDcrd_b(BC2D_NUM_DIM),
    mDcrd_s(BC2D_NUM_DIM),
    mDisp_a_n(3),
    mDisp_b_n(3)
  {
    this->theNodes.set_id_nodes(Nd1,Nd2,NdS,NdL);

    setup_contact_state();
  }

XC::BeamContact2D::BeamContact2D(int tag, const ContactMaterial2D *ptr_mat)
  :ElemWithMaterial<BC2D_NUM_NODE, Contact2D>(tag,ELE_TAG_BeamContact2D,Contact2D(1,ptr_mat)), mLength(0.0), mRadius(0.0), mGapTol(0.0), mForceTol(0.0), mIniContact(1),
   mTangentStiffness(BC2D_NUM_DOF, BC2D_NUM_DOF),
   mInternalForces(BC2D_NUM_DOF),
   mEye1(BC2D_NUM_DIM, BC2D_NUM_DIM),
   mEyeS(BC2D_NUM_DIM, BC2D_NUM_DIM),
   mg_xi(BC2D_NUM_DIM),
   mNormal(BC2D_NUM_DIM),
   mShape(4),
   mDshape(4),
   mBn(BC2D_NUM_DOF-2),
   mBs(BC2D_NUM_DOF-2),
   ma_1(BC2D_NUM_DIM),
   mb_1(BC2D_NUM_DIM),
   mc_1(BC2D_NUM_DIM),
   mIcrd_a(BC2D_NUM_DIM),
   mIcrd_b(BC2D_NUM_DIM),
   mIcrd_s(BC2D_NUM_DIM),
   mDcrd_a(BC2D_NUM_DIM),
   mDcrd_b(BC2D_NUM_DIM),
   mDcrd_s(BC2D_NUM_DIM),
   mDisp_a_n(3),
   mDisp_b_n(3)
  {
    setup_contact_state();
  }

//! @brief Virtual constructor.
XC::Element *XC::BeamContact2D::getCopy(void) const
  { return new BeamContact2D(*this); }

int XC::BeamContact2D::getNumDOF(void) const
  { return BC2D_NUM_DOF; }

void XC::BeamContact2D::setWidth(const double &d)
  { mRadius= d/2; }

double XC::BeamContact2D::getWidth(void) const
  { return 2.0*mRadius; }

void XC::BeamContact2D::setGapTolerance(const double &d)
  { this->mGapTol= d; }

double XC::BeamContact2D::getGapTolerance(void) const
  { return this->mGapTol; }

void XC::BeamContact2D::setForceTolerance(const double &d)
  { mForceTol= d; }

double XC::BeamContact2D::getForceTolerance(void) const
  { return mForceTol; }

void XC::BeamContact2D::setInitialContactFlag(const int &i)
  {
    mIniContact= i;
  }

double XC::BeamContact2D::getInitialContactFlag(void) const
  { return mIniContact; }

bool XC::BeamContact2D::initialize_parameters(void) const
  {
    bool retval= false;
    if((mRadius>0) and (mLength==0.0)) // if width assigned and not
      {                                // already initialized.
        setup_contact_state();
	// initialize coordinate vectors
	mIcrd_a= theNodes[0]->getCrds();
	mIcrd_b= theNodes[1]->getCrds();
	mIcrd_s= theNodes[2]->getCrds();
	mDcrd_a= mIcrd_a;
	mDcrd_b= mIcrd_b;
	mDcrd_s= mIcrd_s;
	mDisp_a_n.Zero();
	mDisp_b_n.Zero();

	// length of beam element
	mLength= (mDcrd_b - mDcrd_a).Norm();

	// initialize tangent vectors at beam nodes
	ma_1= (mDcrd_b - mDcrd_a)/mLength;
	mb_1= ma_1;

	// perform projection of secondary node to beam centerline
	mXi= ((mDcrd_b - mDcrd_s)^(mDcrd_b - mDcrd_a))/mLength;  // initial assumption
	mXi= Project(mXi);                                       // actual location

	// initialize contact state based on projection
	in_bounds= ((mXi > 0.000) && (mXi < 1.000));
	inContact= (was_inContact && in_bounds);

	// centerline projection coordinate
	const Vector x_c= mDcrd_a*mShape(0) + ma_1*mLength*mShape(1) + mDcrd_b*mShape(2) + mb_1*mLength*mShape(3);

	// update surface tangent vector, g_xi
	UpdateBase(mXi);

	// adjust cohesion force
	ContactMaterial2D *contactMaterial= const_cast<ContactMaterial2D *>(physicalProperties.getContactMaterial());
	contactMaterial->ScaleCohesion(mLength);
	contactMaterial->ScaleTensileStrength(mLength);

	// compute vectors Bn and Bs
	ComputeB();
	retval= true;
      }
    return retval;
  }

void XC::BeamContact2D::setDomain(Domain *theDomain)
  {
    setup_contact_state();
    mEye1.Zero();
    mEye1(0,0)= 1.0;
    mEye1(1,1)= 1.0;

    mEyeS.Zero();
    mEyeS(0,1)= -1.0;
    mEyeS(1,0)= 1.0;

    // call the base class method
    ElemWithMaterial<BC2D_NUM_NODE, Contact2D>::setDomain(theDomain);

    initialize_parameters();
  }

int XC::BeamContact2D::commitState(void)
  {
    // update projection
    mXi= Project(mXi);

    // update surface tangent vector, g_xi
    UpdateBase(mXi);

    // update vectors Bn and Bs for next step
    ComputeB();

    //update contact state
    update_contact_state();

    int retVal= 0;
    // call element commitState to do any base class stuff
    if ((retVal= this->ElemWithMaterial<BC2D_NUM_NODE, Contact2D>::commitState()) != 0)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; failed in base class."
		  << Color::def << std::endl;
      }
    retVal= physicalProperties.commitState();
    return retVal;
  }

int XC::BeamContact2D::revertToLastCommit()
  {
    return physicalProperties.revertToLastCommit();
  }

int XC::BeamContact2D::revertToStart()
  {
    setup_contact_state();

    // reset applicable member variables 
    mDcrd_a= mIcrd_a;
    mDcrd_b= mIcrd_b;
    mDcrd_s= mIcrd_s;
    mDisp_a_n.Zero();
    mDisp_b_n.Zero();

    mLength= (mDcrd_b - mDcrd_a).Norm();
    
    ma_1= (mDcrd_b - mDcrd_a)/mLength;
    mb_1= ma_1;

    mXi= ((mDcrd_b - mDcrd_s)^(mDcrd_b - mDcrd_a))/mLength;
    mXi= Project(mXi);

    in_bounds= ((mXi > 0.000) && (mXi < 1.000));
    inContact= (was_inContact && in_bounds);

    UpdateBase(mXi);
    ComputeB();

    return physicalProperties.revertToStart();
  }

//! @brief This function updates variables for an incremental step n to n+1
int XC::BeamContact2D::update(void)
  {
    // update secondary node coordinates
    mDcrd_s= mIcrd_s + theNodes[2]->getTrialDisp();

    // update Lagrange multiplier value
    const Vector &disp_L= theNodes[3]->getTrialDisp();
    mLambda= disp_L(0);

    // update nodal coordinates
    const Vector &disp_a= theNodes[0]->getTrialDisp();
    const Vector &disp_b= theNodes[1]->getTrialDisp();
    
    for (int i= 0; i < 2; i++)
      {
	mDcrd_a(i)= mIcrd_a(i) + disp_a(i);
	mDcrd_b(i)= mIcrd_b(i) + disp_b(i);
      }

	// compute incremental rotation from step n to step n+1
    const double rot_a= disp_a(2) - mDisp_a_n(2);
    const double rot_b= disp_b(2) - mDisp_b_n(2);

    // get tangent vectors from last converged step
    const Vector &a1_n= Geta1();
    const Vector &b1_n= Getb1();

    // linear update of tangent vectors
    const Vector a1= a1_n + rot_a*mEyeS*a1_n;
    const Vector b1= b1_n + rot_b*mEyeS*b1_n;

    // update centerline projection coordinate
    const Vector x_c= mDcrd_a*mShape(0) + a1*mLength*mShape(1) + mDcrd_b*mShape(2) + b1*mLength*mShape(3);

    // update gap
    mGap= (mNormal^(mDcrd_s - x_c)) - mRadius;

    // get tensile strength from contact material
    const double tensileStrength= physicalProperties.getContactMaterial()->getTensileStrength();

    // set the boolean release condition
    should_be_released= (mLambda <= -mForceTol);

    // determine trial strain vector based on contact state
    Vector strain(3);
    if (inContact)
      {
        // tangent at the centerline projection in step n+1
	const Vector c1n1= mDshape(0)*mDcrd_a + mDshape(1)*mLength*ma_1 + mDshape(2)*mDcrd_b + mDshape(3)*mLength*mb_1;

	// update vector c2 for step n+1
	Vector c2n1= (mDcrd_s - x_c)/((mDcrd_s - x_c).Norm());
		
	// update vector c2 for step n+1
	c2n1(0)= -c1n1(1);
	c2n1(1)= c1n1(0);

	// compute the slip
	const double slip= mg_xi^(mDcrd_s - x_c - mrho*c2n1);

	// set the strain vector
	strain(0)= mGap;
	strain(1)= slip;
	strain(2)= mLambda;
		
	physicalProperties[0]->setTrialStrain(strain);
      }	
    else if(to_be_released)
      {
	Vector strain(3);

	// set the strain vector
	strain(0)= mGap;
	strain(1)= 0.0;
	strain(2)= mLambda;
		
	physicalProperties[0]->setTrialStrain(strain);
      }
    return 0;
  }

//! @brief Method to determine centerline projectionç
double XC::BeamContact2D::Project(double xi) const
  {
    // initialize to previous projection location
    double xi_p= xi;

    // update end point tangents
    UpdateEndFrames();

    // set tangent vectors
    const Vector &a1= Geta1();
    const Vector &b1= Getb1();
    
    // Hermitian basis functions and first derivatives
    double H1= 1.0 - 3.0*xi_p*xi_p + 2.0*xi_p*xi_p*xi_p;
    double H2= xi_p - 2.0*xi_p*xi_p + xi_p*xi_p*xi_p;
    double H3= 3.0*xi_p*xi_p - 2*xi_p*xi_p*xi_p;
    double H4= -xi_p*xi_p + xi_p*xi_p*xi_p;
    double dH1= -6.0*xi_p + 6.0*xi_p*xi_p;
    double dH2= 1.0 - 4.0*xi_p + 3.0*xi_p*xi_p;
    double dH3= 6.0*xi_p - 6.0*xi_p*xi_p;
    double dH4= -2.0*xi_p + 3.0*xi_p*xi_p;

    // compute current projection coordinate and tangent
    Vector x_c_p= mDcrd_a*H1 + a1*mLength*H2 + mDcrd_b*H3 + b1*mLength*H4;
    Vector t_c= mDcrd_a*dH1 + a1*mLength*dH2 + mDcrd_b*dH3 + b1*mLength*dH4;
	
    // compute initial value of residual
    double R= (mDcrd_s - x_c_p)^t_c;

    // iterate to determine new value of xi
    int Gapcount= 0;
    double DR;
    double dxi;
    Vector ddx_c(BC2D_NUM_DIM);
    while (fabs(R/mLength) > mGapTol && Gapcount < 50)
      {
	// compute current curvature vector
	ddx_c= Get_dxc_xixi(xi_p);

	// increment projection location
	DR= ((mDcrd_s - x_c_p)^ddx_c) - (t_c^t_c);
	dxi= -R/DR;
	xi_p= xi_p + dxi;

	// Hermitian basis functions and first derivatives
	H1= 1.0 - 3.0*xi_p*xi_p + 2.0*xi_p*xi_p*xi_p;
    	H2= xi_p - 2.0*xi_p*xi_p + xi_p*xi_p*xi_p;
    	H3= 3.0*xi_p*xi_p - 2*xi_p*xi_p*xi_p;
    	H4= -xi_p*xi_p + xi_p*xi_p*xi_p;
    	dH1= -6.0*xi_p + 6.0*xi_p*xi_p;
    	dH2= 1.0 - 4.0*xi_p + 3.0*xi_p*xi_p;
    	dH3= 6.0*xi_p - 6.0*xi_p*xi_p;
    	dH4= -2.0*xi_p + 3.0*xi_p*xi_p;

	// update projection coordinate and tangent
	x_c_p= mDcrd_a*H1 + a1*mLength*H2 + mDcrd_b*H3 + b1*mLength*H4;
	t_c= mDcrd_a*dH1 + a1*mLength*dH2 + mDcrd_b*dH3 + b1*mLength*dH4;

	// compute residual
    	R= (mDcrd_s - x_c_p)^t_c;

	Gapcount += 1;
      }

    // update normal vector for current projection
    mNormal= (mDcrd_s - x_c_p)/((mDcrd_s - x_c_p).Norm());

    // update Hermitian basis functions and derivatives
    mShape(0)= H1;
    mShape(1)= H2;
    mShape(2)= H3;
    mShape(3)= H4;
    mDshape(0)= dH1;
    mDshape(1)= dH2;
    mDshape(2)= dH3;
    mDshape(3)= dH4;

    return xi_p;
  }

//! @brief method to update base vector g_xi
int XC::BeamContact2D::UpdateBase(double xi) const
  {	
    // compute current projection tangent
    const Vector t_c= Get_dxc_xi(xi);

    // set value of unit tangent vector c1
    mc_1= t_c/t_c.Norm();

    // compute current projection curvature
    const Vector ddx_c= Get_dxc_xixi(xi);

    // compute derivative of c1 with respect to xi
    const Vector d_c1= (1/t_c.Norm())*(ddx_c - ((mc_1^ddx_c)*mc_1));

    // determine orthogonal vector c2
    Vector c_2(BC2D_NUM_DIM);
    c_2(0)= -mc_1(1);  c_2(1)= mc_1(0);

    // compute local coordinate transformation term
    mrho= mRadius*(mNormal^c_2);

    // compute change in c2 with xi
    const Vector d_c2= -(d_c1^c_2)*mc_1;

    // compute surface tangent vector
    mg_xi= t_c + mrho*d_c2;

    return 0;
  }

//! @brief This function computes the first derivative of x_c wrt xi
XC::Vector XC::BeamContact2D::Get_dxc_xi(double xi) const
  {
    // first derivatives of Hermitian basis functions
    const double dH1= -6.0*xi + 6.0*xi*xi;
    const double dH2= 1.0 - 4.0*xi + 3.0*xi*xi;
    const double dH3= 6.0*xi - 6.0*xi*xi;
    const double dH4= -2.0*xi + 3.0*xi*xi;

    // tangent vectors
    const Vector &a1= Geta1();
    const Vector &b1= Getb1();

    // compute current projection tangent
    const Vector dx= mDcrd_a*dH1 + a1*mLength*dH2 + mDcrd_b*dH3 + b1*mLength*dH4;

    return dx;
  }

//! @brief this function computes the second derivative of x_c wrt xi
XC::Vector XC::BeamContact2D::Get_dxc_xixi(double xi) const
  {
    // second derivatives of Hermitian basis functions
    const double ddH1= -6.0 + 12.0*xi;
    const double ddH2= -4.0 + 6.0*xi;
    const double ddH3=  6.0 - 12.0*xi;
    const double ddH4= -2.0 + 6.0*xi;

    // tangent vectors
    const Vector &a1= Geta1();
    const Vector &b1= Getb1();

	// compute current curvature vector
    Vector ddx= mDcrd_a*ddH1 + a1*mLength*ddH2 + mDcrd_b*ddH3 + b1*mLength*ddH4;

    return ddx;
  }

//! @brief Method to compute Bn and Bs @ step n
void XC::BeamContact2D::ComputeB(void) const
  {
    // initialize Bn and Bs
    mBn.Zero();
    mBs.Zero();

	// get tangent vectors
    Vector a1= Geta1();
    Vector b1= Getb1();

	// compute terms for vector Bn
    double Ka1n= (mEyeS*a1)^mNormal;
    double Kb1n= (mEyeS*b1)^mNormal;
	
    mBn(0)= -mShape(0)*mNormal(0);
    mBn(1)= -mShape(0)*mNormal(1);
    mBn(2)= -mShape(1)*mLength*Ka1n;
    mBn(3)= -mShape(2)*mNormal(0);
    mBn(4)= -mShape(2)*mNormal(1);
    mBn(5)= -mShape(3)*mLength*Kb1n;
    mBn(6)= mNormal(0);
    mBn(7)= mNormal(1);
    
    // compute terms for vector Bs
    double Ka1g= (mEyeS*a1)^mg_xi;
    double Kb1g= (mEyeS*b1)^mg_xi;

    mBs(0)= -mg_xi(0)*(mShape(0) + mRadius*mDshape(0));
    mBs(1)= -mg_xi(1)*(mShape(0) + mRadius*mDshape(0));
    mBs(2)= -Ka1g*mLength*(mShape(1) + mRadius*mDshape(1));
    mBs(3)= -mg_xi(0)*(mShape(2) + mRadius*mDshape(2));
    mBs(4)= -mg_xi(1)*(mShape(2) + mRadius*mDshape(2));
    mBs(5)= -Kb1g*mLength*(mShape(3) + mRadius*mDshape(3));
    mBs(6)= mg_xi(0);
    mBs(7)= mg_xi(1);

    return;
  }

//! @brief Method to update end node tangent vectors
void XC::BeamContact2D::UpdateEndFrames(void) const
  {
    // recalculate incremental rotations from step n to n+1
    const Vector &disp_a= theNodes[0]->getTrialDisp();
    const Vector &disp_b= theNodes[1]->getTrialDisp();
    const double rot_a= disp_a(2) - mDisp_a_n(2);
    const double rot_b= disp_b(2) - mDisp_b_n(2);

    // perform update for node a
    ma_1 += rot_a*mEyeS*ma_1;

    // perform update for node b
    mb_1 += rot_b*mEyeS*mb_1;

    // update displacement vectors for next step
    mDisp_a_n= disp_a;
    mDisp_b_n= disp_b;

    return;
  } 

//! @brief This function returns the tangent vector at node a from
//! last converged step
XC::Vector XC::BeamContact2D::Geta1(void) const
  {
     const Vector a1(ma_1);
     return a1;
  }

//! @brief This function returns the tangent vector at node a from
//! last converged step
XC::Vector XC::BeamContact2D::Getb1(void) const
  {
    const Vector b1(mb_1);
    return b1;
  }

//! @brief This function computes the tangent stiffness matrix for the element
const XC::Matrix &XC::BeamContact2D::getTangentStiff(void) const
  {
    mTangentStiffness.Zero();
    bool initializeOK= initialize_parameters();
    if(initializeOK)
      {
	if(inContact)
	  {
	    const Matrix &Cmat= physicalProperties[0]->getTangent();

	    const double &Css= Cmat(1,1);
	    const double &Csn= Cmat(1,2);
	    for(int i= 0; i < BC2D_NUM_DOF-2; i++)
	      {
		for(int j= 0; j < BC2D_NUM_DOF-2; j++)
		  {
		    mTangentStiffness(i,j)= mBs(i)*mBs(j)*Css;
		  }
	      }
	    for(int i= 0; i < BC2D_NUM_DOF-2; i++)
	      {
		mTangentStiffness(8,i)= -mBn(i);
		mTangentStiffness(i,8)= -mBn(i) + Csn*mBs(i);
	      }
	    mTangentStiffness(9,9)= 1.0;

	  }
	else
	  {
	    mTangentStiffness(8,8)= 1.0;
	    mTangentStiffness(9,9)= 1.0;
	  }
      }
    else
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; something went wrong in initialization. "
	          << Color::def << std::endl;
	exit(1);
      }
    return mTangentStiffness;
  }

//! @brief This function computes the initial tangent stiffness matrix for
//! the element
const XC::Matrix &XC::BeamContact2D::getInitialStiff(void) const
  { return this->getTangentStiff(); }

void XC::BeamContact2D::zeroLoad(void)
  {}

int XC::BeamContact2D::addLoad(ElementalLoad *theLoad, double loadFactor)
  { return 0; }

int XC::BeamContact2D::addInertiaLoadToUnbalance(const Vector &accel)
  { return 0; }

//! @brief This function computes the resisting force vector for the element
const XC::Vector &XC::BeamContact2D::getResistingForce(void) const
  {
    mInternalForces.Zero();

    // get contact "stress" vector
    const Vector &stress= physicalProperties[0]->getStress();

    if(inContact)
      {
	for(int i= 0; i < BC2D_NUM_DOF - 2; i++)
	  {
	    mInternalForces(i)= -mLambda*mBn(i) + stress(1)*mBs(i);
	  }

	mInternalForces(BC2D_NUM_DOF - 2)= -mGap;

      }
    else
      {
	mInternalForces(BC2D_NUM_DOF - 2)= mLambda;
      }

    return mInternalForces;
  }

const XC::Vector &XC::BeamContact2D::getResistingForceIncInertia(void) const
  { return getResistingForce(); }

//! @brief Send object members through the communicator argument.
int XC::BeamContact2D::sendData(Communicator &comm)
  {
    int res= ElemWithMaterial<BC2D_NUM_NODE, Contact2D>::sendData(comm);
    res+=comm.sendDouble(mGapTol,getDbTagData(),CommMetaData(8));
    res+=comm.sendInt(mIniContact,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Send object members through the communicator argument.
int XC::BeamContact2D::recvData(const Communicator &comm)
  {
    int res= ElemWithMaterial<BC2D_NUM_NODE, Contact2D>::recvData(comm);
    res+=comm.receiveDouble(mGapTol,getDbTagData(),CommMetaData(8));
    res+=comm.receiveInt(mIniContact,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::BeamContact2D::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(10);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data"
	        << Color::def << std::endl;
    return res;
  }


//! @brief Receives object through the communicator argument.
int XC::BeamContact2D::recvSelf(const Communicator &comm)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids."
	        << Color::def << std::endl;
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive data."
	        << Color::def << std::endl;
      }
    return res;
  }

void XC::BeamContact2D::Print(std::ostream &s, int flag) const
  {
	s << "BeamContact2D, element id:  " << this->getTag() << std::endl;
	s << "   Connected external nodes:  ";
	s << theNodes << std::endl;
	return;
  }

XC::Response *XC::BeamContact2D::setResponse(const std::vector<std::string> &argv, Parameter &param)
  {
    if((argv[0] == "force") || (argv[0] == "forces") )
      {
	// forces on secondary node
	return new ElementResponse(this, 1, Vector(2));

      }
    else if((argv[0] == "frictionforce") || (argv[0] == "frictionforces") )
      {
	// frictional force vector
	return new ElementResponse(this, 2, Vector(2));
      }
    else if ((argv[0] == "forcescalar") || (argv[0] == "forcescalars") )
      {
	// scalar contact forces
	return new ElementResponse(this, 3, Vector(2));

      }
    else if ((argv[0] == "masterforce") || (argv[0] == "masterforces") ||
	     (argv[0] == "primaryforce") || (argv[0] == "primaryforces") )
      {
	// reactions (forces and moments) on primary nodes
	return new ElementResponse(this, 4, Vector(6));

      }
    else
      {
	// otherwise response quantity is unknown for the BeamContact2D class
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "(const char **argv, int argc, OPS_Stream &eleInfo): "
		  << argv[0] << " unknown request"
		  << Color::def << std::endl;
	return 0;
      }
  }

int XC::BeamContact2D::getResponse(int responseID, Information &eleInfo)
  {
    // get contact "stress" vector
    const Vector &stress= physicalProperties[0]->getStress();

    if (responseID == 1)
      {
	// forces on secondary node
	Vector secondaryForce(2);
	secondaryForce(0)= -mInternalForces(6);
	secondaryForce(1)= -mInternalForces(7);
	return eleInfo.setVector(secondaryForce);
      }
    else if (responseID == 2)
      {
	Vector frictForce(2);
	// frictional force vector
	frictForce= stress(1)*mg_xi;
	return eleInfo.setVector(frictForce);
      }
    else if (responseID == 3)
      {
	// scalar contact forces
	Vector force(2);
	force(0)= stress(0);
	force(1)= stress(1);
	return eleInfo.setVector(force);
      }
    else if (responseID == 4)
      {
	Vector primaryForce(6);
	// reactions (forces and moments) on primary nodes
	for (int i= 0;  i < 3; i++)
	  {
	    primaryForce(i)= -mInternalForces(i);
	    primaryForce(i+3)= -mInternalForces(i+3);
	  }
	return eleInfo.setVector(primaryForce);

      }
    else
      // otherwise response quantity is unknown for the BeamContact2D class
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "(int responseID= " << responseID
		<< ", Information &eleInfo); "
		<< " unknown request"
		<< Color::def << std::endl;
    return -1;
  }

int XC::BeamContact2D::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    const size_t argc= argv.size();
    if (argc < 1)
      return -1;

    if (argv[0] == "friction")
      { return param.addObject(1, this); }
	
    return -1;
  }

int XC::BeamContact2D::updateParameter(int parameterID, Information &info)
  {
    int res= -1;
    int matRes=  physicalProperties[0]->updateParameter(parameterID, info);
    if (matRes != -1)
      {
	res= matRes;
      }
    return res;
  }
