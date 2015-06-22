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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/28 21:38:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/penalty/PenaltySP_FE.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/model/fe_ele/penalty/PenaltySP_FE.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the code for implementing the methods
// of the XC::PenaltySP_FE class interface.
//
// the interface:

#include <solution/analysis/model/fe_ele/penalty/PenaltySP_FE.h>
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
#include <domain/constraints/SP_Constraint.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

XC::PenaltySP_FE::PenaltySP_FE(int tag, Domain &theDomain, SP_Constraint &TheSP, double Alpha)
  :SP_FE(tag, 1,TheSP,Alpha)
  {
    // get a pointer to the XC::Node
    theNode = theDomain.getNode(theSP->getNodeTag());
    if(!theNode)
      {
	std::cerr << "Error en PenaltySP_FE::PenaltySP_FE() - no se encontró el nodo de tag: ";
	std::cerr << theSP->getNodeTag() << " en el dominio.\n";
	exit(-1);
      }

    // set the XC::DOF_Group tags
    DOF_Group *dofGrpPtr = theNode->getDOF_GroupPtr();
    if(dofGrpPtr != 0) 
      myDOF_Groups(0) = dofGrpPtr->getTag();	    
    
    // set the tangent
    tang(0,0) = alpha;
  }


// void setID(int index, int value);
//	Method to set the corresponding index of the XC::ID to value.
int XC::PenaltySP_FE::setID(void)
  {
    DOF_Group *theNodesDOFs = theNode->getDOF_GroupPtr();
    if(theNodesDOFs == 0)
      {
	std::cerr << "WARNING XC::PenaltySP_FE::setID(void) - no XC::DOF_Group with XC::Node\n";
	return -2;
      }    
    myDOF_Groups(0) = theNodesDOFs->getTag();
    
    int restrainedDOF = theSP->getDOF_Number();
    if(restrainedDOF < 0 || restrainedDOF >= theNode->getNumberDOF())
      {
	std::cerr << "¡OJO! PenaltySP_FE::setID(); - el nodo: "
                  << theNode->getTag() << " no tiene grado de libertad de índice: "
	          << restrainedDOF << std::endl;
	return -3;
      }    	
    const XC::ID &theNodesID = theNodesDOFs->getID();
    if(restrainedDOF >= theNodesID.Size())
      {
	std::cerr << "WARNING XC::PenaltySP_FE::setID(void) - ";
	std::cerr << " Nodes XC::DOF_Group too small\n";
	return -4;
      }    		
    
    myID(0) = theNodesID(restrainedDOF);

    return 0;
  }


const XC::Matrix &XC::PenaltySP_FE::getTangent(Integrator *theNewIntegrator)
  { return tang; }


const XC::Vector &XC::PenaltySP_FE::getResidual(Integrator *theNewIntegrator)
  {
    const double constraint = theSP->getValue();
    int constrainedDOF = theSP->getDOF_Number();
    const Vector &nodeDisp = theNode->getTrialDisp();
	
    if(constrainedDOF < 0 || constrainedDOF >= nodeDisp.Size())
      {
	std::cerr << "WARNING XC::PenaltySP_FE::getTangForce() - ";	
	std::cerr << " constrained DOF " << constrainedDOF << " outside disp\n";
	resid(0) = 0;
      }

    //    resid(0) = alpha * (constraint - nodeDisp(constrainedDOF));    
    // is replace with the following to remove possible problems with
    // subtracting very small numbers

    resid(0) = alpha * (constraint - nodeDisp(constrainedDOF));    
    return resid;
  }


const XC::Vector &XC::PenaltySP_FE::getTangForce(const XC::Vector &disp, double fact)
  {
    //double constraint = theSP->getValue();
    int constrainedID = myID(0);
    if(constrainedID < 0 || constrainedID >= disp.Size())
      {
	std::cerr << "WARNING PenaltySP_FE::getTangForce() - ";	
	std::cerr << " constrained DOF " << constrainedID << " outside disp\n";
	resid(0) = 0.0;
	return resid;
      }
    resid(0)= alpha * disp(constrainedID);
    return resid;
  }

const XC::Vector &XC::PenaltySP_FE::getK_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltySP_FE::getK_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltySP_FE::getC_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltySP_FE::getC_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

const XC::Vector &XC::PenaltySP_FE::getM_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING PenaltySP_FE::getM_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }




