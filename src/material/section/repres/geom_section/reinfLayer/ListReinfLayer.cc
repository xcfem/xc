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
#include "material/section/repres/geom_section/reinfLayer/BarraSuelta.h"
#include "material/section/repres/geom_section/reinfLayer/StraightReinfLayer.h"
#include "material/section/repres/geom_section/reinfLayer/CircReinfLayer.h"
#include "preprocessor/loaders/MaterialLoader.h"

#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "xc_utils/src/geom/pos_vec/Dir2d.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/sis_ref/Ref2d2d.h"

#include "material/section/repres/geom_section/GeomSection.h"

//! @brief Liberta todas las pociciones.
void XC::ListReinfLayer::libera(void)
  {
    for(iterator i=begin();i!=end();i++)
      delete *i;
    erase(begin(),end());
  }

//! @brief Copia las ReinfLayer de otro contenedor.
void XC::ListReinfLayer::copia(const ListReinfLayer &otra)
  {
    libera();
    for(const_iterator i=otra.begin();i!=otra.end();i++)
      push_back(*(*i)->getCopy());
  }

//! @brief Constructor.
XC::ListReinfLayer::ListReinfLayer(GeomSection *owr,MaterialLoader *ml)
  :  l_reg(), SeccionInerte(owr), material_loader(ml) {}

//! @brief Constructor de copia.
XC::ListReinfLayer::ListReinfLayer(const ListReinfLayer  &otro)
  : l_reg(), material_loader(otro.material_loader)
  { copia(otro); }

//! @brief Operador asignación.
XC::ListReinfLayer &XC::ListReinfLayer::operator=(const ListReinfLayer &otro)
  {
    SeccionInerte::operator=(otro);
    material_loader= otro.material_loader;
    copia(otro);
    return *this;
  }

XC::StraightReinfLayer *XC::ListReinfLayer::newStraightReinfLayer(const std::string &cod_mat)
  {
    XC::Material *materialPtr= material_loader->find_ptr(cod_mat);
    if(!materialPtr)
      std::clog << "ListReinfLayer::newStraightReinfLayer WARNING; material: '"
	        << cod_mat << "' not found." << std::endl;
    StraightReinfLayer tmp(this, materialPtr);
    ReinfLayer *ptr= push_back(tmp);
    ptr->set_owner(this);
    return dynamic_cast<StraightReinfLayer *>(ptr);
  }

XC::CircReinfLayer *XC::ListReinfLayer::newCircReinfLayer(const std::string &cod_mat)
  {
    CircReinfLayer tmp(this,material_loader->find_ptr(cod_mat));
    ReinfLayer *ptr= push_back(tmp);
    ptr->set_owner(this);
    return dynamic_cast<CircReinfLayer *>(ptr);
  }

XC::BarraSuelta *XC::ListReinfLayer::newReinfBar(const std::string &cod_mat)
  {
    BarraSuelta tmp(this,material_loader->find_ptr(cod_mat));
    ReinfLayer *ptr= push_back(tmp);
    ptr->set_owner(this);
    return dynamic_cast<BarraSuelta *>(ptr);
  }

//! @brief Destructor.
XC::ListReinfLayer::~ListReinfLayer(void)
  { libera(); }

//! @brief Borra las armaduras definidas.
void XC::ListReinfLayer::clear(void)
  { libera(); }

//! @brief Agrega una región al contenedor.
XC::ReinfLayer *XC::ListReinfLayer::push_back(const ReinfLayer &reg)
  {
    ReinfLayer *tmp= reg.getCopy();
    l_reg::push_back(tmp);
    return tmp;
  }

//! @brief Devuelve una referencia al objeto propietario.
const XC::GeomSection *XC::ListReinfLayer::getGeomSection(void) const
  {
    const EntCmd *owr= Owner();
    assert(owr);
    const GeomSection *retval= dynamic_cast<const GeomSection *>(owr);
    assert(retval);
    return retval;
  }

//! @brief Devuelve el valor mínimo del recubrimiento de las barras.
double XC::ListReinfLayer::getRecubrimiento(void) const
  {
    double retval= 0;
    const_iterator i= begin();
    retval= (*i)->getRecubrimiento();
    i++;
    for(const_iterator i=begin();i!=end();i++)
      retval= std::min(retval,(*i)->getRecubrimiento());
    return retval;
  }

//! @brief Devuelve el número total de celdas.
size_t XC::ListReinfLayer::getNumReinfBars(void) const
  {
    int nbars= 0;
    for(const_iterator i=begin();i!=end();i++)
      nbars+= (*i)->getNumReinfBars();
    return nbars;
  }

// //! Devuelve el subconjunto de barras de ésta que cumplen la condición que se
// //! pasa como parámetro.
// void XC::ListReinfLayer::Cumplen(const std::string &cond,ListReinfLayer &retval,bool clear)
//   {
//     if(clear) retval.clear();
//     const_iterator i= begin();
//     for(;i!= end();i++)
//       (*i)->Cumplen(cond,retval,false);
//   }

//! Devuelve el subconjunto de barras cuyo centro esta contenido en el polígono.
void XC::ListReinfLayer::getBarrasIn(const Poligono2d &plg,ListReinfLayer &retval,bool clear)
  {
    if(clear) retval.clear();
    const_iterator i= begin();
    for(;i!= end();i++)
      (*i)->getBarrasIn(plg,retval,false);
  }

//! Devuelve el subconjunto de barras cuyo centro esta contenido en el semiplano.
void XC::ListReinfLayer::getBarrasIn(const Semiplano2d &sp,ListReinfLayer &retval,bool clear)
  {
    if(clear) retval.clear();
    const_iterator i= begin();
    for(;i!= end();i++)
      (*i)->getBarrasIn(sp,retval,false);
  }

//! @brief Devuelve el área homogeneizada de las regiones.
double XC::ListReinfLayer::getAreaHomogenizedSection(const double &E0) const
  {
    double retval= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      retval+= (*i)->getReinfBars().getAreaHomogenizedSection(E0);
    return retval;
  }

XC::Vector XC::ListReinfLayer::getCdgHomogenizedSection(const double &E0) const
  {
    Vector retval(2);
    double peso= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        peso= (*i)->getReinfBars().getAreaHomogenizedSection(E0);
        retval+= peso*(*i)->getReinfBars().getCdgHomogenizedSection(E0);
        divisor+= peso;
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
    const double zCdg= getCdgHomogenizedSection(E0)[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getReinfBars().getCdgHomogenizedSection(E0)[1]-zCdg;
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
    const double yCdg= getCdgHomogenizedSection(E0)[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getReinfBars().getCdgHomogenizedSection(E0)[0]-yCdg;
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
    const double zCdg= getCdgHomogenizedSection(E0)[0];
    const double yCdg= getCdgHomogenizedSection(E0)[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d2= ((*i)->getReinfBars().getCdgHomogenizedSection(E0)[0]-yCdg)*((*i)->getReinfBars().getCdgHomogenizedSection(E0)[1]-zCdg);
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
XC::Vector XC::ListReinfLayer::getCdgGrossSection(void) const
  {
    Vector retval(2);
    double peso= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        peso= (*i)->getReinfBars().getAreaGrossSection();
        retval+= peso*(*i)->getReinfBars().getCdgGrossSection();
        divisor+= peso;
      }
    retval/= divisor;
    return retval;
  }

//! @brief Inertia of the gross section about an axis parallel to y through its centroid.
double XC::ListReinfLayer::getIyGrossSection(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double zCdg= getCdgGrossSection()[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getReinfBars().getCdgGrossSection()[1]-zCdg;
        retval+= (*i)->getReinfBars().getIyGrossSection()+(*i)->getReinfBars().getAreaGrossSection()*sqr(d);
      }
    return retval;
  }

//! @brief Inertia of the gross section about an axis parallel to z through its centroid.
double XC::ListReinfLayer::getIzGrossSection(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double yCdg= getCdgGrossSection()[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getReinfBars().getCdgGrossSection()[0]-yCdg;
        retval+= (*i)->getReinfBars().getIzGrossSection()+(*i)->getReinfBars().getAreaGrossSection()*sqr(d);
      }
    return retval;
  }

//! @brief Product of inertia of the gross section about y and z axis through its centroid.
double XC::ListReinfLayer::getPyzGrossSection(void) const
  {
    double retval= 0.0;
    double d2= 0.0;
    const double zCdg= getCdgGrossSection()[0];
    const double yCdg= getCdgGrossSection()[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d2= ((*i)->getReinfBars().getCdgGrossSection()[0]-yCdg)*((*i)->getReinfBars().getCdgGrossSection()[1]-zCdg);
        retval+= (*i)->getReinfBars().getPyzGrossSection()+(*i)->getReinfBars().getAreaGrossSection()*d2;
      }
    return retval;
  }

void XC::ListReinfLayer::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      (*i)->Print(os);
  }
