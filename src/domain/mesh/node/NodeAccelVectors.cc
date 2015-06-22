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
//NodeAccelVectors.cpp

#include <domain/mesh/node/NodeAccelVectors.h>
#include <utility/tagged/TaggedObject.h>
#include <utility/matrix/Vector.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::NodeAccelVectors::NodeAccelVectors(void)
  :NodeVectors() {}


//! @brief Lee un objeto XC::Node desde archivo
//!
//! Soporta los comandos:
//! -coo: Lee las coordenadas del nodo.
bool XC::NodeAccelVectors::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(NodeAccelVectors) Procesando comando: " << cmd << std::endl;
    return NodeVectors::procesa_comando(status);
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
//! nnod: Devuelve el número de nodos del dominio.
any_const_ptr XC::NodeAccelVectors::GetProp(const std::string &cod) const
  {
    if(cod=="trial_accel" && trialData)
      return TaggedObject::get_prop_vector(trialData);
    else if(cod=="commit_accel" && commitData)
      return TaggedObject::get_prop_vector(commitData);
    else
      return NodeVectors::GetProp(cod);
  }

void XC::NodeAccelVectors::Print(std::ostream &s,int flag)
  {
    if(commitData)
      s << "\tcommitAccel: " << *trialData;
    s << "\n";
  }
