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
// $Date: 2005/12/22 00:35:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/SFreedom_Constraint.cpp,v $
                                                                        
                                                                        
// File: ~/domain/constraints/SFreedom_Constraint.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the implementation of class XC::SFreedom_Constraint.

#include <domain/constraints/SFreedom_Constraint.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include "xc_basic/src/matrices/m_int.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "utility/actor/actor/MovableVector.h"
#include "vtkCellType.h"

//! @brief Constructor por defecto.
XC::SFreedom_Constraint::SFreedom_Constraint(int clasTag)
  :Constraint(clasTag), dofNumber(0), valueR(0.0), valueC(0.0), isConstant(true), loadPatternTag(-1) {}

//! @brief Constructor.
//! @param tag: Tag de la coacción.
//! @param node: Tag del nodo cuyos grados de libertad se coaccionan.
XC::SFreedom_Constraint::SFreedom_Constraint(int tag, int node)
  :Constraint(tag, node,CNSTRNT_TAG_SFreedom_Constraint),dofNumber(0), valueR(0.0), valueC(0.0), isConstant(true), 
   loadPatternTag(-1) {}

// constructor for a subclass to use
//! @brief Constructor.
//! @param tag: Identificador de la coacción.
//! @param node: Identificador del nodo cuyos grados de libertad se coaccionan.
//! @param ndof: Número del grado de libertad coaccionado.
XC::SFreedom_Constraint::SFreedom_Constraint(int tag, int node, int ndof, int clasTag)
  :Constraint(tag, node,clasTag), dofNumber(ndof), valueR(0.0), valueC(0.0), isConstant(true), 
   loadPatternTag(-1)
 // valueC is set to 1.0 so that homo will be false when recvSelf() invoked
 // should be ok as valueC cannot be used by subclasses and subclasses should
 // not be used if it is a homogeneous constraint.
  {}

// constructor for object of type XC::SFreedom_Constraint
//! @brief Constructor.
//! @param tag: Identificador de la coacción.
//! @param node: Identificador del nodo cuyos grados de libertad se coaccionan.
//! @param ndof: Número del grado de libertad coaccionado.
//! @param value: Valor impuesto para el grado de libertad.
//! @param ISconstant: Verdadero si el valor impuesto no varía con el tiempo.
XC::SFreedom_Constraint::SFreedom_Constraint(int tag, int node, int ndof, double value, bool ISconstant)
  :Constraint(tag, node, CNSTRNT_TAG_SFreedom_Constraint), dofNumber(ndof), valueR(value), valueC(value), isConstant(ISconstant),
 loadPatternTag(-1) {}

//! @brief Constructor virtual.
XC::SFreedom_Constraint *XC::SFreedom_Constraint::getCopy(void) const
  { return new SFreedom_Constraint(*this); }

//! @brief Copia con otro tag, se emplea en ConstraintLoader::addSFreedom_Constraint(tag_nod,semilla).
XC::SFreedom_Constraint *XC::SFreedom_Constraint::getCopy(const int &spTag) const
  { 
    SFreedom_Constraint *retval= new SFreedom_Constraint(*this);
    retval->setTag(spTag);
    return retval;
  }

//! @brief Devuelve el identificador del grado de libertad al que se impone la coacción.
int XC::SFreedom_Constraint::getDOF_Number(void) const
  { return dofNumber; }


//! @brief Devuelve el valor del desplazamiento impuesto.
double XC::SFreedom_Constraint::getValue(void) const
  { return valueC; }

//! @brief Aplica la coacción.
int XC::SFreedom_Constraint::applyConstraint(double loadFactor)
  {
    // as SFreedom_Constraint objects are time invariant nothing is done
    if(isConstant == false)
      valueC = loadFactor*valueR;
    return 0;
  }

//! @brief Devuelve verdadero si se trata de una coacción homogénea (valor impuesto nulo).
bool XC::SFreedom_Constraint::isHomogeneous(void) const
  {
    if(valueR == 0.0)
      return true;
    else
      return false;
  }

//! @brief Establece el "patrón de carga" que se emplea para la coacción.
//! @param tag: Identificador del "patron de carga".
void XC::SFreedom_Constraint::setLoadPatternTag(int tag)
  { loadPatternTag = tag; }

//! @brief Devuelve el identificador del "patrón de carga" de la coacción.
int XC::SFreedom_Constraint::getLoadPatternTag(void) const
  { return loadPatternTag;  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::SFreedom_Constraint::getDbTagData(void) const
  {
    static DbTagData retval(7);
    return retval;
  }

//! @brief Envía los datos through the channel being passed as parameter.
int XC::SFreedom_Constraint::sendData(CommParameters &cp)
  {
    int res= Constraint::sendData(cp);
    res+= cp.sendInts(dofNumber,loadPatternTag,getDbTagData(),CommMetaData(3));
    res+= cp.sendDoubles(valueC,valueR,getDbTagData(),CommMetaData(4));
    res+= cp.sendBool(isConstant,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Recibe los datos through the channel being passed as parameter.
int XC::SFreedom_Constraint::recvData(const CommParameters &cp)
  {
    int res= Constraint::recvData(cp);
    res+= cp.receiveInts(dofNumber,loadPatternTag,getDbTagData(),CommMetaData(3));
    res+= cp.receiveDoubles(valueC,valueR,getDbTagData(),CommMetaData(4));
    res+= cp.receiveBool(isConstant,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::SFreedom_Constraint::sendSelf(CommParameters &cp)
  {
    inicComm(7);
    int result= sendData(cp);
    const int dbTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << "SFreedom_Constraint::sendSelf() - failed to send extra data\n";
    return result;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::SFreedom_Constraint::recvSelf(const CommParameters &cp)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "SFreedom_Constraint::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }

int XC::SFreedom_Constraint::getVtkCellType(void) const
  { return VTK_VERTEX; }


//! @brief Imprime la coacción.
void XC::SFreedom_Constraint::Print(std::ostream &s, int flag) 
  {
    Constraint::Print(s,flag);
    s << " DOF: " << dofNumber;
    s << " ref value: " << valueR << " current value: " << valueC << std::endl;
  }

