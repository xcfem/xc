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

#include "domain/load/ElementBodyLoad.h"
#include <domain/mesh/element/Element.h>
#include <domain/domain/Domain.h>



#include "xc_basic/src/matrices/m_int.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableID.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/MapSet.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "domain/mesh/node/Node.h"

XC::ElementBodyLoad::ElementBodyLoad(int tag, int classTag, const XC::ID &theEleTags)
  :ElementalLoad(tag, classTag,theEleTags), theElements() {}

XC::ElementBodyLoad::ElementBodyLoad(int tag, int classTag)
  :ElementalLoad(tag, classTag), theElements() {}

// provided for the FEM_Object broker; the tag and elementTag need
// to be supplied in recvSelf();
XC::ElementBodyLoad::ElementBodyLoad(int classTag)
  :ElementalLoad(0, classTag), theElements() {}

void XC::ElementBodyLoad::setDomain(Domain *theDomain)
  {
    ElementalLoad::setDomain(theDomain);
    theElements.setPtrs(theDomain,elemTags);
  }

void XC::ElementBodyLoad::applyLoad(double loadFactor) 
  {
    const int sz= theElements.size();
    if(sz!=numElements())
      std::cerr << "ElementBodyLoad::applyLoad; el número de pointers no coincide con el de identificadores." << std::endl;
    for(int i=0; i<sz; i++)
      if(theElements[i])
        theElements[i]->addLoad(this, loadFactor);
  }

//! @brief Elimina el elemento cuyo tag se pasa como parámetro
//! de la lista de elementos afectados por la carga. Si falla
//! devuelve -1.
int XC::ElementBodyLoad::removeElement(int tag) 
  {
    int loc = elemTags.getLocation(tag);
    if(loc >= 0)
      {
        elemTags(loc) = -1;
        theElements.removeElement(tag);
      }
    return numElements();
  }

//! @brief Imprime el objeto.
void XC::ElementBodyLoad::Print(std::ostream &s, int flag) const
  {
    s << "  Elementos sobre los que actúa: " << getElementTags();
  }

//! @brief Envía los datos through the channel being passed as parameter.
int XC::ElementBodyLoad::sendData(CommParameters &cp)
  {
    int res= ElementalLoad::sendData(cp);
    return res;
  }

//! @brief Recibe los datos through the channel being passed as parameter.
int XC::ElementBodyLoad::recvData(const CommParameters &cp)
  {
    int res= ElementalLoad::recvData(cp);
    return res;
  }
