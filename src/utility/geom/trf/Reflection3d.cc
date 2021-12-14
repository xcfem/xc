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
//Reflection3d.cc

#include "Reflection3d.h"
#include "utility/geom/FT_matrix.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/d2/Plane.h"


//! Reflexión en 3d según se describe en "Geometric tools for computer graphics"
//! Philip J. Schneider Morgan Kaufmann Publishers, page 153
FT_matrix Reflection3d::reflection3d_matrix(const  Pos3d &Q,const Vector3d &n)
  {
    const Vector3d nn= n.getNormalized();
    FT_matrix retval(4,4);
    const FT_matrix I= identity(FT_matrix(3,3));
    const FT_matrix T_n= traspuesta(I-2*prod_tensor(nn,nn));
    retval.putBox(1,1,T_n);
    const Vector3d VQ= double_to_FT(2)*dot(Q.VectorPos(),nn)*nn;
    retval.putBox(1,4,VQ.getMatrix());
    retval(4,4)=1;
    return retval;
  }

//! @brief Default constructor.
Reflection3d::Reflection3d(void)
  : Trf3d()
  {
    FT_matrix rf=  reflection3d_matrix(Pos3d(0.0,0.0,0.0),Vector3d(0.0,0.0,1.0));
    putHomogenousMatrix(rf);
  }

//! @brief Constructor.
Reflection3d::Reflection3d(const Pos3d &Q,const Vector3d &d)
  : Trf3d()
  {
    FT_matrix rf=  reflection3d_matrix(Q,d);
    putHomogenousMatrix(rf);
  }

//! @brief Constructor.
Reflection3d::Reflection3d(const Plane &p)
  : Trf3d()
  {
    FT_matrix rf=  reflection3d_matrix(p.Point(),p.Normal());
    putHomogenousMatrix(rf);
  }

