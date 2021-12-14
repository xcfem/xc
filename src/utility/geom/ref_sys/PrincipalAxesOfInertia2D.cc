//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//GeomObj2d.cc

#include "PrincipalAxesOfInertia2D.h"
#include "utility/utils/misc_utils/inertia.h"
#include "utility/geom/ref_sys/Ref2d2d.h"


//! @brief Constructor.
PrincipalAxesOfInertia2D::PrincipalAxesOfInertia2D(const Pos2d &center_of_mass_,const GEOM_FT &Ix,const GEOM_FT &Iy,const GEOM_FT &Pxy)
  : center_of_mass(center_of_mass_), axis1(1,0), i1(0.0), i2(0.0)
  {
    double th1= theta_inertia(Ix,Iy,Pxy);
    const GEOM_FT media= (Ix+Iy)/2;
    const GEOM_FT dif2= (Ix-Iy)/2;
    const double th1_por2= 2*th1;
    const GEOM_FT seno= sin(th1_por2);
    const GEOM_FT coseno= cos(th1_por2);
    i1= media+dif2*coseno-Pxy*seno;
    i2= Ix+Iy-i1;
    if(i1<i2)
      {
	std::swap(i1,i2);
        th1+= (M_PI/2.0);
      }
    axis1= Vector2d(cos(th1),sin(th1));
  }
Ref2d2d PrincipalAxesOfInertia2D::getAxis(void) const
  { return Ref2d2d(center_of_mass,axis1); }
const GEOM_FT &PrincipalAxesOfInertia2D::I1(void) const
  { return i1; }
const GEOM_FT &PrincipalAxesOfInertia2D::I2(void) const
  { return i2; }

//! @brief Return the direction vector of the minor
//! principal axis of inertia.
Vector2d PrincipalAxesOfInertia2D::getAxis2VDir(void) const
  { return Vector2d(-axis1.y(),axis1.x()); }

std::ostream &operator<<(std::ostream &os,const PrincipalAxesOfInertia2D &axis)
  {
    os << "Axis: " << axis.getAxis() << std::endl
       << "I1= " << axis.I1() << std::endl
       << "I2= " << axis.I2();
    return os;
  }

