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
//Rotacion.cc

#include "Rotacion.h"
#include "preprocessor/cad/Cad.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d1/Recta3d.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Lee un objeto Rotacion desde el archivo de entrada.
//!
//! Soporta los comandos:
//!
//! - ang: Define el ángulo de rotación.
bool XC::Rotacion::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= status.Cmd();
    if(verborrea>2)
      std::clog << "(Rotacion) Procesando comando: " << cmd << std::endl;
    static GEOM_FT ang;
    static Recta3d eje;
    if(cmd == "ang")
      {
        ang= interpretaDouble(status.GetString());
        rr= Rotacion3d(eje,ang);
        return true;
      }
    else if(cmd == "eje")
      {
        eje.LeeCmd(status);
        rr= Rotacion3d(eje,ang);
        return true;
      }
    else if(cmd == "puntos_eje")
      {
        const std::vector<MapPuntos::Indice> tmp= crea_vector_size_t(status.GetString());
        if(tmp.size()>1)
          eje= get_preprocessor()->getCad().getPuntos().getRecta(tmp[0],tmp[1]); 
        else
	  std::cerr << "(Rotacion) Procesando comando: " << cmd
                    << " se necesitan dos puntos para definir el eje." << std::endl;
        rr= Rotacion3d(eje,ang);
        return true;
      }
    else
      return TrfGeom::procesa_comando(status);
  }

//! @brief Aplica la transformación a los elementos del conjunto.
Pos3d XC::Rotacion::Transforma(const Pos3d &p) const
  { return rr.Transforma(p); }

//! @brief Aplica la transformación a los elementos del conjunto.
Vector3d XC::Rotacion::Transforma(const Vector3d &v) const
  { return rr.Transforma(v); }
