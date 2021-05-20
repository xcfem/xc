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
//BND.cc

#include "BND3d.h"
#include "../pos_vec/Pos3d.h"
#include "../pos_vec/Vector3d.h"
#include <iostream>
#include <cmath>

const double no_number= nan("");
const Pos3d P(no_number,no_number,no_number);

//! @brief Constructor.
BND3d::BND3d(void)
  : GeomObj3d(), cgisocub(P.ToCGAL(),P.ToCGAL()), undefined(true) {}

//! @brief Constructor.
BND3d::BND3d(const Pos3d &pa,const Pos3d &pb)
  : GeomObj3d(), cgisocub(pos_min(pa,pb).ToCGAL(),pos_max(pa,pb).ToCGAL()),
    undefined(false) {}

//! @brief Area of the face parallel to XY plane.
GEOM_FT BND3d::getAreaXYFace(void) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    return getLength()*getWidth();
  }
//! @brief Area of the face parallel to XZ plane.
GEOM_FT BND3d::getAreaXZFace(void) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    return getLength()*getHeight();
  }
//! @brief Area of the face parallel to YZ plane.
GEOM_FT BND3d::getAreaYZFace(void) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
     return getWidth()*getHeight();
  }
GEOM_FT BND3d::getArea(void) const
  {
    GEOM_FT A= 2*getAreaXYFace();
    A+= 2*getAreaXZFace();
    A+= 2*getAreaYZFace();
    return A;
  }
GEOM_FT BND3d::Ix(void) const
  {
    std::cerr << "Ix() no implementado." << std::endl;
    return 0;
  }
GEOM_FT BND3d::Iy(void) const
  {
    std::cerr << "Iy() no implementado." << std::endl;
    return 0;
  }
GEOM_FT BND3d::Iz(void) const
  {
    std::cerr << "Iy() no implementado." << std::endl;
    return 0;
  }

//! @brief Updates (enlarges) the boundary with the point.
void BND3d::Update(const Pos3d &p)
  {
    if(undefined)
      {
	PutPMin(p);
	PutPMax(p);
	undefined= false;
      }
    else
      if(!In(p))
	{
	  GEOM_FT xmin= GetXMin();
	  GEOM_FT xmax= GetXMax();
	  GEOM_FT ymin= GetYMin();
	  GEOM_FT ymax= GetYMax();
	  GEOM_FT zmin= GetZMin();
	  GEOM_FT zmax= GetZMax();
	  if(p.x()< xmin)
	    xmin= p.x();
	  else if(p.x()>xmax)
	    xmax= p.x();
	  if(p.y()<ymin)
	    ymin= p.y();
	  else if(p.y()>ymax)
	    ymax= p.y();
	  if(p.z()<zmin)
	    zmin= p.z();
	  else if(p.z()>zmax)
	    zmax= p.z();
	  PutPMin(Pos3d(xmin,ymin,zmin));
	  PutPMax(Pos3d(xmax,ymax,zmax));
	}
  }

void BND3d::PutPMax(const Pos3d &pmax)
  { BND3d::operator=(BND3d(GetPMin(),pmax)); }
void BND3d::PutPMin(const Pos3d &pmin)
  { BND3d::operator=(BND3d(pmin,GetPMax())); }
void BND3d::PutPMinMax(const Pos3d &pmin,const Pos3d &pmax)
  { BND3d::operator=(BND3d(pmin,pmax)); }
Pos3d BND3d::GetPMax(void) const
  { return Pos3d(cgisocub.max()); }
Pos3d BND3d::GetPMin(void) const
  { return Pos3d(cgisocub.min()); }

Vector3d BND3d::Diagonal(void) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
     return GetPMax() - GetPMin();
  }
Pos3d BND3d::getCenterOfMass(void) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    Pos3d center_of_mass= GetPMin() + Diagonal()/2;
    return center_of_mass;
  }
bool BND3d::ClipLine(const Pos3d &p1,const Pos3d &p2) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
     return LBClipLine(p1,p2);
  }
BND3d &BND3d::operator +=(const Pos3d &p)
  {
    if(undefined)
      Update(p);
    else
      PutPMinMax(pos_min(GetPMin(),p),pos_max(GetPMax(),p));
    return *this;
  }
BND3d &BND3d::operator +=(const BND3d &a)
  {
    if(undefined)
      BND3d::operator=(a);
    else      
      PutPMinMax(pos_min(GetPMin(),a.GetPMin()),pos_max(GetPMax(),a.GetPMax()));
    return *this;
  }
BND3d operator +(const BND3d &a, const BND3d &b)
  {
    BND3d retval(a);
    return (retval+=b);
  }
bool operator ==(const BND3d &a,const BND3d &b)
  {
    if ( a.GetPMax() != b.GetPMax() ) return false; 
    if ( a.GetPMin() != b.GetPMin() ) 
      return false;
    else
      return true;
  }

//! @brief This functions is part of the line clipping algorithm of Liang-Barsky
//! (page 231 of the book Computer Graphics by Donald Hearn y 
//! Pauline Baker isbn 0-13-578634-7).
bool BND3d::LBClipTest(const GEOM_FT &p,const GEOM_FT &q,GEOM_FT &u1,GEOM_FT &u2) const
  {
    GEOM_FT r;
    int retval= true;
    if(p<0)
      {
        r= q/p;
        if(r>u2)
          retval= false;
        else
          if(r>u1) u1= r;
      }
    else
      if(p>0)
        {
          r= q/p;
          if(r<u1)
            retval= false;
          else
            if(r<u2) u2= r;
        }
      else
        //p= 0, so the line is parallel to this boundary plane.
        if(q<0)
          retval= false; //the line is outside the boundary.
    return retval;
  }

bool BND3d::LBClipLine(const Pos3d &pa,const Pos3d &pb) const
  {
    const Pos3d PMin= GetPMin();
    const Pos3d PMax= GetPMax();
    Pos3d p1(pa);
    Pos3d p2(pb);
    GEOM_FT u1= 0, u2= 1, dx= p2.x()-p1.x(),dy,dz;
    if(LBClipTest(-dx,p1.x()-PMin.x(),u1,u2))
      if(LBClipTest(dx,PMax.x()-p1.x(),u1,u2))
        {
          dy= p2.y() - p1.y();
          if(LBClipTest(-dy,p1.y()-PMin.y(),u1,u2))
            if(LBClipTest(dy,PMax.y()-p1.y(),u1,u2))
              {
                dz= p2.z() - p1.z();
                if(LBClipTest(-dz,p1.z()-PMin.z(),u1,u2))
                  if(LBClipTest(dz,PMax.z()-p1.z(),u1,u2))
                    {
                      const Vector3d dd(dx,dy,dz);
                      if(u2<1)
                        {
                          p2= p1+ u2*dd;
                        }
                      if(u1>0)
                        {
                          p1+= u1*dd;
                        }
                      return true;
                    } //dz
              } //dy 
        } //dx
    return false;
  }

// Numeración de vértices y caras.
//
//      Z
//    ^
//    | /Y
//    |/
//    -----> X
//   
//   
//        4----------7
//       /|        / |
//      / |       /  |
//     /  |      /   |    0: base
//    5----------6   |    5: tapa 
//    |   |      |   |
//    |   3------|---2
//    |  /       |  /
//    | /        | /
//    |/         |/
//    0----------1
Pos3d BND3d::Vertice(unsigned int i) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    unsigned int indice= i%8;
    return Pos3d(cgisocub.vertex(indice));
  }

unsigned short int BND3d::RegionCode(const Pos3d &p,const double &tol) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    const Pos3d PMin= GetPMin();
    const Pos3d PMax= GetPMax();
    int reg_code= 0;
    const GEOM_FT tol_ft= tol;
    if ((p.x()-PMin.x()) < -tol_ft) reg_code= 1;  //00000001
    if ((p.x()-PMax.x()) >  tol_ft) reg_code|= 2; //00000010
    if ((p.y()-PMin.y()) < -tol_ft) reg_code|= 4; //00000100
    if ((p.y()-PMax.y()) >  tol_ft) reg_code|= 8; //00001000
    if ((p.z()-PMin.z()) < -tol_ft) reg_code|= 16;//00010000
    if ((p.z()-PMax.z()) >  tol_ft) reg_code|= 32;//00100000
    return reg_code;
  }
bool BND3d::In(const Pos3d &p,const double &tol) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    CGAL::Bounded_side side= cgisocub.bounded_side(p.ToCGAL());
    return (side != CGAL::ON_UNBOUNDED_SIDE);
  }

//! @brief Recrece el BND en la cantidad que se pasa como parámetro.
BND3d BND3d::Offset(const GEOM_FT &o) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    const Vector3d vo(o,o,o);
    return BND3d(GetPMin()-vo,GetPMax()+vo);
  }

CGBbox_3 BND3d::GetCGALBbox_3(void) const
  {
    if(undefined)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; boundary is undefined." << std::endl;
    return CGBbox_3(GetXMin(),GetYMin(),GetZMin(),GetXMax(),GetYMax(),GetZMax()); }
void BND3d::Print(std::ostream &stream) const
  {
    stream << "PMax= " << GetPMax() << ','
           << "PMin= " << GetPMin();
  }
