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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.4 $
// $Date: 2003/03/04 00:48:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/component/DomainComponent.cpp,v $
                                                                        
                                                                        
// File: ~/domain/component//DomainComponent.C
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class implementation for XC::DomainComponent
//
// What: "@(#) DomainComponent.h, revA"


#include <domain/component/DomainComponent.h>
#include <domain/domain/Domain.h>
#include "preprocessor/Preprocessor.h"
#include "boost/any.hpp"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::DomainComponent::DomainComponent(int tag, int classTag)
  :TaggedObject(tag), MovableObject(classTag), theDomain(nullptr), idx(0)
  {}

//! @brief Destructor.
XC::DomainComponent::~DomainComponent(void)
  { setDomain(nullptr); }

//! @brief Asigna un puntero al dominio. 
void XC::DomainComponent::setDomain(Domain *model)
  { theDomain = model; }

//! @brief Asigna el índice al objeto (ver numera en Set).
void XC::DomainComponent::set_indice(const size_t &i)
  { idx= i; }

//! @brief Devuelve un puntero al dominio.
XC::Domain *XC::DomainComponent::getDomain(void) const
  { return theDomain; }

//! @brief Devuelve (si puede) un puntero al preprocesador.
const XC::Preprocessor *XC::DomainComponent::GetPreprocessor(void) const
  {
    const Preprocessor *retval= nullptr;
    if(theDomain)
      retval= theDomain->GetPreprocessor();
    return retval;
  }

//! @brief Devuelve (si puede) un puntero al preprocesador.
XC::Preprocessor *XC::DomainComponent::GetPreprocessor(void)
  {
    Preprocessor *retval= nullptr;
    if(theDomain)
      retval= theDomain->GetPreprocessor();
    return retval;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::DomainComponent::sendData(CommParameters &cp)
  {
    setDbTagDataPos(0,getTag());
    int res= cp.sendSzt(idx,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::DomainComponent::recvData(const CommParameters &cp)
  {
    setTag(getDbTagDataPos(0));
    int res= cp.receiveSzt(idx,getDbTagData(),CommMetaData(1));
    return res;
  }
