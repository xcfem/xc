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
///////////////////////////////////////////////////////////////////////////////
//
// COPYRIGHT (C):     :-))
// PROJECT:           Object Oriented Finite XC::Element Program
// FILE:              BrickSelfWeight
// CLASS:             
// MEMBER FUNCTIONS:
//
// MEMBER VARIABLES
//
// PURPOSE:           Finite XC::Element Class
// RETURN:
// VERSION:
// LANGUAGE:          C++
// TARGET OS:         DOS || UNIX || . . .
// DESIGNER:          Zhaohui Yang
// PROGRAMMER:        Zhaohui Yang 
// DATE:              March 2002
// UPDATE HISTORY:
//
//
//
///////////////////////////////////////////////////////////////////////////////
//

#ifndef BRICKSELFWEIGHT_CPP
#define BRICKSELFWEIGHT_CPP

                                                                        
// Written: ZHYang UC Davis
// Purpose: This file contains the class definition for 8 node brick self weigth load.

#include <domain/load/volumen/BrickSelfWeight.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

XC::Vector XC::BrickSelfWeight::data(1);

XC::BrickSelfWeight::BrickSelfWeight(int tag, const XC::ID &theElementTags)
  :ElementBodyLoad(tag, LOAD_TAG_BrickSelfWeight, theElementTags) {}

XC::BrickSelfWeight::BrickSelfWeight(int tag)
  :ElementBodyLoad(tag, LOAD_TAG_BrickSelfWeight) {}

XC::BrickSelfWeight::BrickSelfWeight(void)
  :ElementBodyLoad(LOAD_TAG_BrickSelfWeight) {}

int XC::BrickSelfWeight::getType(void)
  { return LOAD_TAG_BrickSelfWeight; }

//! @brief Send members del objeto a través del
//! canal being passed as parameter.
int XC::BrickSelfWeight::sendData(CommParameters &cp)
  {
    int res= ElementBodyLoad::sendData(cp);
    return res;
  }

//! @brief Receives members del objeto a través del
//! canal being passed as parameter.
int XC::BrickSelfWeight::recvData(const CommParameters &cp)
  {        
    int res= ElementBodyLoad::recvData(cp);
    return res;
  }

int XC::BrickSelfWeight::sendSelf(CommParameters &cp)
  {
    static ID data(5);
    int result= sendData(cp);
    
    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "BrickSelfWeight::sendSelf - failed to send data\n";
    return result;
  }

int XC::BrickSelfWeight::recvSelf(const CommParameters &cp)
  {
    static ID data(5);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "BrickSelfWeight::recvSelf() - failed to recv data\n";
    else
      res+= recvData(cp);
    return res;
  }

void XC::BrickSelfWeight::Print(std::ostream &s, int flag)
  {
    s << "BrickSelfWeight...";
    s << "  elements acted on: " << this->getElementTags();
  }

#endif

