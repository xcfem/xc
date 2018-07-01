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
//Preprocessor.h

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/actor/actor/MovableObject.h"
#include "prep_handlers/MaterialHandler.h"
#include "prep_handlers/BeamIntegratorHandler.h"
#include "prep_handlers/TransfCooHandler.h"
#include "prep_handlers/NodeHandler.h"
#include "prep_handlers/ElementHandler.h"
#include "prep_handlers/LoadHandler.h"
#include "prep_handlers/BoundaryCondHandler.h"
#include "set_mgmt/MapSet.h"
#include "multi_block_topology/MultiBlockTopology.h"
#include "utility/handler/DataOutputHandler.h"


namespace XC {
class Domain;
class Constraint;
class FEProblem;
class FE_Datastore;

//!  @defgroup Preprocessor Preprocessor.

//!  @ingroup Preprocessor
//! 
//!  @brief Finite element model generation tools.
//!  
class Preprocessor: public EntCmd, public MovableObject
  {
  private:
    static const int theDbTag= 0; //!< the Preprocessor unique database tag == 0

    Domain *domain; //!< Proble domain.

    MaterialHandler materialHandler; //!< Material creation tools.
    TransfCooHandler transf; //!< Coordinate transformation.
    BeamIntegratorHandler beamIntegrators; //!< Beam integrator schemes.
    NodeHandler nodes; //! Node creation tools.
    ElementHandler elements; //!< Element creation tools.
    LoadHandler loads; //!< Load creation tools.
    BoundaryCondHandler constraints; //! Constraint creation tools.

    MultiBlockTopology mbt; //!< Geometric entities: points, lines,...

    MapSet sets; //!< Sets of entities.

    friend class MultiBlockTopology;
    friend class SetMeshComp;
    friend class Set;

    Preprocessor(const Preprocessor &);
    Preprocessor &operator=(const Preprocessor &);
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class EntMdlr;
    friend class MapSet;
    friend class SetEstruct;
    friend class ProtoElementHandler;
    friend class ElementHandler;
    friend class BoundaryCondHandler;
    friend class FEProblem;
    void UpdateSets(Element *);
    void UpdateSets(Constraint *);

    SetEstruct *busca_set_estruct(const std::string &nmb);
  public:
    Preprocessor(EntCmd *owr,DataOutputHandler::map_output_handlers *oh= nullptr);
    inline Domain *getDomain(void)
      { return domain; }
    inline const Domain *getDomain(void) const
      { return domain; }
    FE_Datastore *getDataBase(void);

    void UpdateSets(Node *);

    MapSet &get_sets(void)
      { return sets; }
    const MapSet &get_sets(void) const
      { return sets; }
    MaterialHandler &getMaterialHandler(void)
      { return materialHandler; }
    const MaterialHandler &getMaterialHandler(void) const
      { return materialHandler; }
    inline NodeHandler &getNodeHandler(void)
      { return nodes; }
    inline const NodeHandler &getNodeHandler(void) const
      { return nodes; }
    ElementHandler &getElementHandler(void)
      { return elements; }
    const ElementHandler &getElementHandler(void) const
      { return elements; }
    LoadHandler &getLoadHandler(void)
      { return loads; }
    const LoadHandler &getLoadHandler(void) const
      { return loads; }
    BoundaryCondHandler &getBoundaryCondHandler(void)
      { return constraints; }
    const BoundaryCondHandler &getBoundaryCondHandler(void) const
      { return constraints; }
    TransfCooHandler &getTransfCooHandler(void)
      { return transf; }
    const TransfCooHandler &getTransfCooHandler(void) const
      { return transf; }
    BeamIntegratorHandler &getBeamIntegratorHandler(void)
      { return beamIntegrators; }
    const BeamIntegratorHandler &getBeamIntegratorHandler(void) const
      { return beamIntegrators; }

    const MultiBlockTopology &getMultiBlockTopology(void) const
      { return mbt; }
    MultiBlockTopology &getMultiBlockTopology(void)
      { return mbt; }

    void resetLoadCase(void);
    void clearAll(void);

    static void setDeadSRF(const double &);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);


    virtual ~Preprocessor(void);
  };
} // end of XC namespace

#endif
