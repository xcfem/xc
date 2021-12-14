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

#include "GeomObj3d.h"
#include "BND3d.h"
#include "../pos_vec/Pos3d.h"
#include "../pos_vec/Vector3d.h"
#include "../ref_sys/Ref3d3d.h"
#include "utility/geom/d1/Line3d.h"
#include "utility/geom/FT_matrix.h"

#include "utility/geom/d3/HalfSpace3d.h"


// std::deque<HalfSpace3d> GeomObj3d::interpretaHalfSpaces3d(const std::string &str)
//   {
//     typedef std::deque<boost::any> dq_objetos;
//     const dq_objetos tmp= crea_deque_boost_any(str);
//     std::deque<HalfSpace3d> retval;
//     for(dq_objetos::const_iterator i= tmp.begin();i!=tmp.end();i++)
//       {
//         if(boost_any_is_empty(*i))
//           std::cerr << "La expresión '" << str 
//                     << "' da como resultado un objeto vacío." << std::endl;
//         else
//           retval.push_back(convert_to_se3d(*i));
//       }
//     return retval;
//   }

//! @brief Return the orthogonal projection onto the line.
//! @param p: point to project.
Pos3d GeomObj3d::Projection(const Pos3d &p) const
  {
    Pos3d retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return retval;
  }

//! @brief Return the projection onto the line.
//! @param v: vector to project.
Vector3d GeomObj3d::Projection(const Vector3d &v) const
  {
    Vector3d retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return retval;
  }

Pos3d GeomObj3d::GetPMax(void) const
  {
    Pos3d p(GetXMax(),GetYMax(),GetZMax());
    return p;
  }
Pos3d GeomObj3d::GetPMin(void) const
  {
    Pos3d p(GetXMin(),GetYMin(),GetZMin());
    return p;
  }

//! @brief Return the boudary of the object.
BND3d GeomObj3d::Bnd(void) const
  { return BND3d(GetPMin(),GetPMax()); }

//! @brief Return true if point lies inside the object.
bool GeomObj3d::In(const Pos3d &p, const double &tol) const
  {
    std::cerr << "Function: '" << __FUNCTION__
              << "' not implemented yet." << std::endl;
    return (BND3d().In(p,tol));
  }

//! @brief Return true if the point lies outside the object.
bool GeomObj3d::Out(const Pos3d &p, const double &tol) const
  { return !In(p,tol); }

GEOM_FT GeomObj3d::I( const unsigned short int &i, const unsigned short int &j) const
  {
    unsigned short int k= i + (j-1)*3;
    switch(k)
      {
	    case 1:
          return Ix();
	    case 2:
	    case 4:
          return Pxy();
	    case 3:
	    case 7:
          return Pxz();
	    case 5:
          return Iy();
	    case 6:
	    case 8:
          return Pyz();
	    case 9:
          return Iz();
	    default:
          return Ix();
      }        
  }

//! @brief Return el moment of inertia with respect to the axis that passes
//! through O with the direction of e.
GEOM_FT GeomObj3d::I(const Pos3d &O,const Vector3d &e) const
  {
    FT_matrix Io= I(O);
    return dot(e,Io*e.getMatrix())/Abs2(e);
  }

//! @brief Return el moment of inertia with respect to the argument line.
GEOM_FT GeomObj3d::I(const Line3d &r) const
  { return I(r.Point(),r.VDir()); }

//! @brief Return el moment of inertia (i,j) respect to the axis parallel to i
//! through o.
GEOM_FT GeomObj3d::I( const unsigned short int i,
                     const unsigned short int j,
                     const Pos3d &o) const
  {
    const GEOM_FT Iij= I(i,j);
    if(hasCenterOfMass())
      {
        Ref3d3d axis(getCenterOfMass()); //
        Pos3d pos_local= axis.getLocalPosition(o);
        return Iij + IArea() * pos_local(i) * pos_local(j);
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "The object has not a centroid." << std::endl;
        return Iij;
      }
  }

//! @brief Return the inertia tensor with respect to the center of mass
//! of the object.
FT_matrix GeomObj3d::I(void) const
  {
    FT_matrix i(3,3);
    i(1,1)= Ix();i(1,2)= -Pxy();i(1,3)= -Pxz();
    i(2,1)= -Pxy();i(2,2)= Iy();i(2,3)= -Pyz();
    i(3,1)= -Pxz();i(3,2)= -Pyz();i(3,3)= Iz();
    return i;
  }

//! @brief Return the inertia tensor with respect to the point o.
FT_matrix GeomObj3d::I(const Pos3d &o) const
  {
    FT_matrix Ig= I();
    Vector3d og=getCenterOfMass() - o;
    GEOM_FT m= IArea();
    return Ig+m*(Abs2(og)*identity(Ig)-(og & og));
  }

//! @brief Return the polar moment of inertia with respect to the point o.
GEOM_FT GeomObj3d::IO(const Pos3d &o) const
  { return (I(1,1,o)+I(2,2,o)+I(3,3,o))/2; }

