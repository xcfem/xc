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
//GeomObj3d.cc

#include "PrincipalAxes3D.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/geom/coo_sys/ref_sys/Ref3d3d.h"

//! @brief Constructor.
PrincipalAxes3D::PrincipalAxes3D(void)
  : center_of_mass(), axis1(1,0,0), axis2(0,1,0), i1(0.0), i2(0.0), i3(0.0)
  {}

//! @brief Constructor.
PrincipalAxes3D::PrincipalAxes3D(const Pos3d &center_of_mass_, const double A[3][3])
  : center_of_mass(center_of_mass_)
  {
    double V[3][3];
    double d[3];
    eigen_decomposition_3x3(A, V, d);
    this->i1= d[2]; // bigger eigenvalue;
    this->i2= d[1];
    this->i3= d[0];
    this->axis1= Vector3d(V[0][2], V[1][2], V[2][2]); // Third column.
    this->axis2= Vector3d(V[0][1], V[1][1], V[2][1]); // Second column.
    //this->axis3= Vector3d(V[0][0], V[0][1], V[0][2]);
  }

//! @brief Constructor.
PrincipalAxes3D::PrincipalAxes3D(const Pos3d &center_of_mass_,const GEOM_FT &Ix, const GEOM_FT &Iy, const GEOM_FT &Iz, const GEOM_FT &Pxy, const GEOM_FT &Pxz, const GEOM_FT &Pyz)
  : center_of_mass(center_of_mass_), axis1(1,0,0), axis2(0,1,0), i1(0.0), i2(0.0), i3(0.0)
  {
    double A[3][3];
    A[0][0]= Ix;  A[0][1]= Pxy; A[0][2]= Pxz;
    A[1][0]= Pxy; A[1][1]= Iy;  A[1][2]= Pyz;
    A[2][0]= Pxz; A[2][1]= Pyz;  A[2][2]= Iz;
    double V[3][3];
    double d[3];
    eigen_decomposition_3x3(A, V, d);
    this->i1= d[0]; // bigger eigenvalue;
    this->i2= d[1];
    this->i3= d[2];
    this->axis1= Vector3d(V[0][0], V[0][1], V[0][2]);
    this->axis2= Vector3d(V[1][0], V[1][1], V[1][2]);
    //this->axis3= Vector3d(V[2][0], V[2][1], V[2][2]);
  }

Ref3d3d PrincipalAxes3D::getAxis(void) const
  { return Ref3d3d(center_of_mass, axis1, axis2); }

const GEOM_FT &PrincipalAxes3D::I1(void) const
  { return i1; }
const GEOM_FT &PrincipalAxes3D::I2(void) const
  { return i2; }
const GEOM_FT &PrincipalAxes3D::I3(void) const
  { return i3; }

//! @brief Return the direction vector of the minor
//! principal axis of inertia.
Vector3d PrincipalAxes3D::getAxis3VDir(void) const
  { return axis1.getCross(axis2); }

std::ostream &operator<<(std::ostream &os,const PrincipalAxes3D &axis)
  {
    os << "Axis: " << axis.getAxis() << std::endl
       << "I1= " << axis.I1() << std::endl
       << "I2= " << axis.I2()
       << "I3= " << axis.I3();
    return os;
  }

