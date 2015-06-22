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
                                                                        
// $Revision: 1.6 $
// $Date: 2005/11/28 21:38:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/penalty/PenaltyMP_FE.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the code for implementing the methods
// of the XC::PenaltyMP_FE class interface.
//
// the interface:

#include <solution/analysis/model/fe_ele/penalty/PenaltyMP_FE.h>
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
#include <domain/constraints/MP_Constraint.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

XC::PenaltyMP_FE::PenaltyMP_FE(int tag, Domain &theDomain, 
			   MP_Constraint &TheMP, double Alpha)
:MP_FE(tag, 2,(TheMP.getConstrainedDOFs()).Size()+
	    (TheMP.getRetainedDOFs()).Size(), TheMP,Alpha)
  {
    
    const ID &id1 = theMP->getConstrainedDOFs();
    int size= id1.Size();
    const ID &id2 = theMP->getRetainedDOFs();    
    size += id2.Size();

    tang= Matrix(size,size);
    resid= Vector(size);
    C= Matrix(id1.Size(),size);

    theRetainedNode = theDomain.getNode(theMP->getNodeRetained());    
    theConstrainedNode = theDomain.getNode(theMP->getNodeConstrained());

    if (theRetainedNode == 0 || theConstrainedNode == 0)
      {
	std::cerr << "FATAL PenaltyMP_FE::PenaltyMP_FE() - Constrained or Retained";
	std::cerr << " Node does not exist in Domain\n";
	std::cerr << theMP->getNodeRetained() << " " << theMP->getNodeConstrained() << std::endl;
	exit(-1);
      }	


    // set up the dof groups tags
    myDOF_Groups(0)= determineConstrainedNodeDofGrpPtr()->getTag();
    myDOF_Groups(1)= determineRetainedNodeDofGrpPtr()->getTag();
    
    if(theMP->isTimeVarying() == false)
      this->determineTangent();
  }

// void setID(int index, int value);
//	Method to set the correMPonding index of the ID to value.
int XC::PenaltyMP_FE::setID(void)
  {
    // first determine the IDs in myID for those DOFs marked
    // as constrained DOFs, this is obtained from the XC::DOF_Group
    // associated with the constrained node
    const int size1= determineConstrainedDOFsIDs(0);
    
    // then determine the IDs for the retained dof's
    int result= determineRetainedDOFsIDs(size1);
    return result;
  }

const XC::Matrix &XC::PenaltyMP_FE::getTangent(Integrator *theNewIntegrator)
  {
    if (theMP->isTimeVarying() == true)
	this->determineTangent();    
    return tang;
  }

const XC::Vector &XC::PenaltyMP_FE::getResidual(Integrator *theNewIntegrator)
  {
    // zero residual, CD = 0
    return resid;
  }



const XC::Vector &XC::PenaltyMP_FE::getTangForce(const Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltyMP_FE::getTangForce() - not yet implemented.";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltyMP_FE::getK_Force(const Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltyMP_FE::getK_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltyMP_FE::getC_Force(const Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltyMP_FE::getC_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltyMP_FE::getM_Force(const Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltyMP_FE::getM_Force() - not yet implemented.\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

void XC::PenaltyMP_FE::determineTangent(void)
  {
    // first determine [C] = [-I [Ccr]]
    C.Zero();
    const XC::Matrix &constraint = theMP->getConstraint();
    int noRows = constraint.noRows();
    int noCols = constraint.noCols();
    
    for(int j=0; j<noRows; j++)
      C(j,j) = -1.0;
    
    for(int i=0; i<noRows; i++)
      for(int j=0; j<noCols; j++)
	    C(i,j+noRows) = constraint(i,j);
    

    // now form the tangent: [K] = alpha * [C]^t[C]
    // *(tang) = C^C;
    // *(tang) *= alpha;

	// THIS IS A WORKAROUND UNTIL WE GET addMatrixTransposeProduct() IN
	// THE Matrix CLASS OR UNROLL THIS COMPUTATION
	const int rows= C.noRows();
	const int cols= C.noCols();
	Matrix CT(cols,rows);
	const Matrix &Cref = C;
	// Fill in the transpose of C
	for (int k = 0; k < cols; k++)
		for (int l = 0; l < rows; l++)
			CT(k,l) = Cref(l,k);
	// Compute alpha*(C^C)
	tang.addMatrixProduct(0.0, CT, Cref, alpha);
  }


