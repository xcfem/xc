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
                                                                        
// $Revision: 1.2 $
// $Date: 2004/09/01 04:01:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/joint/MFreedom_Joint3D.cpp,v $

// Written: Arash Altoontash, Gregory Deierlein
// Created: 04/03
// Revision: Arash

// Purpose: This file contains the implementation of class XC::MFreedom_Joint3D.


#include "MFreedom_Joint3D.h"
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>

#include <cstdlib>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <cmath>

//! @brief Constructor.
XC::MFreedom_Joint3D::MFreedom_Joint3D(void)
 :MFreedom_Joint(0, CNSTRNT_TAG_MFreedom_Joint3D ),
  nodeRotation(0), RotDOF(0),
  nodeDisplacement(0), DispDOF(0),
  RotationNode(nullptr), DisplacementNode(nullptr),
  RotNormVect(0), DspNormVect(0), dbTag3(0)
  {}


//! @brief Constructor.
//! @param theDomain: domain where the constraint is defined.
//! @param tag: tag for the multi-freedom constraint.
//! @param nodeRetain: identifier of the retained node.
//! @param nodeConstr: identifier of the constrained node.
//! @param LrgDsp: true if large displacement (geometric non-linearity) must be expected: 0 for constant constraint matrix(small deformations), 1 for time varying constraint matrix(large deformations), 2 for large deformations with length correction.
XC::MFreedom_Joint3D::MFreedom_Joint3D(Domain *theDomain, int tag, int nodeRetain, int nodeConstr,
		int nodeRot, int Rotdof, int nodeDisp, int Dispdof, int LrgDsp)
  :MFreedom_Joint(theDomain,tag,CNSTRNT_TAG_MFreedom_Joint3D,nodeRetain,nodeConstr,LrgDsp),
  nodeRotation(nodeRot), RotDOF(Rotdof), nodeDisplacement(nodeDisp), DispDOF(Dispdof), 
  RotationNode(nullptr), DisplacementNode(nullptr),
  RotNormVect(3), DspNormVect(3), dbTag3(0)
  {
    setDomain(theDomain);

    // check for proper degrees of freedom
    int RnumDOF = RetainedNode->getNumberDOF();
    int CnumDOF = ConstrainedNode->getNumberDOF();
    if(RnumDOF != 9 || CnumDOF != 6 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; mismatch in numDOF\n DOF not supported by this type of constraint";
        return;
      }
  
  
    // check the main degree of freedom. Assign auxiliary DOF 
    if( RotDOF<6 || RotDOF>8 || DispDOF<6 || DispDOF>8 || RotDOF==DispDOF )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Wrong degrees of freedom" ;
        return;
      }
  
    // check for proper dimensions of coordinate space
    const Vector &crdRet = RetainedNode->getCrds();
    const int dimRet = crdRet.Size();
    const Vector &crdCon = ConstrainedNode->getCrds();
    const int dimCon = crdCon.Size();
    const Vector &crdRot = RotationNode->getCrds();
    const int dimRot = crdRot.Size();
    const Vector &crdDsp = DisplacementNode->getCrds();
    const int dimDsp = crdDsp.Size();

    if(dimRet != 3 || dimCon != 3 || dimRot != 3 || dimDsp != 3 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; mismatch in dimnesion\n dimension not supported by this type of constraint";
        return;
      }
  
  
    // calculate the initial length of the rigid link
    const double deltaX = crdCon(0) - crdRet(0);
    const double deltaY = crdCon(1) - crdRet(1);
    const double deltaZ = crdCon(2) - crdRet(2);
  
    Length0= sqrt( deltaX*deltaX + deltaY*deltaY + deltaY*deltaY );
    if(Length0 <= 1.0e-12)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the constraint length is zero\n";
      }
  
    // calculate the normal vectors for the rotation mode and displacement mode
    for(int i = 0 ; i<3 ; i++ )
      {
        RotNormVect(i)= crdRot(i) - crdRet(i);
        DspNormVect(i)= crdDsp(i) - crdRet(i);	
      }
  
    if(RotNormVect.Norm() <= 1.0e-12 || DspNormVect.Norm() <= 1.0e-12 )
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; the normal vector for the rotation mode or the displacement mode is zero\n";
      }
    RotNormVect= RotNormVect / RotNormVect.Norm();
    DspNormVect= DspNormVect / DspNormVect.Norm();
  
  
    // allocate and set up the constranted and retained id's
  
    // the end is released
    constrDOF= ID(6);
    retainDOF= ID(8);
    for(int j = 0 ; j<6 ; j++ )
      {
        constrDOF(j) = j;
        retainDOF(j) = j;	
      }
    retainDOF(6) = RotDOF;
    retainDOF(7) = DispDOF;
  
  
    // allocate and set up the constraint matrix		
    constraintMatrix= Matrix(constrDOF.Size(),retainDOF.Size());
  
    constraintMatrix (0,0) = 1.0 ;
    constraintMatrix (1,1) = 1.0 ;
    constraintMatrix (2,2) = 1.0 ;
    constraintMatrix (1,3) = -deltaZ;
    constraintMatrix (2,3) = deltaY;
    constraintMatrix (3,3) = 1.0 ;
    constraintMatrix (0,4) = deltaZ;
    constraintMatrix (2,4) = -deltaX;
    constraintMatrix (4,4) = 1.0 ;
    constraintMatrix (0,5) = -deltaY;
    constraintMatrix (1,5) = deltaX;
    constraintMatrix (5,5) = 1.0 ;
    constraintMatrix (3,6) = RotNormVect(0);
    constraintMatrix (4,6) = RotNormVect(1);
    constraintMatrix (5,6) = RotNormVect(2);
    constraintMatrix (0,7) = deltaZ*DspNormVect(1) - deltaY*DspNormVect(2);
    constraintMatrix (1,7) = deltaX*DspNormVect(2) - deltaZ*DspNormVect(0) ;
    constraintMatrix (1,7) = deltaY*DspNormVect(0) - deltaX*DspNormVect(1) ;
  }

//! @brief Destructor.
XC::MFreedom_Joint3D::~MFreedom_Joint3D(void)
  {
    if(RotationNode)
      RotationNode->disconnect(this);
    if(DisplacementNode)
      DisplacementNode->disconnect(this);
  }

//! @brief Applies the constraint at the pseudo-time
//! being passed as parameter.
int XC::MFreedom_Joint3D::applyConstraint(double timeStamp)
  {
    if(LargeDisplacement!=0)
      {
        // calculate the constraint at this moment
      
        // get the coordinates of the two nodes - check dimensions are the same FOR THE MOMENT
        const Vector &crdRet = RetainedNode->getCrds();
        const Vector &crdCon = ConstrainedNode->getCrds();
        const Vector &crdRot = RotationNode->getCrds();
        const Vector &crdDsp = DisplacementNode->getCrds();
      
        // get committed displacements of nodes to get updated coordinates
        const Vector &dispRet = RetainedNode->getDisp();
        const Vector &dispCon = ConstrainedNode->getDisp();
        const Vector &dispRot = RotationNode->getDisp();
        const Vector &dispDsp = DisplacementNode->getDisp();
      
        const double deltaX = dispCon(0) + crdCon(0) - dispRet(0) - crdRet(0);
        const double deltaY = dispCon(1) + crdCon(1) - dispRet(1) - crdRet(1);
        const double deltaZ = dispCon(2) + crdCon(2) - dispRet(2) - crdRet(2);
      
        for( int i = 0 ; i<3 ; i++ )
	  {
	    RotNormVect(i)= dispRot(i) + crdRot(i) - dispRet(i) - crdRet(i);
	    DspNormVect(i)= dispDsp(i) + crdDsp(i) - dispRet(i) - crdRet(i);	
	  }
      
        RotNormVect = RotNormVect / RotNormVect.Norm();
        DspNormVect = DspNormVect / DspNormVect.Norm();
      
      
        constraintMatrix.Zero();
      
        constraintMatrix (0,0) = 1.0 ;
        constraintMatrix (1,1) = 1.0 ;
        constraintMatrix (2,2) = 1.0 ;
        constraintMatrix (1,3) = -deltaZ;
        constraintMatrix (2,3) = deltaY;
        constraintMatrix (3,3) = 1.0 ;
        constraintMatrix (0,4) = deltaZ;
        constraintMatrix (2,4) = -deltaX;
        constraintMatrix (4,4) = 1.0 ;
        constraintMatrix (0,5) = -deltaY;
        constraintMatrix (1,5) = deltaX;
        constraintMatrix (5,5) = 1.0 ;
        constraintMatrix (3,6) = RotNormVect(0);
        constraintMatrix (4,6) = RotNormVect(1);
        constraintMatrix (5,6) = RotNormVect(2);
        constraintMatrix (0,7) = deltaZ*DspNormVect(1) - deltaY*DspNormVect(2);
        constraintMatrix (1,7) = deltaX*DspNormVect(2) - deltaZ*DspNormVect(0) ;
        constraintMatrix (2,7) = deltaY*DspNormVect(0) - deltaX*DspNormVect(1) ;
      }
    return 0;
  }



//! @brief Sends the object through the communicator argument.
int XC::MFreedom_Joint3D::sendSelf(Communicator &comm)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << ": not implemented." << std::endl;
    return 0;
  }

//! @brief Receives the object through the communicator argument.
int XC::MFreedom_Joint3D::recvSelf(const Communicator &comm)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << ": not implemented." << std::endl;
    return 0;
  }

//! @brief Returns the constraint matrix.
const XC::Matrix &XC::MFreedom_Joint3D::getConstraint(void)
  {
    if(constraintMatrix.isEmpty())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no matrix was set\n";
        exit(-1);
      }    
    
    // Length correction
    // to correct the trial displacement
    if(LargeDisplacement == 2)
      {
	// get the coordinates of the two nodes - check dimensions are the same FOR THE MOMENT
	const Vector &crdR = RetainedNode->getCrds();
	const Vector &crdC = ConstrainedNode->getCrds();
	
	// get committed displacements of nodes to get updated coordinates
	const Vector &dispR = RetainedNode->getTrialDisp();
	const Vector &dispC = ConstrainedNode->getTrialDisp();
	
	double deltaX = dispC(0) + crdC(0) - dispR(0) - crdR(0);
	double deltaY = dispC(1) + crdC(1) - dispR(1) - crdR(1);
	double deltaZ = dispC(2) + crdC(2) - dispR(2) - crdR(2);
	
	
	Vector Direction(3);
	Direction(0) = deltaX;
	Direction(1) = deltaY;
	Direction(2) = deltaZ;
	double NewLength = Direction.Norm();
	if ( NewLength < 1e-12 ) std::cerr << "XC::MFreedom_Joint3D::applyConstraint : length of rigid link is too small or zero"; 
	Direction = Direction * (Length0/NewLength);		// correct the length
	// find new displacements of the constrainted node
	
	Vector NewLocation(6);
	NewLocation(0) = Direction(0) + dispR(0) + crdR(0) - crdC(0);
	NewLocation(1) = Direction(1) + dispR(1) + crdR(1) - crdC(1);
	NewLocation(2) = Direction(2) + dispR(2) + crdR(2) - crdC(2);
	NewLocation(3) = dispC(3);
	NewLocation(4) = dispC(4);
	NewLocation(5) = dispC(5);
	
	//int dummy = ConstrainedNode->setTrialDisp( NewLocation );
      }
    // end of length correction procedure
    
    // return the constraint matrix Ccr
    return constraintMatrix;
  }

//! @brief Printing.
void XC::MFreedom_Joint3D::Print(std::ostream &s, int flag ) const
  {
    s << "MFreedom_Joint3D: " << this->getTag() << "\n";
    s << "\tConstrained XC::Node: " << getNodeConstrained();
    s << " Retained XC::Node: " << getNodeRetained();
    s << " Large Disp: " << LargeDisplacement;
    s << " constrained dof: " << constrDOF;    
    s << " retained dof: " << retainDOF;        
    s << " constraint matrix: " << constraintMatrix << "\n";
  }


//! @brief Sets the domain of the constraint.
void XC::MFreedom_Joint3D::setDomain(Domain *theDomain)
  {
    MFreedom_Joint::setDomain(theDomain);

    if(theDomain)
      {
        RotationNode = theDomain->getNode(nodeRotation);
        if(RotationNode)
	  RotationNode->connect(this);
	else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; nodeRotation: " << nodeRotation
		      << "does not exist in model\n";
          }
	  
        DisplacementNode = theDomain->getNode(nodeDisplacement);
        if(DisplacementNode)
	  DisplacementNode->connect(this);
        else    
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
                      << "; nodeDisplacement: " << nodeDisplacement
		      << "does not exist in model\n";
          }
      }
  }
