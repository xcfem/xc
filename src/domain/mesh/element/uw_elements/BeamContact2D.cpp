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
#include <domain/mesh/node/Node.h>
#include "material/nD/uw_materials/ContactMaterial2D.h"


// constructors
XC::BeamContact2D::BeamContact2D(int tag, int Nd1, int Nd2, int NdS, int NdL, ContactMaterial2D &theMat, double width, double tolG, double tolF, int cSwitch)
  : ElemWithMaterial<4, Contact2D>(tag,ELE_TAG_BeamContact2D, Contact2D(1,theMat)),
    mTangentStiffness(BC2D_NUM_DOF, BC2D_NUM_DOF),
    mInternalForces(BC2D_NUM_DOF),
    mIniContact(cSwitch),
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


    mRadius = width/2;
    mGapTol = tolG;
    mForceTol = tolF;
    mIniContact = cSwitch;

    if (mIniContact == 0)
      {
	inContact          = true;
	was_inContact      = true;
	to_be_released     = false;
	should_be_released = false;
	in_bounds          = true;

      }
    else
      {
	inContact          = false;
	was_inContact      = false;
	to_be_released     = false;
	should_be_released = false;
	in_bounds          = true;
      }

    mGap    = 0.0;
    mLambda = 0.0;

  }

XC::BeamContact2D::BeamContact2D(int tag, const ContactMaterial2D *ptr_mat)
  :ElemWithMaterial<4, Contact2D>(tag,ELE_TAG_BeamContact2D,Contact2D(1,ptr_mat)),
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
  {}

int XC::BeamContact2D::getNumDOF(void)
  { return BC2D_NUM_DOF; }

void XC::BeamContact2D::setDomain(Domain *theDomain)
  {
    Vector x_c(BC2D_NUM_DIM);
	
    mEye1.Zero();
    mEye1(0,0) = 1.0;
    mEye1(1,1) = 1.0;

    mEyeS.Zero();
    mEyeS(0,1) = -1.0;
    mEyeS(1,0) = 1.0;

    ElemWithMaterial<BC2D_NUM_NODE, Contact2D>::setDomain(theDomain);

    // initialize coordinate vectors
    mIcrd_a = theNodes[0]->getCrds();
    mIcrd_b = theNodes[1]->getCrds();
    mIcrd_s = theNodes[2]->getCrds();
    mDcrd_a = mIcrd_a;
    mDcrd_b = mIcrd_b;
    mDcrd_s = mIcrd_s;
    mDisp_a_n.Zero();
    mDisp_b_n.Zero();

    // length of beam element
    mLength = (mDcrd_b - mDcrd_a).Norm();

    // initialize tangent vectors at beam nodes
    ma_1 = (mDcrd_b - mDcrd_a)/mLength;
    mb_1 = ma_1;

    // perform projection of secondary node to beam centerline
    mXi = ((mDcrd_b - mDcrd_s)^(mDcrd_b - mDcrd_a))/mLength;  // initial assumption
    mXi = Project(mXi);                                       // actual location

    // initialize contact state based on projection
    in_bounds = ((mXi > 0.000) && (mXi < 1.000));
    inContact = (was_inContact && in_bounds);

    // centerline projection coordinate
    x_c = mDcrd_a*mShape(0) + ma_1*mLength*mShape(1) + mDcrd_b*mShape(2) + mb_1*mLength*mShape(3);

    // update surface tangent vector, g_xi
    UpdateBase(mXi);

    // adjust cohesion force
    ContactMaterial2D *contactMaterial= physicalProperties.getContactMaterial();
    contactMaterial->ScaleCohesion(mLength);
    contactMaterial->ScaleTensileStrength(mLength);

    // compute vectors Bn and Bs
    ComputeB();

    // call the base class method
    this->DomainComponent::setDomain(theDomain);
  }

int XC::BeamContact2D::commitState(void)
  {
    // update projection
    mXi = Project(mXi);

    // update surface tangent vector, g_xi
    UpdateBase(mXi);

    // update vectors Bn and Bs for next step
    ComputeB();

    //update contact state 
    was_inContact =  (mGap < mGapTol);
    in_bounds =      ((mXi > 0.000) && (mXi < 1.000));
    to_be_released = (should_be_released || !in_bounds);
    inContact =      (was_inContact && !to_be_released && in_bounds);

    int retVal = 0;
    // call element commitState to do any base class stuff
    if ((retVal = this->ElemWithMaterial<BC2D_NUM_NODE, Contact2D>::commitState()) != 0) {
	    std::cerr << "XC::BeamContact2D::commitState() - failed in base class";
	}
	retVal = physicalProperties.commitState();

	return retVal;
}

int XC::BeamContact2D::revertToLastCommit()
  {
    return physicalProperties.revertToLastCommit();
  }

int XC::BeamContact2D::revertToStart()
  {
	if (mIniContact == 0) {
		inContact          = true;
		was_inContact      = true;
		to_be_released     = false;
		should_be_released = false;
		in_bounds          = true;
	} else {
		inContact          = false;
		was_inContact      = false;
		to_be_released     = false;
		should_be_released = false;
		in_bounds          = true;
	}

	// reset applicable member variables 
	mDcrd_a = mIcrd_a;
	mDcrd_b = mIcrd_b;
	mDcrd_s = mIcrd_s;
	mDisp_a_n.Zero();
	mDisp_b_n.Zero();

	mLength = (mDcrd_b - mDcrd_a).Norm();

	ma_1 = (mDcrd_b - mDcrd_a)/mLength;
	mb_1 = ma_1;

	mXi = ((mDcrd_b - mDcrd_s)^(mDcrd_b - mDcrd_a))/mLength;
	mXi = Project(mXi);

	in_bounds = ((mXi > 0.000) && (mXi < 1.000));
	inContact = (was_inContact && in_bounds);

	UpdateBase(mXi);
	ComputeB();

	return physicalProperties.revertToStart();
}

int XC::BeamContact2D::update(void)
// this function updates variables for an incremental step n to n+1
{
    double tensileStrength;
	Vector a1(BC2D_NUM_DIM);
    Vector b1(BC2D_NUM_DIM);
	Vector a1_n(BC2D_NUM_DIM);
    Vector b1_n(BC2D_NUM_DIM);
    Vector disp_a(3);
    Vector disp_b(3);
    Vector disp_L(BC2D_NUM_DIM);
    double rot_a;
    double rot_b;
    Vector x_c(BC2D_NUM_DIM);

	// update secondary node coordinates
	mDcrd_s = mIcrd_s + theNodes[2]->getTrialDisp();

	// update Lagrange multiplier value
	disp_L  = theNodes[3]->getTrialDisp();
	mLambda = disp_L(0);

	// update nodal coordinates
	disp_a = theNodes[0]->getTrialDisp();
	disp_b = theNodes[1]->getTrialDisp();

	for (int i = 0; i < 2; i++) {
	    mDcrd_a(i) = mIcrd_a(i) + disp_a(i);
		mDcrd_b(i) = mIcrd_b(i) + disp_b(i);
	}

	// compute incremental rotation from step n to step n+1
	rot_a = disp_a(2) - mDisp_a_n(2);
	rot_b = disp_b(2) - mDisp_b_n(2);

	// get tangent vectors from last converged step
	a1_n = Geta1();
	b1_n = Getb1();

	// linear update of tangent vectors
	a1 = a1_n + rot_a*mEyeS*a1_n;
	b1 = b1_n + rot_b*mEyeS*b1_n;

	// update centerline projection coordinate
	x_c = mDcrd_a*mShape(0) + a1*mLength*mShape(1) + mDcrd_b*mShape(2) + b1*mLength*mShape(3);

	// update gap
	mGap = (mNormal^(mDcrd_s - x_c)) - mRadius;

	// get tensile strength from contact material
	tensileStrength = physicalProperties.getContactMaterial()->getTensileStrength();

	// set the boolean release condition
	should_be_released = (mLambda <= -mForceTol);

    // determine trial strain vector based on contact state
	if (inContact) {
	    Vector strain(3);
		double slip;
		Vector c1n1(2);
		Vector c2n1(2);

        // tangent at the centerline projection in step n+1
		c1n1 = mDshape(0)*mDcrd_a + mDshape(1)*mLength*ma_1 + mDshape(2)*mDcrd_b + mDshape(3)*mLength*mb_1;

		// update vector c2 for step n+1
		c2n1 = (mDcrd_s - x_c)/((mDcrd_s - x_c).Norm());
		
		// update vector c2 for step n+1
		c2n1(0) = -c1n1(1);
		c2n1(1) = c1n1(0);

		// compute the slip
		slip = mg_xi^(mDcrd_s - x_c - mrho*c2n1);

		// set the strain vector
		strain(0) = mGap;
		strain(1) = slip;
		strain(2) = mLambda;
		
		physicalProperties[0]->setTrialStrain(strain);
	}
	
	else if (to_be_released) {
	    Vector strain(3);

        // set the strain vector
		strain(0) = mGap;
		strain(1) = 0.0;
		strain(2) = mLambda;
		
		physicalProperties[0]->setTrialStrain(strain);
	}

	return 0;
}

double XC::BeamContact2D::Project(double xi)
// this function computes the centerline projection for the current step
{
    double xi_p;
	double H1;
	double H2;
	double H3;
	double H4;
    double dH1;
	double dH2;
	double dH3;
	double dH4;
	double R;
	double DR;
	double dxi;
	Vector a1(BC2D_NUM_DIM);
    Vector b1(BC2D_NUM_DIM);
	Vector x_c_p(BC2D_NUM_DIM);
	Vector t_c(BC2D_NUM_DIM);
	Vector ddx_c(BC2D_NUM_DIM);

	// initialize to previous projection location
	xi_p = xi;

	// update end point tangents
	UpdateEndFrames();

	// set tangent vectors
	a1 = Geta1();
	b1 = Getb1();

	// Hermitian basis functions and first derivatives
	H1 = 1.0 - 3.0*xi_p*xi_p + 2.0*xi_p*xi_p*xi_p;
	H2 = xi_p - 2.0*xi_p*xi_p + xi_p*xi_p*xi_p;
	H3 = 3.0*xi_p*xi_p - 2*xi_p*xi_p*xi_p;
	H4 = -xi_p*xi_p + xi_p*xi_p*xi_p;
	dH1 = -6.0*xi_p + 6.0*xi_p*xi_p;
	dH2 = 1.0 - 4.0*xi_p + 3.0*xi_p*xi_p;
	dH3 = 6.0*xi_p - 6.0*xi_p*xi_p;
	dH4 = -2.0*xi_p + 3.0*xi_p*xi_p;

    // compute current projection coordinate and tangent
	x_c_p = mDcrd_a*H1 + a1*mLength*H2 + mDcrd_b*H3 + b1*mLength*H4;
	t_c   = mDcrd_a*dH1 + a1*mLength*dH2 + mDcrd_b*dH3 + b1*mLength*dH4;
	
	// compute initial value of residual
	R = (mDcrd_s - x_c_p)^t_c;

	// iterate to determine new value of xi
	int Gapcount = 0;
	while (fabs(R/mLength) > mGapTol && Gapcount < 50) {
	
		// compute current curvature vector
		ddx_c = Get_dxc_xixi(xi_p);

		// increment projection location
		DR   = ((mDcrd_s - x_c_p)^ddx_c) - (t_c^t_c);
		dxi  = -R/DR;
		xi_p = xi_p + dxi;

		// Hermitian basis functions and first derivatives
	    H1 = 1.0 - 3.0*xi_p*xi_p + 2.0*xi_p*xi_p*xi_p;
    	H2 = xi_p - 2.0*xi_p*xi_p + xi_p*xi_p*xi_p;
    	H3 = 3.0*xi_p*xi_p - 2*xi_p*xi_p*xi_p;
    	H4 = -xi_p*xi_p + xi_p*xi_p*xi_p;
    	dH1 = -6.0*xi_p + 6.0*xi_p*xi_p;
    	dH2 = 1.0 - 4.0*xi_p + 3.0*xi_p*xi_p;
    	dH3 = 6.0*xi_p - 6.0*xi_p*xi_p;
    	dH4 = -2.0*xi_p + 3.0*xi_p*xi_p;

		// update projection coordinate and tangent
		x_c_p = mDcrd_a*H1 + a1*mLength*H2 + mDcrd_b*H3 + b1*mLength*H4;
	    t_c   = mDcrd_a*dH1 + a1*mLength*dH2 + mDcrd_b*dH3 + b1*mLength*dH4;

		// compute residual
    	R = (mDcrd_s - x_c_p)^t_c;

		Gapcount += 1;
	}

	// update normal vector for current projection
	mNormal = (mDcrd_s - x_c_p)/((mDcrd_s - x_c_p).Norm());

	// update Hermitian basis functions and derivatives
	mShape(0)  = H1;
	mShape(1)  = H2;
	mShape(2)  = H3;
	mShape(3)  = H4;
	mDshape(0) = dH1;
	mDshape(1) = dH2;
	mDshape(2) = dH3;
	mDshape(3) = dH4;

	return xi_p;
}

int XC::BeamContact2D::UpdateBase(double xi)
// this function computes the surface tangent vector g_xi
{
    Vector t_c(BC2D_NUM_DIM);
	Vector ddx_c(BC2D_NUM_DIM);
	Vector d_c1(BC2D_NUM_DIM);
	Vector c_2(BC2D_NUM_DIM);
	Vector d_c2(BC2D_NUM_DIM);
	
    // compute current projection tangent
	t_c = Get_dxc_xi(xi);

	// set value of unit tangent vector c1
	mc_1 = t_c/t_c.Norm();

	// compute current projection curvature
	ddx_c = Get_dxc_xixi(xi);

	// compute derivative of c1 with respect to xi
	d_c1 = (1/t_c.Norm())*(ddx_c - ((mc_1^ddx_c)*mc_1));

	// determine orthogonal vector c2
	c_2(0) = -mc_1(1);  c_2(1) = mc_1(0);

	// compute local coordinate transformation term
	mrho = mRadius*(mNormal^c_2);

	// compute change in c2 with xi
	d_c2 = -(d_c1^c_2)*mc_1;

    // compute surface tangent vector
	mg_xi = t_c + mrho*d_c2;

	return 0;
}

XC::Vector XC::BeamContact2D::Get_dxc_xi(double xi)
// this function computes the first derivative of x_c wrt xi
{
    double dH1;
	double dH2;
	double dH3;
	double dH4;
	Vector a1(BC2D_NUM_DIM);
	Vector b1(BC2D_NUM_DIM);
	Vector dx(BC2D_NUM_DIM);

	// first derivatives of Hermitian basis functions
    dH1 = -6.0*xi + 6.0*xi*xi;
	dH2 = 1.0 - 4.0*xi + 3.0*xi*xi;
	dH3 = 6.0*xi - 6.0*xi*xi;
	dH4 = -2.0*xi + 3.0*xi*xi;

	// tangent vectors
	a1 = Geta1();
	b1 = Getb1();

    // compute current projection tangent
	dx = mDcrd_a*dH1 + a1*mLength*dH2 + mDcrd_b*dH3 + b1*mLength*dH4;

	return dx;
}

XC::Vector XC::BeamContact2D::Get_dxc_xixi(double xi)
// this function computes the second derivative of x_c wrt xi
{
    double ddH1;
	double ddH2;
	double ddH3;
	double ddH4;
	Vector a1(BC2D_NUM_DIM);
	Vector b1(BC2D_NUM_DIM);
	Vector ddx(BC2D_NUM_DIM);
	
	// second derivatives of Hermitian basis functions
	ddH1 = -6.0 + 12.0*xi;
	ddH2 = -4.0 + 6.0*xi;
	ddH3 =  6.0 - 12.0*xi;
	ddH4 = -2.0 + 6.0*xi;

	// tangent vectors
	a1 = Geta1();
	b1 = Getb1();

	// compute current curvature vector
	ddx = mDcrd_a*ddH1 + a1*mLength*ddH2 + mDcrd_b*ddH3 + b1*mLength*ddH4;

	return ddx;
}

void
XC::BeamContact2D::ComputeB(void)
// this function computes the finite element equation vectors Bn and Bs
{
    double Ka1n;
	double Kb1n;
	double Ka1g;
	double Kb1g;
	Vector a1(BC2D_NUM_DIM);
	Vector b1(BC2D_NUM_DIM);

	// initialize Bn and Bs
	mBn.Zero();
	mBs.Zero();

	// get tangent vectors
	a1 = Geta1();
	b1 = Getb1();

	// compute terms for vector Bn
    Ka1n = (mEyeS*a1)^mNormal;
	Kb1n = (mEyeS*b1)^mNormal;
	
	mBn(0) = -mShape(0)*mNormal(0);
	mBn(1) = -mShape(0)*mNormal(1);
	mBn(2) = -mShape(1)*mLength*Ka1n;
	mBn(3) = -mShape(2)*mNormal(0);
	mBn(4) = -mShape(2)*mNormal(1);
	mBn(5) = -mShape(3)*mLength*Kb1n;
	mBn(6) = mNormal(0);
	mBn(7) = mNormal(1);

	// compute terms for vector Bs
	Ka1g = (mEyeS*a1)^mg_xi;
	Kb1g = (mEyeS*b1)^mg_xi;

	mBs(0) = -mg_xi(0)*(mShape(0) + mRadius*mDshape(0));
	mBs(1) = -mg_xi(1)*(mShape(0) + mRadius*mDshape(0));
	mBs(2) = -Ka1g*mLength*(mShape(1) + mRadius*mDshape(1));
	mBs(3) = -mg_xi(0)*(mShape(2) + mRadius*mDshape(2));
	mBs(4) = -mg_xi(1)*(mShape(2) + mRadius*mDshape(2));
	mBs(5) = -Kb1g*mLength*(mShape(3) + mRadius*mDshape(3));
    mBs(6) = mg_xi(0);
	mBs(7) = mg_xi(1);

	return;
}

void XC::BeamContact2D::UpdateEndFrames(void)
// this function updates the tangent and displacement at nodes a and b
{
	Vector disp_a(3);
	Vector disp_b(3);
	double rot_a;
	double rot_b;

	// recalculate incremental rotations from step n to n+1
	disp_a = theNodes[0]->getTrialDisp();
	disp_b = theNodes[1]->getTrialDisp();
	rot_a  = disp_a(2) - mDisp_a_n(2);
	rot_b  = disp_b(2) - mDisp_b_n(2);

	// perform update for node a
	ma_1 += rot_a*mEyeS*ma_1;

	// perform update for node b
	mb_1 += rot_b*mEyeS*mb_1;

	// update displacement vectors for next step
	mDisp_a_n = disp_a;
	mDisp_b_n = disp_b;

	return;
} 

XC::Vector XC::BeamContact2D::Geta1(void)
// this function returns the tangent vector at node a from last converged step
{
	Vector a1(BC2D_NUM_DIM);

	a1 = ma_1;

	return a1;
}

XC::Vector XC::BeamContact2D::Getb1(void)
// this function returns the tangent vector at node a from last converged step
{
	Vector b1(BC2D_NUM_DIM);

	b1 = mb_1;

	return b1;
}

const XC::Matrix &XC::BeamContact2D::getTangentStiff(void) const
// this function computes the tangent stiffness matrix for the element
{
	mTangentStiffness.Zero();

	if (inContact) {
		Matrix Cmat = physicalProperties[0]->getTangent();

		double Css = Cmat(1,1);
		double Csn = Cmat(1,2);

		for (int i = 0; i < BC2D_NUM_DOF-2; i++) {
			for (int j = 0; j < BC2D_NUM_DOF-2; j++) {

				mTangentStiffness(i,j) = mBs(i)*mBs(j)*Css;
			}
		}
		for (int i = 0; i < BC2D_NUM_DOF-2; i++) {
			mTangentStiffness(8,i) = -mBn(i);
			mTangentStiffness(i,8) = -mBn(i) + Csn*mBs(i);
		}
		mTangentStiffness(9,9) = 1.0;

	} else {
		mTangentStiffness(8,8) = 1.0;
		mTangentStiffness(9,9) = 1.0;
	}

	return mTangentStiffness;
}

const XC::Matrix &XC::BeamContact2D::getInitialStiff(void) const
// this function computes the initial tangent stiffness matrix for the element
{
	return getTangentStiff();
}

void XC::BeamContact2D::zeroLoad(void)
{
	return;
}

int XC::BeamContact2D::addLoad(ElementalLoad *theLoad, double loadFactor)
{
	return 0;
}

int XC::BeamContact2D::addInertiaLoadToUnbalance(const Vector &accel)
{
	return 0;
}

const XC::Vector &XC::BeamContact2D::getResistingForce(void) const
// this function computes the resisting force vector for the element
{
	mInternalForces.Zero();

	// get contact "stress" vector
	Vector stress = physicalProperties[0]->getStress();

	if (inContact) {

		for (int i = 0; i < BC2D_NUM_DOF - 2; i++) {
			mInternalForces(i) = -mLambda*mBn(i) + stress(1)*mBs(i);
		}

		mInternalForces(BC2D_NUM_DOF - 2) = -mGap;

	} else {

		mInternalForces(BC2D_NUM_DOF - 2) = mLambda;
	
	}

	return mInternalForces;
}

const XC::Vector &XC::BeamContact2D::getResistingForceIncInertia(void) const
{
	return getResistingForce();
}

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
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Receives object through the communicator argument.
int XC::BeamContact2D::recvSelf(const Communicator &comm)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
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
	if ((argv[0] == "force") || (argv[0] == "forces") ) {
		// forces on secondary node
		return new ElementResponse(this, 1, Vector(2));

	} else if ((argv[0] == "frictionforce") || (argv[0] == "frictionforces") ) {
		// frictional force vector
		return new ElementResponse(this, 2, Vector(2));

	} else if ((argv[0] == "forcescalar") || (argv[0] == "forcescalars") ) {
		// scalar contact forces
		return new ElementResponse(this, 3, Vector(2));

	} else if ((argv[0] == "masterforce") || (argv[0] == "masterforces") ||
		   (argv[0] == "primaryforce") || (argv[0] == "primaryforces") ) {
		// reactions (forces and moments) on primary nodes
		return new ElementResponse(this, 4, Vector(6));

	} else {
		// otherwise response quantity is unknown for the BeamContact2D class
		std::cerr << "XC::BeamContact2D::setResponse(const char **argv, int argc, OPS_Stream &eleInfo): "
		  << argv[0] << " unknown request" << std::endl;
	return 0;
	}
}

int
XC::BeamContact2D::getResponse(int responseID, Information &eleInfo)
{
	// initialize variables
	Vector force(2);
	Vector frictForce(2);
	Vector secondaryForce(2);
	Vector primaryForce(6);

	// get contact "stress" vector
	Vector stress = physicalProperties[0]->getStress();

	if (responseID == 1) {

		// forces on secondary node
		secondaryForce(0) = -mInternalForces(6);
		secondaryForce(1) = -mInternalForces(7);
		return eleInfo.setVector(secondaryForce);
	
	} else if (responseID == 2) {

		// frictional force vector
		frictForce = stress(1)*mg_xi;
		return eleInfo.setVector(frictForce);

    } else if (responseID == 3) {
		
		// scalar contact forces
		force(0) = stress(0);
		force(1) = stress(1);
		return eleInfo.setVector(force);

	} else if (responseID == 4) {

		// reactions (forces and moments) on primary nodes
		for (int i = 0;  i < 3; i++) {

			primaryForce(i)   = -mInternalForces(i);
			primaryForce(i+3) = -mInternalForces(i+3);
		}
		return eleInfo.setVector(primaryForce);

	} else
		// otherwise response quantity is unknown for the BeamContact2D class
		std::cerr << "XC::BeamContact2D::getResponse(int responseID = " << responseID << ", Information &eleInfo); "
		  << " unknown request" << std::endl;
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

int
XC::BeamContact2D::updateParameter(int parameterID, Information &info)
{
	int res = -1;
	int matRes =  physicalProperties[0]->updateParameter(parameterID, info);
	if (matRes != -1) {
		res = matRes;
	}
	return res;
}
