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
//BodyMap.cc

#include "BodyMap.h"
#include "Block.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/multi_block_topology/entities/Pnt.h"
#include "preprocessor/multi_block_topology/entities/Body.h"
#include "preprocessor/multi_block_topology/entities/Block.h"
#include "preprocessor/multi_block_topology/entities/QuadSurface.h"
#include "preprocessor/set_mgmt/Set.h"



//! @brief Constructor.
XC::BodyMap::BodyMap(MultiBlockTopology *mbt)
  : EntityMap<Body>(mbt) {}

//! @brief Inserts the new body in the total set and in the set
//! that are open.
void XC::BodyMap::UpdateSets(Body *newBody) const
  {
    Preprocessor *preprocessor= const_cast<Preprocessor *>(getPreprocessor());
    preprocessor->get_sets().get_set_total()->getBodies().push_back(newBody);
    preprocessor->get_sets().insert_ent_mdlr(newBody);
    MapSet::map_sets &open_sets= preprocessor->get_sets().get_open_sets();
    for(MapSet::map_sets::iterator i= open_sets.begin();i!= open_sets.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->getBodies().push_back(newBody);
      }
  }

//! @brief Conciliates the number of divisions of the lines.
bool XC::BodyMap::conciliaNDivs(void)
  {
    getMultiBlockTopology()->conciliaNDivs();
    return checkNDivs();
  }

//! @brief Checks that the number of divisions of the edges
//! are compatible.
bool XC::BodyMap::checkNDivs(void) const
  {
    size_t conta= 0;
    if(!empty())
      for(const_iterator i= begin();i!=end();i++)
        if(!(*i).second->checkNDivs()) conta++;
    return (conta==0);
  }

//! @brief New block.
XC::Block *XC::BodyMap::newBlockPts(const size_t &id_p1, const size_t &id_p2, const size_t &id_p3, const size_t &id_p4, const size_t &id_p5, const size_t &id_p6, const size_t &id_p7, const size_t &id_p8)
  {
    Block *retval= dynamic_cast<Block *>(this->New<Block>());
    assert(retval);
    ID tmp(8);
    tmp[0]= id_p1; tmp[1]= id_p2; tmp[2]= id_p3; tmp[3]= id_p4;
    tmp[4]= id_p5; tmp[5]= id_p6; tmp[6]= id_p7; tmp[7]= id_p8;
    retval->setPoints(tmp);
    return retval;
  }
