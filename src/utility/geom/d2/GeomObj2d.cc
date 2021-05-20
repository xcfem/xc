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
//GeomObj2d.cc

#include "GeomObj2d.h"
#include "utility/utils/misc_utils/inertia.h"
#include "utility/geom/pos_vec/Dir2d.h"
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/ref_sys/Ref2d2d.h"
#include "utility/geom/FT_matrix.h"
#include "BND2d.h"


#include "utility/geom/trf/Translation2d.h"

Pos2d GeomObj2d::GetPMax(void) const
  {
    Pos2d p(GetXMax(),GetYMax());
    return p;
  }

Pos2d GeomObj2d::GetPMin(void) const
  {
    Pos2d p(GetXMin(),GetYMin());
    return p;
  }

//! @brief Return the orthogonal projection onto the line.
//! @param p: point to project.
Pos2d GeomObj2d::Projection(const Pos2d &p) const
  {
    Pos2d retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return retval;
  }

//! @brief Return the projection onto the line.
//! @param v: vector to project.
Vector2d GeomObj2d::Projection(const Vector2d &v) const
  {
    Vector2d retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return retval;
  }

//! @brief Return the bounding rectangle.
BND2d GeomObj2d::Bnd(void) const
  { return BND2d(GetPMin(),GetPMax()); }

//! @brief Return true if the point lies inside the object.
bool GeomObj2d::In(const Pos2d &p, const double &tol) const
  { 
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; método In(p,tol) not implemented" << std::endl;
    return false;
  }

//! @brief Return true if the point lies outside the object.
bool GeomObj2d::Out(const Pos2d &p, const double &tol) const
  { return !In(p,tol); }


//! @brief Return the angle of one of the principal axis of inertia.
double GeomObj2d::Theta_p(void) const
  { return theta_inertia(Ix(),Iy(),Pxy()); }

//! @brief Return the dirección de un principal axis of inertia (we don't know
//! if it's the major or the minor yet).
Dir2d GeomObj2d::IAxisDir_a(void) const
  { return Dir2d(Theta_p()); }

//! @brief Return the direction of the other principal axis of inertia (we don't know
//! if it's the major or the minor yet).
Dir2d GeomObj2d::IAxisDir_b(void) const
  { return IAxisDir_a().Perpendicular(); }

//! @brief Return un principal axis of inertia (no sabemos si
//! el mayor o el menor
Line2d GeomObj2d::IAxis_a(void) const
  { return Line2d(getCenterOfMass(),IAxisDir_a()); }

//! @brief Return un principal axis of inertia (no sabemos si
//! el mayor o el menor
Line2d GeomObj2d::IAxis_b(void) const
  { return Line2d(getCenterOfMass(),IAxisDir_b()); }

//! @brief Return los principal axis of inertia.
Ref2d2d GeomObj2d::PrincipalAxesOfInertia(void) const
  { return Ref2d2d(getCenterOfMass(),IAxisDir_a()); }

//! @brief Return the moment of inertia principal mayor.
GEOM_FT GeomObj2d::I1(void) const
  { return I1_inertia(Ix(),Iy(),Pxy()); }

//! @brief Return the moment of inertia principal menor.
GEOM_FT GeomObj2d::I2(void) const
  { return I2_inertia(Ix(),Iy(),Pxy()); }

//! @brief Return the componente i,j of the inertia tensor computed
//! with respect to the center of mass.
GEOM_FT GeomObj2d::I(const unsigned short int &i,const unsigned short int &j) const
  {
    unsigned short int k= i + (j-1)*2;
    GEOM_FT retval= 0.0;
    switch(k)
      {
        case 1: //(1,1)
          retval= Ix();
          break;
        case 2: //(2,1)
        case 3: //(1,2)
          retval= -Pxy();
          break;
	case 4:
          retval= Iy();
          break;
      }
    return retval;
  }

//! @brief Return the component i,j of the inertia tensor computed
//! with respect to the point "o".
GEOM_FT GeomObj2d::I(const unsigned short int i,const unsigned short int j,const Pos2d &o) const
  {
    const GEOM_FT Iij= I(i,j);
    if(hasCenterOfMass())
      {
        Ref2d2d axis(getCenterOfMass()); //
        Pos2d pos_local= axis.GetPosLocal(o);
        return Iij + IArea() * pos_local(i) * pos_local(j);
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; object has not a center of mass." << std::endl;
        return Iij;
      }
  }

//! @brief Return the inertia tensor with respect to the center of mass of the object.
FT_matrix GeomObj2d::I(void) const
  {
    FT_matrix i(2,2);
    i(1,1)= Ix();   i(1,2)= -Pxy();
    i(2,1)= i(1,2); i(2,2)= Iy();
    return i;
  }

//! @brief Return the inertia tensor with respect to the point o.
FT_matrix GeomObj2d::I(const Pos2d &o) const
  {
    FT_matrix Ig= I();
    Vector2d og=getCenterOfMass() - o;
    GEOM_FT m= IArea();
    return Ig+m*(Abs2(og)*identity(Ig)-(og & og));
  }

//! @brief Return the moment of inertia with respect to the line argument.
GEOM_FT GeomObj2d::I(const Line2d &r) const
  { return I(r.Point(),r.VDir()); }


//! @brief Return the moment of inertia with respect to the axis that passes
//! through O with the direction of e.
GEOM_FT GeomObj2d::I(const Pos2d &O,const Vector2d &e) const
  {
    const FT_matrix Io= I(O);
    return dot(e,Io*e)/Abs2(e);
  }

//! @brief Return the polar moment of inertia with respect to the point o.
GEOM_FT GeomObj2d::IO(const Pos2d &o) const
  { return (I(1,1,o)+I(2,2,o)+I(3,3,o))/2; }

//! @brief Desplaza el objeto.
void GeomObj2d::Move(const Vector2d &v)
  {
    Translation2d m(v);
    Transform(m);
  }
