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
//Trf3d.cc

#include "Trf3d.h"
#include "../pos_vec/Pos3d.h"
#include "../pos_vec/Vector3d.h"
#include "../pos_vec/Pos3dArray.h"
#include "../pos_vec/Pos3dArray3d.h"
#include "utility/geom/FT_matrix.h"

// Trf3d from_racionales( const GEOM_FT &r00,const GEOM_FT &r01,const GEOM_FT &r02,const GEOM_FT &r03,
//                        const GEOM_FT &r10,const GEOM_FT &r11,const GEOM_FT &r12,const GEOM_FT &r13,
//                        const GEOM_FT &r20,const GEOM_FT &r21,const GEOM_FT &r22,const GEOM_FT &r23)
//   {
//     const GEOM_RT hw= r00.denominator()*r01.denominator()*r02.denominator()*r03.denominator()
//                              *r10.denominator()*r11.denominator()*r12.denominator()*r13.denominator()
//                              *r20.denominator()*r21.denominator()*r22.denominator()*r23.denominator();
//     const GEOM_RT m00= r00.numerator()*r01.denominator()*r02.denominator()*r03.denominator()
//                               *r10.denominator()*r11.denominator()*r12.denominator()*r13.denominator()
//                               *r20.denominator()*r21.denominator()*r22.denominator()*r23.denominator();
//     const GEOM_RT m01= r00.denominator()*r01.numerator()*r02.denominator()*r03.denominator()
//                               *r10.denominator()*r11.denominator()*r12.denominator()*r13.denominator()
//                               *r20.denominator()*r21.denominator()*r22.denominator()*r23.denominator();
//     const GEOM_RT m02= r00.denominator()*r01.denominator()*r02.numerator()*r03.denominator()
//                               *r10.denominator()*r11.denominator()*r12.denominator()*r13.denominator()
//                               *r20.denominator()*r21.denominator()*r22.denominator()*r23.denominator();
//     const GEOM_RT m03= r00.denominator()*r01.denominator()*r02.denominator()*r03.numerator()
//                               *r10.denominator()*r11.denominator()*r12.denominator()*r13.denominator()
//                               *r20.denominator()*r21.denominator()*r22.denominator()*r23.denominator();

//     const GEOM_RT m10= r00.denominator()*r01.denominator()*r02.denominator()*r03.denominator()
//                               *r10.numerator()*r11.denominator()*r12.denominator()*r13.denominator()
//                               *r20.denominator()*r21.denominator()*r22.denominator()*r23.denominator();
//     const GEOM_RT m11= r00.denominator()*r01.denominator()*r02.denominator()*r03.denominator()
//                               *r10.denominator()*r11.numerator()*r12.denominator()*r13.denominator()
//                               *r20.denominator()*r21.denominator()*r22.denominator()*r23.denominator();
//     const GEOM_RT m12= r00.denominator()*r01.denominator()*r02.denominator()*r03.denominator()
//                               *r10.denominator()*r11.denominator()*r12.numerator()*r13.denominator()
//                               *r20.denominator()*r21.denominator()*r22.denominator()*r23.denominator();
//     const GEOM_RT m13= r00.denominator()*r01.denominator()*r02.denominator()*r03.denominator()
//                               *r10.denominator()*r11.denominator()*r12.denominator()*r13.numerator()
//                               *r20.denominator()*r21.denominator()*r22.denominator()*r23.denominator();

//     const GEOM_RT m20= r00.denominator()*r01.denominator()*r02.denominator()*r03.denominator()
//                               *r10.denominator()*r11.denominator()*r12.denominator()*r13.denominator()
//                               *r20.numerator()*r21.denominator()*r22.denominator()*r23.denominator();
//     const GEOM_RT m21= r00.denominator()*r01.denominator()*r02.denominator()*r03.denominator()
//                               *r10.denominator()*r11.denominator()*r12.denominator()*r13.denominator()
//                               *r20.denominator()*r21.numerator()*r22.denominator()*r23.denominator();
//     const GEOM_RT m22= r00.denominator()*r01.denominator()*r02.denominator()*r03.denominator()
//                               *r10.denominator()*r11.denominator()*r12.denominator()*r13.denominator()
//                               *r20.denominator()*r21.denominator()*r22.numerator()*r23.denominator();
//     const GEOM_RT m23= r00.denominator()*r01.denominator()*r02.denominator()*r03.denominator()
//                               *r10.denominator()*r11.denominator()*r12.denominator()*r13.denominator()
//                               *r20.denominator()*r21.denominator()*r22.denominator()*r23.numerator();

//     return Trf3d(m00,m01,m02,m03,m10,m11,m12,m13,m20,m21,m22,m23,hw);
//   }

// Trf3d::Trf3d( const GEOM_RT &m00,const GEOM_RT &m01,const GEOM_RT &m02,const GEOM_RT &m03,
//               const GEOM_RT &m10,const GEOM_RT &m11,const GEOM_RT &m12,const GEOM_RT &m13,
//               const GEOM_RT &m20,const GEOM_RT &m21,const GEOM_RT &m22,const GEOM_RT &m23,
//               const GEOM_RT & hw)
//   : Trf(), cgtrf(m00,m01,m02,m03,m10,m11,m12,m13,m20,m21,m22,m23,hw) {}


Trf3d::Trf3d( const GEOM_FT &r00,const GEOM_FT &r01,const GEOM_FT &r02,const GEOM_FT &r03,
              const GEOM_FT &r10,const GEOM_FT &r11,const GEOM_FT &r12,const GEOM_FT &r13,
              const GEOM_FT &r20,const GEOM_FT &r21,const GEOM_FT &r22,const GEOM_FT &r23)
  : Trf(), cgtrf(r00,r01,r02,r03,r10,r11,r12,r13,r20,r21,r22,r23) 
  {
    //Trf3d::operator=(from_racionales(r00,r01,r02,r03,r10,r11,r12,r13,r20,r21,r22,r23));
  }

Trf3d::Trf3d(const CGAL::Identity_transformation &i)
  : Trf(), cgtrf(i) {}

void Trf3d::putHomogenousMatrix(const FT_matrix &mh)
  {
    assert(mh(4,4)==1);
    //Trf3d::operator=(from_racionales(mh(1,1),mh(1,2),mh(1,3),mh(1,4),mh(2,1),mh(2,2),mh(2,3),mh(2,4),mh(3,1),mh(3,2),mh(3,3),mh(3,4)));
    Trf3d::operator=(Trf3d(mh(1,1),mh(1,2),mh(1,3),mh(1,4),mh(2,1),mh(2,2),mh(2,3),mh(2,4),mh(3,1),mh(3,2),mh(3,3),mh(3,4)));
  }

Trf3d::Trf3d(const CGAL::Translation &tr,const Vector3d &v)
  : Trf(), cgtrf(tr,v.ToCGAL()) {}
// Trf3d::Trf3d(const CGAL::Rotation &rot,const GEOM_FT &seno,const GEOM_FT &coseno)
//   : CGTrfAfin_3(rot,seno,coseno), Trf() {}
Trf3d::Trf3d(const CGAL::Scaling &sc,const GEOM_FT &factor_escala)
  : Trf(), cgtrf(sc,factor_escala) {}
//  : Trf(), cgtrf(sc,factor_escala.numerator(),factor_escala.denominator()) {}

//! @brief Return the transformation matrix en cartesianas.
FT_matrix Trf3d::Cartesianas(void) const
  {
    FT_matrix retval(4,4,0.0);
    retval(1,1)= Cartesianas(1,1); retval(1,2)= Cartesianas(1,2); retval(1,3)= Cartesianas(1,3);
    retval(2,1)= Cartesianas(2,1); retval(2,2)= Cartesianas(2,2); retval(2,3)= Cartesianas(2,3);
    retval(2,1)= Cartesianas(3,1); retval(2,2)= Cartesianas(3,2); retval(3,3)= Cartesianas(3,3);
    retval(4,4)= Cartesianas(4,4);
    return retval;
  }

//! @brief Return the transformation matrix en homogéneas.
FT_matrix Trf3d::Homogeneas(void) const
  {
    FT_matrix retval(4,4,0.0);
    retval(1,1)= Cartesianas(1,1); retval(1,2)= Cartesianas(1,2); retval(1,3)= Cartesianas(1,3); retval(1,4)= Cartesianas(1,4);
    retval(2,1)= Cartesianas(2,1); retval(2,2)= Cartesianas(2,2); retval(2,3)= Cartesianas(2,3); retval(2,4)= Cartesianas(2,4);
    retval(2,1)= Cartesianas(3,1); retval(2,2)= Cartesianas(3,2); retval(3,3)= Cartesianas(3,3); retval(3,4)= Cartesianas(3,4);
    retval(4,4)= Cartesianas(4,4);
    return retval;
  }

//! @brief Return the transformed of the point.
Pos3d Trf3d::Transform(const Pos3d &p) const
  { return Pos3d(cgtrf.transform(p.ToCGAL())); }

//! @brief Return el transformado del vector being passed as parameter.
Vector3d Trf3d::Transform(const Vector3d &v) const
  { return Vector3d(cgtrf.transform(v.ToCGAL())); }

//! @brief Transform the points of the matrix.
void Trf3d::Transform(Pos3dArray &m) const
  {
    for(Pos3dArray::iterator i= m.begin();i!=m.end();i++)
      (*i)= Transform(*i);
  }

//! @brief Transform the points of the matrix.
const Pos3dArray &Trf3d::Transform(const Pos3dArray &m) const
  {
    static Pos3dArray retval;
    retval= m;
    Transform(retval);
    return retval;
  }

//! @brief Transform the points of the argument.
void Trf3d::Transform(Pos3dArray3d &m) const
  {
    const size_t n_layers= m.getNumberOfLayers();
    const size_t n_rows= m.getNumberOfRows();
    const size_t n_columns= m.getNumberOfColumns();
    for(size_t k=1;k<=n_layers;k++) //For each layer.
      for(size_t i=1;i<=n_rows;i++) //For each row.
        for(size_t j=1;j<=n_columns;j++) //For each column.
          m(i,j,k)= Transform(m(i,j,k));
  }

//! @brief Transform the points of the argument.
const Pos3dArray3d &Trf3d::Transform(const Pos3dArray3d &m) const
  {
    static Pos3dArray3d retval;
    retval= m;
    Transform(retval);
    return retval;
  }

//! @brief Transform the point.
Pos3d Trf3d::operator()(const Pos3d &p) const
  { return Transform(p); }
//! @brief Transform the vector being passed as parameter.
Vector3d Trf3d::operator()(const Vector3d &v) const
  { return Transform(v); }
//! @brief Transform the positions in the array argument.
Pos3dArray Trf3d::operator()(const Pos3dArray &m) const
  { return Transform(m); }

Trf3d operator*(const Trf3d &a,const Trf3d &b)
  {
    Trf3d retval(a*b);
    return retval;
  }


Trf3d giroX3d(const double &ang_rad)
  {
    GEOM_FT c= cos(ang_rad);
    GEOM_FT s= sin(ang_rad);
    return Trf3d( 1,0,0,0,
                  0,c,s,0,
                  0,-s,c,0);
  }
Trf3d giroY3d(const double &ang_rad)
  {
    GEOM_FT c= cos(ang_rad);
    GEOM_FT s= sin(ang_rad);
    return Trf3d( c,0,s,0,
                  0,1,0,0,
                  -s,0,c,0);
  }
Trf3d giroZ3d(const double &ang_rad)
  {
    GEOM_FT c= cos(ang_rad);
    GEOM_FT s= sin(ang_rad);
    return Trf3d(c,s,0,0,
                 -s,c,0,0,
                 0,0,1,0);
  }
