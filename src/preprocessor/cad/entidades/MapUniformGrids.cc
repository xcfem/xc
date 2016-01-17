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
//MapUniformGrids.cc

#include "MapUniformGrids.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/cad/entidades/Pnt.h"
#include "preprocessor/cad/entidades/UniformGrid.h"
#include "preprocessor/cad/entidades/SupCuadrilatera.h"
#include "preprocessor/set_mgmt/Set.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::MapUniformGrids::MapUniformGrids(Cad *cad)
  : MapEnt<UniformGrid>(cad) {}

//! @brief Inserta la nueva linea en el conjunto total y los conjuntos abiertos.
void XC::MapUniformGrids::UpdateSets(UniformGrid *nuevo_unif_grid) const
  {
    Preprocessor *preprocessor= const_cast<Preprocessor *>(getPreprocessor());
    preprocessor->get_sets().get_set_total()->GetUniformGrids().push_back(nuevo_unif_grid);
    preprocessor->get_sets().inserta_ent_mdlr(nuevo_unif_grid);
    MapSet::map_sets &abiertos= preprocessor->get_sets().get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->GetUniformGrids().push_back(nuevo_unif_grid);
      }
  }

//! @brief Crea una nueva malla uniforme.
XC::UniformGrid *XC::MapUniformGrids::Nueva(void)
  {
    UniformGrid *retval= busca(getTag());
    if(!retval) //La malla es nueva.
      {
        Preprocessor *preprocessor= getPreprocessor();
        retval= new UniformGrid(preprocessor);
        (*this)[getTag()]= retval;
        UpdateSets(retval);
        tag++;
      }
    return retval;
  }
