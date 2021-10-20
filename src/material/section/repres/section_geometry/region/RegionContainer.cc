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
//RegionContainer.cc

#include "RegionContainer.h"
#include <material/section/repres/section_geometry/region/QuadSectRegion.h>
#include <material/section/repres/section_geometry/region/CircularSectRegion.h>
#include <material/section/repres/section_geometry/region/PolygonSectRegion.h>
#include "material/uniaxial/UniaxialMaterial.h" 
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include "utility/matrix/Vector.h"
#include "utility/geom/pos_vec/Dir2d.h"
#include "utility/geom/pos_vec/Vector2d.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/ref_sys/Ref2d2d.h"



#include "utility/geom/d2/BND2d.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"
#include "utility/geom/d2/2d_polygons/polygon2d_bool_op.h"

//! @brief Liberta todas las pociciones.
void XC::RegionContainer::free_mem(void)
  {
    for(iterator i=begin();i!=end();i++)
      delete *i;
    erase(begin(),end());
  }

//! @brief Copy the regions from another container.
void XC::RegionContainer::copy(const RegionContainer &other)
  {
    free_mem();
    for(const_iterator i=other.begin();i!=other.end();i++)
      push_back(*(*i)->getCopy());
  }

//! @brief Constructor.
XC::RegionContainer::RegionContainer(MaterialHandler *ml)
  : l_reg(), material_handler(ml) {}

//! @brief Copy constructor.
XC::RegionContainer::RegionContainer(const RegionContainer  &other)
  : l_reg(), material_handler(other.material_handler)
  { copy(other); }

//! @brief Assignment operator.
XC::RegionContainer &XC::RegionContainer::operator=(const RegionContainer &other)
  {
    SectionMassProperties::operator=(other);
    material_handler= other.material_handler;
    copy(other);
    return *this;
  }

//! @brief Aggregates a new quadrilateral region.
XC::QuadSectRegion *XC::RegionContainer::newQuadRegion(const std::string &cod_mat)
  {
    Material *mat= material_handler->find_ptr(cod_mat);
    if(!mat)
      std::cerr << getClassName() << __FUNCTION__
	        << "; warning!, material: '"
                << cod_mat << "' not found. Material definition pending.\n";
    QuadSectRegion tmp(mat);
    QuadSectRegion *ptr= dynamic_cast<XC::QuadSectRegion *>(push_back(tmp));
    ptr->set_owner(this);
    return ptr;
  }

//! @brief Aggregates a new circularl region.
XC::CircularSectRegion *XC::RegionContainer::newCircularRegion(const std::string &cod_mat)
  {
    Material *mat= material_handler->find_ptr(cod_mat);
    if(!mat)
      std::cerr << getClassName() << __FUNCTION__
	        << "; warning!, material: '"
                << cod_mat << "' not found. Material definition pending.\n";
    CircularSectRegion tmp(mat);
    CircularSectRegion *ptr= dynamic_cast<XC::CircularSectRegion *>(push_back(tmp));
    ptr->set_owner(this);
    return ptr;
  }

//! @brief Destructor.
XC::RegionContainer::~RegionContainer(void)
  { free_mem(); }

//! @brief Erases all regions.
void XC::RegionContainer::clear(void)
  { free_mem(); }

//! @brief Adds a region to the container.
XC::SectRegion *XC::RegionContainer::push_back(const SectRegion &reg)
  {
    SectRegion *tmp= reg.getCopy();
    l_reg::push_back(tmp);
    return tmp;
  }

XC::RegionContainer::const_iterator XC::RegionContainer::begin(void) const
  { return l_reg::begin(); }
XC::RegionContainer::const_iterator XC::RegionContainer::end(void) const
  { return l_reg::end(); }
XC::RegionContainer::iterator XC::RegionContainer::begin(void)
  { return l_reg::begin(); }
XC::RegionContainer::iterator XC::RegionContainer::end(void)
  { return l_reg::end(); }

//! @brief Returns the total number of cells.
size_t XC::RegionContainer::getNumCells(void) const
  {
    size_t ncells= 0;
    for(const_iterator i=begin();i!=end();i++)
      {
        const SectRegion *ptr= *i;
        ncells+= ptr->getNumCells();
      }
    return ncells;
  }

//! @brief Returns a list with the regions contours.
std::list<Polygon2d> XC::RegionContainer::getRegionsContours(void) const
  {
    std::list<Polygon2d> retval;
    for(const_iterator i= begin();i!=end();i++)
      retval.push_back((*i)->getPolygon());
    return retval;
  }

//! @brief Return the regions contours.
std::list<Polygon2d> XC::RegionContainer::getContours(void) const
  {
    std::list<Polygon2d> retval= join(getRegionsContours());
    return retval;
  }

BND2d XC::RegionContainer::getBnd(void) const
  {
    BND2d retval;
    if(!empty())
      {
        const_iterator i= begin();
        retval= (*i)->getPolygon().Bnd();
        i++;
        for(;i!=end();i++)
          retval+= (*i)->getPolygon().Bnd();
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; region container is empty. Boundary has no sense."
		<< std::endl;
    return retval;
  }

XC::RegionContainer XC::RegionContainer::Intersection(const HalfPlane2d &sp) const
  {
    RegionContainer retval(material_handler);
    for(const_iterator i= begin();i!=end();i++)
      retval.push_back((*i)->Intersection(sp));
    return retval;
  }

//! @brief Returns the regions area.
double XC::RegionContainer::getAreaGrossSection(void) const
  {
    double retval= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      retval+= (*i)->getArea();
    return retval;
  }

//! @brief Returns the center of gravity of the gross cross-section.
XC::Vector XC::RegionContainer::getCenterOfMassGrossSection(void) const
  {
    Vector retval(2);
    double weight= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        weight= (*i)->getArea();
        if(weight>0)
          {
            retval+= weight*(*i)->getCenterOfMass();
            divisor+= weight;
          }
        else
          std::cerr << getClassName() << __FUNCTION__
		    << "; region: "
                    << *i << " has zero or negative weight." 
                    << std::endl;
      }
    retval/= divisor;
    return retval;
  }

//! @brief Returns the moment of inertia of the gross cross-section with respect to the axis parallel to y passing through the centroid.
double XC::RegionContainer::getIyGrossSection(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double zCenterOfMass= getCenterOfMassGrossSection()[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getCenterOfMass()[1]-zCenterOfMass;
        retval+= (*i)->Iy()+(*i)->getArea()*sqr(d);
      }
    return retval;
  }

//! @brief Returns the moment of inertia of the gross cross-section with respect to the axis parallel to z axis through the centroid.
double XC::RegionContainer::getIzGrossSection(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double yCenterOfMass= getCenterOfMassGrossSection()[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getCenterOfMass()[0]-yCenterOfMass;
        retval+= (*i)->Iz()+(*i)->getArea()*sqr(d);
      }
    return retval;
  }

//! @brief Returns the product of inertia of the gross cross-section respecto a los axis parallel to the y y al z por el centroid.
double XC::RegionContainer::getPyzGrossSection(void) const
  {
    double retval= 0.0;
    double d2= 0.0;
    const Vector center_of_mass_coordinates= getCenterOfMassGrossSection();
    const double zCenterOfMass= center_of_mass_coordinates[1];
    const double yCenterOfMass= center_of_mass_coordinates[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d2= ((*i)->getCenterOfMass()[0]-yCenterOfMass)*((*i)->getCenterOfMass()[1]-zCenterOfMass);
        retval+= (*i)->Pyz()+(*i)->getArea()*d2;
      }
    return retval;
  }

//! @brief Returns the homogenized area of the regions.
double XC::RegionContainer::getAreaHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= "
		<< E0 << std::endl; 
    double retval= 0.0;
    double n= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            retval+= n*(*i)->getArea();
          }
        else
	  std::cerr << getClassName() << __FUNCTION__
		    << "; can't get region material." << std::endl; 
      }
    return retval;
  }

XC::Vector XC::RegionContainer::getCenterOfMassHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= "
		<< E0 << std::endl; 
    Vector retval(2);
    double weight= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            weight= mat->getTangent()/E0*(*i)->getArea();
            if(weight>0)
              {
                retval+= weight*(*i)->getCenterOfMass();
                divisor+= weight;
              }
            else
	      std::cerr << getClassName() << __FUNCTION__
			<< "; region: "
                        << *i << " weight is zero or negative." 
                        << std::endl;
          }
        else
	  std::cerr << getClassName() << __FUNCTION__
		    << "; can't get region material." << std::endl; 
      }
    retval/= divisor;
    return retval;
  }

//! @brief Returns homogenized moment of inertia of the cross-section with respect to the axis parallel to y passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::RegionContainer::getIyHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= "
		<< E0 << std::endl; 
    double retval= 0.0;
    double n= 0.0;
    double d= 0.0;
    const double zCenterOfMass= getCenterOfMassHomogenizedSection(E0)[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            d= (*i)->getCenterOfMass()[1]-zCenterOfMass;
            retval+= n*((*i)->Iy()+(*i)->getArea()*sqr(d));
          }
        else
	  std::cerr << getClassName() << __FUNCTION__
	            << "; can't get section material." << std::endl; 
      }
    return retval;
  }

//! @brief Returns homogenized moment of inertia of the cross-section with respect to the axis parallel to z passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::RegionContainer::getIzHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= "
		<< E0 << std::endl; 
    double retval= 0.0;
    double n= 0.0;
    double d= 0.0;
    const double yCenterOfMass= getCenterOfMassHomogenizedSection(E0)[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            d= (*i)->getCenterOfMass()[0]-yCenterOfMass;
            retval+= n*((*i)->Iz()+(*i)->getArea()*sqr(d));
          }
        else
	  std::cerr << getClassName() << __FUNCTION__
	            << "; can't get region material." << std::endl; 
      }
    return retval;
  }

//! @brief Returns homogenized product of inertia of the cross-section with respect to the axis parallel to y and z passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::RegionContainer::getPyzHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= "
		<< E0 << std::endl; 
    double retval= 0.0;
    double n= 0.0;
    double d2= 0.0;
    const Vector center_of_mass_coordinates= getCenterOfMassHomogenizedSection(E0);
    const double zCenterOfMass= center_of_mass_coordinates[1];
    const double yCenterOfMass= center_of_mass_coordinates[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            d2= ((*i)->getCenterOfMass()[0]-yCenterOfMass)*((*i)->getCenterOfMass()[1]-zCenterOfMass);
            retval+= n*((*i)->Pyz()+(*i)->getArea()*d2);
          }
        else
	  std::cerr << getClassName() << __FUNCTION__
	            << "; can't get region material." << std::endl; 
      }
    return retval;
  }

//! @brief Print stuff.
void XC::RegionContainer::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      (*i)->Print(os);
  }

std::ostream &XC::operator<<(std::ostream &os,const RegionContainer &lr)
  {
    lr.Print(os);
    return os;
  }
