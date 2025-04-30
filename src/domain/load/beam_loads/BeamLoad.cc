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
//BeamLoad.cpp

#include "domain/load/beam_loads/BeamLoad.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/ID.h"
#include "material/ResponseId.h"
#include "domain/mesh/element/Element.h"
#include "material/section/PrismaticBarCrossSection.h"
#include "domain/mesh/element/truss_beam_column/BeamColumn.h"

XC::BeamLoad::BeamLoad(int tag,int classTag,const XC::ID &theElementTags)
  :ElementBodyLoad(tag, classTag, theElementTags) {}

XC::BeamLoad::BeamLoad(int tag,int classTag)
  :ElementBodyLoad(tag, classTag) {}

void XC::BeamLoad::Print(std::ostream &s, int flag) const
  {
    s << "BeamLoad - Reference load" << std::endl;
    s << "  Elements acted on: " << this->getElementTags();
  }

std::string XC::BeamLoad::Category(void) const
  { return "undefined"; }

//! @brief Return the strains at both ends of the beam element.
XC::ResponseId XC::BeamLoad::getElementResponseId(const Element &e) const
  {
    ResponseId retval;
    const int elemTag= e.getTag();
    if(this->actsOnElement(elemTag))
      {
	const BeamColumn *ptrBeam= dynamic_cast<const BeamColumn *>(&e);
	if(ptrBeam)
	  {
	    const PrismaticBarCrossSection *section= ptrBeam->getSectionPtr(0);
	    retval= section->getResponseType();
	  }
	else
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << ": element: " << elemTag
		      << " with type: "
		      << e.getClassName()
		      << " is incompatible with this type of load."
		      << std::endl;
	  }
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << ": element with tag: " << elemTag
		  << " not loaded."
		  << std::endl;
      }
    return retval;
  }
