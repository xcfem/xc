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
//Traslacion.cc

#include "Traslacion.h"
#include "preprocessor/cad/Cad.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

void XC::Traslacion::setVector(const Vector3d &v)
  { tr= Traslacion3d(v); }

//! @brief Lee un objeto Traslación desde el archivo de entrada.
bool XC::Traslacion::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= status.Cmd();
    if(verborrea>2)
      std::clog << "(Traslacion) Procesando comando: " << cmd << std::endl;
    static Vector3d v;
    if(cmd == "vector")
      {
        v.LeeCmd(status);
        tr= Traslacion3d(v);
        return true;
      }
    else if(cmd == "puntos_vector")
      {
        const std::vector<MapPuntos::Indice> tmp= crea_vector_size_t(status.GetString());
        if(tmp.size()>1)
          v= get_modelador()->getCad().getPuntos().getVector(tmp[0],tmp[1]); 
        else
	  std::cerr << "(Traslacion) Procesando comando: " << cmd
                    << " se necesitan dos puntos para definir el vector." << std::endl;
        tr= Traslacion3d(v);
        return true;
      }
    else
      return TrfGeom::procesa_comando(status);
  }

//! @brief Aplica la transformación a los elementos del conjunto.
Pos3d XC::Traslacion::Transforma(const Pos3d &p) const
  { return tr.Transforma(p); }

//! @brief Aplica la transformación a los elementos del conjunto.
Vector3d XC::Traslacion::Transforma(const Vector3d &v) const
  { return tr.Transforma(v); }
