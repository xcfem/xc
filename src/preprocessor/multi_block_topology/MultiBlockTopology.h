// -*-c++-*-
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
//MultiBlockTopology.h

#ifndef MULTIBLOCKTOPOLOGY_H
#define MULTIBLOCKTOPOLOGY_H

#include "preprocessor/PreprocessorContainer.h"
#include <map>
#include "boost/lexical_cast.hpp"
#include "preprocessor/multi_block_topology/entities/containers/PntMap.h"
#include "preprocessor/multi_block_topology/entities/containers/LineMap.h"
#include "preprocessor/multi_block_topology/entities/containers/SurfaceMap.h"
#include "preprocessor/multi_block_topology/entities/containers/BodyMap.h"
#include "preprocessor/multi_block_topology/entities/containers/UniformGridMap.h"
#include "preprocessor/multi_block_topology/trf/MapTrfGeom.h"
#include "preprocessor/multi_block_topology/ReferenceFrameMap.h"
#include "preprocessor/multi_block_topology/matrices/Framework2d.h"
#include "preprocessor/multi_block_topology/matrices/Framework3d.h"

class Pos3d;
class Line3d;
class Plane;
class Vector3d;

namespace XC {

class Pnt;
class UniformGrid;
class SetEstruct;
class ReferenceFrame;

//! @ingroup Preprocessor
//!
//! @defgroup MultiBlockTopology Model geometry (block topology).
//
//! @ingroup MultiBlockTopology
//
//! @brief Model geometry manager.
//! Management of geometry entities:
//! points, lines, surfaces, bodies, etc.
//!
//! Maintain containers for this entities.
class MultiBlockTopology: public PreprocessorContainer, public MovableObject
  {
    friend class Preprocessor;
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);
  private:
    template <class L>
    static void numera_lista(L &l);
    void numera(void);
    ReferenceFrameMap reference_systems; //!< Reference systems container.
    MapTrfGeom geometric_transformations; //!< Geometric transformations container.

    PntMap points; //!< Points container.
    LineMap edges; //!< Edges container.
    SurfaceMap faces; //!< Surfaces container.
    BodyMap bodies; //!< Bodys container.
    UniformGridMap unif_grid; //!< Uniform grids container.
    Framework2d framework2d; //!< Bi-dimensional framework container.
    Framework3d framework3d; //!< Three-dimensional framework container.

  protected:

    SetEstruct *busca_set_estruct(const UniformGridMap::Indice &nmb);
  public:
    //! @brief Constructor.
    MultiBlockTopology(Preprocessor *mod= nullptr);

    Edge *find_edge_by_endpoints(const PntMap::Indice &,const PntMap::Indice &);
    const Edge *find_edge_by_endpoints(const PntMap::Indice &,const PntMap::Indice &) const;
    
    void conciliaNDivs(void);
    inline std::set<const XC::Edge *> getHomologousSides(const Edge *e) const
      { return faces.getHomologousSides(e); }

    void clearAll(void);
    //! @brief Destructor.
    virtual ~MultiBlockTopology(void);

    inline const PntMap &getPoints(void) const
      { return points; }
    inline PntMap &getPoints(void)
      { return points; }
    inline const LineMap &getLines(void) const
      { return edges; }
    inline LineMap &getLines(void)
      { return edges; }
    inline const SurfaceMap &getSurfaces(void) const
      { return faces; }
    inline SurfaceMap &getSurfaces(void)
      { return faces; }
    inline const BodyMap &getBodies(void) const
      { return bodies; }
    inline BodyMap &getBodies(void)
      { return bodies; }
    inline const MapTrfGeom &getGeometricTransformations(void) const
      { return geometric_transformations; }
    inline MapTrfGeom &getGeometricTransformations(void)
      { return geometric_transformations; }
    inline const ReferenceFrameMap &getReferenceSystems(void) const
      { return reference_systems; }
    inline ReferenceFrameMap &getReferenceSystems(void)
      { return reference_systems; }
    inline const Framework2d &getFramework2d(void) const
      { return framework2d; }
    inline Framework2d &getFramework2d(void)
      { return framework2d; }
    inline const Framework3d &getFramework3d(void) const
      { return framework3d; }
    inline Framework3d &getFramework3d(void)
      { return framework3d; }
    inline const UniformGridMap &getUniformGrids(void) const
      { return unif_grid; }
    inline UniformGridMap &getUniformGrids(void)
      { return unif_grid; }
    
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
  };


} //end of XC namespace
#endif
