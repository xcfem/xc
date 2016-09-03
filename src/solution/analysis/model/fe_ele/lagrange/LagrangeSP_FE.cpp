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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/lagrange/LagrangeSP_FE.cpp,v $
                                                                        
// Written: fmk 
// Created: 02/99
// Revision: A
//
// Purpose: This file contains the code for iSPlementing the methods
// of the LagrangeSP_FE class interface.
//
// the interface:

#include <solution/analysis/model/fe_ele/lagrange/LagrangeSP_FE.h>
#include <cstdlib>

#include <domain/mesh/element/Element.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/integrator/Integrator.h>
#include "domain/domain/subdomain/Subdomain.h"
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/ID.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/SP_Constraint.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

XC::LagrangeSP_FE::LagrangeSP_FE(int tag, Domain &theDomain, SP_Constraint &TheSP,DOF_Group &theGroup, double Alpha)
  :SP_FE(tag,2,TheSP,Alpha), Lagrange_FE(theGroup)
  {
    // zero the XC::Matrix and XC::Vector
    resid.Zero();
    tang.Zero();

    theNode= theDomain.getNode(theSP->getNodeTag());    
    if(theNode == 0)
      {
	std::cerr << "WARNING XC::LagrangeSP_FE::LagrangeSP_FE()";
	std::cerr << "- no asscoiated XC::Node\n";
	exit(-1);
      }

    // set the tangent
    tang(0,1)= alpha;
    tang(1,0)= alpha;
    
    // set the myDOF_Groups tags indicating the attached id's of the
    // DOF_Group objects
    DOF_Group *theNodesDOFs = theNode->getDOF_GroupPtr();
    if(theNodesDOFs == 0)
      {
	std::cerr << "WARNING XC::LagrangeSP_FE::LagrangeSP_FE()";
	std::cerr << " - no XC::DOF_Group with Constrained XC::Node\n";
	exit(-1);	
      }    

    myDOF_Groups(0) = theNodesDOFs->getTag();
    myDOF_Groups(1) = getLagrangeDOFGroup()->getTag();
  }

//! @brief Method to set the correSPonding index of the XC::ID to value.
int XC::LagrangeSP_FE::setID(void)
  {
    int result = 0;

    // first determine the IDs in myID for those DOFs marked
    // as constrained DOFs, this is obtained from the XC::DOF_Group
    // associated with the constrained node
    DOF_Group *theNodesDOFs = theNode->getDOF_GroupPtr();
    if(theNodesDOFs == 0)
      {
	std::cerr << "WARNING XC::LagrangeSP_FE::setID(void)";
	std::cerr << " - no XC::DOF_Group with Constrained XC::Node\n";
	return -1;
      }

    int restrainedDOF = theSP->getDOF_Number();
    const XC::ID &theNodesID = theNodesDOFs->getID();
    
    if(restrainedDOF < 0 || restrainedDOF >= theNodesID.Size())
      {
	std::cerr << "WARNING XC::LagrangeSP_FE::setID(void)";
	std::cerr << " - restrained DOF invalid\n";
	return -2;
      }
    
    myID(0) = theNodesID(restrainedDOF);
    myID(1) = (getLagrangeDOFGroup()->getID())(0);
    
    return result;
  }

const XC::Matrix &XC::LagrangeSP_FE::getTangent(Integrator *theIntegrator)
  { return tang; }

const XC::Vector &XC::LagrangeSP_FE::getResidual(Integrator *theNewIntegrator)
  {
    double constraint = theSP->getValue();
    int constrainedDOF = theSP->getDOF_Number();
    const XC::Vector &nodeDisp = theNode->getTrialDisp();

    if(constrainedDOF < 0 || constrainedDOF >= nodeDisp.Size())
      {
	std::cerr << "LagrangeSP_FE::formResidual() -";
	std::cerr << " constrained DOF " << constrainedDOF << " ouside range\n";
	resid(1)= 0;
      }
    
    resid(1)= alpha *(constraint - nodeDisp(constrainedDOF));
    return resid;
  }




const XC::Vector &XC::LagrangeSP_FE::getTangForce(const XC::Vector &disp, double fact)
  {
    double constraint = theSP->getValue();
    int constrainedID = myID(1);
    if(constrainedID < 0 || constrainedID >= disp.Size())
      {
	std::cerr << "WARNING XC::LagrangeSP_FE::getTangForce() - ";	
	std::cerr << " constrained DOF " << constrainedID << " outside disp\n";
	resid(1)= constraint*alpha;
	return resid;
      }
    resid(1)= disp(constrainedID);
    return resid;    
  }

const XC::Vector &XC::LagrangeSP_FE::getK_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeSP_FE::getK_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeSP_FE::getC_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeSP_FE::getC_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

const XC::Vector &XC::LagrangeSP_FE::getM_Force(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING LagrangeSP_FE::getM_Force() - not yet implemented\n";
    resid.Zero(); //Añadida LCPT.
    return resid;
  }

