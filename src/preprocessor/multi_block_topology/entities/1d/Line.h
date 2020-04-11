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
//Line.h

#ifndef LINE_H
#define LINE_H

#include "LineBase.h"

class Segment3d;

namespace XC {

//! @ingroup MultiBlockTopologyEnt
//!
//! @brief Line: base class for 1D objects.
class Line: public LineBase
  {
  protected:
    Pos3dArray get_positions(void) const;
    Edge *split_at(Pnt *,const double &,const double &);
  public:
    Line(Preprocessor *m,const size_t &ndiv= 4);
    Line(const std::string &name= "",Preprocessor *m= nullptr,const size_t &ndiv= 4);
    virtual SetEstruct *getCopy(void) const;

    double getLength(void) const;
    Pos3d getCentroid(void) const;
    virtual Vector3d getIVector(void) const;
    virtual Vector3d getJVector(void) const;
    Edge *splitAtPoint(Pnt *p);
    Edge *splitAtLambda(const double &);
    Edge *splitAtNaturalCoord(const double &);
    Edge *splitAtPos3d(const Pos3d &, const double &tol= 1e-6);

    double getLambda(const Pos3d &) const;
    
    //! @brief Return the number of vertices.
    inline virtual size_t getNumberOfVertices(void) const
      { return 2; }
    virtual BND3d Bnd(void) const;
    double getSquaredDistanceTo(const Pos3d &pt) const;

    const Vector &getVector(void) const;
    Segment3d getLineSegment(void) const;
    virtual const Vector &getTang(const double &) const;
    int getVtkCellType(void) const;


  };

} //end of XC namespace
#endif
