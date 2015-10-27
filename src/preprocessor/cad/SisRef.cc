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
//SisRef.cc

#include "SisRef.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Lee un objeto SisRef desde el archivo de entrada.
//!
//! Soporta los comandos:
//!
//! - pos: Lee las coordenadas del punto.
bool XC::SisRef::procesa_comando(CmdStatus &status)
  {
    return EntMdlrBase::procesa_comando(status);
  }

//! @brief Devuelve el vector unitario I expresado en el sistema global
//! en la posición que se pasa como parámetro.
Vector3d XC::SisRef::GetI(const Pos3d &p) const
  { return GetVDirEje(1,p); }
//! @brief Devuelve el vector unitario J expresado en el sistema global
//! en la posición que se pasa como parámetro.
Vector3d XC::SisRef::GetJ(const Pos3d &p) const
  { return GetVDirEje(2,p); }
//! @brief Devuelve el vector unitario I expresado en el sistema global
//! en la posición que se pasa como parámetro.
Vector3d XC::SisRef::GetK(const Pos3d &p) const
  { return GetVDirEje(3,p); }


//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
//!
//! Soporta las propiedades:
//! -nlineas: Devuelve el número de líneas que empiezan o acaban en este punto.
any_const_ptr XC::SisRef::GetProp(const std::string &cod) const
  {
    return EntMdlrBase::GetProp(cod);
  }

