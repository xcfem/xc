//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
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
//MEDBaseInfo

#include "MEDBaseInfo.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"
#include "xc_utils/src/base/utils_any.h"
#include "domain/mesh/Mesh.h"
#include "domain/mesh/node/NodeIter.h"
#include "domain/mesh/node/Node.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

//! Borra la malla.
void XC::MEDBaseInfo::clear(void)
  { indices.clear(); }

//! @brief Agrega las coordenadas del vértice de índice i.
void XC::MEDBaseInfo::nuevo_tag(size_t i)
  { indices.nuevo_tag(i); }

//! @brief Acceso al mapa de índices.
const XC::MEDMapIndices &XC::MEDBaseInfo::getMapIndices(void) const
  { return indices; }

//! @brief Lectura del objeto desde archivo.
bool XC::MEDBaseInfo::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(MEDBaseInfo) Procesando comando: " << cmd << std::endl;

    if(cmd == "clear")
      {
        status.GetString(); //Ignoramos argumentos.
        clear();
        return true;
      }
    else    
      return EntCmd::procesa_comando(status);
  }

//! Devuelve la propiedad del objeto cuyo código se pasa 
//! como parámetro. 
any_const_ptr XC::MEDBaseInfo::GetProp(const std::string &cod) const 
  { return MEDObject::GetProp(cod); }
