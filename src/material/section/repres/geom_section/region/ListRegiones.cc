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
//ListRegiones.cc

#include "ListRegiones.h"
#include <material/section/repres/geom_section/region/RgSccCuadrilatero.h>
#include <material/section/repres/geom_section/region/RgSccCirc.h>
#include <material/section/repres/geom_section/region/RgSccPoligono.h>
#include "material/uniaxial/UniaxialMaterial.h" 
#include "preprocessor/loaders/MaterialLoader.h"
#include "utility/matrix/Vector.h"
#include "xc_utils/src/geom/pos_vec/Dir2d.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/sis_ref/Ref2d2d.h"



#include "xc_utils/src/geom/d2/BND2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/bool_op_poligono2d.h"

//! @brief Liberta todas las pociciones.
void XC::ListRegiones::libera(void)
  {
    for(iterator i=begin();i!=end();i++)
      delete *i;
    erase(begin(),end());
  }

//! @brief Copia las regiones de otro contenedor.
void XC::ListRegiones::copia(const ListRegiones &otra)
  {
    libera();
    for(const_iterator i=otra.begin();i!=otra.end();i++)
      push_back(*(*i)->getCopy());
  }

//! @brief Constructor.
XC::ListRegiones::ListRegiones(MaterialLoader *ml)
  : l_reg(), material_loader(ml) {}

//! @brief Copy constructor.
XC::ListRegiones::ListRegiones(const ListRegiones  &otro)
  : l_reg(), material_loader(otro.material_loader)
  { copia(otro); }

//! @brief Assignment operator.
XC::ListRegiones &XC::ListRegiones::operator=(const ListRegiones &otro)
  {
    SeccionInerte::operator=(otro);
    material_loader= otro.material_loader;
    copia(otro);
    return *this;
  }

//! @brief Aggregates a new quadrilateral region.
XC::RgSccCuadrilatero *XC::ListRegiones::newQuadRegion(const std::string &cod_mat)
  {
    Material *mat= material_loader->find_ptr(cod_mat);
    if(!mat)
      std::cerr << "ListRegiones::newQuadRegion; ¡ojo!, no se encontró el material: '"
                << cod_mat << "' deberá asignarse un material a la región.\n";
    RgSccCuadrilatero tmp(mat);
    RgSccCuadrilatero *ptr= dynamic_cast<XC::RgSccCuadrilatero *>(push_back(tmp));
    ptr->set_owner(this);
    return ptr;
  }

//! @brief Aggregates a new quadrilateral region.
XC::RgSccCirc *XC::ListRegiones::newCircularRegion(const std::string &cod_mat)
  {
    Material *mat= material_loader->find_ptr(cod_mat);
    if(!mat)
      std::cerr << "ListRegiones::newQuadRegion; ¡ojo!, no se encontró el material: '"
                << cod_mat << "' deberá asignarse un material a la región.\n";
    RgSccCirc tmp(mat);
    RgSccCirc *ptr= dynamic_cast<XC::RgSccCirc *>(push_back(tmp));
    ptr->set_owner(this);
    return ptr;
  }

//! @brief Destructor.
XC::ListRegiones::~ListRegiones(void)
  { libera(); }

//! @brief Borra todas las regiones.
void XC::ListRegiones::clear(void)
  { libera(); }

//! @brief Agrega una región al contenedor.
XC::RegionSecc *XC::ListRegiones::push_back(const RegionSecc &reg)
  {
    RegionSecc *tmp= reg.getCopy();
    l_reg::push_back(tmp);
    return tmp;
  }

XC::ListRegiones::const_iterator XC::ListRegiones::begin(void) const
  { return l_reg::begin(); }
XC::ListRegiones::const_iterator XC::ListRegiones::end(void) const
  { return l_reg::end(); }
XC::ListRegiones::iterator XC::ListRegiones::begin(void)
  { return l_reg::begin(); }
XC::ListRegiones::iterator XC::ListRegiones::end(void)
  { return l_reg::end(); }

//! @brief Returns the número total de celdas.
size_t XC::ListRegiones::getNumCells(void) const
  {
    size_t ncells= 0;
    for(const_iterator i=begin();i!=end();i++)
      {
        const RegionSecc *ptr= *i;
        ncells+= ptr->getNumCells();
      }
    return ncells;
  }

//! @brief Returns a list with the regions contours.
std::list<Poligono2d> XC::ListRegiones::getRegionsContours(void) const
  {
    std::list<Poligono2d> retval;
    for(const_iterator i= begin();i!=end();i++)
      retval.push_back((*i)->getPoligono());
    return retval;
  }

//! @brief Return the regions contours.
std::list<Poligono2d> XC::ListRegiones::getContours(void) const
  {
    std::list<Poligono2d> retval= join(getRegionsContours());
    return retval;
  }

BND2d XC::ListRegiones::getBnd(void) const
  {
    BND2d retval;
    if(!empty())
      {
        const_iterator i= begin();
        retval= (*i)->getPoligono().Bnd();
        i++;
        for(;i!=end();i++)
          retval+= (*i)->getPoligono().Bnd();
      }
    else
      std::cerr << "region container is empty. Boundary has no sense." << std::endl;
    return retval;
  }

XC::ListRegiones XC::ListRegiones::Interseccion(const Semiplano2d &sp) const
  {
    ListRegiones retval(material_loader);
    for(const_iterator i= begin();i!=end();i++)
      retval.push_back((*i)->Interseccion(sp));
    return retval;
  }

//! @brief Returns the área de las regiones.
double XC::ListRegiones::getAreaGrossSection(void) const
  {
    double retval= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      retval+= (*i)->Area();
    return retval;
  }

//! @brief Returns the centro de gravedad de la sección bruta.
XC::Vector XC::ListRegiones::getCdgGrossSection(void) const
  {
    Vector retval(2);
    double weight= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        weight= (*i)->Area();
        if(weight>0)
          {
            retval+= weight*(*i)->Cdg();
            divisor+= weight;
          }
        else
          std::cerr << "ListRegiones::getCdgGrossSection; la región: "
                    << *i << " tiene weight nulo o negativo." 
                        << std::endl;
      }
    retval/= divisor;
    return retval;
  }

//! @brief Returns the moment of inertia of the gross cross-section with respecto to the axis parallel to y passing through the centroid.
double XC::ListRegiones::getIyGrossSection(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double zCdg= getCdgGrossSection()[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->Cdg()[1]-zCdg;
        retval+= (*i)->Iy()+(*i)->Area()*sqr(d);
      }
    return retval;
  }

//! @brief Returns the momento de inercia de la sección bruta respecto al eje paralelo al z por el CDG.
double XC::ListRegiones::getIzGrossSection(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double yCdg= getCdgGrossSection()[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->Cdg()[0]-yCdg;
        retval+= (*i)->Iz()+(*i)->Area()*sqr(d);
      }
    return retval;
  }

//! @brief Returns the producto de inercia de la sección bruta respecto a los ejes paralelos al y y al z por el CDG.
double XC::ListRegiones::getPyzGrossSection(void) const
  {
    double retval= 0.0;
    double d2= 0.0;
    const Vector cooCdg= getCdgGrossSection();
    const double zCdg= cooCdg[1];
    const double yCdg= cooCdg[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d2= ((*i)->Cdg()[0]-yCdg)*((*i)->Cdg()[1]-zCdg);
        retval+= (*i)->Pyz()+(*i)->Area()*d2;
      }
    return retval;
  }

// //! Returns the regions subset that satisfy the condition
// //! being passed as parameter.
// void XC::ListRegiones::Cumplen(const std::string &cond,ListRegiones &retval,bool clear)
//   {
//     if(clear) retval.clear();
//     const_iterator i= begin();
//     for(;i!= end();i++)
//       if((*i)->verdadero(cond))
//         retval.push_back(**i);
//   }


//! @brief Returns the área homogeneizada de las regiones.
double XC::ListRegiones::getAreaHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 
    double retval= 0.0;
    double n= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            retval+= n*(*i)->Area();
          }
        else
	  std::cerr << "ListRegiones::getAreaHomogenizedSection; no se pudo obtener el material la región." << std::endl; 
      }
    return retval;
  }

XC::Vector XC::ListRegiones::getCdgHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 
    Vector retval(2);
    double weight= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            weight= mat->getTangent()/E0*(*i)->Area();
            if(weight>0)
              {
                retval+= weight*(*i)->Cdg();
                divisor+= weight;
              }
            else
	      std::cerr << "ListRegiones::getCdgHomogenizedSection; region: "
                        << *i << " weight is zero or negative." 
                        << std::endl;
          }
        else
	  std::cerr << "ListRegiones::getCdgHomogenizedSection; can't get material for region." << std::endl;
      }
    retval/= divisor;
    return retval;
  }

//! @brief Returns homogenized moment of inertia of the cross-section with respecto to the axis parallel to y passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::ListRegiones::getIyHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 
    double retval= 0.0;
    double n= 0.0;
    double d= 0.0;
    const double zCdg= getCdgHomogenizedSection(E0)[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            d= (*i)->Cdg()[1]-zCdg;
            retval+= n*((*i)->Iy()+(*i)->Area()*sqr(d));
          }
        else
	  std::cerr << "ListRegiones::getIyHomogenizedSection; no se pudo obtener el material la región." << std::endl; 
      }
    return retval;
  }

//! @brief Returns homogenized moment of inertia of the cross-section with respecto to the axis parallel to z passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::ListRegiones::getIzHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 
    double retval= 0.0;
    double n= 0.0;
    double d= 0.0;
    const double yCdg= getCdgHomogenizedSection(E0)[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            d= (*i)->Cdg()[0]-yCdg;
            retval+= n*((*i)->Iz()+(*i)->Area()*sqr(d));
          }
        else
	  std::cerr << "ListRegiones::getIyHomogenizedSection; no se pudo obtener el material la región." << std::endl; 
      }
    return retval;
  }

//! @brief Returns homogenized product of inertia of the cross-section with respecto to the axis parallel to y and z passing through the centroid.
//! @param E0: Reference elastic modulus.
double XC::ListRegiones::getPyzHomogenizedSection(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 
    double retval= 0.0;
    double n= 0.0;
    double d2= 0.0;
    const Vector cooCdg= getCdgHomogenizedSection(E0);
    const double zCdg= cooCdg[1];
    const double yCdg= cooCdg[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            d2= ((*i)->Cdg()[0]-yCdg)*((*i)->Cdg()[1]-zCdg);
            retval+= n*((*i)->Pyz()+(*i)->Area()*d2);
          }
        else
	  std::cerr << "ListRegiones::getIyHomogenizedSection; no se pudo obtener el material la región." << std::endl; 
      }
    return retval;
  }

void XC::ListRegiones::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      (*i)->Print(os);
  }

std::ostream &XC::operator<<(std::ostream &os,const ListRegiones &lr)
  {
    lr.Print(os);
    return os;
  }
