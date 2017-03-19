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
//Eje.cc

#include "Eje.h"
#include "Spot.h"
#include "xc_basic/src/matrices/m_int.h"
#include "xc_utils/src/geom/d2/BND2d.h"


#include "boost/any.hpp"

//! @brief Constructor.
XC::Eje::Eje(GeomSection *sr,const size_t &nd)
  : EntGeomSection("",sr), ndiv(nd) {}

//! @brief Constructor.
//! @param name: object identifier.
//! @param sr: pointer to section geometry.
//! @param nd: number of divisions.
XC::Eje::Eje(const std::string &name,GeomSection *sr,const size_t &nd)
  : EntGeomSection(name,sr), ndiv(nd) {}

//! @brief Destructor.
XC::Eje::~Eje(void)
  {
    if(P1()) P1()->borra_linea(this);
    if(P2()) P2()->borra_linea(this);
  }

//! @brief Updates topology.
void XC::Eje::actualiza_topologia(void)
  {
    if(P1()) P1()->inserta_linea(this);
    if(P2()) P2()->inserta_linea(this);
  }

//! @brief Returns a pointer to start point.
XC::Spot *XC::Eje::P1(void)
  {
    std::cerr << "Eje::P1; this function must be redefined on derived"
              << " classes, nullptr returned;" << std::endl;
    return nullptr;
  }
//! @brief Returns a constant pointer to start point.
const XC::Spot *XC::Eje::P1(void) const
  {
    std::cerr << "Eje::P1; this function must be redefined on derived"
              << " classes, nullptr returned;" << std::endl;
    return nullptr;
  }
//! @brief Returns a pointer to end point.
XC::Spot *XC::Eje::P2(void)
  {
    std::cerr << "Eje::P2; this function must be redefined on derived"
              << " classes, nullptr returned;" << std::endl;
    return nullptr;
  }
//! @brief Returns a constant pointer to end point.
const XC::Spot *XC::Eje::P2(void) const
  {
    std::cerr << "Eje::P2; his function must be redefined on derived"
              << " classes, nullptr returned;" << std::endl;
    return nullptr;
  }

//! @brief Returns the set of lines that touch the point being passed as parameter.
std::set<const XC::Eje *> XC::GetLineasTocan(const XC::Spot &p)
  {
    std::set<const Eje *> retval;
    const std::set<const Eje *> &tocan= p.EjesTocan();
    for(std::set<const Eje *>::const_iterator i= tocan.begin(); i!=tocan.end();i++)
      retval.insert(*i);
    return retval;
  }
