// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//Xd3dCooSys.h
//Coordinate system of dimension X defined in a three-dimensional space.

#ifndef XD3D_COO_SYS_H
#define XD3D_COO_SYS_H

#include "CooSys.h"

class Vector3d;
class Dir3d;
class Pos3d;

//! @ingroup CooSys
//! 
//! @brief Base class for coordinate systems
//! defined in a three-dimensional space.
class Xd3dCooSys: public CooSys
  {
  public:
    typedef Vector3d VGlobal; //!< Vector in the 3D space.
    typedef Pos3d PGlobal; //!< Point in the 3D space.
    typedef Dir3d DGlobal; //!< Direction in the 3D space.
  protected:
    void vectores_unitarios(const VGlobal &i_,const VGlobal &j_,const VGlobal &k_);
    Xd3dCooSys(const size_t &i)
      : CooSys(i,3) {}
    Xd3dCooSys(const size_t &i,const VGlobal &v1);
    Xd3dCooSys(const size_t &i,const VGlobal &v1,const VGlobal &v2);
    Xd3dCooSys(const size_t &i,const PGlobal &o,const PGlobal &p);
    Xd3dCooSys(const size_t &i,const PGlobal &p1,const PGlobal &p2, const PGlobal &p3);
    Xd3dCooSys(const size_t &i,const VGlobal &v1,const VGlobal &v2,const VGlobal &v3);


    void putRow(const size_t &axis,const VGlobal &v);
    void XAxisVector(const VGlobal &v);
    void TwoPoints(const PGlobal &o,const PGlobal &p);
    void ThreePoints(const PGlobal &o,const PGlobal &p1,const PGlobal &p2);

  public:
    //@ brief Constructor virtual.
    virtual Xd3dCooSys *CopiaXd3d(void) const= 0;
    virtual VGlobal getAxisVDir(const size_t &axis) const;
    //Return the direction vector of the axis.
    virtual DGlobal getAxisDir(const size_t &axis) const;

    VGlobal GetCooGlobales(const FT_matrix &v) const;
    FT_matrix GetCooLocales(const VGlobal &v) const;
  };

#endif
