//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//Loader.cc

#include "Loader.h"
#include "preprocessor/Preprocessor.h"
#include "domain/domain/Domain.h"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"


//! @brief Default constructor.
XC::Loader::Loader(Preprocessor *owr)
  : EntCmd(owr), MovableObject(0), preprocessor(owr) 
  {}

//! @brief Devuelve el domain en el que se cargan los elementos.
XC::Domain *XC::Loader::getDomain(void) const
  {
    if(preprocessor)
      return preprocessor->getDomain();
    else
      {
	std::cerr << "(Loader) necesito un preprocesador." << std::endl;
        return nullptr;
      }
  }

//! @brief Asigna el preprocesador.
void XC::Loader::set_preprocessor(Preprocessor *p)
  { preprocessor= p; }

XC::Loader::~Loader(void)
  { preprocessor= nullptr; }

//! @brief Sends object through the channel being passed as parameter.
int XC::Loader::sendSelf(CommParameters &cp)
  {
    std::cerr << "Loader::sendSelf() no implementada.\n";    
    return -1;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::Loader::recvSelf(const CommParameters &cp)
  {
    std::cerr << "Loader::recvSelf() no implementada.\n";    
    return -1;
  }

