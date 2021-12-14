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
//AnnulusSector2d.cc

#include "AnnulusSector2d.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/pos_vec/Pos2dArray.h"
#include "utility/geom/ref_sys/Ref2d2d.h"
#include "utility/geom/d1/Line2d.h"

#include "utility/geom/d2/2d_polygons/Polygon2d.h"


//! @brief Builds the object from its center and its radius.
AnnulusSector2d::AnnulusSector2d(const CircularSector2d &c,const double &r)
  : CircularSector2d(c), inner_radius(r) {}

//! @brief Comparison operator.
bool AnnulusSector2d::operator==(const AnnulusSector2d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= CircularSector2d::operator==(other);
        if(retval)
          retval= (inner_radius==other.inner_radius);
       }
    return retval;
  }

//! @brief Return the position of the center of mass.
Pos2d AnnulusSector2d::getCenterOfMass(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented." << std::endl;
    return Centro();
  }

//! @brief Return a circular sector with the same center and angles and
//! a different radius.
CircularSector2d AnnulusSector2d::getSector(const double &R) const
  { return CircularSector2d(Circle2d(Centro(),R),Theta1(),Theta2()); }

//! @brief Return the circular sector interior to the annulus.
CircularSector2d AnnulusSector2d::SectorInterior(void) const
  { return getSector(inner_radius); }


//! @brief Return the point inicial del arco exterior.
Pos2d AnnulusSector2d::PInicExt(void) const
  { return CircularSector2d::PInic(); }

//! @brief Return the point final del arco exterior.
Pos2d AnnulusSector2d::PFinExt(void) const
  { return CircularSector2d::PFin(); }

//! @brief Return the point inicial del arco interior.
Pos2d AnnulusSector2d::PInicInt(void) const
  { return SectorInterior().PInic(); }

//! @brief Return the point final del arco interior.
Pos2d AnnulusSector2d::PFinInt(void) const
  { return SectorInterior().PFin(); }


//! @brief Return the point medio del arco exterior.
Pos2d AnnulusSector2d::PMedExt(void) const
  { return CircularSector2d::PMed(); }

//! @brief Return the point medio del arco interior.
Pos2d AnnulusSector2d::PMedInt(void) const
  { return SectorInterior().PMed(); }

//! @brief Return the length of the arco exterior del sector de anillo circular.
GEOM_FT AnnulusSector2d::getExteriorArcLength(void) const
  { return double_to_FT(getIncludedAngle())*outerRadius(); }

//! @brief Return the length of the arco interior del sector de anillo circular.
GEOM_FT AnnulusSector2d::getInteriorArcLength(void) const
  { return double_to_FT(getIncludedAngle())*innerRadius(); }

//! @brief Return the length of the object.
GEOM_FT AnnulusSector2d::getLength(void) const
  { return getExteriorArcLength()+getInteriorArcLength()+2*(outerRadius()-innerRadius()); }

//! @brief Return the area of the object.
GEOM_FT AnnulusSector2d::getArea(void) const
  { return CircularSector2d::getArea()-SectorInterior().getArea(); }

//! @brief Return the maximum value of the i coordinate of the object points.
GEOM_FT AnnulusSector2d::GetMax(unsigned short int i) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0;
  }
//! @brief Return el valor mínimo of the i coordinate of the object points.
GEOM_FT AnnulusSector2d::GetMin(unsigned short int i) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0;
  }

//! @brief Return el moment of inertia of the circular sector with respect to 
//! axis parallel to x que pasa por su centro.
GEOM_FT AnnulusSector2d::Ix(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0;
  }

//! @brief Return el moment of inertia of the circular sector with respect to 
//! axis parallel to y que pasa por su centro.
GEOM_FT AnnulusSector2d::Iy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0;
  }

//! @brief Return el moment of inertia of the circular sector with respect to 
//! axis parallel to z que pasa por su centro.
GEOM_FT AnnulusSector2d::Iz(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0;
  }

//! @brief Return el product of inertia of the circular sector with respect to
//! the axis parallel to x e y que pasan por su centro.
GEOM_FT AnnulusSector2d::Pxy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0;
  }

//! @brief Return true if the point is inside the object.
bool AnnulusSector2d::In(const Pos2d &p, const double &tol) const
  {
    if(CircularSector2d::In(p,tol))
      {
        return !(SectorInterior().In(p,tol)); //XXX ¿tol?
      }
    else
      return false;
  }

Pos2dArray AnnulusSector2d::getExtArcPoints(const size_t &n) const
  { return CircularSector2d::getArcPoints(n); }

Pos2dArray AnnulusSector2d::getIntArcPoints(const size_t &n) const
  { return SectorInterior().getArcPoints(n); }

//! @brief Return n points equally spaced over the object perimeter.
Pos2dArray AnnulusSector2d::getPointsOnPerimeter(const size_t &n) const
  {
    Pos2dArray retval;
    if(n>3)
      {
        retval= Pos2dArray(n,2);
        if(n==4)
          {
            retval(1,2)= PInicExt(); //Initial and final points
            retval(n,2)= PFinExt();
            retval(1,1)= PInicInt(); //Initial and final points
            retval(n,1)= PFinInt();
          }
        else
          {
            retval.PutCol(2,getExtArcPoints(n));
            retval.PutCol(1,getIntArcPoints(n));
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; number of points must be greater or equal to four."
		<< std::endl;
    return retval;
  }
//! @brief Returns a point mesh equiespaciados sobre el sector del anillo circular.
Pos2dArray AnnulusSector2d::genMesh(const size_t &nDivRad,const size_t &nDivCirc) const
  {
    Pos2dArray retval(nDivCirc+1,nDivRad+1);
    if(nDivRad>0  && nDivCirc>0)
      {
        const double deltaRad= (outerRadius() - innerRadius())/nDivRad;

        double rad_j= innerRadius();
        for(size_t j= 0;j<nDivRad+1;j++)
          {
            Pos2dArray tmp= getSector(rad_j).getArcPoints(nDivCirc+1);
            retval.PutCol(j+1,tmp);
            rad_j+= deltaRad;
          }
      }
    return retval;
  }

Polygon2d AnnulusSector2d::getPolygon2d(const size_t &n) const
  {
    Polygon2d retval;
    Pos2dArray ptos= getPointsOnPerimeter(n);
    for(size_t i=1;i<=n;i++)
      retval.push_back(ptos(i,2));
    for(size_t i=n;i>0;i--)
      retval.push_back(ptos(i,1));
    return retval;
  }

void AnnulusSector2d::Print(std::ostream &os) const
  {
    CircularSector2d::Print(os);
    os << " inner_radius= " << inner_radius;
  }

