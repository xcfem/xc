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

//! @brief Constructor. Provided for the FEM_Object broker; the tag
//! and elementTag need to be supplied in recvSelf();
XC::ElementBodyLoad::ElementBodyLoad(int classTag)
  :ElementalLoad(0, classTag), theElements() {}

//! @brief Constructor.
XC::ElementBodyLoad::ElementBodyLoad(int tag, int classTag)
  :ElementalLoad(tag, classTag), theElements() {}

//! @brief Constructor.
XC::ElementBodyLoad::ElementBodyLoad(int tag, int classTag, const ID &theEleTags)
  :ElementalLoad(tag, classTag,theEleTags), theElements() {}


//! @brief Set the domain for the load (assigns the pointers
//! to the elements.
void XC::ElementBodyLoad::setDomain(Domain *theDomain)
  {
    ElementalLoad::setDomain(theDomain);
    const size_t sz= elemTags.Size();
    if(sz==0)
      std::clog << getClassName() << "::" << __FUNCTION__
                << "; no element identifiers." << std::endl;
    else
      theElements.setPtrs(theDomain,elemTags);
  }

//! @brief Applies the load to the elements.
void XC::ElementBodyLoad::applyLoad(double loadFactor) 
  {
    int sz= theElements.size();
    const int numEle= numElements();
    if(sz!=numEle)
      {
	// Try to set the pointers.
	Domain *dom= getDomain();
	if(dom)
	  theElements.setPtrs(dom, elemTags);
	sz= theElements.size();
	if(sz!=numEle)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; the number of pointers (" << sz
		    << ") does not match the number of identifiers ("
		    << numEle << "); something went wrong."
		    << " Load tag: " << getTag()
		    << " load pattern tag: " << getLoadPatternTag()
		    << " element tags: " << elemTags << std::endl;
      }
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

//! @brief Send data through the communicator argument.
int XC::ElementBodyLoad::sendData(Communicator &comm)
  {
    int res= ElementalLoad::sendData(comm);
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::ElementBodyLoad::recvData(const Communicator &comm)
  {
    int res= ElementalLoad::recvData(comm);
    return res;
  }
