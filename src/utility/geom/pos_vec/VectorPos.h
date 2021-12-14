// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//VectorPos.h
//Positions vector.

#ifndef VECTORPOS_H
#define VECTORPOS_H

#include "PosArray.h"


//! @ingroup GEOM
//
//! @brief Clase base para los vectores de posiciones.
template <class POS>
class VectorPos: public PosArray<POS>
  {
  public:
    typedef PosArray<POS> m_pos;
  public:
    VectorPos(const size_t &f=1): m_pos(f,1) {}
    VectorPos(const POS &p1,const POS &p2,const size_t &num,const GEOM_FT &ratio);
    VectorPos(const POS &p1,const POS &p2,const size_t &ndiv);
    inline VectorPos<POS> getBox(size_t f1, size_t f2) const
      { return VectorPos(*this,f1,1,f2,1); }
    inline const POS &getRow(size_t iRow) const
      { return m_pos::operator()(iRow,1); }
    inline VectorPos<POS> getColumn(size_t col) const
      { return m_pos::getColumn(1); }
  };

template <class POS>
VectorPos<POS>::VectorPos(const POS &p1,const POS &p2,const size_t &num,const GEOM_FT &ratio)
  : m_pos(p1,p2,num,ratio) {}

template <class POS>
VectorPos<POS>::VectorPos(const POS &p1,const POS &p2,const size_t &ndiv)
  : m_pos(p1,p2,ndiv) {}

#endif
