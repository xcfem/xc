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
//RangoIndice.cc

#include "RangoIndice.h"
#include "utility/utils/text/text_string.h"
#include <deque>

const char RangoIndice::sep=':';

//! @brief Constructor.
RangoIndice::RangoIndice(const std::vector<size_t> &v)
 :mn(1),mx(1)
  {
    if(v.size()>1)
      SetInfSup(v[0],v[1]);
    else
      std::cerr << "RangoIndice::RangoIndice; error: se pasó un vector"
                << " de dimensión inferior a 2." << std::endl;
  }

//! @brief Imprime el rango.
void RangoIndice::Print(std::ostream &os) const
  { os << mn << ':' << mx; }

void RangoIndice::SetInfSup(const size_t &imin,const size_t &imax)
  {
    mn= std::min(imin,imax);
    mx= std::max(imin,imax);
  }

//! @brief Reduce los valores del rango de manera que ambos sean menores que
//! el que se pasa como parámetro.
void RangoIndice::Clip(const size_t &imax)
  {
    mn= std::min(mn,imax);
    mx= std::min(mx,imax);
  }

//! @brief Asigna a ESTE la intersección de ambos rangos.
void RangoIndice::Intersec(const RangoIndice &otro)
  {
    mn= std::max(mn,otro.mn);
    mx= std::min(mx,otro.mx);
  }

//! @brief Devuelve la intersección de ambos rangos.
RangoIndice RangoIndice::Intersec(const RangoIndice &otro) const
  {
    RangoIndice retval(*this);
    retval.Intersec(otro);
    return retval;
  }

std::ostream &operator<<(std::ostream &os,const RangoIndice &rango)
  {
    rango.Print(os);
    return os;
  }

RangoIndice clip(const RangoIndice &r,const size_t &mx)
  {
    RangoIndice retval(r);
    retval.Clip(mx);
    return retval;
  }

//! @brief Devuelve la intersección de ambos rangos.
RangoIndice intersec(const RangoIndice &r1,const RangoIndice &r2)
  { return r1.Intersec(r2); }


