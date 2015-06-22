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
//Reflexion.cc

#include "Reflexion.h"
#include "modelador/cad/Cad.h"
#include "modelador/Modelador.h"
#include "modelador/cad/entidades/Pnt.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Lee un objeto Reflexion desde el archivo de entrada.
//!
//! Soporta los comandos:
//!
//! - ang: Define el ángulo de rotación.
bool XC::Reflexion::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= status.Cmd();
    if(verborrea>2)
      std::clog << "(Reflexion) Procesando comando: " << cmd << std::endl;
    static Plano3d plano;
    if(cmd == "plano")
      {
        plano.LeeCmd(status);
        rf= Reflexion3d(plano);
        return true;
      }
    else if(cmd == "puntos_plano")
      {
        const std::vector<MapPuntos::Indice> tmp= crea_vector_size_t(status.GetString());
        if(tmp.size()>2)
          plano= get_modelador()->getCad().getPuntos().getPlano(tmp[0],tmp[1],tmp[2]); 
        else
	  std::cerr << "(Reflexión) Procesando comando: " << cmd
                    << " se necesitan tres puntos para definir el plano." << std::endl;
        rf= Reflexion3d(plano);
        return true;
      }
    else
      return TrfGeom::procesa_comando(status);
  }

//! @brief Aplica la transformación a los elementos del conjunto.
Pos3d XC::Reflexion::Transforma(const Pos3d &p) const
  { return rf.Transforma(p); }

//! @brief Aplica la transformación a los elementos del conjunto.
Vector3d XC::Reflexion::Transforma(const Vector3d &v) const
  { return rf.Transforma(v); }
