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
//VectorReinfBar.cc

#include "VectorReinfBar.h"
#include <material/section/repres/geom_section/reinfBar/ReinfBar.h> 
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "xc_utils/src/geom/pos_vec/Dir2d.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/sis_ref/Ref2d2d.h"
#include "material/uniaxial/UniaxialMaterial.h" 




//! @brief Libera todas las pociciones.
void XC::VectorReinfBar::libera(void)
  {
    const size_t sz= size();
    for(size_t i= 0;i<sz;i++)
      libera(i);
  }

//! @brief Libera la posición i.
void XC::VectorReinfBar::libera(const size_t i)
  {
    if((*this)[i])
      delete (*this)[i];
    (*this)[i]= nullptr;
  }

//! @brief Guarda una copia de la celda en la posición i.
void XC::VectorReinfBar::alloc(const size_t i,const ReinfBar &c)
  {
    libera(i);
    (*this)[i]= c.getCopy();
  }

//! @brief Constructor.
XC::VectorReinfBar::VectorReinfBar(const size_t &sz)
  : v_rfBar(sz,nullptr) {}

//! @brief Constructor de copia.
XC::VectorReinfBar::VectorReinfBar(const VectorReinfBar  &otro)
  : v_rfBar(otro.size(),nullptr)
  {
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      {
        const ReinfBar *tmp= otro[i];
        if(tmp)
          (*this)[i]= tmp->getCopy();
      }
  }

//! @brief Operador asignación.
XC::VectorReinfBar &XC::VectorReinfBar::operator=(const VectorReinfBar &otro)
  {
    resize(otro.size());
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      {
        const ReinfBar *tmp= otro[i];
        if(tmp)
          (*this)[i]= tmp->getCopy();
      }
    return *this;
  }

//! @brief Destructor.
XC::VectorReinfBar::~VectorReinfBar(void)
  { libera(); }

//! @brief Devuelve el área homogeneizada de las regiones.
double XC::VectorReinfBar::getAreaSeccHomogeneizada(const double &E0) const
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
            retval+= n*(*i)->getArea();
          }
        else
	  std::cerr << "VectorReinfBar::getAreaSeccHomogeneizada; no se pudo obtener el material la región." << std::endl; 
      }
    return retval;
  }

XC::Vector XC::VectorReinfBar::getCdgSeccHomogeneizada(const double &E0) const
  {
    if(fabs(E0)<1e-6)
      std::clog << "homogenization reference modulus too small; E0= " << E0 << std::endl; 

    Vector retval(2);
    double peso= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            peso= mat->getTangent()/E0*(*i)->getArea();
            retval+= peso*(*i)->getPosition();
            divisor+= peso;
          }
        else
	  std::cerr << "VectorReinfBar::getCdgSeccHomogeneizada; no se pudo obtener el material la región." << std::endl;
      }
    retval/= divisor;
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección homogeneizada respecto al eje paralelo al y por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::VectorReinfBar::getIySeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    double n= 0.0;
    double d= 0.0;
    const double zCdg= getCdgSeccHomogeneizada(E0)[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            d= (*i)->getPosition()[1]-zCdg;
            retval+= n*((*i)->Iy()+(*i)->getArea()*sqr(d));
          }
        else
	  std::cerr << "VectorReinfBar::getIySeccHomogeneizada; no se pudo obtener el material la región." << std::endl; 
      }
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección homogeneizada respecto al eje paralelo al z por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::VectorReinfBar::getIzSeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    double n= 0.0;
    double d= 0.0;
    const double yCdg= getCdgSeccHomogeneizada(E0)[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            d= (*i)->getPosition()[0]-yCdg;
            retval+= n*((*i)->Iz()+(*i)->getArea()*sqr(d));
          }
        else
	  std::cerr << "VectorReinfBar::getIySeccHomogeneizada; no se pudo obtener el material la región." << std::endl; 
      }
    return retval;
  }

//! @brief Devuelve el producto de inercia de la sección homogeneizada respecto a los ejes paralelos al y y al z por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::VectorReinfBar::getPyzSeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    double n= 0.0;
    double d2= 0.0;
    const double zCdg= getCdgSeccHomogeneizada(E0)[0];
    const double yCdg= getCdgSeccHomogeneizada(E0)[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            d2= ((*i)->getPosition()[0]-yCdg)*((*i)->getPosition()[1]-zCdg);
            retval+= n*((*i)->Pyz()+(*i)->getArea()*d2);
          }
        else
	  std::cerr << "VectorReinfBar::getIySeccHomogeneizada; no se pudo obtener el material la región." << std::endl; 
      }
    return retval;
  }


//! @brief Devuelve el área bruta de las barras.
double XC::VectorReinfBar::getAreaSeccBruta(void) const
  {
    double retval= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      retval+= (*i)->getArea();
    return retval;
  }

//! @brief Devuelve el centro de gravedad de la sección bruta.
XC::Vector XC::VectorReinfBar::getCdgSeccBruta(void) const
  {
    Vector retval(2);
    double peso= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        peso= (*i)->getArea();
        retval+= peso*(*i)->getPosition();
        divisor+= peso;
      }
    retval/= divisor;
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección bruta respecto al eje paralelo al y por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::VectorReinfBar::getIySeccBruta(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double zCdg= getCdgSeccBruta()[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getPosition()[1]-zCdg;
        retval+= (*i)->Iy()+(*i)->getArea()*sqr(d);
      }
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección bruta respecto al eje paralelo al z por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::VectorReinfBar::getIzSeccBruta(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double yCdg= getCdgSeccBruta()[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getPosition()[0]-yCdg;
        retval+= (*i)->Iz()+(*i)->getArea()*sqr(d);
      }
    return retval;
  }

//! @brief Devuelve el producto de inercia de la sección bruta respecto a los ejes paralelos al y y al z por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::VectorReinfBar::getPyzSeccBruta(void) const
  {
    double retval= 0.0;
    double d2= 0.0;
    const Vector posCdg= getCdgSeccBruta();
    const double zCdg= posCdg[1];
    const double yCdg= posCdg[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d2= ((*i)->getPosition()[0]-yCdg)*((*i)->getPosition()[1]-zCdg);
        retval+= (*i)->Pyz()+(*i)->getArea()*d2;
      }
    return retval;
  }

//! @brief Cambia tamaño.
void XC::VectorReinfBar::resize(const size_t sz)
  {
    libera();
    v_rfBar::resize(sz);
    for(size_t i= 0;i<sz;i++)
      (*this)[i]= nullptr;
  }

void XC::VectorReinfBar::put(const size_t i,const ReinfBar &c)
  { alloc(i,c); }

void XC::VectorReinfBar::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      (*i)->Print(os);
  }
