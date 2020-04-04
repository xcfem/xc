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

// $Revision: 1.2 $
// $Date: 2004/09/01 04:01:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/joint/Joint3D.h,v $

// Written: Arash Altoontash, Gregory Deierlein
// Created: 04/03
// Revision: Arash

// Joint3D.h: interface for the Joint3D class.
//
//////////////////////////////////////////////////////////////////////

#ifndef Joint3D_h
#define Joint3D_h


#include <domain/mesh/element/ElemWithMaterial.h>
#include <domain/mesh/element/utils/physical_properties/Joint3DPhysicalProperties.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <domain/domain/Domain.h>

namespace XC {
class Node;
class UniaxialMaterial;
class Response;

//! @ingroup Elem
//!
//! @defgroup SpecialElem Special purpose elements.
//
//! @ingroup SpecialElem
//!
//! @defgroup ElemJoint Joint modelling elements.
//
//! @ingroup ElemJoint
//
//! @brief Joint element for three-dimensional problems.
//!
//! Joint3D is a three dimensional element, with six nodes. It generates an internal node
//! with three extra degrees of freedom to represent the shear modes. The external nodes
//! are connected to the internal node by multi-point constraints.
//! 
//! The local x' axis is defined on nodes Nd1 and Nd2; y' and z' axes are defined on Nd3, Nd4 , and Nd5 and Nd6 respectively. The middle point of Nd3 and Nd4, and Nd5 and Nd6 must be located on the middle point of Nd1 and Nd2 so it would be possible to construct a shear block on these six nodes.
//! 
//! The tag for the internal node must be an unused tag that does not belong to any other
//! existing node.
//! 
//! The shear modes are defined as the shear deformation of the joint block for rotations along each local axes. Three rotational springs provide the stiffness for the three shear modes. Each rotational spring is defnied in a plane which shares the same normal vector as the local axis, so the imposed moment of each spring has the same direction as its corresponding local axis.
//! 
//! The multi-point constraints are developed for the joint element and they are
//! responsible for simulating the shear block kinematics and maintaining the opsite faces parallel. These six multi-point constraints are automatically added
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
//! Joint3D must be used along with 'Penalty', or 'Transformation' constraint handler
//! to allow the multi point constraints work perfectly.
//! 
//! The valid queries to a Joint3D element when creating an element recorder are
//! 'internalNode', 'deformation', `plasticDeformation`, 'force', 'deformationANDforce',
//! 'size', 'stiff' and 'materials ...'. 	   
class Joint3D: public ElemWithMaterial<7,Joint3DPhysicalProperties>  
  {
  private:
    ID ExternalNodes;
    ID InternalConstraints;
    int numDof, nodeDbTag, dofDbTag;
    static Matrix K;
    static Vector V;
  protected:
   int addMFreedom_Joint(Domain *theDomain, int mpNum, int RetNodeID, int ConNodeID,
                    int RotNodeID, int Rdof, int DspNodeID, int Ddof, 
                    int LrgDispFlag );   
  public:
    Joint3D(void);
    Joint3D(int tag, int nd1, int nd2, int nd3, int nd4, int nd5, int nd6, int IntNodeTag, const SpringModels &springModels, Domain *theDomain, int LrgDisp);
    Element *getCopy(void) const;  
    ~Joint3D(void);
  
    // methods dealing with domain
    int getNumDOF(void) const;
  
    void setDomain(Domain *theDomain);  
    bool isSubdomain(void) { return false; } ;
        
    // methods dealing with committed state and update
    int update(void);
  
    // methods to return the current linearized stiffness,
    // damping and mass matrices
    const        Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;   
    const        Matrix &getDamp(void) const;
    const        Matrix &getMass(void) const;
        
    // methods for returning and applying loads
    //virtual Vector &getUVLoadVector(double q1, double q2);
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const        Vector &getResistingForce(void) const;
    const        Vector &getResistingForceIncInertia(void) const;     

    // method for obtaining information specific to an element
    Response* setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif
