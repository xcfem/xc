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
//SisRefCartesianas3d.cc

#include "SisRefCartesianas3d.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

Pos3d XC::SisRefCartesianas3d::getOrg(void) const
  { return ref.Org(); }

void XC::SisRefCartesianas3d::setOrg(const Pos3d &p)
  { ref.Org()= p; }

void XC::SisRefCartesianas3d::TresPuntos(const Pos3d &p1,const Pos3d &p2, const Pos3d &p3)
  { ref= Ref3d3d(p1,p2,p3); }

//! @brief Devuelve el vector unitario en la dirección del eje que se
//! pasa como parámetro expresado en el sistema global 
//! para la posición que se pasa como parámetro.
Vector3d XC::SisRefCartesianas3d::GetVDirEje(const size_t &i,const Pos3d &) const
  { return ref.GetVDirEje(i); }

//! @brief Devuelve la posicion del punto p expresado en locales
//! expresada en coordenadas globales.
Pos3d XC::SisRefCartesianas3d::GetPosGlobal(const Pos3d &p) const
  { return ref.GetPosGlobal(p); }
//! @brief Devuelve el vector v expresado en locales
//! expresado en coordenadas globales.
Vector3d XC::SisRefCartesianas3d::GetCooGlobales(const Vector3d &v) const
  { return ref.GetCooGlobales(v); }
//! @brief Devuelve la posicion del punto p expresado en globales
//! expresada en coordenadas locales.
Pos3d XC::SisRefCartesianas3d::GetPosLocal(const Pos3d &p) const
  { return ref.GetPosLocal(p); }
//! @brief Devuelve el vector v expresado en globales
//! expresado en coordenadas locales.
Vector3d XC::SisRefCartesianas3d::GetCooLocales(const Vector3d &v) const
  { return ref.GetCooLocales(v); }
