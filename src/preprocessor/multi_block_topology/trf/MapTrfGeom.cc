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
//MapTrfGeom.cc

#include "MapTrfGeom.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/multi_block_topology/trf/TrfGeom.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/set_mgmt/Set.h"


#include "Rotation.h"
#include "Reflection.h"
#include "Scaling.h"
#include "Translation.h"

//! @brief Constructor.
XC::MapTrfGeom::MapTrfGeom(MultiBlockTopology *mbt)
  : ModelComponentContainer<TrfGeom>(mbt) {}

//! @brief Defines a new transformation.
XC::TrfGeom *XC::MapTrfGeom::newTransformation(const std::string &type)
  {
    TrfGeom *retval= nullptr;
    if(type == "rotation") //Creates a new rotation.
      retval= this->New<Rotation>();
    else if(type == "reflection") //Creates a new reflection.
      retval= this->New<Reflection>();
    else if(type == "scaling") //Creates a new scaling.
      retval= this->New<Scaling>();
    else if(type == "translation") //Crea una nueva translation.
      retval= this->New<Translation>();
    else
      std::cerr << "Transformation type: '" << type
                << "' unknown." << std::endl;
    return retval;
  }
