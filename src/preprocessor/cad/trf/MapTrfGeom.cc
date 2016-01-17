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
//MapTrfGeom.cc

#include "MapTrfGeom.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/cad/trf/TrfGeom.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "preprocessor/set_mgmt/Set.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "Rotacion.h"
#include "Reflexion.h"
#include "Escalado.h"
#include "Traslacion.h"

//! @brief Constructor.
XC::MapTrfGeom::MapTrfGeom(Cad *cad)
  : MapCadMember<TrfGeom>(cad) {}

//! @brief Defines a new transformation.
XC::TrfGeom *XC::MapTrfGeom::newTransformation(const std::string &type)
  {
    TrfGeom *retval= nullptr;
    if(type == "rotation") //Crea una nueva rotación.
      retval= Nueva<Rotacion>();
    else if(type == "reflection") //Crea una nueva reflexión.
      retval= Nueva<Reflexion>();
    else if(type == "scaling") //Crea un nuevo escalado.
      retval= Nueva<Escalado>();
    else if(type == "translation") //Crea una nueva traslacion.
      retval= Nueva<Traslacion>();
    else
      std::cerr << "Transformation type: '" << type
                << "' unknown." << std::endl;
    return retval;
  }
