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
//CooSys.h
//Coordinate systems base class.

#ifndef SIS_COO_H
#define SIS_COO_H

#include "../ProtoGeom.h"
#include "utility/geom/FT_matrix.h"

//! @ingroup GEOM
//
//!  @defgroup CooSys Coordinate systems.
//
//!  @ingroup CooSys
//! 
//!  @brief Coordinate systems base class.
//!  
class CooSys: public ProtoGeom
  {
    FT_matrix rot; //Basis vector matrix (as rows).
  protected:
    void identity(void);
    void put(const size_t &i,const size_t &j,const GEOM_FT &);
    void normalize(void);
    void ortogonaliza(void);
    void orthonormalize(void);
    CooSys(const size_t &,const size_t &);
    CooSys(const CooSys &other)
      : ProtoGeom(other), rot(other.rot) {}
    CooSys &operator=(const CooSys &other)
      {
	ProtoGeom::operator=(other);
        rot= other.rot;
        return *this;
      }
    virtual CooSys *Copia(void) const= 0;
  public:
    inline virtual bool operator==(const CooSys &other) const
      { return (rot==other.rot); } 
    inline bool operator!=(const CooSys &other)
      { return !(*this==other); }
    
    virtual size_t numberOfAxis(void) const
      { return rot.getNumberOfRows(); }
    bool EsNormal(void) const;
    bool EsOrtogonal(void) const;
    bool EsOrtonormal(void) const;
    bool EsDextrogiro(void) const;
    FT_matrix getRow(const size_t &i) const;
    void putRow(const size_t &axis,const FT_matrix &v);

    virtual FT_matrix TransAGlobal(void) const;
      //Return the matrix que transforma un vector expresado
      // en locales al mismo vector expresado en globales.
    virtual FT_matrix TransDeGlobal(void) const;
      //Return the matrix que transforma un vector expresado
      // en globales al mismo vector expresado en locales.
    FT_matrix GetTransA(const CooSys &dest) const;
      //Return the transformation matrix of this coordinates system to dest.
    FT_matrix GetCooGlobales(const FT_matrix &v) const;
    FT_matrix GetCooLocales(const FT_matrix &v) const;

    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
    virtual void Print(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &os,const CooSys &sc);
  };

#endif
