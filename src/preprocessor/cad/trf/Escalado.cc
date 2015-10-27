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
//Escalado.cc

#include "Escalado.h"
#include "preprocessor/cad/Cad.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d1/Recta3d.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Lee un objeto Escalado desde el archivo de entrada.
//!
//! Soporta los comandos:
//!
//! - ang: Define el ángulo de rotación.
bool XC::Escalado::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= status.Cmd();
    if(verborrea>2)
      std::clog << "(Escalado) Procesando comando: " << cmd << std::endl;
    if(cmd == "fe")
      {
        GEOM_FT fe= interpretaDouble(status.GetString());
        ee= Escalado3d(fe);
        return true;
      }
    else
      return TrfGeom::procesa_comando(status);
  }

//! @brief Aplica la transformación a los elementos del conjunto.
Pos3d XC::Escalado::Transforma(const Pos3d &p) const
  { return ee.Transforma(p); }

//! @brief Aplica la transformación a los elementos del conjunto.
Vector3d XC::Escalado::Transforma(const Vector3d &v) const
  { return ee.Transforma(v); }
