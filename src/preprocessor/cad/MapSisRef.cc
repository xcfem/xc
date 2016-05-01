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
//MapSisRef.cc

#include "MapSisRef.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/cad/SisRef.h"
#include "preprocessor/set_mgmt/Set.h"


#include "SisRefCartesianas3d.h"

//! @brief Constructor.
XC::MapSisRef::MapSisRef(Cad *cad)
  : MapCadMember<SisRef>(cad) {}

//! @brief Crea un nuevo sistema de referencia.
XC::SisRef *XC::MapSisRef::Nuevo(const std::string &tipo)
  {
    SisRef *retval= busca(getTag());
    if(!retval) //El sistema de referencia es nuevo.
      {
        if(tipo == "cartesianas")
          {
            Preprocessor *preprocessor= getPreprocessor();
            retval= new SisRefCartesianas3d("r"+boost::lexical_cast<std::string>(getTag()),preprocessor);
            (*this)[getTag()]= retval;
            tag++;
          }
        else
	  std::cerr << "MapSisRef::NuevoSisRef; el tipo de sistema de referencia: '" << tipo
                    << "' es desconocido." << std::endl;
      }
    return retval;
  }
