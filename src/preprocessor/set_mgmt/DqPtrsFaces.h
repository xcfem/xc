// -*-c++-*-
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
//DqPtrsFaces.h
//Face Container.


#ifndef DQPTRSFACES_H
#define DQPTRSFACES_H

#include "DqPtrsEntities.h"
#include "preprocessor/multi_block_topology/entities/2d/Face.h"
#include "preprocessor/multi_block_topology/entities/1d/EdgeContainer.h"

namespace XC {

//! @brief Container for preprocessor faces.
class DqPtrsFaces: public DqPtrsEntities<Face>
  {
  public:
    DqPtrsFaces(CommandEntity *owr= nullptr);
    DqPtrsFaces(const DqPtrs<Face> &);
    explicit DqPtrsFaces(const std::deque<Face *> &);
    explicit DqPtrsFaces(const std::set<const Face *> &);
    
    DqPtrsFaces &operator-=(const DqPtrsFaces &);
    DqPtrsFaces &operator*=(const DqPtrsFaces &);

    DqPtrsFaces pickSurfacesInside(const GeomObj3d &, const double &tol= 0.0) const;
    Face *getNearest(const Pos3d &);
    const Face *getNearest(const Pos3d &) const;
    

    // Surface orientation.
    Vector3d getIVectorAverage(void) const;
    Vector3d getJVectorAverage(void) const;
    Vector3d getKVectorAverage(void) const;
    void reverse(void);
    void setKOrientation(const Vector3d &);
    std::deque< EdgeContainer::point_sequence > getContoursPointSequences(void) const;
    boost::python::list getContoursPointSequencesPy(void) const;
    std::deque<Polyline3d> getContours(void) const;
  };
 
} //end of XC namespace

#endif

