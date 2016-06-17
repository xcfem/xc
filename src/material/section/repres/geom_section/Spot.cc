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
//Spot.cc

#include "Spot.h"
#include "Eje.h"
#include "SisRefScc.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/d2/BND2d.h"



XC::Spot::Spot(const Pos2d &pto)
  : EntGeomSection(), p(pto) {}

//! Inserta la línea being passed as parameter en la lista
//! de líneas que tocan al punto.
void XC::Spot::inserta_linea(Eje *l)
  { ejes_pt.insert(l); }
//! Borra la línea being passed as parameter de la lista
//! de líneas que tocan al punto.
void XC::Spot::borra_linea(Eje *l)
  {
    std::set<const Eje *>::iterator i= find(ejes_pt.begin(),ejes_pt.end(),l);
    if(i!= ejes_pt.end()) //La ha encontrado.
      ejes_pt.erase(i);
  }
//! Devuelve el vector de posición del punto.
Vector2d XC::Spot::VectorPos(void) const
  { return p.VectorPos();  }

//! @brief Actualiza la topología.
void XC::Spot::actualiza_topologia(void)
  {}

//! @brief Devuelve el BND del objeto.
BND2d XC::Spot::Bnd(void) const
  { return BND2d(p,p);  }

//! @brief Devuelve verdadero si el punto toca a la línea.
bool XC::Spot::Toca(const Eje &l) const
  {
    std::set<const Eje *>::const_iterator i= ejes_pt.find(&l);
//     for(std::set<const Eje *>::const_iterator i= ejes_pt.begin(); i!=ejes_pt.end();i++)
//       { if(&l == (*i)) return true; }
    return (i != ejes_pt.end());
  }


//! @brief Devuelve la distancia a la posición being passed as parameter.
double XC::Spot::DistanciaA(const Pos2d &pt) const
  { return dist(p,pt);  }


