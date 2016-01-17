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
//elem_load.h

#ifndef ELEM_LOAD_H
#define ELEM_LOAD_H

#include "domain/load/beam_loads/Beam2dPointLoad.h"
#include "domain/load/beam_loads/Beam2dUniformLoad.h"
#include "domain/load/beam_loads/BeamStrainLoad.h"
#include "domain/load/beam_loads/Beam3dPointLoad.h"
#include "domain/load/beam_loads/Beam3dUniformLoad.h"
#include "domain/load/beam_loads/TrussStrainLoad.h"
#include "domain/load/volumen/BrickSelfWeight.h"
#include "domain/load/plane/ShellUniformLoad.h"
#include "domain/load/plane/BidimStrainLoad.h"
#include "domain/load/plane/ShellStrainLoad.h"
#include "domain/load/edge_loads/ElementEdge3dUniformLoad.h"
#include "domain/load/edge_loads/ElementEdge2dUniformLoad.h"
#include "domain/load/pattern/LoadPattern.h"

namespace XC {

//! @brief Añade una carga sobre elementos.
template <class ELEM_LOAD>
ELEM_LOAD *new_elem_load(LoadPattern *lp,int &tag_el)
  {
    ELEM_LOAD *load= NULL;
    if(lp)
      {
        load= new ELEM_LOAD(tag_el);
        if(load)
          {
            tag_el++;
            if(!lp->addElementalLoad(load))
              {
                delete load;
                std::cerr << "LoadLoader::new_elem_load; no se agregó la carga sobre el elemento." << std::endl;
              }
          }
        else
          std::cerr << "new_elem_load: no se pudo crear la carga sobre elementos: " << tag_el << std::endl;
      }
    else
       std::cerr << "new_elem_load: se paso un puntero nulo." << std::endl;
    return load;
  }

ElementalLoad *procesa_element_load(LoadPattern *,int &,const std::string &);

} // end of XC namespace

#endif
