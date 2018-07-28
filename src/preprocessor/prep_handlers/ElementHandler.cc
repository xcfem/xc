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
//ElementHandler.cc

#include "ElementHandler.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "preprocessor/Preprocessor.h"

#include "boost/any.hpp"

#include "domain/mesh/node/Node.h"
#include "utility/tagged/DefaultTag.h"

void XC::ElementHandler::SeedElemHandler::free_mem(void)
  {
    if(seed) delete seed;
    seed= nullptr;
  }

//! @brief Copy constructor.
XC::ElementHandler::SeedElemHandler::SeedElemHandler(const SeedElemHandler &other)
  : ProtoElementHandler(other), seed(other.seed->getCopy()) {}

//! @brief Assignment operator.
XC::ElementHandler::SeedElemHandler &XC::ElementHandler::SeedElemHandler::operator=(const SeedElemHandler &other)
  {
    ProtoElementHandler::operator=(other);
    seed= other.seed->getCopy();
    return *this;
  }

//! @brief Defines seed element.
void XC::ElementHandler::SeedElemHandler::add(XC::Element *e)
  {
    free_mem();
    assert(e);
    seed= e;
  }

//! @brief Destructor.
XC::ElementHandler::SeedElemHandler::~SeedElemHandler(void)
  { free_mem(); }

void XC::ElementHandler::SeedElemHandler::clearAll(void)
  { free_mem(); }

int XC::ElementHandler::SeedElemHandler::getDefaultTag(void) const
  {
    int retval= -1;
    const ProtoElementHandler *ldr= dynamic_cast<const ProtoElementHandler *>(Owner());
    if(ldr)
      retval= ldr->getDefaultTag();
    else
      std::cerr << "Error en SeedElemHandler::getTagElem." << std::endl;
    return retval;
  }

XC::ElementHandler::ElementHandler(Preprocessor *preprocessor)
  : ProtoElementHandler(preprocessor), seed_elem_handler(preprocessor) 
  { seed_elem_handler.set_owner(this); }

//! @brief Returns the default tag for next element.
int XC::ElementHandler::getDefaultTag(void) const
  { return Element::getDefaultTag().getTag(); }

//! @brief Sets the default tag for next element.
void XC::ElementHandler::setDefaultTag(const int &tag)
  { Element::getDefaultTag().setTag(tag); }

//! @brief Returns a pointer to the element identified
//! by the tag being passed as parameter.
XC::Element *XC::ElementHandler::getElement(int tag)
  {
    Element *retval= nullptr;
    Domain *tmp= getDomain();
    if(tmp)
      retval= tmp->getElement(tag);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; domain not yet defined." << std::endl;
    return retval;
  }

//! @brief Adds the element to the model.
void XC::ElementHandler::add(Element *e)
  {
    if(e)
      new_element(e);
  }

void XC::ElementHandler::clearAll(void)
  {
    seed_elem_handler.clearAll();
    Element::getDefaultTag().setTag(0);
  }

//! @brief Adds the element and set its identifier (tag),
//! use in EntPMdlr class.
void XC::ElementHandler::Add(Element *e)
  {
    if(e)
      {
        e->setTag(Element::getDefaultTag().getTag());
        add(e);
	Element::getDefaultTag()++;
      }
  }


//! @brief Adds a new element to the model.
void XC::ElementHandler::new_element(Element *e)
  {
    getDomain()->addElement(e);
    getPreprocessor()->UpdateSets(e);
  }

