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
//Rotation3d.cc

#include "Rotation3d.h"
#include "utility/geom/d1/Line3d.h"
#include "utility/geom/FT_matrix.h"

//! Rotation in 3D as described in "Geometric tools for computer graphics"
//! Philip J. Schneider Morgan Kaufmann Publishers, page 141
FT_matrix rotation_3d_matrix(const Line3d &axis,const double &theta)
  {
    FT_matrix retval(4,4);
    const GEOM_FT cos_theta= double_to_FT(cos(theta));
    const GEOM_FT sin_theta= double_to_FT(sin(theta));
    const FT_matrix I= identity(FT_matrix(3,3));
    const Vector3d u= axis.VDir().getNormalized();
    const FT_matrix T_u_theta_1= cos_theta*I;
    const FT_matrix T_u_theta_2= (double_to_FT(1.0)-cos_theta)*prod_tensor(u,u);
    const FT_matrix T_u_theta_3= sin_theta*skew_symm_matrix_pre(u);
    const FT_matrix T_u_theta= traspuesta(T_u_theta_1+T_u_theta_2+T_u_theta_3);
    retval.putBox(1,1,T_u_theta);
    const Pos3d Q= axis.Point();
    const Vector3d tmp= Vector3d(T_u_theta*Q.VectorPos());
    const Vector3d VQ= Q.VectorPos()-tmp;
    retval.putBox(1,4,VQ.getMatrix());
    retval(4,4)=1;
    return retval;
  }

//! @brief Default constructor.
Rotation3d::Rotation3d(void)
  : Trf3d()
  {
    FT_matrix rr= identity(4);
    putHomogenousMatrix(rr);
  }

//! @brief Constructor.
Rotation3d::Rotation3d(const Line3d &axis,const GEOM_FT &theta)
  : Trf3d()
  {
    FT_matrix rr=  rotation_3d_matrix(axis,theta);
    putHomogenousMatrix(rr);
  }
