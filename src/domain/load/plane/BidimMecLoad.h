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
//BidimMecLoad.h
                                                                        
#ifndef BidimMecLoad_h
#define BidimMecLoad_h


#include "BidimLoad.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
class SlidingVectorsSystem3d;

namespace XC {
class Matrix;

//! @ingroup ElemLoads
//
//! @brief Mechanical load (forces) over bidimensional elements.
class BidimMecLoad: public BidimLoad
  {
  public:
    BidimMecLoad(int tag, int classTag, const ID &theElementTags);
    BidimMecLoad(int tag, int classTag);

    virtual SlidingVectorsSystem3d getResultant(const Pos3d &p= Pos3d(), bool initialGeometry= true) const;
  };
} // end of XC namespace

#endif

