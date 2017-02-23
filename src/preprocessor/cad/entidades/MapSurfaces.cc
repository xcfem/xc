//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//MapSurfaces.cc

#include "MapSurfaces.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/cad/entidades/Pnt.h"
#include "preprocessor/cad/entidades/Face.h"
#include "preprocessor/cad/entidades/QuadSurface.h"
#include "preprocessor/set_mgmt/Set.h"



//! @brief Constructor.
XC::MapSurfaces::MapSurfaces(Cad *cad)
  : MapEnt<Face>(cad) {}

//! @brief Inserta la nueva linea en the set total y the opened sets.
void XC::MapSurfaces::UpdateSets(Face *nueva_face) const
  {
    Preprocessor *preprocessor= const_cast<Preprocessor *>(getPreprocessor());
    preprocessor->get_sets().get_set_total()->getSurfaces().push_back(nueva_face);
    preprocessor->get_sets().inserta_ent_mdlr(nueva_face);
    MapSet::map_sets &abiertos= preprocessor->get_sets().get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->getSurfaces().push_back(nueva_face);
      }
  }

//! @brief Crea concilia el número de divisiones de las líneas.
bool XC::MapSurfaces::conciliaNDivs(void)
  { 
    getCad()->conciliaNDivs();
    return checkNDivs();
  }

//! @brief Verifica que los números de divisiones de los lados
//! son compatibles.
bool XC::MapSurfaces::checkNDivs(void) const
  {
    size_t conta= 0;
    if(!empty())
      for(const_iterator i= begin();i!=end();i++)
        if(!(*i).second->checkNDivs()) conta++;
    return (conta==0);
  }

//! @brief New quadrilateral surface.
XC::QuadSurface *XC::MapSurfaces::newQuadSurfacePts(const size_t &id_p1, const size_t &id_p2, const size_t &id_p3, const size_t &id_p4)
  {
    QuadSurface *retval= dynamic_cast<QuadSurface *>(Nueva<QuadSurface>());
    assert(retval);
    ID tmp(4);
    tmp[0]= id_p1; tmp[1]= id_p2; tmp[2]= id_p3; tmp[3]= id_p4;
    retval->setPuntos(tmp);
    return retval;
  }

//! @brief New quadrilateral surface.
XC::QuadSurface *XC::MapSurfaces::newQuadSurfaceLines(const size_t &id_p1, const size_t &id_p2, const size_t &id_p3, const size_t &id_p4)
  {
    QuadSurface *retval= dynamic_cast<QuadSurface *>(Nueva<QuadSurface>());
    assert(retval);
    ID tmp(4);
    tmp[0]= id_p1; tmp[1]= id_p2; tmp[2]= id_p3; tmp[3]= id_p4;
    retval->addLines(tmp);
    return retval;
  }

//! @brief New quadrilateral surface.
XC::QuadSurface *XC::MapSurfaces::newQuadSurfaceGridPoints(const boost::python::list &l)
  {
    QuadSurface *retval= dynamic_cast<QuadSurface *>(Nueva<QuadSurface>());
    assert(retval);
    retval->defGridPoints(l);
    return retval;
  }
