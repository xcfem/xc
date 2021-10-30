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
//Segment.cc

#include "Segment.h"
#include "SectionGeometry.h"
#include "Spot.h"
#include "utility/geom/d2/BND2d.h"

#include "vtkCellType.h"

//! @brief Constructor.
XC::Segment::Segment(SectionGeometry *m,const size_t &ndiv)
  : Axis(m,ndiv), p1(nullptr), p2(nullptr) {}

//! @brief Constructor.
XC::Segment::Segment(const std::string &name,SectionGeometry *m,const size_t &ndiv)
  : Axis(name,m,ndiv), p1(nullptr), p2(nullptr) {}

//! @brief Returns a pointer to start point.
XC::Spot *XC::Segment::P1(void)
  { return p1; }
//! @brief Returns a constant pointer to start point.
const XC::Spot *XC::Segment::P1(void) const
  { return p1; }
//! @brief Returns a pointer to end point.
XC::Spot *XC::Segment::P2(void)
  { return p2; }
//! @brief Returns a constant pointer to end point.
const XC::Spot *XC::Segment::P2(void) const
  { return p2; }


//! @brief Assigns end points.
void XC::Segment::setEndPoints(size_t idP1, size_t idP2)
  {
    p1= BuscaSpot(idP1);
    if(!p1)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; point: '" << idP1 << "' not found \n";
    update_topology();
    p2= BuscaSpot(idP2);
    if(!p2)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; point: '" << idP2 << "' not found \n";
    update_topology();
  }

//! @brief Returns the i-th vertex.
const XC::Spot *XC::Segment::getVertex(const size_t &i) const
  {
    if(i==1)
      return p1;
    else
      return p2;
  }

//! @brief Returns the i-th vertex.
XC::Spot *XC::Segment::getVertex(const size_t &i)
  {
    if(i==1)
      return p1;
    else
      return p2;
  }

//! @brief Sets the i-th vertex.
void XC::Segment::SetVertice(const size_t &i,Spot *p)
  {
    if(i==1)
      p1= p;
    else
      p2= p;    
  }

//! @brief Returns the 2D interval occupied by the segment.
BND2d XC::Segment::Bnd(void) const
  {
    BND2d retval;
    if(p1 && p2)
      {
        retval= BND2d(p1->getPos(),p1->getPos());
	retval+= p2->getPos();
      }
    else
      {
        if(p1) retval= BND2d(p1->getPos(),p1->getPos());
        if(p2) retval= BND2d(p2->getPos(),p2->getPos());
      }
    return retval;
  }

//! @brief VTK interface.
int XC::Segment::getVtkCellType(void) const
  { return VTK_LINE; }

//! @brief Return segment length.
double XC::Segment::getLength(void) const
  {
    double retval= -1;
    if(p1 && p2)
      retval= dist(p1->getPos(),p2->getPos());
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; endpoint not defined in line: '" << getName()
                << std::endl;
    return retval;
  }
