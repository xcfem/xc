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
//BidimMecLoad.cpp

#include "BidimMecLoad.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

#include "domain/mesh/element/utils/coordTransformation/CrdTransf.h"
#include "utility/actor/actor/MovableVector.h"
#include "domain/mesh/node/Node.h"
#include "utility/geom/pos_vec/SlidingVectorsSystem3d.h"

XC::BidimMecLoad::BidimMecLoad(int tag, int classTag, const ID &theElementTags)
  :BidimLoad(tag, classTag, theElementTags) {}

XC::BidimMecLoad::BidimMecLoad(int tag,int classTag)
  :BidimLoad(tag, classTag) {}

//! brief Returns load resultant (force and moment integration over the elements).
//!
//! @param center: origin for the sliding vector system.
//! @param initialGeometry: if true don't use deformed geometry.
SlidingVectorsSystem3d XC::BidimMecLoad::getResultant(const Pos3d &center, bool initialGeometry) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not yet implemented." << std::endl;
    return SlidingVectorsSystem3d(center);
  }

