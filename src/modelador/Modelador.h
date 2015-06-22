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
//Modelador.h
//Objeto encargado de interpretar la entrada de datos
//del preproceso y generar la malla de elementos finitos.

#ifndef MODELADOR_H
#define MODELADOR_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/actor/actor/MovableObject.h"
#include "loaders/MaterialLoader.h"
#include "loaders/BeamIntegratorLoader.h"
#include "loaders/TransfCooLoader.h"
#include "loaders/NodeLoader.h"
#include "loaders/ElementLoader.h"
#include "loaders/LoadLoader.h"
#include "loaders/ConstraintLoader.h"
#include "MapSet.h"
#include "cad/Cad.h"
#include "utility/handler/DataOutputHandler.h"


namespace XC {
class Domain;
class Constraint;
class ProblemaEF;
class FE_Datastore;

//!  @defgroup Mdlr Modelador.

//!  \ingroup Mdlr
//! 
//!  \brief Generación del modelo de elementos finitos.
//!  
class Modelador: public EntCmd, public MovableObject
  {
  private:
    static const int theDbTag= 0; //!< the Modelador unique database tag == 0

    Domain *dominio; //!< Dominio del problema.

    MaterialLoader materiales; //!< Materiales para el problema.
    TransfCooLoader transf; //!< Transformaciones de coordenadas.
    BeamIntegratorLoader integradores; //!< Integradores para elementos unidimensionales.
    NodeLoader nodos; //! Cargador de nodos.
    ElementLoader elementos; //!< Cargador de elementos.
    LoadLoader loads; //!< Cargador de cargas.
    ConstraintLoader constraints; //! Cargador de coacciones.

    Cad cad; //!< Entidades de tipo CAD: puntos, lineas,...

    MapSet sets; //!< Conjuntos de entidades.

    friend class Cad;
    friend class SetMeshComp;
    friend class Set;

    Modelador(const Modelador &otro);
    Modelador &operator=(const Modelador &otro);
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    virtual bool procesa_comando(CmdStatus &);

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
    Modelador(EntCmd *owr,DataOutputHandler::map_output_handlers *oh= NULL);
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
      { return integradores; }
    const BeamIntegratorLoader &getBeamIntegratorLoader(void) const
      { return integradores; }

    const Cad &getCad(void) const
      { return cad; }
    Cad &getCad(void)
      { return cad; }

    void resetLoadCase(void);
    void clearAll(void);

    static void setDeadSRF(const double &);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    virtual any_const_ptr GetProp(const std::string &cod) const;

    virtual ~Modelador(void);
  };
} // fin namespace XC

#endif
