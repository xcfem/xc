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



#include "xc_utils/src/matrices/m_int.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableID.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/MapSet.h"
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
    const size_t sz= elemTags.Size();
    if(sz==0)
      std::clog << getClassName() << "::" << __FUNCTION__
                << "; no element identifiers." << std::endl;
    theElements.setPtrs(theDomain,elemTags);
  }

//! @brief Applies the load to the elements.
void XC::ElementBodyLoad::applyLoad(double loadFactor) 
  {
    const int sz= theElements.size();
    const int numEle= numElements();
    if(sz!=numEle)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; the number of pointers (" << sz
	        << ") does not match the number of identifiers ("
	        << numEle << "); something went wrong." << std::endl;
    for(int i=0; i<sz; i++)
      if(theElements[i])
        theElements[i]->addLoad(this, loadFactor);
  }

//! @brief Removes the element from those affected by
//! the load. If it fails returns -1.
int XC::ElementBodyLoad::removeElement(int tag) 
  {
    int loc = elemTags.getLocation(tag);
    if(loc>=0)
      {
        elemTags(loc) = -1;
        theElements.removeElement(tag);
      }
    return numElements();
  }

//! @brief Print stuff.
void XC::ElementBodyLoad::Print(std::ostream &s, int flag) const
  {
    s << "  loaded elements: " << getElementTags();
  }

//! @brief Send data through the channel being passed as parameter.
int XC::ElementBodyLoad::sendData(CommParameters &cp)
  {
    int res= ElementalLoad::sendData(cp);
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::ElementBodyLoad::recvData(const CommParameters &cp)
  {
    int res= ElementalLoad::recvData(cp);
    return res;
  }
