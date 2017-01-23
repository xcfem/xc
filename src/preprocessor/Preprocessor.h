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
//Objeto encargado de interpretar la entrada de datos
//del preproceso y generar la malla de elementos finitos.

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/actor/actor/MovableObject.h"
#include "loaders/MaterialLoader.h"
#include "loaders/BeamIntegratorLoader.h"
#include "loaders/TransfCooLoader.h"
#include "loaders/NodeLoader.h"
#include "loaders/ElementLoader.h"
#include "loaders/LoadLoader.h"
#include "loaders/ConstraintLoader.h"
#include "set_mgmt/MapSet.h"
#include "cad/Cad.h"
#include "utility/handler/DataOutputHandler.h"


namespace XC {
class Domain;
class Constraint;
class ProblemaEF;
class FE_Datastore;

//!  @defgroup Preprocessor Preprocessor.

//!  \ingroup Preprocessor
//! 
//!  \brief Finite element model generation.
//!  
class Preprocessor: public EntCmd, public MovableObject
  {
  private:
    static const int theDbTag= 0; //!< the Preprocessor unique database tag == 0

    Domain *dominio; //!< Dominio del problema.

    MaterialLoader materiales; //!< Materiales para el problema.
    TransfCooLoader transf; //!< Transformaciones de coordenadas.
    BeamIntegratorLoader beamIntegrators; //!< Beam integrator schemes.
    NodeLoader nodos; //! Cargador de nodos.
    ElementLoader elementos; //!< Cargador de elementos.
    LoadLoader loads; //!< Cargador de cargas.
    ConstraintLoader constraints; //! Cargador de coacciones.

    Cad cad; //!< Entidades de tipo CAD: puntos, lineas,...

    MapSet sets; //!< Conjuntos de entidades.

    friend class Cad;
    friend class SetMeshComp;
    friend class Set;

    Preprocessor(const Preprocessor &otro);
    Preprocessor &operator=(const Preprocessor &otro);
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class EntMdlr;
    friend class MapSet;
    friend class SetEstruct;
    friend class ProtoElementLoader;
    friend class ElementLoader;
    friend class ConstraintLoader;
    friend class ProblemaEF;
    void UpdateSets(Element *);
    void UpdateSets(Constraint *);

    SetEstruct *busca_set_estruct(const std::string &nmb);
  public:
    Preprocessor(EntCmd *owr,DataOutputHandler::map_output_handlers *oh= NULL);
    inline Domain *GetDominio(void)
      { return dominio; }
    inline const Domain *GetDominio(void) const
      { return dominio; }
    FE_Datastore *getDataBase(void);

    void UpdateSets(Node *);

    MapSet &get_sets(void)
      { return sets; }
    const MapSet &get_sets(void) const
      { return sets; }
    MaterialLoader &getMaterialLoader(void)
      { return materiales; }
    const MaterialLoader &getMaterialLoader(void) const
      { return materiales; }
    inline NodeLoader &getNodeLoader(void)
      { return nodos; }
    inline const NodeLoader &getNodeLoader(void) const
      { return nodos; }
    ElementLoader &getElementLoader(void)
      { return elementos; }
    const ElementLoader &getElementLoader(void) const
      { return elementos; }
    LoadLoader &getLoadLoader(void)
      { return loads; }
    const LoadLoader &getLoadLoader(void) const
      { return loads; }
    ConstraintLoader &getConstraintLoader(void)
      { return constraints; }
    const ConstraintLoader &getConstraintLoader(void) const
      { return constraints; }
    TransfCooLoader &getTransfCooLoader(void)
      { return transf; }
    const TransfCooLoader &getTransfCooLoader(void) const
      { return transf; }
    BeamIntegratorLoader &getBeamIntegratorLoader(void)
      { return beamIntegrators; }
    const BeamIntegratorLoader &getBeamIntegratorLoader(void) const
      { return beamIntegrators; }

    const Cad &getCad(void) const
      { return cad; }
    Cad &getCad(void)
      { return cad; }

    void resetLoadCase(void);
    void clearAll(void);

    static void setDeadSRF(const double &);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);


    virtual ~Preprocessor(void);
  };
} // end of XC namespace

#endif
