// -*-c++-*-
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
                                                                       
#ifndef BeamContact2D_h
#define BeamContact2D_h

// Created: C.McGann, UW, 12.2010
//
// Description: This file contains the class definition for BeamContact2D

#include "domain/mesh/element/ElemWithMaterial.h"
#include "domain/mesh/element/utils/physical_properties/Contact2D.h"

// number of nodes per element
#define BC2D_NUM_NODE 4
// number of dimensions
#define BC2D_NUM_DIM  2
// degrees of freedom per element
#define BC2D_NUM_DOF  10

namespace XC {
class ContactMaterial2D;

//! @brief 2D beam contact element.
//! The BeamContact2D element is a three-dimensional beam-to-node contact element which defines a frictional contact
//! interface between a beam element and a separate body. The retained nodes (3 DOF) are the endpoints of the beam
//! element, and the constrained node (2 DOF) is a node from a second body. The Lagrange multiplier node (2 DOF)
//! is required to enforce the contact condition. Each contact element should have a unique Lagrange multiplier
//! node. The Lagrange multiplier node should not be fixed, otherwise the contact condition will not work. 
//! @ingroup Elem  
class BeamContact2D : public ElemWithMaterial<BC2D_NUM_NODE, Contact2D>
  {
  private:
    // objects
	
    // input quantities
    mutable double mLength; //!< length of beam element
    double mRadius; //!< radius of beam
    double mGapTol; //!< gap tolerance
    double mForceTol; //!< force tolerance
    int mIniContact; //!< initial contact switch (0 = notInContact, 1 = inContact) default is set for initially inContact 
    
    // boolean variables
    mutable bool inContact;
    mutable bool was_inContact;
    mutable bool to_be_released;
    mutable bool should_be_released;
    mutable bool in_bounds;

    mutable Matrix mTangentStiffness; //!< tangent stiffness matrix
    mutable Vector mInternalForces; //!< vector of internal forces
    
    // calculation variables
    mutable double mXi; //!< centerline projection coordinate
    mutable double mGap; //!< current value of the gap
    mutable double mLambda; //!< current value of contact force (Lagrange Mult)

    Matrix mEye1; //!< identity tensor
    Matrix mEyeS; //!< skew symmetric transformation tensor
    mutable Vector mg_xi; //!< surface tangent vector
    mutable Vector mNormal; //!< normal vector
    mutable Vector mShape; //!< vector of Hermitian shape functions
    mutable Vector mDshape; //!< vector of Hermitian shape function derivatives

    mutable Vector mBn; //!< gap-displacement vector
    mutable Vector mBs; //!< slip-displacement vector
	
    mutable Vector ma_1; //!< tangent vector at node a
    mutable Vector mb_1; //!< tangent vector at node b
    mutable Vector mc_1; //!< tangent vector at centerline projection
    mutable double mrho; //!< local coordinate transformation term
	
    mutable Vector mIcrd_a; //!< initial coordinates of node a
    mutable Vector mIcrd_b; //!< initial coordinates of node b
    mutable Vector mIcrd_s; //!< initial coordinates of secondary node
    mutable Vector mDcrd_a; //!< initial coordinates of node a
    mutable Vector mDcrd_b; //!< initial coordinates of node b
    mutable Vector mDcrd_s; //!< initial coordinates of secondary node
    mutable Vector mDisp_a_n; //!< total disp & rotation of node a @ step n
    mutable Vector mDisp_b_n; //!< total disp & rotation of node b @ step n

    // member functions
    double Project(double xi) const;
    void ComputeB(void) const;
    int UpdateBase(double xi) const;       
    void UpdateEndFrames(void) const;
    Vector Get_dxc_xi(double xi) const;
    Vector Get_dxc_xixi(double xi) const;
    Vector Geta1(void) const;
    Vector Getb1(void) const;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
    void setup_contact_state(void) const;
    void update_contact_state(void);
    bool initialize_parameters(void) const;
  public:
    BeamContact2D(int tag= 0, const ContactMaterial2D *mat= nullptr);
    BeamContact2D(int tag, int Nd1, int Nd2, int NdS, int NdL, ContactMaterial2D &theMat, double width, double tolG, double tolF, int cSwitch = 0);
    Element *getCopy(void) const;

    // public methods to obtain information about dof and connectivity
    int getNumDOF(void) const;
    void setDomain(Domain *theDomain);

    void setWidth(const double &);
    double getWidth(void) const;
    void setGapTolerance(const double &);
    double getGapTolerance(void) const;
    void setForceTolerance(const double &);
    double getForceTolerance(void) const;
    void setInitialContactFlag(const int &);
    double getInitialContactFlag(void) const;

    // public methods to set the state of the element
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    int update(void);

    // public methods to obtain stiffness, mass, damping, and residual info
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;

    void zeroLoad(void);
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

	// public methods for element output
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;

    Response *setResponse(const std::vector<std::string> &argv, Parameter &param);
    int getResponse(int responseID, Information &eleInformation);

	// public methods for material stage update
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);

};
} // end of XC namespace

#endif
