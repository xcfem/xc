//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//Eje.cc

#include "Eje.h"
#include "Spot.h"
#include "xc_basic/src/matrices/m_int.h"
#include "xc_utils/src/geom/d2/BND2d.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "boost/any.hpp"

//! @brief Constructor.
XC::Eje::Eje(GeomSection *sr,const size_t &nd)
  : EntGeomSection("",sr), ndiv(nd) {}

//! @brief Constructor.
//! @param nombre: Identificador del objeto.
//! @param i: Índice para gráficos.
//! @param m: Apuntador a la representación de la sección.
XC::Eje::Eje(const std::string &nombre,GeomSection *sr,const size_t &nd)
  : EntGeomSection(nombre,sr), ndiv(nd) {}

//! @brief Destructor.
XC::Eje::~Eje(void)
  {
    if(P1()) P1()->borra_linea(this);
    if(P2()) P2()->borra_linea(this);
  }

//! @brief Actualiza la topología.
void XC::Eje::actualiza_topologia(void)
  {
    if(P1()) P1()->inserta_linea(this);
    if(P2()) P2()->inserta_linea(this);
  }

//! @brief Devuelve un apuntador al punto inicial.
XC::Spot *XC::Eje::P1(void)
  {
    std::cerr << "Eje::P1; esta función debe redefinirse en las clases"
              << " derivadas, se devuelve nullptr;" << std::endl;
    return nullptr;
  }
//! @brief Devuelve un apuntador (constante) al punto inicial.
const XC::Spot *XC::Eje::P1(void) const
  {
    std::cerr << "Eje::P1; esta función debe redefinirse en las clases"
              << " derivadas, se devuelve nullptr;" << std::endl;
    return nullptr;
  }
//! @brief Devuelve un apuntador al punto final.
XC::Spot *XC::Eje::P2(void)
  {
    std::cerr << "Eje::P2; esta función debe redefinirse en las clases"
              << " derivadas, se devuelve nullptr;" << std::endl;
    return nullptr;
  }
//! @brief Devuelve un apuntador (constante) al punto final.
const XC::Spot *XC::Eje::P2(void) const
  {
    std::cerr << "Eje::P2; esta función debe redefinirse en las clases"
              << " derivadas, se devuelve nullptr;" << std::endl;
    return nullptr;
  }

//! @brief Devuelve el conjunto de líneas que tocan al punto.
std::set<const XC::Eje *> XC::GetLineasTocan(const XC::Spot &p)
  {
    std::set<const Eje *> retval;
    const std::set<const Eje *> &tocan= p.EjesTocan();
    for(std::set<const Eje *>::const_iterator i= tocan.begin(); i!=tocan.end();i++)
      retval.insert(*i);
    return retval;
  }
