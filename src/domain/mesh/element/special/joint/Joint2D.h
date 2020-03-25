// -*-c++-*-
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

// $Revision: 1.7 $
// $Date: 2004/09/01 04:01:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/joint/Joint2D.h,v $

// Written: Arash & GGD
// Created: 03/02
// Revision: Arash

// Joint2D.h: interface for the Joint2d class.
//
//////////////////////////////////////////////////////////////////////

#ifndef Joint2D_h
#define Joint2D_h


#include <domain/mesh/element/ElemWithMaterial.h>
#include <domain/mesh/element/utils/physical_properties/Joint2DPhysicalProperties.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <domain/domain/Domain.h>

namespace XC {
class Node;
class Response;
class DamageModelVector;
class SpringModels;


//! @ingroup ElemJoint
//!
//! @brief Joint element for two-dimensional problems.
//!
//! Joint2D is a two dimensional element, with four nodes. It generates an internal node
//! with an extra degree of freedom to represent the shear deformation. The external nodes
//! are connected to the internal node by multi-point constraints.
//! 
//! The external nodes Nd1, Nd2, Nd3 and Nd4 must be located on the mid points of a
//! virtual parallelogram, so the joint panel can be constructed on them. They should also
//! be entered in a clockwise or counter-clockwise order.
//! 
//! The tag for the internal node must be an unused tag that does not belong to any other
//! existing node.
//! 
//! It is possible to introduce rotational springs at the external nodes to allow
//! member-end rotations, or simply leave them fixed. If uniaxial materials Mat1, Mat2,
//! Mat3 and Mat4 are not introduced, the member-end rotations will be assumed fixed and
//! frame elements will rigidly connected to the joint panel. It is possible to introduce
//! the springs separately or leave them fixed by entering zero as material tags.
//! 
//! A uniaxial material model is required for the rotational spring relating the panel
//! shear deformation to the shear equivalent moment. MatC indicates the material tag for
//! the shear panel and it must belong to a valid existing material tag.
//! 
//! The multi-point constraints are developed for the joint element and they are
//! responsible for simulating the shear panel kinematics and maintaining a parallelogram
//! shape for the joint element. These four multi-point constraints are automatically added
//! to the domain and they connect the central node to external nodes.
//! 
//! These constraints might be set for the large deformations, using the LrgDisp flag.
//! If the LrgDisp flag is set to zero, the element will use a constant constraint matrix,
//! calculated based on the initial configuration and the element can not undergo the exact
//! deformation for large deformations. Non-zero value for LrgDisp indicates a time varying
//! constraint matrix that can go through large deformations with more precise results. In
//! this case the constraint matrix is updated for every time step, based on the current
//! nodal positions. Value 1 for LrgDisp indicates time varying constraint without length
//! correction. If value 2 is chosen, the time varying constraint will be applied with
//! length correction to maintain the initial length of the multi-point constraints.
//! 
//! Joint2D must be used along with 'Penalty', or 'Transformation' constraint handler
//! to allow the multi point constraints work perfectly.
//! 
//! The valid queries to a Joint2D element when creating an element recorder are
//! 'internalNode', 'deformation', `plasticDeformation`, 'force', 'deformationANDforce',
//! 'size', 'stiff' and 'materials ...'. 	   
class Joint2D: public ElemWithMaterial<5,Joint2DPhysicalProperties>
  {
  private:
    typedef ElemWithMaterial<5,Joint2DPhysicalProperties> Joint2dBase;
    ID	InternalConstraints;
    int	numDof, nodeDbTag, dofDbTag;
    static Matrix K;
    static Vector V;

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int parameterID;
    Vector *theLoadSens;
    // AddingSensitivity:END ///////////////////////////////////////////
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    Joint2D(void);
    Joint2D(int tag, int nd1, int nd2, int nd3, int nd4, int IntNodeTag, const SpringModels &springModels, Domain *theDomain, int LrgDisp);
  
    Joint2D(int tag, int nd1, int nd2, int nd3, int nd4, int IntNodeTag, const SpringModels &springModels, Domain *theDomain, int LrgDisp, const DamageModelVector &damageModels);
    Element *getCopy(void) const;
    ~Joint2D();
    // methods dealing with domain
    int getNumDOF(void) const;
  
    void setDomain(Domain *theDomain);  
    bool isSubdomain(void) { return false; } ;
	
    // methods dealing with committed state and update
    int update(void);
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
  
    // methods to return the current linearized stiffness,
    // damping and mass matrices
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;   
    const Matrix &getDamp(void) const;
    const Matrix &getMass(void) const;
	
  // methods for returning and applying loads
  //virtual Vector &getUVLoadVector(double q1, double q2);
  int addLoad(ElementalLoad *theLoad, double loadFactor);
  int addInertiaLoadToUnbalance(const Vector &accel);

  const	Vector &getResistingForce(void) const;
  const	Vector &getResistingForceIncInertia(void) const;     

  // method for obtaining information specific to an element
  Response* setResponse(const std::vector<std::string> &argv, Information &eleInformation);
  int getResponse(int responseID, Information &eleInformation);
  int sendSelf(CommParameters &);
  int recvSelf(const CommParameters &);
  void Print(std::ostream &s, int flag =0) const;

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int	  addInertiaLoadSensitivityToUnbalance(const Vector &accel, bool tag);
    int   setParameter(const std::vector<std::string> &argv, Parameter &param);
    int   updateParameter(int parameterID, Information &info);
    int   activateParameter(int parameterID);
    const Vector & getResistingForceSensitivity(int gradNumber);
    const Matrix & getKiSensitivity(int gradNumber);
    const Matrix & getMassSensitivity(int gradNumber);
    int   commitSensitivity(int gradNumber, int numGrads);
    // AddingSensitivity:END ///////////////////////////////////////////

 protected:
  int 	addMFreedom_Joint(Domain *theDomain, int mpNum, int RnodeID, int CnodeID, int MainDOF, int FixedEnd, int LrgDispFlag );
};
} // end of XC namespace

#endif
