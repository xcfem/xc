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
//PntMap.h

#ifndef PNTMAP_H
#define PNTMAP_H

#include "EntityMap.h"
#include <map>

class Pos3d;
class Line3d;
class Plane;
class Vector3d;

namespace XC {

class Preprocessor;
class MultiBlockTopology;
class Pnt;
class SetEstruct;
class TrfGeom;

//! @ingroup MultiBlockTopology
//
//! @brief Point container.
class PntMap: public EntityMap<Pnt>
  {
    const double tol= 0.01;
  private:
    void updateSets(Pnt *) const;
  protected:
    Pnt *Crea(void);
  public:
    PntMap(MultiBlockTopology *mbt= nullptr);

    Pos3d getCentroid(void) const;
    Vector3d getVector(const Indice &i,const Indice &j) const;
    Line3d getRecta(const Indice &i,const Indice &j) const;
    Plane getPlane(const Indice &i,const Indice &j,const Indice &k) const;

    bool checkPosition(const Pos3d &);
    Pnt *New(void);
    Pnt *New(const Pos3d &);
    Pnt *New(const size_t &,const Pos3d &);

    Pnt *Copy(const Pnt *,const Vector3d &v);
    void Copy(const std::vector<PntMap::Indice> &);
    void Transform(const TrfGeom &trf,const std::vector<PntMap::Indice> &);
    double Dist(const Indice &i,const Indice &j) const;
  };

} //end of XC namespace
#endif
