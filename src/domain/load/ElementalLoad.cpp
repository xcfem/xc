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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:01:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/ElementalLoad.cpp,v $
                                                                        
                                                                        
// Written: fmk 11/95
//          modified 11/01 for new design

// Purpose: This file contains the methods for class XC::ElementalLoad.

#include "domain/load/ElementalLoad.h"


XC::ElementalLoad::ElementalLoad(int tag, int classTag)
  :Load(tag, classTag) {}

XC::ElementalLoad::ElementalLoad(int tag, int classTag,const ID &tags_elems)
  :Load(tag, classTag), elemTags(tags_elems) {}

int XC::ElementalLoad::numElements(void) const
  { return elemTags.Size(); }

const XC::ID &XC::ElementalLoad::getElementTags(void) const
  { return elemTags; }

int XC::ElementalLoad::setElementTags(const ID &theEleTags)
  {
    elemTags= theEleTags;
    return 0;
  }

//! @brief Envía los datos through the channel being passed as parameter.
int XC::ElementalLoad::sendData(CommParameters &cp)
  {
    int res= Load::sendData(cp);
    res+= cp.sendID(elemTags,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Recibe los datos through the channel being passed as parameter.
int XC::ElementalLoad::recvData(const CommParameters &cp)
  {
    int res= Load::recvData(cp);
    res+= cp.receiveID(elemTags,getDbTagData(),CommMetaData(2));
    return res;
  }
