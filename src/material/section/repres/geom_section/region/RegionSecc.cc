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
//RegionSecc.cpp
// Written by Remo M. de Souza
// December 1998

#include "RegionSecc.h"
#include "RgSccPoligono.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"
#include "xc_utils/src/geom/pos_vec/Dir2d.h"

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "xc_utils/src/geom/matriz_FT.h"

XC::RegionSecc::RegionSecc(Material *mat)
  : DiscretBase(mat) {}

Poligono2d XC::RegionSecc::getPoligono(void) const
  {
    std::cerr << "RegionSecc::getPoligono not implemented." << std::endl;
    Poligono2d retval;
    return retval;
  }

XC::RgSccPoligono XC::RegionSecc::Interseccion(const Semiplano2d &sp) const
  {
    std::list<Poligono2d> tmpList= getPoligono().Interseccion(sp);
    if(tmpList.size()>1)
     std::cerr << "RegionSecc::Interseccion; la región no es simplemente conexa."
               << std::endl;
    Poligono2d tmp= *tmpList.begin();
    return RgSccPoligono(getMaterialPtr(),1,1,tmp);
  }

//! @brief Returns the coordenadas del centro de gravedad
const XC::Vector &XC::RegionSecc::Cdg(void) const
  {
    const Pos2d p= getPoligono().Cdg();
    static Vector retval(2);
    retval[0]= p.x();
    retval[1]= p.y();
    return retval;
  }

//! @brief Returns the region contour lenght.
double XC::RegionSecc::Longitud(void) const
  { return getPoligono().Longitud(); }

//! @brief Returns the region area.
double XC::RegionSecc::Area(void) const
  { return getPoligono().Area(); }

//! @brief Returns the moment of inertia respecto al eje paralelo al y por el CDG.
double XC::RegionSecc::Iy(void) const
  { return getPoligono().Ix(); }

//! @brief Returns the moment of inertia respecto al eje paralelo al z por el CDG.
double XC::RegionSecc::Iz(void) const
  { return getPoligono().Iy(); }

//! @brief Returns the producto de inercia respecto a los ejes paralelos por el CDG.
double XC::RegionSecc::Pyz(void) const
  { return getPoligono().Pxy(); }

//! @brief Returns the moment of inertia polar respecto al CDG en ejes locales.
double XC::RegionSecc::Ix(void) const
  { return Iy()+Iz(); }

//! @brief Returns the ángulo que define un eje principal de inercia.
double XC::RegionSecc::Theta_p(void) const
  { return getPoligono().Theta_p(); }

//! @brief Return the dirección de un eje principal de inercia (no sabemos si
//! el mayor o el menor
const XC::Vector &XC::RegionSecc::DirEjeI_a(void) const
  {
    const Dir2d p= getPoligono().DirEjeI_a();
    static Vector retval(2);
    retval[0]= p.dx();
    retval[1]= p.dy();
    return retval;
  }

//! @brief Return the dirección del otro eje principal de inercia (no sabemos si
//!     //el mayor o el menor
const XC::Vector &XC::RegionSecc::DirEjeI_b(void) const
  {
    const Dir2d p= getPoligono().DirEjeI_b();
    static Vector retval(2);
    retval[0]= p.dx();
    retval[1]= p.dy();
    return retval;
  }

//     Recta2d EjeI_a(void) const;
//     //Returns a eje principal de inercia (no sabemos si
//     //el mayor o el menor).
//     Recta2d EjeI_b(void) const;
//     //Returns the otro eje principal de inercia (no sabemos si
//     //el mayor o el menor).
//     Ref2d2d EjesPrincipalesInercia(void) const;
//     //Returns the ejes principales de inercia.

//! @brief Returns the moment of inertia principal mayor.
double XC::RegionSecc::getI1(void) const
  { return getPoligono().I1(); }

//! @brief Returns the moment of inertia principal menor.
double XC::RegionSecc::getI2(void) const
  { return getPoligono().I2(); }

//     inline PrincipalAxesOfInertia2D Inercia(void)
//       { return PrincipalAxesOfInertia2D(Cdg(),Iy(),Iz(),Pyz()); }

//! @brief Return the i,j component of the tensor of inertia calculado respecto al CDG.
double XC::RegionSecc::getI(const unsigned short int &i,const unsigned short int &j) const
  { return getPoligono().I(i+1,j+1); }

//! @brief Returns the moment of inertia respecto al eje que pasa por O con dirección la de e.
double XC::RegionSecc::getI(const Pos2d &O,const Vector &e) const
  { return getPoligono().I(O,Vector2d(e[0],e[1])); }

//     double I(const Recta2d &r) const;
//       //Returns the moment of inertia respecto a la recta being passed
//       //as parameter.

//! @brief Return the i,j component of the tensor of inertia calculado respecto al punto "o".
double XC::RegionSecc::getI(const unsigned short int i,const unsigned short int j,const Pos2d &o) const
  { return getPoligono().I(i+1,j+1,o); }

//! @brief Returns the momento polar de inercia respecto al punto o.
double XC::RegionSecc::getIO(const Pos2d &o)
  { return getPoligono().IO(o); }

//! @brief Returns the tensor of inertia computed with respect to the object centroid.
XC::Matrix &XC::RegionSecc::getI(void) const
  {
    static Matrix i(2,2);
    const Poligono2d &pol= getPoligono();
    i(0,0)= pol.Ix(); i(0,1)= -pol.Pxy();
    i(1,0)= i(0,1);   i(1,1)= pol.Iy();
    return i;
  }

//! @brief Returns the tensor of inertia respector al punto o.
XC::Matrix &XC::RegionSecc::getI(const Pos2d &o) const
  {
    matriz_FT tmp= getPoligono().I(o);
    static Matrix i(2,2);
    i(0,0)= tmp(1,1); i(0,1)= tmp(1,2);
    i(1,0)= tmp(1,2); i(1,1)= tmp(2,2);
    return i;
  }
