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
//ElementLoader.cc

#include "ElementLoader.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "preprocessor/Preprocessor.h"

#include "boost/any.hpp"

#include "domain/mesh/node/Node.h"
#include "utility/tagged/DefaultTag.h"

void XC::ElementLoader::SeedElemLoader::libera(void)
  {
    if(semilla) delete semilla;
    semilla= nullptr;
  }

//! @brief Copy constructor.
XC::ElementLoader::SeedElemLoader::SeedElemLoader(const SeedElemLoader &otro)
  : ProtoElementLoader(otro), semilla(otro.semilla->getCopy()) {}

//! @brief Assignment operator.
XC::ElementLoader::SeedElemLoader &XC::ElementLoader::SeedElemLoader::operator=(const SeedElemLoader &otro)
  {
    ProtoElementLoader::operator=(otro);
    semilla= otro.semilla->getCopy();
    return *this;
  }

//! @brief Defines seed element.
void XC::ElementLoader::SeedElemLoader::agrega(XC::Element *e)
  {
    libera();
    assert(e);
    semilla= e;
  }

//! @brief Destructor.
XC::ElementLoader::SeedElemLoader::~SeedElemLoader(void)
  { libera(); }

void XC::ElementLoader::SeedElemLoader::clearAll(void)
  { libera(); }

int XC::ElementLoader::SeedElemLoader::getDefaultTag(void) const
  {
    int retval= -1;
    const ProtoElementLoader *ldr= dynamic_cast<const ProtoElementLoader *>(Owner());
    if(ldr)
      retval= ldr->getDefaultTag();
    else
      std::cerr << "Error en SeedElemLoader::getTagElem." << std::endl;
    return retval;
  }

XC::ElementLoader::ElementLoader(Preprocessor *preprocessor)
  : ProtoElementLoader(preprocessor), seed_elem_loader(preprocessor) 
  { seed_elem_loader.set_owner(this); }

//! @brief Returns the default tag for next element.
int XC::ElementLoader::getDefaultTag(void) const
  { return Element::getDefaultTag().getTag(); }

//! @brief Sets the default tag for next element.
void XC::ElementLoader::setDefaultTag(const int &tag)
  { Element::getDefaultTag().setTag(tag); }

//! @brief Returns a pointer to the elemento cuyo tag se pasa como parametro.
XC::Element *XC::ElementLoader::getElement(int tag)
  {
    Element *retval= nullptr;
    Domain *tmp= getDomain();
    if(tmp)
      retval= tmp->getElement(tag);
    else
      std::cerr << "XC::ElementLoader::getElement; the pointer al domain es nulo." << std::endl;
    return retval;
  }

//! @brief Agrega el elemento al modelo.
void XC::ElementLoader::agrega(XC::Element *e)
  {
    if(e)
      {
        nuevo_elemento(e);
      }
  }

void XC::ElementLoader::clearAll(void)
  {
    seed_elem_loader.clearAll();
    Element::getDefaultTag().setTag(0);
  }

//! @brief Agrega el elemento asignándole primero el tag que corresponde,
//! se emplea en la clase EntPMdlr.
void XC::ElementLoader::Agrega(XC::Element *e)
  {
    if(e)
      {
        e->setTag(Element::getDefaultTag().getTag());
        agrega(e);
	Element::getDefaultTag()++;
      }
  }


//! @brief Añade un nuevo elemento al modelo.
void XC::ElementLoader::nuevo_elemento(Element *e)
  {
    getDomain()->addElement(e);
    preprocessor->UpdateSets(e);
  }

