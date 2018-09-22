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
//ListReinfLayer.cc

#include "ListReinfLayer.h"
#include "material/section/repres/geom_section/reinfLayer/SingleBar.h"
#include "material/section/repres/geom_section/reinfLayer/StraightReinfLayer.h"
#include "material/section/repres/geom_section/reinfLayer/CircReinfLayer.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"

#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "xc_utils/src/geom/pos_vec/Dir2d.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/sis_ref/Ref2d2d.h"

#include "material/section/repres/geom_section/GeomSection.h"

//! @brief Release memory.
void XC::ListReinfLayer::free_mem(void)
  {
    for(iterator i=begin();i!=end();i++)
      delete *i;
    erase(begin(),end());
  }

//! @brief Copy the layers from another container.
void XC::ListReinfLayer::copy(const ListReinfLayer &otra)
  {
    free_mem();
    for(const_iterator i=otra.begin();i!=otra.end();i++)
      push_back(*(*i)->getCopy());
  }

//! @brief Constructor.
XC::ListReinfLayer::ListReinfLayer(GeomSection *owr,MaterialHandler *ml)
  :  l_reg(), SectionMassProperties(owr), material_handler(ml) {}

//! @brief Copy constructor.
XC::ListReinfLayer::ListReinfLayer(const ListReinfLayer  &other)
  : l_reg(), material_handler(other.material_handler)
  { copy(other); }

//! @brief Assignment operator.
XC::ListReinfLayer &XC::ListReinfLayer::operator=(const ListReinfLayer &other)
  {
    SectionMassProperties::operator=(other);
    material_handler= other.material_handler;
    copy(other);
    return *this;
  }

XC::StraightReinfLayer *XC::ListReinfLayer::newStraightReinfLayer(const std::string &cod_mat)
  {
    XC::Material *materialPtr= material_handler->find_ptr(cod_mat);
    if(!materialPtr)
      std::clog << getClassName() << __FUNCTION__
		<< "; WARNING, material: '"
	        << cod_mat << "' not found." << std::endl;
    StraightReinfLayer tmp(this, materialPtr);
    ReinfLayer *ptr= push_back(tmp);
    ptr->set_owner(this);
    return dynamic_cast<StraightReinfLayer *>(ptr);
  }

XC::CircReinfLayer *XC::ListReinfLayer::newCircReinfLayer(const std::string &cod_mat)
  {
    CircReinfLayer tmp(this,material_handler->find_ptr(cod_mat));
    ReinfLayer *ptr= push_back(tmp);
    ptr->set_owner(this);
    return dynamic_cast<CircReinfLayer *>(ptr);
  }

//! Returns a single bar with the material with the identifier being passed
//! as parameter.
XC::SingleBar *XC::ListReinfLayer::newReinfBar(const std::string &cod_mat)
  {
    SingleBar tmp(this,material_handler->find_ptr(cod_mat));
    ReinfLayer *ptr= push_back(tmp);
    ptr->set_owner(this);
    return dynamic_cast<SingleBar *>(ptr);
  }

//! @brief Destructor.
XC::ListReinfLayer::~ListReinfLayer(void)
  { free_mem(); }

//! @brief Erases the reinforcement layers.
void XC::ListReinfLayer::clear(void)
  { free_mem(); }

//! @brief Adds a rebar layer to the container.
XC::ReinfLayer *XC::ListReinfLayer::push_back(const ReinfLayer &reg)
  {
    ReinfLayer *tmp= reg.getCopy();
    l_reg::push_back(tmp);
    return tmp;
  }

//! @brief Returns a reference to the GeomSection object
//! that owns this one.
const XC::GeomSection *XC::ListReinfLayer::getGeomSection(void) const
  {
    const CommandEntity *owr= Owner();
    assert(owr);
    const GeomSection *retval= dynamic_cast<const GeomSection *>(owr);
    assert(retval);
    return retval;
  }

//! @brief Returns the minimum value of the bars concrete cover.
double XC::ListReinfLayer::getCover(void) const
  {
    double retval= 0;
    const_iterator i= begin();
    retval= (*i)->getCover();
    i++;
    for(const_iterator i=begin();i!=end();i++)
      retval= std::min(retval,(*i)->getCover());
    return retval;
  }

//! @brief Returns the número total de celdas.
size_t XC::ListReinfLayer::getNumReinfBars(void) const
  {
    int nbars= 0;
    for(const_iterator i=begin();i!=end();i++)
      nbars+= (*i)->getNumReinfBars();
    return nbars;
  }

//! Returns the rebar subset which center lies inside the polygon.
//! @param plg: 2D polygon.
//! @param retval: rebars which centers lies inside the polygon.
void XC::ListReinfLayer::getBarrasIn(const Polygon2d &plg,ListReinfLayer &retval,bool clear)
  {
    if(clear) retval.clear();
    const_iterator i= begin();
    for(;i!= end();i++)
      (*i)->getBarrasIn(plg,retval,false);
  }

//! Returns the rebar subset which center lies in the half plane.
//! @param sp: Half plane.
//! @param retval: rebars which centers lies inside the half plane.
void XC::ListReinfLayer::getBarrasIn(const HalfPlane2d &sp,ListReinfLayer &retval,bool clear)
  {
    if(clear) retval.clear();
    const_iterator i= begin();
    for(;i!= end();i++)
      (*i)->getBarrasIn(sp,retval,false);
  }

//! @brief Returns the homogenized area of the regions.
double XC::ListReinfLayer::getAreaHomogenizedSection(const double &E0) const
  {
    double retval= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      retval+= (*i)->getReinfBars().getAreaHomogenizedSection(E0);
    return retval;
  }

XC::Vector XC::ListReinfLayer::getCenterOfMassHomogenizedSection(const double &E0) const
  {
    Vector retval(2);
    double weight= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        weight= (*i)->getReinfBars().getAreaHomogenizedSection(E0);
        retval+= weight*(*i)->getReinfBars().getCenterOfMassHomogenizedSection(E0);
        divisor+= weight;
      }
    retval/= divisor;
    return retval;
  }

//! @brief Returns homogenized moment of inertia of the cross-section with respecto to the axis parallel to y passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::ListReinfLayer::getIyHomogenizedSection(const double &E0) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double zCenterOfMass= getCenterOfMassHomogenizedSection(E0)[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getReinfBars().getCenterOfMassHomogenizedSection(E0)[1]-zCenterOfMass;
        retval+= (*i)->getReinfBars().getIyHomogenizedSection(E0)+(*i)->getReinfBars().getAreaHomogenizedSection(E0)*sqr(d);
      }
    return retval;
  }

//! @brief Returns homogenized moment of inertia of the cross-section with respecto to the axis parallel to z passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::ListReinfLayer::getIzHomogenizedSection(const double &E0) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double yCenterOfMass= getCenterOfMassHomogenizedSection(E0)[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getReinfBars().getCenterOfMassHomogenizedSection(E0)[0]-yCenterOfMass;
        retval+= (*i)->getReinfBars().getIzHomogenizedSection(E0)+(*i)->getReinfBars().getAreaHomogenizedSection(E0)*sqr(d);
      }
    return retval;
  }

//! @brief Returns homogenized product of inertia of the cross-section with respecto to the axis parallel to y and z passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::ListReinfLayer::getPyzHomogenizedSection(const double &E0) const
  {
    double retval= 0.0;
    double d2= 0.0;
    const double zCenterOfMass= getCenterOfMassHomogenizedSection(E0)[1];
    const double yCenterOfMass= getCenterOfMassHomogenizedSection(E0)[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d2= ((*i)->getReinfBars().getCenterOfMassHomogenizedSection(E0)[0]-yCenterOfMass)*((*i)->getReinfBars().getCenterOfMassHomogenizedSection(E0)[1]-zCenterOfMass);
        retval+= (*i)->getReinfBars().getPyzHomogenizedSection(E0)+(*i)->getReinfBars().getAreaHomogenizedSection(E0)*d2;
      }
    return retval;
  }

//! @brief Returns region's gross section area.
double XC::ListReinfLayer::getAreaGrossSection(void) const
  {
    double retval= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      retval+= (*i)->getReinfBars().getAreaGrossSection();
    return retval;
  }

//! @brief Returns gross section centroid position.
XC::Vector XC::ListReinfLayer::getCenterOfMassGrossSection(void) const
  {
    Vector retval(2);
    double weight= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        weight= (*i)->getReinfBars().getAreaGrossSection();
        retval+= weight*(*i)->getReinfBars().getCenterOfMassGrossSection();
        divisor+= weight;
      }
    retval/= divisor;
    return retval;
  }

//! @brief Inertia of the gross section about an axis parallel to y through its centroid.
double XC::ListReinfLayer::getIyGrossSection(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double zCenterOfMass= getCenterOfMassGrossSection()[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getReinfBars().getCenterOfMassGrossSection()[1]-zCenterOfMass;
        retval+= (*i)->getReinfBars().getIyGrossSection()+(*i)->getReinfBars().getAreaGrossSection()*sqr(d);
      }
    return retval;
  }

//! @brief Inertia of the gross section about an axis parallel to z through its centroid.
double XC::ListReinfLayer::getIzGrossSection(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double yCenterOfMass= getCenterOfMassGrossSection()[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getReinfBars().getCenterOfMassGrossSection()[0]-yCenterOfMass;
        retval+= (*i)->getReinfBars().getIzGrossSection()+(*i)->getReinfBars().getAreaGrossSection()*sqr(d);
      }
    return retval;
  }

//! @brief Product of inertia of the gross section about y and z axis through its centroid.
double XC::ListReinfLayer::getPyzGrossSection(void) const
  {
    double retval= 0.0;
    double d2= 0.0;
    const double zCenterOfMass= getCenterOfMassGrossSection()[0];
    const double yCenterOfMass= getCenterOfMassGrossSection()[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d2= ((*i)->getReinfBars().getCenterOfMassGrossSection()[0]-yCenterOfMass)*((*i)->getReinfBars().getCenterOfMassGrossSection()[1]-zCenterOfMass);
        retval+= (*i)->getReinfBars().getPyzGrossSection()+(*i)->getReinfBars().getAreaGrossSection()*d2;
      }
    return retval;
  }

void XC::ListReinfLayer::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      (*i)->Print(os);
  }
