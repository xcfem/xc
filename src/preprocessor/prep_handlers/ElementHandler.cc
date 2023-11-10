//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
#include "utility/utils/misc_utils/colormod.h"

//! @brief Frees the seed element pointer.
void XC::ElementHandler::SeedElemHandler::free(void)
  {
    if(seed)
      {
	delete seed;
        seed= nullptr;
      }
  }

//! @brief Set the seed element pointer.
void XC::ElementHandler::SeedElemHandler::alloc(Element *e)
  {
    free();
    e= e; // Element allocated elsewhere.
  }

//! @brief Default constructor.
XC::ElementHandler::SeedElemHandler::SeedElemHandler(Preprocessor *preprocessor)
  : ProtoElementHandler(preprocessor), seed(nullptr), elementType() {}

//! @brief Copy constructor.
XC::ElementHandler::SeedElemHandler::SeedElemHandler(const SeedElemHandler &other)
  : ProtoElementHandler(other), seed(other.seed->getCopy()), elementType(other.elementType) {}

//! @brief Assignment operator.
XC::ElementHandler::SeedElemHandler &XC::ElementHandler::SeedElemHandler::operator=(const SeedElemHandler &other)
  {
    ProtoElementHandler::operator=(other);
    this->elementType= other.elementType;
    seed= other.seed->getCopy();
    return *this;
  }

//! @brief Defines seed element.
void XC::ElementHandler::SeedElemHandler::add(Element *e)
  {
    // This function is called by ProtoElementHandler::newElement.
    free();
    assert(e);
    seed= e; // Assign seed element.
  }

//! @brief Destructor.
XC::ElementHandler::SeedElemHandler::~SeedElemHandler(void)
  { clearAll(); }

//! @brief Sets the default material name for new elements.
//! @param name: name of the material.
void XC::ElementHandler::SeedElemHandler::setDefaultMaterial(const std::string &name)
  {
    if(name!=this->getDefaultMaterial()) // If really changes.
      {
	if(seed) // If we have a seed element already
	  {
	    // The seed could be no longer valid with the new material so
	    // we remove it and it will be create in getSeeElement if needed.
	    free();
	  }
	ProtoElementHandler::setDefaultMaterial(name); //update the material name-
      }
  }

void XC::ElementHandler::SeedElemHandler::clearAll(void)
  { free(); }

int XC::ElementHandler::SeedElemHandler::getDefaultTag(void) const
  {
    int retval= -1;
    const ProtoElementHandler *ldr= dynamic_cast<const ProtoElementHandler *>(Owner());
    if(ldr)
      retval= ldr->getDefaultTag();
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; error: pointer to element handler is null."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Return a pointer to the seed element.
XC::Element *XC::ElementHandler::SeedElemHandler::getSeedElement(void)
  {
    if(!seed)
      {
	if(!this->elementType.empty()) // It element type is defined.
	  {
            ID nodeTags; // Empty int vector.
            ProtoElementHandler::newElement(this->elementType, nodeTags);
	  }
      }
    return seed;
  }

//! @brief Return a pointer to the seed element.
const XC::Element *XC::ElementHandler::SeedElemHandler::getSeedElement(void) const
  {
    if(seed)
      return seed;
    else
      {
	SeedElemHandler *this_no_const= const_cast<SeedElemHandler *>(this);
	return this_no_const->getSeedElement();
      }
  }

//! @brief Create a new seed element.
//! @param type: type of element. Available types:'Truss','TrussSection','CorotTruss','CorotTrussSection','Spring', 'Beam2d02', 'Beam2d03',  'Beam2d04', 'Beam3d01', 'Beam3d02', 'ElasticBeam2d', 'ElasticTimoshenkoBeam2d', 'ElasticBeam3d', 'ElasticTimoshenkoBeam3d', 'BeamWithHinges2d', 'BeamWithHinges3d', 'NlBeamColumn2d', 'NlBeamColumn3d','ForceBeamColumn2d', 'ForceBeamColumn3d', 'ShellMitc4', ' shellNl', 'Quad4n', 'Tri31', 'Brick', 'ZeroLength', 'ZeroLengthContact2d', 'ZeroLengthContact3d', 'ZeroLengthSection'.
XC::Element *XC::ElementHandler::SeedElemHandler::newElement(const std::string &type)
  {
    ID nodeTags; // Empty int vector.
    this->elementType= type;
    return ProtoElementHandler::newElement(type, nodeTags);
  }

//! @brief Create a new seed element.
//! @param type: type of element. Available types:'Truss','TrussSection','CorotTruss','CorotTrussSection','Spring', 'Beam2d02', 'Beam2d03',  'Beam2d04', 'Beam3d01', 'Beam3d02', 'ElasticBeam2d', 'ElasticTimoshenkoBeam2d', 'ElasticBeam3d', 'ElasticTimoshenkoBeam3d', 'BeamWithHinges2d', 'BeamWithHinges3d', 'NlBeamColumn2d', 'NlBeamColumn3d','ForceBeamColumn2d', 'ForceBeamColumn3d', 'ShellMitc4', ' shellNl', 'Quad4n', 'Tri31', 'Brick', 'ZeroLength', 'ZeroLengthContact2d', 'ZeroLengthContact3d', 'ZeroLengthSection'.
XC::Element *XC::ElementHandler::SeedElemHandler::newElement(const std::string &type, const ID &nodeTags)
  {
    // std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
    //           << "; seed element does not need to initialize its nodes."
    //           << " Remove the node tags parameter."
    //           << Color::def << std::endl;
    this->elementType= type;
    return ProtoElementHandler::newElement(type, nodeTags);
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
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; domain not yet defined."
		<< Color::def << std::endl;
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
    // Don't reset the tags, they can be already in use by other
    // FEProblem objects.
    // setDefaultTag(0);
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
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; null pointer to element."
		<< Color::def << std::endl;
  }


//! @brief Adds a new element to the model.
void XC::ElementHandler::new_element(Element *e)
  {
    getDomain()->addElement(e);
    getPreprocessor()->updateSets(e);
  }

