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
//Reflection2d.cc

#include "Reflection2d.h"
#include "utility/geom/FT_matrix.h"
#include "../pos_vec/Vector2d.h"
#include "../pos_vec/Pos2d.h"
#include "../d1/Line2d.h"


// Reflection2d::Reflection2d(const GEOM_RT &m00,const GEOM_RT &m01,const GEOM_RT &m02,
//                          const GEOM_RT &m10,const GEOM_RT &m11,const GEOM_RT &m12,
//                          const GEOM_RT &hw)
// //Define the transformation matrix for any affine transformation.
// //the sub-matrix:
// //
// //    -1 [[ m00 m01]
// //  hw    [ m10 m11]] 
// //
// // contains the information about scaling and rotation, 
// // and the vector:
// //
// //    -1 [[ m02]
// //  hw    [ m12]] 
// //
// // contains the translational part of the transformation.
//   : Trf2d(m00,m01,m02,m10,m11,m12,hw) {}

//! @brief Default constructor.
Reflection2d::Reflection2d(void)
  : Trf2d(1.0,0.0,0.0,0.0,1.0,0.0)
  {}

Reflection2d::Reflection2d(const GEOM_FT &m00,const GEOM_FT &m01,const GEOM_FT &m02,
			 const GEOM_FT &m10,const GEOM_FT &m11,const GEOM_FT &m12)
  : Trf2d(m00,m01,m02,m10,m11,m12) {}

Reflection2d Reflection2d::crea_reflection2d(const  Pos2d &Q,const Vector2d &d)
  {
    if(!d.EsUnitario())
      {
        std::cerr << "Reflection2d; el vector: " << d 
                  << " no es unitario." << std::endl;
      }
    const Vector2d d_perp= d.Perpendicular(CGAL::POSITIVE);
    const FT_matrix T_d=traspuesta(identity(FT_matrix(2,2))-2*(d_perp & d_perp));
    const FT_matrix v= (2*dot(Q.VectorPos(),d_perp)*d_perp).getMatrix();
    return Reflection2d(T_d(1,1),T_d(1,2),v(1,1),T_d(2,1),T_d(2,2),v(2,1));
  }

Reflection2d::Reflection2d(const Pos2d &Q,const Vector2d &d)
  : Trf2d(crea_reflection2d(Q,d)) {}

Reflection2d::Reflection2d(const Line2d &r)
  : Trf2d(crea_reflection2d(r.Point(),r.VersorDir())) {}

