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
//Cad.h

#ifndef CAD_H
#define CAD_H

#include "preprocessor/PreprocessorContainer.h"
#include <map>
#include "boost/lexical_cast.hpp"
#include "preprocessor/cad/entities/PntMap.h"
#include "preprocessor/cad/entities/MapLineas.h"
#include "preprocessor/cad/entities/MapSurfaces.h"
#include "preprocessor/cad/entities/MapCuerpos.h"
#include "preprocessor/cad/entities/MapUniformGrids.h"
#include "preprocessor/cad/trf/MapTrfGeom.h"
#include "preprocessor/cad/ReferenceFrameMap.h"
#include "preprocessor/cad/matrices/MapEsquemas2d.h"
#include "preprocessor/cad/matrices/MapEsquemas3d.h"

class Pos3d;
class Recta3d;
class Plano3d;
class Vector3d;

namespace XC {

class Pnt;
class UniformGrid;
class SetEstruct;
class ReferenceFrame;

//! @ingroup Preprocessor
//
//! @defgroup Cad Model geometry (block topology).
//
//! @ingroup Cad
//
//! @brief Model geometry manager.
//! Management of geometry entities:
//! points, lines, surfaces, bodies, etc.
//!
//! Maintain containers for this entities.
class Cad: public PreprocessorContainer
  {
    friend class Preprocessor;
  private:
    template <class L>
    static void numera_lista(L &l);
    void numera(void);
    ReferenceFrameMap reference_systems; //!< Reference systems container.
    MapTrfGeom transformaciones_geometricas; //!< Geometric transformations container.

    PntMap puntos; //!< Points container.
    MapLineas edges; //!< Edges container.
    MapSurfaces faces; //!< Surfaces container.
    MapCuerpos cuerpos; //!< Bodys container.
    MapUniformGrids unif_grid; //!< Uniform grids container.
    MapEsquemas2d esquemas2d; //!< Bi-dimensional schemes container.
    MapEsquemas3d esquemas3d; //!< Three-dimensional schemes container.

  protected:

    SetEstruct *busca_set_estruct(const MapUniformGrids::Indice &nmb);
  public:
    //! @brief Constructor.
    Cad(Preprocessor *mod= nullptr);

    Edge *busca_edge_extremos(const PntMap::Indice &,const PntMap::Indice &);
    const Edge *busca_edge_extremos(const PntMap::Indice &,const PntMap::Indice &) const;
    
    void conciliaNDivs(void);

    void clearAll(void);
    //! @brief Destructor.
    virtual ~Cad(void);

    inline const PntMap &getPuntos(void) const
      { return puntos; }
    inline PntMap &getPuntos(void)
      { return puntos; }
    inline const MapLineas &getLineas(void) const
      { return edges; }
    inline MapLineas &getLineas(void)
      { return edges; }
    inline const MapSurfaces &getSurfaces(void) const
      { return faces; }
    inline MapSurfaces &getSurfaces(void)
      { return faces; }
    inline const MapTrfGeom &getTransformacionesGeometricas(void) const
      { return transformaciones_geometricas; }
    inline MapTrfGeom &getTransformacionesGeometricas(void)
      { return transformaciones_geometricas; }
    inline const ReferenceFrameMap &getReferenceSystems(void) const
      { return reference_systems; }
    inline ReferenceFrameMap &getReferenceSystems(void)
      { return reference_systems; }
    inline const MapEsquemas2d &getEsquemas2d(void) const
      { return esquemas2d; }
    inline MapEsquemas2d &getEsquemas2d(void)
      { return esquemas2d; }
    inline const MapEsquemas3d &getEsquemas3d(void) const
      { return esquemas3d; }
    inline MapEsquemas3d &getEsquemas3d(void)
      { return esquemas3d; }
    inline const MapUniformGrids &getUniformGrids(void) const
      { return unif_grid; }
    inline MapUniformGrids &getUniformGrids(void)
      { return unif_grid; }
  };


} //end of XC namespace
#endif
