//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
#include "material/section/repres/section_geometry/reinfLayer/SingleBar.h"
#include "material/section/repres/section_geometry/reinfLayer/StraightReinfLayer.h"
#include "material/section/repres/section_geometry/reinfLayer/PolylineReinfLayer.h"
#include "material/section/repres/section_geometry/reinfLayer/CircReinfLayer.h"
#include "preprocessor/prep_handlers/MaterialHandler.h"
#include "material/Material.h" 
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "utility/geom/pos_vec/Dir2d.h"
#include "utility/geom/pos_vec/Vector2d.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/coo_sys/ref_sys/Ref2d2d.h"
#include "utility/geom/d2/BND2d.h"
#include "utility/geom/d1/Segment2d.h"
#include "material/section/repres/section_geometry/SectionGeometry.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Release memory.
void XC::ListReinfLayer::free_mem(void)
  {
    for(iterator i=begin();i!=end();i++)
      delete *i;
    erase(begin(),end());
  }

//! @brief Copy the layers from another container.
void XC::ListReinfLayer::copy(const ListReinfLayer &other)
  {
    free_mem();
    for(const_iterator i=other.begin();i!=other.end();i++)
      push_back(*(*i)->getCopy());
  }

//! @brief Constructor.
XC::ListReinfLayer::ListReinfLayer(SectionGeometry *owr,MaterialHandler *ml)
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
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; WARNING, material: '"
	        << cod_mat << "' not found."
		<< Color::def << std::endl;
    StraightReinfLayer tmp(this, materialPtr);
    ReinfLayer *ptr= push_back(tmp);
    ptr->set_owner(this);
    return dynamic_cast<StraightReinfLayer *>(ptr);
  }

XC::PolylineReinfLayer *XC::ListReinfLayer::newPolylineReinfLayer(const std::string &cod_mat)
  {
    XC::Material *materialPtr= material_handler->find_ptr(cod_mat);
    if(!materialPtr)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; WARNING, material: '"
	        << cod_mat << "' not found."
		<< Color::def << std::endl;
    PolylineReinfLayer tmp(this, materialPtr);
    ReinfLayer *ptr= push_back(tmp);
    ptr->set_owner(this);
    return dynamic_cast<PolylineReinfLayer *>(ptr);
  }

XC::CircReinfLayer *XC::ListReinfLayer::newCircReinfLayer(const std::string &cod_mat)
  {
    CircReinfLayer tmp(this,material_handler->find_ptr(cod_mat));
    ReinfLayer *ptr= push_back(tmp);
    ptr->set_owner(this);
    return dynamic_cast<CircReinfLayer *>(ptr);
  }

//! @brief Returns a single bar with the material with the identifier being
//! passed as parameter.
XC::SingleBar *XC::ListReinfLayer::newReinfBar(const std::string &cod_mat)
  {
    SingleBar tmp(this,material_handler->find_ptr(cod_mat));
    ReinfLayer *ptr= push_back(tmp);
    ptr->set_owner(this);
    return dynamic_cast<SingleBar *>(ptr);
  }

//! @brief Create the StraightReinfLayer object defining the reinforcement along
//! the given segment.
XC::StraightReinfLayer *XC::ListReinfLayer::reinforceSegment(const std::string &cod_mat, const double &spacing, const double &diameter, const Segment2d &s)
  {
    StraightReinfLayer *retval= this->newStraightReinfLayer(cod_mat);
    const int numReinfBars= int(s.getLength()/spacing)+1;
    retval->setNumReinfBars(numReinfBars);
    retval->setReinfBarDiameter(diameter);
    retval->setLineSegment(s);
    return retval;
  }

//! @brief Create a secondary StraightReinfLayer object whose rebars are
//! placed between those of the given layer.
//! @param offset: distance from the first rebar of the secondary reinforcement
//!                layer to the first rebar of the primary one.
//! @param spacing: distance between consecutive rebars for the secondary
//!                 reinforcement layer.
//! @param diameter: diameter of the rebars of the secondary reinforcement
//!                  layer.
XC::StraightReinfLayer *XC::ListReinfLayer::secondaryReinfLayer(const StraightReinfLayer &rl, const double &offset, const double &spacing, const double &diameter)
  {
    StraightReinfLayer tmp= rl._get_secondary_reinf_layer(offset, spacing, diameter);
    StraightReinfLayer *retval= dynamic_cast<StraightReinfLayer *>(push_back(tmp));
    retval->set_owner(this);
    return (retval);
  }

//! @brief Create a StraightReinfLayer object whose rebars are placed between
//! those of the given layer.
//! @param diameter: diameter of the rebars of the returned reinforcement layer.
XC::StraightReinfLayer *XC::ListReinfLayer::reinforceMidPoints(const StraightReinfLayer &rl, const double &diameter)
  {
    StraightReinfLayer tmp= rl._reinforce_mid_points(diameter);
    StraightReinfLayer *retval= dynamic_cast<StraightReinfLayer *>(push_back(tmp));
    retval->set_owner(this);
    return (retval);
  }

//! @brief Create the PolylineReinfLayer object defining the reinforcement along
//! the given segment.
XC::PolylineReinfLayer *XC::ListReinfLayer::reinforcePolyline(const std::string &cod_mat, const double &spacing, const double &diameter, const Polyline2d &pline)
  {
    PolylineReinfLayer *retval= this->newPolylineReinfLayer(cod_mat);
    const int numReinfBars= int(pline.getLength()/spacing)+1;
    retval->setNumReinfBars(numReinfBars);
    retval->setReinfBarDiameter(diameter);
    retval->setPolyline(pline);
    return retval;
  }

//! @brief Create a PolylineReinfLayer object whose rebars are placed between
//! those of the given layer.
//! @param diameter: diameter of the rebars of the returned reinforcement layer.
XC::PolylineReinfLayer *XC::ListReinfLayer::reinforceMidPoints(const PolylineReinfLayer &rl, const double &diameter)
  {
    PolylineReinfLayer tmp= rl._reinforce_mid_points(diameter);

    PolylineReinfLayer *retval= dynamic_cast<PolylineReinfLayer *>(push_back(tmp));
    retval->set_owner(this);
    return (retval);
  }

//! @brief Create a secondary PolylineReinfLayer object whose rebars are
//! placed between those of the given layer.
//! @param offset: distance from the first rebar of the secondary reinforcement
//!                layer to the first rebar of the primary one.
//! @param spacing: distance between consecutive rebars for the secondary
//!                 reinforcement layer.
//! @param diameter: diameter of the rebars of the secondary reinforcement
//!                  layer.
XC::PolylineReinfLayer *XC::ListReinfLayer::secondaryReinfLayer(const PolylineReinfLayer &rl, const double &offset, const double &spacing, const double &diameter)
  {
    PolylineReinfLayer tmp= rl._get_secondary_reinf_layer(offset, spacing, diameter);
    PolylineReinfLayer *retval= dynamic_cast<PolylineReinfLayer *>(push_back(tmp));
    retval->set_owner(this);
    return (retval);
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

//! @brief Returns a reference to the SectionGeometry object
//! that owns this one.
const XC::SectionGeometry *XC::ListReinfLayer::getSectionGeometry(void) const
  {
    const CommandEntity *owr= Owner();
    assert(owr);
    const SectionGeometry *retval= dynamic_cast<const SectionGeometry *>(owr);
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

//! @brief Returns the total number of cells (reinforcing bars).
size_t XC::ListReinfLayer::getNumReinfBars(void) const
  {
    int nbars= 0;
    for(const_iterator i=begin();i!=end();i++)
      nbars+= (*i)->getNumReinfBars();
    return nbars;
  }

//! @brief Return a list of const pointers to the different reinforcing
//! layers materials.
std::set<const XC::Material *>XC::ListReinfLayer::getMaterials(void) const
  {
    std::set<const Material *> retval;
    for(const_iterator i=begin();i!=end();i++)
      {
        const ReinfLayer *ptr= *i;
	const Material *mat= ptr->getMaterialPtr();
        if(mat)
	  retval.insert(mat);
      }
    return retval;
  }

//! @brief Return a list of pointers to the different reinforcing layers
//! materials.
std::set<XC::Material *>XC::ListReinfLayer::getMaterials(void)
  {
    std::set<Material *> retval;
    for(iterator i=begin();i!=end();i++)
      {
        ReinfLayer *ptr= *i;
	Material *mat= ptr->getMaterialPtr();
        if(mat)
	  retval.insert(mat);
      }
    return retval;
  }
  
//! @brief Return a Python list of pointers to the different reinforcing
//! layers materials.
boost::python::list XC::ListReinfLayer::getMaterialsPy(void)
  {
    boost::python::list retval;
    std::set<Material *> tmp= this->getMaterials();
    for(std::set<Material *>::const_iterator i= tmp.begin(); i!=tmp.end(); i++)
      {
	Material *mat= *i;
	if(mat)
	  {
	    boost::python::object pyObj(boost::ref(*mat));
	    retval.append(pyObj);
	  }
      }
    return retval;
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

//! @brief Return the reinforcement layers boundary.
BND2d XC::ListReinfLayer::getBnd(void) const
  {
    BND2d retval;
    const_iterator i= this->begin();
    if(i!= this->end())
      {
        retval= (*i)->getBnd();
        i++;
      }
    for(;i!= end();i++)
      retval+= (*i)->getBnd();
    return retval;
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

//! @brief Return the linear density of the regions in this container.
double XC::ListReinfLayer::getLinearRho(void) const
  { 
    double retval= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
	const ReinfLayer *reinfLayer= *i;
	retval+= reinfLayer->getLinearRho();
      }
    return retval;
  }

void XC::ListReinfLayer::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      (*i)->Print(os);
  }
