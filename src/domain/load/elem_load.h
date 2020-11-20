// -*-c++-*-
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
//elem_load.h

#ifndef ELEM_LOAD_H
#define ELEM_LOAD_H

#include "domain/load/beam_loads/Beam2dPointLoad.h"
#include "domain/load/beam_loads/Beam2dUniformLoad.h"
#include "domain/load/beam_loads/Beam2dPartialUniformLoad.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"
#include "domain/load/beam_loads/Beam3dPointLoad.h"
#include "domain/load/beam_loads/Beam3dUniformLoad.h"
#include "domain/load/beam_loads/TrussStrainLoad.h"
#include "domain/load/volumetric/BrickSelfWeight.h"
#include "domain/load/plane/ShellRawLoad.h"
#include "domain/load/plane/ShellUniformLoad.h"
#include "domain/load/plane/BidimStrainLoad.h"
#include "domain/load/plane/ShellStrainLoad.h"
#include "domain/load/pattern/LoadPattern.h"

namespace XC {

//! @brief Adds a load over elements.
template <class ELEM_LOAD>
ELEM_LOAD *new_elem_load(LoadPattern *lp,int &tag_el)
  {
    ELEM_LOAD *load= nullptr;
    if(lp)
      {
        load= new ELEM_LOAD(tag_el);
        if(load)
          {
            tag_el++;
            if(!lp->addElementalLoad(load))
              {
                delete load;
                std::cerr << __FUNCTION__
		          << "; load over element was not added." << std::endl;
              }
          }
        else
          std::cerr << __FUNCTION__
		    << "; can't create load over elements: " << tag_el << std::endl;
      }
    else
       std::cerr << __FUNCTION__
		 << "; null pointer to load." << std::endl;
    return load;
  }

ElementalLoad *procesa_element_load(LoadPattern *,int &,const std::string &);

} // end of XC namespace

#endif
