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
// $Date: 2005/12/22 00:35:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/MP_Constraint.cpp,v $
                                                                        
                                                                        
// File: ~/domain/constraints//MP_Constraint.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the implementation of class XC::MP_Constraint.
//
// The class XC::MP_Constraint interface:
//

#include <domain/constraints/MP_Constraint.h>

#include <cstdlib>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "xc_utils/src/base/CmdStatus.h"
#include <boost/any.hpp>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_basic/src/matrices/m_int.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "med.h"
#include "vtkCellType.h"

void XC::MP_Constraint::set_retained_dofs(const ID &retainedDOF)
  { retainDOF= retainedDOF; }

void XC::MP_Constraint::set_constrained_retained_dofs(const ID &constrainedDOF,const ID &retainedDOF)
  {
    set_constrained_dofs(constrainedDOF);
    set_retained_dofs(retainedDOF);
  }

// constructor for FEM_ObjectBroker			// Arash
XC::MP_Constraint::MP_Constraint(int tag,int clasTag)		
 : MP_ConstraintBase(tag,0,clasTag),
   retainedNodeTag(0) {}

// constructor for FEM_ObjectBroker			// LCPT
XC::MP_Constraint::MP_Constraint(int tag)		
 : MP_ConstraintBase(tag, 0,CNSTRNT_TAG_MP_Constraint),
   retainedNodeTag(0) {}

// constructor for Subclass
XC::MP_Constraint::MP_Constraint(int tag, int nodeRetain, int nodeConstr, int clasTag)
 : MP_ConstraintBase(tag, nodeConstr, clasTag),
   retainedNodeTag(nodeRetain) {}

// constructor for Subclass
XC::MP_Constraint::MP_Constraint(int tag, int nodeRetain, int nodeConstr,const ID &constrainedDOF,const ID &retainedDOF, int clasTag)
 : MP_ConstraintBase(tag, nodeConstr, clasTag),
   retainedNodeTag(nodeRetain)
  { set_constrained_retained_dofs(constrainedDOF,retainedDOF); }

// constructor for Subclass
XC::MP_Constraint::MP_Constraint(int tag, int nodeRetain, int nodeConstr,const ID &constrainedDOF,const ID &retainedDOF)
 : MP_ConstraintBase(tag, nodeConstr,CNSTRNT_TAG_MP_Constraint), 
   retainedNodeTag(nodeRetain)
  { set_constrained_retained_dofs(constrainedDOF,retainedDOF); }


// general constructor for XC::ModelBuilder
XC::MP_Constraint::MP_Constraint(int tag, int nodeRetain, int nodeConstr, Matrix &constr,
			     ID &constrainedDOF, ID &retainedDOF)
 : MP_ConstraintBase(tag, nodeConstr, CNSTRNT_TAG_MP_Constraint),
  retainedNodeTag(nodeRetain)
  {
    set_constrained_retained_dofs(constrainedDOF,retainedDOF);    
    set_constraint(constr);
  }

//! @brief Constructor de copia.
XC::MP_Constraint::MP_Constraint(const MP_Constraint &otro)
  : MP_ConstraintBase(otro), retainedNodeTag(otro.retainedNodeTag)
  {
    set_constrained_dofs(otro.constrDOF);
    set_retained_dofs(otro.retainDOF);
    set_constraint(otro.constraintMatrix);
  }
//! @brief Operador de asignación.
XC::MP_Constraint &XC::MP_Constraint::operator=(const MP_Constraint &otro)
  {
    MP_ConstraintBase::operator=(otro);
    retainedNodeTag= otro.retainedNodeTag;
    set_constrained_dofs(otro.constrDOF);
    set_retained_dofs(otro.retainDOF);
    set_constraint(otro.constraintMatrix);
    return *this;
  }

//! @brief Devuelve verdadero si afecta al nodo cuyo tag se pasa como parámetro.
bool XC::MP_Constraint::afectaANodo(int tagNodo) const
  { return ( (tagNodo== getNodeConstrained()) || (tagNodo== getNodeRetained()));}

const XC::ID &XC::MP_Constraint::getRetainedDOFs(void) const
  {
    // return the XC::ID corresponding to retained DOF of Ccr
    return retainDOF;    
  }

int XC::MP_Constraint::applyConstraint(double timeStamp)
  {
    // does nothing XC::MP_Constraint objects are time invariant
    return 0;
  }

//! @brief Envía los datos a través del canal que se pasa como parámetro.
int XC::MP_Constraint::sendData(CommParameters &cp)
  {
    int res= MP_ConstraintBase::sendData(cp);
    res+= cp.sendID(retainDOF,getDbTagData(),CommMetaData(5));
    res+= cp.sendInt(retainedNodeTag,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
int XC::MP_Constraint::recvData(const CommParameters &cp)
  {
    int res= MP_ConstraintBase::recvData(cp);
    res+= cp.receiveID(retainDOF,getDbTagData(),CommMetaData(5));
    res+= cp.receiveInt(retainedNodeTag,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::MP_Constraint::sendSelf(CommParameters &cp)
  {
    inicComm(14);
    int result= sendData(cp);
    const int dataTag= getDbTag();
    result = cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "WARNING MP_Constraint::sendSelf - error sending XC::ID data\n";
    return result;
  }


//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::MP_Constraint::recvSelf(const CommParameters &cp)
  {
    inicComm(14);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "MP_Constraint::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }


void XC::MP_Constraint::Print(std::ostream &s, int flag)
  {     
    s << "MP_Constraint: " << this->getTag() << "\n";
    s << "\tNode Constrained: " << getNodeConstrained();
    s << " node Retained: " << getNodeRetained() ;
    s << " constrained dof: " << constrDOF;    
    s << " retained dof: " << retainDOF;        
    s << " constraint matrix: " << constraintMatrix << "\n";
  }


//! @brief Interfaz con VTK.
int XC::MP_Constraint::getVtkCellType(void) const
  {
    std::cerr << "MP_Constraint::getVtkCellType; no implementada." << std::endl;
    return VTK_EMPTY_CELL;
  }

//! @brief Interfaz con el formato MED de Salome.
int XC::MP_Constraint::getMEDCellType(void) const
  { return MED_SEG2; }

