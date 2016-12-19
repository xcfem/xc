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
                                                                        
// File: ~/domain/constraints//MFreedom_ConstraintBase.C
//
// Written: lcpt 
// Created: 03/2015
// Revision: A
//
// Purpose: This file contains the implementation of class MFreedom_ConstraintBase.

#include <domain/constraints/MFreedom_ConstraintBase.h>

#include <cstdlib>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <boost/any.hpp>

#include "xc_basic/src/matrices/m_int.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "med.h"
#include "vtkCellType.h"

void XC::MFreedom_ConstraintBase::set_constraint(const Matrix &c)
  { constraintMatrix= c; }

void XC::MFreedom_ConstraintBase::set_constrained_dofs(const ID &constrainedDOF)
  { constrDOF= constrainedDOF; }

// constructor for FEM_ObjectBroker
XC::MFreedom_ConstraintBase::MFreedom_ConstraintBase(int tag,int clasTag)		
  :Constraint(tag,0,clasTag) {}

// constructor for Subclass
XC::MFreedom_ConstraintBase::MFreedom_ConstraintBase(int tag, int nodeConstr, int clasTag)
  :Constraint(tag, nodeConstr, clasTag) {}

// constructor for Subclass
XC::MFreedom_ConstraintBase::MFreedom_ConstraintBase(int tag, int nodeConstr,const ID &constrainedDOF, int clasTag)
  :Constraint(tag, nodeConstr, clasTag)
  {
    set_constrained_dofs(constrainedDOF);    
  }


// general constructor for XC::ModelBuilder
XC::MFreedom_ConstraintBase::MFreedom_ConstraintBase(int tag, int nodeConstr, const Matrix &constr,const ID &constrainedDOF,int classTag)
  :Constraint(tag, nodeConstr, classTag)
  {
    set_constrained_dofs(constrainedDOF);    
    set_constraint(constr);
  }

//! @brief Devuelve verdadero si afecta al nodo cuyo tag se pasa como parámetro.
bool XC::MFreedom_ConstraintBase::afectaANodo(int tagNodo) const
  {
    return (tagNodo== getNodeConstrained());
  }

const XC::ID &XC::MFreedom_ConstraintBase::getConstrainedDOFs(void) const
  {
    // return the ID corresponding to constrained DOF of Ccr
    return constrDOF;
  }


bool XC::MFreedom_ConstraintBase::isTimeVarying(void) const
  { return false; }


//! @brief Add reactions to nodes.
int XC::MFreedom_ConstraintBase::addResistingForceToNodalReaction(bool inclInertia)
  {
    std::cerr << "MFreedom_ConstraintBase::addResistingForceToNodalReaction not yet implemented."
              << std::endl;
    return 0;
  }

const XC::Matrix &XC::MFreedom_ConstraintBase::getConstraint(void) const
  { return constraintMatrix; }

//! @brief Envía los datos through the channel being passed as parameter.
int XC::MFreedom_ConstraintBase::sendData(CommParameters &cp)
  {
    int res= Constraint::sendData(cp);
    res+= cp.sendMatrix(constraintMatrix,getDbTagData(),CommMetaData(3));
    res+= cp.sendID(constrDOF,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Recibe los datos through the channel being passed as parameter.
int XC::MFreedom_ConstraintBase::recvData(const CommParameters &cp)
  {
    int res= Constraint::recvData(cp);
    res+= cp.receiveMatrix(constraintMatrix,getDbTagData(),CommMetaData(3));
    res+= cp.receiveID(constrDOF,getDbTagData(),CommMetaData(4));
    return res;
  }

void XC::MFreedom_ConstraintBase::Print(std::ostream &s, int flag)
  {     
    s << "MFreedom_ConstraintBase: " << this->getTag() << "\n";
    s << "\tNode Constrained: " << getNodeConstrained();
    s << " constrained dof: " << constrDOF;    
    s << " constraint matrix: " << constraintMatrix << "\n";
  }


