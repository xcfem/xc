//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
// $Source: /usr/local/cvs/OpenSees/SRC/element/joint/MFreedom_Joint3D.cpp,v $

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

// constructor for FEM_ObjectBroker
XC::MFreedom_Joint3D::MFreedom_Joint3D(void)
 :MFreedom_Constraint(0, CNSTRNT_TAG_MFreedom_Joint3D ),
  nodeRotation(0), RotDOF(0),
  nodeDisplacement(0), DispDOF(0), LargeDisplacement(0),
  RetainedNode(nullptr), ConstrainedNode(nullptr), RotationNode(nullptr), DisplacementNode(nullptr),
  RotNormVect(0), DspNormVect(0), dbTag3(0), Length0(0.0)
  {}


// general constructor for XC::ModelBuilder
XC::MFreedom_Joint3D::MFreedom_Joint3D(Domain *theDomain, int tag, int nodeRetain, int nodeConstr,
		int nodeRot, int Rotdof, int nodeDisp, int Dispdof, int LrgDsp )
  :MFreedom_Constraint(tag,nodeRetain,nodeConstr,CNSTRNT_TAG_MFreedom_Joint3D),
  nodeRotation(nodeRot), RotDOF(Rotdof), nodeDisplacement(nodeDisp), DispDOF(Dispdof), 
  LargeDisplacement(LrgDsp), RetainedNode(nullptr), ConstrainedNode(nullptr),
  RotationNode(nullptr), DisplacementNode(nullptr),
  RotNormVect(3), DspNormVect(3), dbTag3(0), Length0(0.0)
  {
    setDomain(theDomain);

    this->setTag(tag);


  
  
    // check for proper degrees of freedom
    int RnumDOF = RetainedNode->getNumberDOF();
    int CnumDOF = ConstrainedNode->getNumberDOF();
    if(RnumDOF != 9 || CnumDOF != 6 )
      {
        std::cerr << "MFreedom_Joint3D::MFreedom_Joint3D - mismatch in numDOF\n DOF not supported by this type of constraint";
        return;
      }
  
  
    // check the main degree of freedom. Assign auxilary DOF 
    if( RotDOF<6 || RotDOF>8 || DispDOF<6 || DispDOF>8 || RotDOF==DispDOF )
      {
        std::cerr << "MFreedom_Joint3D::MFreedom_Joint3D - Wrong degrees of freedom" ;
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
        std::cerr << "XC::MFreedom_Joint3D::MFreedom_Joint3D - mismatch in dimnesion\n dimension not supported by this type of constraint";
        return;
      }
  
  
    // calculate the initial length of the rigid link
    const double deltaX = crdCon(0) - crdRet(0);
    const double deltaY = crdCon(1) - crdRet(1);
    const double deltaZ = crdCon(2) - crdRet(2);
  
    Length0= sqrt( deltaX*deltaX + deltaY*deltaY + deltaY*deltaY );
    if(Length0 <= 1.0e-12)
      { std::cerr << "XC::MFreedom_Joint3D::MFreedom_Joint3D - The constraint length is zero\n"; }
  
    // calculate the normal vectors for the rotation mode and displacement mode
    for(int i = 0 ; i<3 ; i++ )
      {
        RotNormVect(i)= crdRot(i) - crdRet(i);
        DspNormVect(i)= crdDsp(i) - crdRet(i);	
      }
  
    if(RotNormVect.Norm() <= 1.0e-12 || DspNormVect.Norm() <= 1.0e-12 )
      { std::cerr << "XC::MFreedom_Joint3D::MFreedom_Joint3D - the normal vector for the rotation mode or the displacement mode is zero\n"; }
    RotNormVect = RotNormVect / RotNormVect.Norm();
    DspNormVect = DspNormVect / DspNormVect.Norm();
  
  
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

XC::MFreedom_Joint3D::~MFreedom_Joint3D(void)
  {
    if(RetainedNode)
      RetainedNode->disconnect(this);
    if(ConstrainedNode)
      ConstrainedNode->disconnect(this);
    if(RotationNode)
      RotationNode->disconnect(this);
    if(DisplacementNode)
      DisplacementNode->disconnect(this);
  }

int XC::MFreedom_Joint3D::applyConstraint(double timeStamp)
{
  if ( LargeDisplacement != 0 )
    {
      // calculate the constraint at this moment
      
      // get the coordinates of the two nodes - check dimensions are the same FOR THE MOMENT
      const Vector &crdRet = RetainedNode->getCrds();
      const Vector &crdCon = ConstrainedNode->getCrds();
      const Vector &crdRot = RotationNode->getCrds();
      const Vector &crdDsp = DisplacementNode->getCrds();
      
      // get commited displacements of nodes to get updated coordinates
      const Vector &dispRet = RetainedNode->getDisp();
      const Vector &dispCon = ConstrainedNode->getDisp();
      const Vector &dispRot = RotationNode->getDisp();
      const Vector &dispDsp = DisplacementNode->getDisp();
      
      const double deltaX = dispCon(0) + crdCon(0) - dispRet(0) - crdRet(0);
      const double deltaY = dispCon(1) + crdCon(1) - dispRet(1) - crdRet(1);
      const double deltaZ = dispCon(2) + crdCon(2) - dispRet(2) - crdRet(2);
      
      for ( int i = 0 ; i<3 ; i++ )
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


bool
XC::MFreedom_Joint3D::isTimeVarying(void) const
{
  if ( LargeDisplacement != 0 ) return true;
  
  return false;
}


int XC::MFreedom_Joint3D::sendSelf(CommParameters &cp)
  {
    std::cerr << "MFreedom_Joint3D::sendSelf no implementada." << std::endl;
    return 0;
  }


int XC::MFreedom_Joint3D::recvSelf(const CommParameters &cp)
  {
    std::cerr << "MFreedom_Joint3D::recvSelf no implementada." << std::endl;
    return 0;
  }


const XC::Matrix &XC::MFreedom_Joint3D::getConstraint(void)
  {
    if(constraintMatrix.Nula())
      {
        std::cerr << "MFreedom_Joint3D::getConstraint - no XC::Matrix was set\n";
        exit(-1);
      }    
    
    // Length correction
    // to correct the trial displacement
    if( LargeDisplacement == 2 )
      {
	// get the coordinates of the two nodes - check dimensions are the same FOR THE MOMENT
	const Vector &crdR = RetainedNode->getCrds();
	const Vector &crdC = ConstrainedNode->getCrds();
	
	// get commited displacements of nodes to get updated coordinates
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

void XC::MFreedom_Joint3D::Print(std::ostream &s, int flag )
  {
    s << "MFreedom_Joint3D: " << this->getTag() << "\n";
    s << "\tConstrained XC::Node: " << getNodeConstrained();
    s << " Retained XC::Node: " << getNodeRetained();
    s << " Large Disp: " << LargeDisplacement;
    s << " constrained dof: " << constrDOF;    
    s << " retained dof: " << retainDOF;        
    s << " constraint matrix: " << constraintMatrix << "\n";
  }


void XC::MFreedom_Joint3D::setDomain(Domain *theDomain)
  {
    if(theDomain == nullptr)
      {
        std::cerr << "WARNING MFreedom_Joint3D::SetDomain: Specified domain does not exist";
        std::cerr << "Domain = 0\n";
        return;
      }
    MFreedom_Constraint::setDomain(theDomain);

    if(theDomain)
      {
        RetainedNode = theDomain->getNode(getNodeRetained());
        if(RetainedNode)  RetainedNode->connect(this);
        ConstrainedNode = theDomain->getNode(getNodeConstrained());
        if(ConstrainedNode) ConstrainedNode->connect(this);
        RotationNode = theDomain->getNode(nodeRotation);
        if(RotationNode) RotationNode->connect(this);
        DisplacementNode = theDomain->getNode(nodeDisplacement);
        if(DisplacementNode) DisplacementNode->connect(this);
    
        if(ConstrainedNode == nullptr)
          {
            std::cerr << "MFreedom_Joint3D::setDomain: constrained node: ";
            std::cerr << getNodeConstrained() << "does not exist in model\n";
          }
  
        if(RetainedNode == nullptr)
          {
            std::cerr << "MFreedom_Joint3D::setDomain: retained node: ";
            std::cerr << getNodeRetained() << "does not exist in model\n";
          }
  
        if(RotationNode == nullptr)
          {
            std::cerr << "MFreedom_Joint3D::setDomain: nodeRotation: ";
            std::cerr << nodeRotation << "does not exist in model\n";
          }
  
        if(DisplacementNode == nullptr)
          {
            std::cerr << "MFreedom_Joint3D::setDomain: nodeDisplacement: ";
            std::cerr << nodeDisplacement << "does not exist in model\n";
          }
      }
  }
