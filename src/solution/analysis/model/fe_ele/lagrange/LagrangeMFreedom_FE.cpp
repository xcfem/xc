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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/11/28 21:38:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/lagrange/LagrangeMFreedom_FE.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 02/99
// Revision: A
//
// Purpose: This file contains the code for implementing the methods
// of the XC::LagrangeMFreedom_FE class interface.

#include <solution/analysis/model/fe_ele/lagrange/LagrangeMFreedom_FE.h>
#include <cstdlib>

#include <domain/mesh/element/Element.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/integrator/Integrator.h>
#include "domain/domain/subdomain/Subdomain.h"
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

XC::LagrangeMFreedom_FE::LagrangeMFreedom_FE(int tag, Domain &theDomain, MFreedom_Constraint &TheMP, DOF_Group &theGroup, double Alpha)
  :MFreedom_FE(tag, 3,(TheMP.getConstrainedDOFs().Size()+TheMP.getRetainedDOFs().Size()+TheMP.getRetainedDOFs().Size()),TheMP,Alpha), Lagrange_FE(theGroup)
  {
    const Matrix &constraint = theMP->getConstraint();
    const int noRows = constraint.noRows();
    const int noCols = constraint.noCols();
    const int size = 2*noRows+noCols;
    tang= Matrix(size,size);
    resid= Vector(size);
    tang.Zero();	
    resid.Zero();

    theRetainedNode= theDomain.getNode(theMP->getNodeRetained());    
    theConstrainedNode= theDomain.getNode(theMP->getNodeConstrained());

    if(theRetainedNode == 0)
      {
	std::cerr << "WARNING XC::LagrangeMFreedom_FE::LagrangeMFreedom_FE()";
	std::cerr << "- no asscoiated Retained node\n";
	exit(-1);
      }
    
    if(theConstrainedNode == 0)
      {
	std::cerr << "WARNING XC::LagrangeMFreedom_FE::LagrangeMFreedom_FE()";
	std::cerr << "- no asscoiated constrained nNode\n";
	exit(-1);
      }
    
    if(theMP->isTimeVarying() == false)
      { this->determineTangent(); }
    

    myDOF_Groups(0)= determineConstrainedNodeDofGrpPtr()->getTag();
    myDOF_Groups(1)= determineRetainedNodeDofGrpPtr()->getTag();
    myDOF_Groups(2) = getLagrangeDOFGroup()->getTag();
  }

// void setID(int index, int value);
//	Method to set the corresponding index of the ID to value.
int XC::LagrangeMFreedom_FE::setID(void)
  {
    int offset = 0;

    // first determine the IDs in myID for those DOFs marked
    // as constrained DOFs, this is obtained from the XC::DOF_Group
    // associated with the constrained node
    const int offset1= determineConstrainedDOFsIDs(0);
    if(offset1>=0) offset= offset1;
 
    // then determine the IDs for the retained dof's
    const int offset2= determineRetainedDOFsIDs(offset1);
    if(offset2>=0) offset= offset2;
    
    // finally set the ID corresponding to the ID's at the LagrangeDOF_Group
    const ID &theGroupsID = getLagrangeDOFGroup()->getID();
    int size3 = theGroupsID.Size();
    for(int k=0; k<size3; k++) 
      myID(k+offset) = theGroupsID(k);
    
    return offset;
  }

const XC::Matrix &XC::LagrangeMFreedom_FE::getTangent(Integrator *theNewIntegrator)
  {
    if(theMP->isTimeVarying() == true)
      this->determineTangent();
    return tang;
  }

const XC::Vector &XC::LagrangeMFreedom_FE::getResidual(Integrator *theNewIntegrator)
  { return resid; }



const XC::Vector &XC::LagrangeMFreedom_FE::getTangForce(const Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeMFreedom_FE::getTangForce() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return  resid;
  }

const XC::Vector &XC::LagrangeMFreedom_FE::getK_Force(const Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeMFreedom_FE::getK_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeMFreedom_FE::getC_Force(const Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeMFreedom_FE::getC_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeMFreedom_FE::getM_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeMFreedom_FE::getM_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

void XC::LagrangeMFreedom_FE::determineTangent(void)
  {
    const Matrix &constraint = theMP->getConstraint();
    const int noRows= constraint.noRows();
    const int noCols= constraint.noCols();
    const int n= noRows+noCols;
    
    tang.Zero();    

    for(int j=0; j<noRows; j++)
      {
	tang(n+j, j)= -alpha;
	tang(j, n+j)= -alpha;	
      }
    
    for(int i=0; i<noRows; i++)
      for(int j=0; j<noCols; j++)
        {
	  const double val = constraint(i,j) * alpha;
	  tang(n+i, j+noRows) = val;
	  tang(noRows+j, n+i) = val;
	}

  }





