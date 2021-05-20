//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//Trf2d.cc

#include "Trf2d.h"
#include "../pos_vec/Vector2d.h"
#include "../pos_vec/Pos2d.h"
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/FT_matrix.h"

// Trf2d from_racionales(const GEOM_FT &r00,const GEOM_FT &r01,const GEOM_FT &r02,
//              const GEOM_FT &r10,const GEOM_FT &r11,const GEOM_FT &r12)
//   {
//     const GEOM_RT hw= r00.denominator()*r01.denominator()
//                              *r10.denominator()*r11.denominator()
//                              *r02.denominator()*r12.denominator();
//     const GEOM_RT m00= r00.numerator()*r01.denominator()
//                              *r10.denominator()*r11.denominator()
//                              *r02.denominator()*r12.denominator();
//     const GEOM_RT m01= r00.denominator()*r01.numerator()
//                              *r10.denominator()*r11.denominator()
//                              *r02.denominator()*r12.denominator();
//     const GEOM_RT m02= r00.denominator()*r01.denominator()
//                              *r10.denominator()*r11.denominator()
//                              *r02.numerator()*r12.denominator();
//     const GEOM_RT m10= r00.denominator()*r01.denominator()
//                              *r10.numerator()*r11.denominator()
//                              *r02.denominator()*r12.denominator();
//     const GEOM_RT m11= r00.denominator()*r01.denominator()
//                              *r10.denominator()*r11.numerator()
//                              *r02.denominator()*r12.denominator();
//     const GEOM_RT m12= r00.denominator()*r01.denominator()
//                              *r10.denominator()*r11.denominator()
//                              *r02.denominator()*r12.numerator();
//     return Trf2d(m00,m01,m02,m10,m11,m12,hw);

//   }
// Trf2d::Trf2d( const GEOM_RT & m00,const GEOM_RT & m01,const GEOM_RT & m02,
//               const GEOM_RT & m10,const GEOM_RT & m11,const GEOM_RT & m12,
//               const GEOM_RT & hw)
// //Define the matrix of an affine transformation.
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
// // contanins the translational part of the transformation.
//   : Trf(), cgtrf(m00,m01,m02,m10,m11,m12,hw) {}

Trf2d::Trf2d( const GEOM_FT & m00,const GEOM_FT & m01,const GEOM_FT & m02,
              const GEOM_FT & m10,const GEOM_FT & m11,const GEOM_FT & m12)
  : Trf(), cgtrf(m00,m01,m02,m10,m11,m12) 
  {
    //Trf2d::operator=(from_racionales(m00,m01,m02,m10,m11,m12));
  }


Trf2d::Trf2d(const CGAL::Translation &tr,const Vector2d &v)
  : Trf(), cgtrf(tr,v.ToCGAL()) {}
Trf2d::Trf2d(const CGAL::Rotation &rot,const GEOM_RT &seno,const GEOM_RT &coseno)
  : Trf(), cgtrf(rot,seno,coseno) {}
// Trf2d::Trf2d(const CGAL::Reflection &ref,const Line2d &r)
//   : Trf(), cgtrf(ref)) {}
Trf2d::Trf2d(const CGAL::Scaling &sc,const GEOM_FT &factor_escala)
  : Trf(), cgtrf(sc,factor_escala) {}
//  : Trf(), cgtrf(sc,factor_escala.numerator(),factor_escala.denominator()) {}
Trf2d::Trf2d(const CGAL::Identity_transformation &i)
  : Trf(), cgtrf(i) {}

//! @brief Return the transformation matrix expressed in
//! cartesian coordinates.
FT_matrix Trf2d::Cartesianas(void) const
  {
    FT_matrix retval(3,3,0.0);
    retval(1,1)= Cartesianas(1,1); retval(1,2)= Cartesianas(1,2); retval(1,3)= Cartesianas(1,3);
    retval(2,1)= Cartesianas(2,1); retval(2,2)= Cartesianas(2,2); retval(2,3)= Cartesianas(2,3);
    retval(3,3)= Cartesianas(3,3);
    return retval;
  }
//! @brief Return the transformation matrix expressed in
//! homogeneous coordinates.
FT_matrix Trf2d::Homogeneas(void) const
  {
    FT_matrix retval(3,3,0.0);
    retval(1,1)= Homogeneas(1,1); retval(1,2)= Homogeneas(1,2); retval(1,3)= Homogeneas(1,3);
    retval(2,1)= Homogeneas(2,1); retval(2,2)= Homogeneas(2,2); retval(2,3)= Homogeneas(2,3);
    retval(3,3)= Homogeneas(3,3);
    return retval;
  }
CGPoint_2 Trf2d::Transform(const CGPoint_2 &cgpt2) const
  { return cgtrf.transform(cgpt2);  }
Pos2d Trf2d::Transform(const Pos2d &p) const
  { return Pos2d(Transform(p.ToCGAL())); }
Vector2d Trf2d::Transform(const Vector2d &v) const
  { return Vector2d(cgtrf.transform(v.ToCGAL())); }
//! @brief Return the transformed points.
GeomObj::list_Pos2d Trf2d::Transform(const GeomObj::list_Pos2d &lp2d) const
  {
    GeomObj::list_Pos2d retval(lp2d);
    Transform(retval.begin(),retval.end());
    return retval;
  }
Pos2d Trf2d::operator()(const Pos2d &p) const
  { return Transform(p); }
Vector2d Trf2d::operator()(const Vector2d &v) const
  { return Transform(v); }
GeomObj::list_Pos2d Trf2d::operator()(const GeomObj::list_Pos2d &lp2d) const
  { return Transform(lp2d); }
Trf2d operator*(const Trf2d &a,const Trf2d &b)
  {
    Trf2d retval(a*b);
    return retval;
  }
