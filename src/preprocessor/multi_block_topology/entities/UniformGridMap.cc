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
//UniformGridMap.cc

#include "UniformGridMap.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/multi_block_topology/entities/Pnt.h"
#include "preprocessor/multi_block_topology/entities/UniformGrid.h"
#include "preprocessor/multi_block_topology/entities/QuadSurface.h"
#include "preprocessor/set_mgmt/Set.h"



//! @brief Constructor.
XC::UniformGridMap::UniformGridMap(MultiBlockTopology *mbt)
  : EntityMap<UniformGrid>(mbt) {}

//! @brief Insert the new line in the total and the opened sets.
void XC::UniformGridMap::UpdateSets(UniformGrid *new_unif_grid) const
  {
    Preprocessor *preprocessor= const_cast<Preprocessor *>(getPreprocessor());
    preprocessor->get_sets().get_set_total()->getUniformGrids().push_back(new_unif_grid);
    preprocessor->get_sets().insert_ent_mdlr(new_unif_grid);
    MapSet::map_sets &open_sets= preprocessor->get_sets().get_open_sets();
    for(MapSet::map_sets::iterator i= open_sets.begin();i!= open_sets.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->getUniformGrids().push_back(new_unif_grid);
      }
  }

//! @brief New uniform grid.
XC::UniformGrid *XC::UniformGridMap::Nueva(void)
  {
    UniformGrid *retval= busca(getTag());
    if(!retval) //new grid.
      {
        Preprocessor *preprocessor= getPreprocessor();
        retval= new UniformGrid(preprocessor);
        (*this)[getTag()]= retval;
        UpdateSets(retval);
        tag++;
      }
    return retval;
  }
