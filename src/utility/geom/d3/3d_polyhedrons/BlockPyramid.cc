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
//BlockPyramid3c.cc

#include "BlockPyramid.h"

#include "utility/geom/d1/Line3d.h"

#include "utility/geom/pos_vec/Vector2d.h"
#include "utility/geom/ref_sys/Ref2d3d.h"


//! @brief Return el haz de vectors that results from intersecting the
//! half spaces 2 by 2.
std::deque<Vector3d> BlockPyramid::haz_vectores(void) const
  {
    std::deque<Vector3d> retval;
    const std::deque<Vector3d> normales= vectores_normales(half_spaces);
    const size_t sz= normales.size();
    for(size_t i=0;i<sz;i++)
      for(size_t j=i+1;j<sz;j++)
        {
          const Vector3d v= cross(normales[i],normales[j]);
          retval.push_back(v);
          retval.push_back(-v);
        }
    return retval;
  }

//! @brief Return true if the vector is oriented inwards the half space.
bool BlockPyramid::es_interior(const HalfSpace3d &se,const Vector3d &v)
  {
    const Pos3d p= se.getPlane().Point()+1e4*v;
    const bool retval= se.In(p,1e-4);
    return retval;
  }

//! @brief Return true if the vector is oriented inwards each of the
//! half spaces.
bool BlockPyramid::es_interior(const Vector3d &v) const
  {
    bool retval= true;
    for(std::deque<HalfSpace3d>::const_iterator j=half_spaces.begin();j!=half_spaces.end();j++)
      {
        if(!es_interior(*j,v))
          {
            retval= false;
            break;
          }
      }
    return retval;
  }

//! @brief From the haz de vectors that results from intersecting the
//! half spaces 2 by 2, return those thar are interior to all the
//! half spaces.
std::deque<Vector3d> BlockPyramid::haz_vectores_interiores(void) const
  {
    std::deque<Vector3d> retval;
    const std::deque<Vector3d> haz= haz_vectores();
    for(std::deque<Vector3d>::const_iterator i= haz.begin();i!=haz.end();i++)
      if(es_interior(*i))
        retval.push_back(*i);
    return retval;
  }

//! @brief Default constructor.
BlockPyramid::BlockPyramid(void)
  : GeomObj3d() {}

//! @brief Constructor.
BlockPyramid::BlockPyramid(const std::deque<HalfSpace3d> &se)
  : GeomObj3d(), half_spaces(se) {}

//! @brief Return the object length.
GEOM_FT BlockPyramid::getLength(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented." << std::endl;
    return 0.0;
  }

//! @brief Return the area of the object surface.
GEOM_FT BlockPyramid::getArea(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0.0;
  }

//! @brief Return the volume of the object.
GEOM_FT BlockPyramid::getVolume(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0.0;
  }

//! @brief Return the moment of inertia with respect to the x axis.
GEOM_FT BlockPyramid::Ix(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0.0;
  }

//! @brief Return the moment of inertia with respect to the y axis.
GEOM_FT BlockPyramid::Iy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0.0;
  }

//! @brief Return the moment of inertia with respect to the z axis.
GEOM_FT BlockPyramid::Iz(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0.0;
  }

//! @brief Return the position of the center of mass.
Pos3d BlockPyramid::getCenterOfMass(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return Pos3d();
  }

GEOM_FT BlockPyramid::GetMax(unsigned short int i) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0.0;
  }
GEOM_FT BlockPyramid::GetMin(unsigned short int i) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return 0.0;
  }

m_double BlockPyramid::getVectoresBorde(void) const
  {
    const std::deque<Vector3d> haz= haz_vectores_interiores();
    m_double retval;
    if(!haz.empty())
      {
        const size_t sz= haz.size();
        retval= m_double(sz,3);
        size_t conta= 1;
        for(std::deque<Vector3d>::const_iterator i= haz.begin();i!=haz.end();i++,conta++)
          { retval(conta,1)= i->x(); retval(conta,2)= i->y(); retval(conta,3)= i->z(); }
      }
    return retval;
  }

Vector2d BlockPyramid::getVectorExterno1(const Ref2d3d &projPlane) const
  {
    const std::deque<Vector3d> haz= haz_vectores_interiores();
    Vector2d retval;
    if(!haz.empty())
      {
        std::deque<Vector3d>::const_iterator i= haz.begin();
        retval= projPlane.getLocalCoordinates(*i);
        double angle= retval.XAxisAngle();
        i++;
        for(;i!=haz.end();i++)
          {
            const Vector2d tmpV= projPlane.getLocalCoordinates(*i);
            const double tmpAng= tmpV.XAxisAngle();
            if(tmpAng<angle)
              {
                retval= tmpV;
                angle= tmpAng;
              }
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << " no hay vectores externos." << std::endl;
    return retval;
  }

Vector2d BlockPyramid::getVectorExterno2(const Ref2d3d &projPlane) const
  {
    const std::deque<Vector3d> haz= haz_vectores_interiores();
    Vector2d retval;
    if(!haz.empty())
      {
        std::deque<Vector3d>::const_iterator i= haz.begin();
        retval= projPlane.getLocalCoordinates(*i);
        double angle= retval.XAxisAngle();
        i++;
        for(;i!=haz.end();i++)
          {
            const Vector2d tmpV= projPlane.getLocalCoordinates(*i);
            const double tmpAng= tmpV.XAxisAngle();
            if(tmpAng>angle)
              {
                retval= tmpV;
                angle= tmpAng;
              }
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << " no hay vectores externos." << std::endl;
    return retval;
  }

//! @brief Return the pairs of external vectors of the
//! projected pyramid.
//! a: Return the projection direction (tunnel axis).
std::deque<Vector2d> BlockPyramid::getVectoresExternos(const Ref2d3d &a) const
  {
    std::deque<Vector2d> retval;
    retval.push_back(getVectorExterno1(a));
    retval.push_back(getVectorExterno2(a));
    if(retval.size()>1)
      if(retval[0]==retval[1])
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; sólo se encontró un vector de borde."
		  << std::endl;
    return retval;
  }

//! @brief Return true if has no borders.
bool BlockPyramid::Vacio(void) const
  {
    const std::deque<Vector3d> haz= haz_vectores_interiores();
    return haz.empty();
  }

//! @brief Print stuff.
void BlockPyramid::Print(std::ostream &os) const
  {}


