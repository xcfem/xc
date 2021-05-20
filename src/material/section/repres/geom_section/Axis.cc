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
//Axis.cc

#include "Axis.h"
#include "Spot.h"
#include "utility/matrices/m_int.h"
#include "utility/geom/d2/BND2d.h"


#include "boost/any.hpp"

//! @brief Constructor.
XC::Axis::Axis(GeomSection *sr,const size_t &nd)
  : EntGeomSection("",sr), ndiv(nd) {}

//! @brief Constructor.
//! @param name: object identifier.
//! @param sr: pointer to section geometry.
//! @param nd: number of divisions.
XC::Axis::Axis(const std::string &name,GeomSection *sr,const size_t &nd)
  : EntGeomSection(name,sr), ndiv(nd) {}

//! @brief Destructor.
XC::Axis::~Axis(void)
  {
    if(P1()) P1()->borra_linea(this);
    if(P2()) P2()->borra_linea(this);
  }

//! @brief Updates topology.
void XC::Axis::update_topology(void)
  {
    if(P1()) P1()->insert_linea(this);
    if(P2()) P2()->insert_linea(this);
  }

//! @brief Returns a pointer to start point.
XC::Spot *XC::Axis::P1(void)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; this function must be redefined on derived"
              << " classes, nullptr returned;" << std::endl;
    return nullptr;
  }
//! @brief Returns a constant pointer to start point.
const XC::Spot *XC::Axis::P1(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; this function must be redefined on derived"
              << " classes, nullptr returned;" << std::endl;
    return nullptr;
  }
//! @brief Returns a pointer to end point.
XC::Spot *XC::Axis::P2(void)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; this function must be redefined on derived"
              << " classes, nullptr returned;" << std::endl;
    return nullptr;
  }
//! @brief Returns a constant pointer to end point.
const XC::Spot *XC::Axis::P2(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; his function must be redefined on derived"
              << " classes, nullptr returned;" << std::endl;
    return nullptr;
  }

//! @brief Return the set of lines that touch the point being passed as
//! parameter.
std::set<const XC::Axis *> XC::getConnectedLines(const XC::Spot &p)
  {
    std::set<const Axis *> retval;
    const std::set<const Axis *> &connected= p.getConnectedAxes();
    for(std::set<const Axis *>::const_iterator i= connected.begin(); i!=connected.end();i++)
      retval.insert(*i);
    return retval;
  }
