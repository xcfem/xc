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
//CircularArc.h

#ifndef ARCOCIRCUNF_H
#define ARCOCIRCUNF_H

#include "LineBase.h"


class CircularSector3d;

namespace XC {

//! @ingroup MultiBlockTopologyEnt
//!
//! @brief Circumference arc.
  
class CircularArc: public LineBase
  {
    Pnt *p3; //!< Midpoint of the arc.
  protected:
    const CircularSector3d get_sector_circular3d(void) const;
    Pos3dArray get_positions(void) const;
    bool check_points(void) const;
  public:
    CircularArc(Preprocessor *m);
    CircularArc(const std::string &name= "",Preprocessor *m= nullptr);
    virtual bool operator==(const CircularArc &) const;
    virtual SetEstruct *getCopy(void) const;
    const Pnt *P3(void) const;

    void update_topology(void);

    double getLength(void) const;
    Pos3d getCentroid(void) const;
    double getIncludedAngle(void) const;
    double getTheta1(void) const;
    double getTheta2(void) const;
    Pos3d getCenter(void) const;
    Pos3d getPInic(void) const;
    Pos3d getPFin(void) const;
    Pos3d getPMed(void) const;
    double getRadius(void) const;
    double getLambda(const Pos3d &) const;

    //! @brief Return the number of vertices.
    inline virtual size_t getNumberOfVertices(void) const
      { return 3; }
    virtual const Pnt *getVertex(const size_t &i) const;
    virtual void SetVertice(const size_t &,Pnt *);
    virtual BND3d Bnd(void) const;
    int getVtkCellType(void) const;
    virtual ID getKPoints(void) const;
    virtual std::deque<Segment3d> getSegments(void) const;
  };

} //end of XC namespace
#endif
