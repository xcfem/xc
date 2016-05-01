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
//MapSuperficies.cc

#include "MapSuperficies.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/cad/entidades/Pnt.h"
#include "preprocessor/cad/entidades/Face.h"
#include "preprocessor/cad/entidades/SupCuadrilatera.h"
#include "preprocessor/set_mgmt/Set.h"



//! @brief Constructor.
XC::MapSuperficies::MapSuperficies(Cad *cad)
  : MapEnt<Face>(cad) {}

//! @brief Inserta la nueva linea en el conjunto total y los conjuntos abiertos.
void XC::MapSuperficies::UpdateSets(Face *nueva_face) const
  {
    Preprocessor *preprocessor= const_cast<Preprocessor *>(getPreprocessor());
    preprocessor->get_sets().get_set_total()->GetSuperficies().push_back(nueva_face);
    preprocessor->get_sets().inserta_ent_mdlr(nueva_face);
    MapSet::map_sets &abiertos= preprocessor->get_sets().get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->GetSuperficies().push_back(nueva_face);
      }
  }

//! @brief Crea concilia el número de divisiones de las líneas.
bool XC::MapSuperficies::conciliaNDivs(void)
  { 
    getCad()->conciliaNDivs();
    return checkNDivs();
  }

//! @brief Verifica que los números de divisiones de los lados
//! son compatibles.
bool XC::MapSuperficies::checkNDivs(void) const
  {
    size_t conta= 0;
    if(!empty())
      for(const_iterator i= begin();i!=end();i++)
        if(!(*i).second->checkNDivs()) conta++;
    return (conta==0);
  }

//! @brief New quadrilateral surface.
XC::SupCuadrilatera *XC::MapSuperficies::newQuadSurfacePts(const size_t &id_p1, const size_t &id_p2, const size_t &id_p3, const size_t &id_p4)
  {
    SupCuadrilatera *retval= dynamic_cast<SupCuadrilatera *>(Nueva<SupCuadrilatera>());
    assert(retval);
    ID tmp(4);
    tmp[0]= id_p1; tmp[1]= id_p2; tmp[2]= id_p3; tmp[3]= id_p4;
    retval->setPuntos(tmp);
    return retval;
  }

//! @brief New quadrilateral surface.
XC::SupCuadrilatera *XC::MapSuperficies::newQuadSurfaceLines(const size_t &id_p1, const size_t &id_p2, const size_t &id_p3, const size_t &id_p4)
  {
    SupCuadrilatera *retval= dynamic_cast<SupCuadrilatera *>(Nueva<SupCuadrilatera>());
    assert(retval);
    ID tmp(4);
    tmp[0]= id_p1; tmp[1]= id_p2; tmp[2]= id_p3; tmp[3]= id_p4;
    retval->addLines(tmp);
    return retval;
  }

//! @brief New quadrilateral surface.
XC::SupCuadrilatera *XC::MapSuperficies::newQuadSurfaceGridPoints(const boost::python::list &l)
  {
    SupCuadrilatera *retval= dynamic_cast<SupCuadrilatera *>(Nueva<SupCuadrilatera>());
    assert(retval);
    retval->defGridPoints(l);
    return retval;
  }
