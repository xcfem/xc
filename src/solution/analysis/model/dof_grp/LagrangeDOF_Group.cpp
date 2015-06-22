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
                                                                        
// $Revision: 1.3 $
// $Date: 2004/10/12 21:52:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/dof_grp/LagrangeDOF_Group.cpp,v $
                                                                        
// Written: fmk 
// Created: 02/99
// Revision: A
//
// Purpose: This file contains the code for implementing the methods
// of the XC::LagrangeDOF_Group class interface.
//
// What: "@(#) LagrangeDOF_Group.C, revA"


#include <solution/analysis/model/dof_grp/LagrangeDOF_Group.h>
#include <cstdlib>

#include <domain/mesh/node/Node.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <solution/analysis/integrator/TransientIntegrator.h>
#include <domain/constraints/SP_Constraint.h>
#include <domain/constraints/MP_Constraint.h>
#include <domain/constraints/MRMP_Constraint.h>

XC::LagrangeDOF_Group::LagrangeDOF_Group(int tag, SP_Constraint &spPtr)
  :DOF_Group(tag, 1) {}

XC::LagrangeDOF_Group::LagrangeDOF_Group(int tag, MP_Constraint &mpPtr)
  :DOF_Group(tag, (mpPtr.getRetainedDOFs()).Size())
  {}

XC::LagrangeDOF_Group::LagrangeDOF_Group(int tag, MRMP_Constraint &mrmpPtr)
  :DOF_Group(tag, mrmpPtr.getRetainedDOFs().Size())
  { }


const XC::Matrix &XC::LagrangeDOF_Group::getTangent(Integrator *theIntegrator)
  {
    // does nothing - the Lagrange FE_Elements provide coeffs to tangent
    unbalAndTangent.getTangent().Zero();
    return unbalAndTangent.getTangent();
  }

const XC::Vector &XC::LagrangeDOF_Group::getUnbalance(Integrator *theIntegrator)
  {
    // does nothing - the Lagrange FE_Elements provide residual 
    unbalAndTangent.getResidual().Zero();
    return unbalAndTangent.getResidual();
  }

// void setNodeDisp(const XC::Vector &u);
//	Method to set the corresponding nodes displacements to the
//	values in u, components identified by myID;

void XC::LagrangeDOF_Group::setNodeDisp(const XC::Vector &u)
  { return; }
	
	
// void setNodeVel(const XC::Vector &udot);
//	Method to set the corresponding nodes velocities to the
//	values in udot, components identified by myID;

void XC::LagrangeDOF_Group::setNodeVel(const XC::Vector &udot)
  { return; }



// void setNodeAccel(const XC::Vector &udotdot);
//	Method to set the corresponding nodes accelerations to the
//	values in udotdot, components identified by myID;

void XC::LagrangeDOF_Group::setNodeAccel(const XC::Vector &udotdot)
  { return; }


// void setNodeIncrDisp(const XC::Vector &u);
//	Method to set the corresponding nodes displacements to the
//	values in u, components identified by myID;

void XC::LagrangeDOF_Group::incrNodeDisp(const XC::Vector &u)
  { return; }
	
	
// void setNodeincrVel(const XC::Vector &udot);
//	Method to set the corresponding nodes velocities to the
//	values in udot, components identified by myID;

void XC::LagrangeDOF_Group::incrNodeVel(const XC::Vector &udot)
  { return; }



// void setNodeIncrAccel(const XC::Vector &udotdot);
//	Method to set the corresponding nodes accelerations to the
//	values in udotdot, components identified by myID;

void XC::LagrangeDOF_Group::incrNodeAccel(const XC::Vector &udotdot)
  { return; }


const XC::Vector &XC::LagrangeDOF_Group::getCommittedDisp(void)
  { 
    unbalAndTangent.getResidual().Zero();
    return unbalAndTangent.getResidual();
  }

const XC::Vector &XC::LagrangeDOF_Group::getCommittedVel(void)
  {
    unbalAndTangent.getResidual().Zero();
    return unbalAndTangent.getResidual();
  }

const XC::Vector &XC::LagrangeDOF_Group::getCommittedAccel(void)
  {
    unbalAndTangent.getResidual().Zero();
    return unbalAndTangent.getResidual();
  }

void XC::LagrangeDOF_Group::addMtoTang(double fact)
  {}

void XC::LagrangeDOF_Group::zeroUnbalance(void)
  {}

void XC::LagrangeDOF_Group::zeroTangent(void)
  {}

void XC::LagrangeDOF_Group::addPtoUnbalance(double fact)
  {}

void XC::LagrangeDOF_Group::addPIncInertiaToUnbalance(double fact)
  {}

void XC::LagrangeDOF_Group::addM_Force(const XC::Vector &Udotdot, double fact)
  {}

const XC::Vector &XC::LagrangeDOF_Group::getTangForce(const XC::Vector &disp, double fact)
  {
    std::cerr << "WARNING XC::LagrangeDOF_Group::getTangForce() - not yet implemented\n";
    unbalAndTangent.getResidual().Zero();
    return unbalAndTangent.getResidual();
  }

const XC::Vector &XC::LagrangeDOF_Group::getC_Force(const XC::Vector &disp, double fact)
  {
    unbalAndTangent.getResidual().Zero();
    return unbalAndTangent.getResidual();
  }

const XC::Vector &XC::LagrangeDOF_Group::getM_Force(const XC::Vector &disp, double fact)
  {
    unbalAndTangent.getResidual().Zero();
    return unbalAndTangent.getResidual();
  }


