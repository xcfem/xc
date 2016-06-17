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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.33 $
// $Date: 2005/12/22 00:35:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/domain/Domain.cpp,v $

// Written: fmk
//
// Purpose: This file contains the class definition for Domain
// Domain is a container class. The class is responsible for holding
// and providing access to the Elements, Nodes, LoadCases, SP_Constraints
// and MP_Constraints. These objects are all added to the Domain by a
// ModelBuilder.
//
// What: "@(#) Domain.C, revA"


#include "Domain.h"
#include "../../ProblemaEF.h"
#include <cstdlib>
#include "utility/recorder/Recorder.h"

#include "domain/mesh/element/ElementIter.h"
#include "domain/mesh/node/NodeIter.h"
#include <domain/load/ElementalLoadIter.h>
#include <domain/load/NodalLoadIter.h>
#include <domain/mesh/element/Element.h>
#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFD.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/SP_Constraint.h>
#include <domain/constraints/MP_Constraint.h>
#include <domain/constraints/MRMP_Constraint.h>
#include <domain/load/NodalLoad.h>
#include <domain/load/ElementalLoad.h>
#include <domain/load/pattern/LoadPattern.h>
#include <domain/load/pattern/Combinacion.h>

#include <utility/tagged/storage/MapOfTaggedObjects.h>
#include <utility/tagged/storage/MapOfTaggedObjectsIter.h>

#include <domain/domain/single/SingleDomEleIter.h>
#include <domain/domain/single/SingleDomNodIter.h>
#include <domain/domain/single/SingleDomSP_Iter.h>
#include <domain/domain/single/SingleDomMP_Iter.h>
#include <domain/domain/single/SingleDomMRMP_Iter.h>
#include <domain/load/pattern/LoadPatternIter.h>
#include <domain/load/pattern/NodeLockerIter.h>
#include <domain/domain/single/SingleDomAllSP_Iter.h>

#include <solution/graph/graph/Vertex.h>
#include "solution/graph/graph/Graph.h"
#include "domain/mesh/region/MeshRegion.h"
#include "domain/mesh/region/DqMeshRegion.h"
#include <solution/analysis/analysis/Analysis.h>
#include "utility/database/FE_Datastore.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"

#include "preprocessor/Preprocessor.h"


#include "utility/actor/actor/ArrayCommMetaData.h"


void XC::Domain::libera(void)
  {
    //delete the objects in the domain
    clearAll();
  }

//! @brief Constructor.
XC::Domain::Domain(EntCmd *owr,DataOutputHandler::map_output_handlers *oh)
  :ObjWithRecorders(owr,oh),timeTracker(),CallbackCommit(""), dbTag(0), currentGeoTag(0),
   hasDomainChangedFlag(false), commitTag(0), mesh(this), constraints(this),
   theRegions(nullptr), nmbCombActual(""), lastChannel(0), lastGeoSendTag(-1) {}

//! @brief Constructor.
XC::Domain::Domain(EntCmd *owr,int numNodes, int numElements, int numSPs, int numMPs, int numLoadPatterns,int numNodeLockers,DataOutputHandler::map_output_handlers *oh)
  :ObjWithRecorders(owr,oh),timeTracker(), CallbackCommit(""), dbTag(0), currentGeoTag(0),
   hasDomainChangedFlag(false), commitTag(0), mesh(this), constraints(this,numSPs,numMPs,numNodeLockers,numLoadPatterns),
  theRegions(nullptr), nmbCombActual(""), lastChannel(0), lastGeoSendTag(-1) {}

//! @brief Removes all components from domain (nodes, elements, loads & constraints).
//! GENERAL NOTE ON REMOVAL OF COMPONENTS:
//!   downward casts (while bad) are o.k. as only the type
//!  of components can be added to the storage objects, e.g.
//!  only elements can be added to theElements therefore
//!  casting a XC::DomainComponent from theElements to an XC::Element is o.k.
void XC::Domain::clearAll(void)
  {
    constraints.clearAll();
    
    // clean out the containers
    mesh.clearAll();

    if(theRegions)
      {
        theRegions->clearAll();
        delete theRegions;
        theRegions= nullptr;
      }
    nmbCombActual= "";

    // set the time back to 0.0
    timeTracker.Zero();

    // rest the flag to be as initial
    hasDomainChangedFlag = false;

    currentGeoTag = 0;
    lastGeoSendTag = -1;
    lastChannel = 0;
  }

//! @brief Destructor.
//!
//! destructor, this calls delete on all components of the model,
//! i.e. calls delete on all that is added to the model.
//! WARNING: if 3rd constructor, TaggedObjectStorage objects passed
//! must have been created with new and nowhere else must the
//! destructor be called.
XC::Domain::~Domain(void)
  { libera(); }

//! @brief Prepara el dominio para resolver un nuevo caso
//! de carga.
void XC::Domain::resetLoadCase(void)
  {
    nmbCombActual= "";
    revertToStart();
    setCommitTag(0);
    timeTracker.Zero();
  }

//! @brief Assigns Stress Reduction Factor for element deactivation.
void XC::Domain::setDeadSRF(const double &d)
  { Element::setDeadSRF(d); }


//! @brief Agrega al dominio el elemento being passed as parameter.
bool XC::Domain::addElement(Element *element)
  { return mesh.addElement(element); }

//! @brief Agrega al dominio el nodo being passed as parameter.
bool XC::Domain::addNode(Node * node)
  { return mesh.addNode(node); }

//! @brief Agrega al dominio una constraint monopunto.
bool XC::Domain::addSP_Constraint(SP_Constraint *spConstraint)
  {
    bool result= constraints.addSP_Constraint(spConstraint);
    if(result)
      {
        spConstraint->setDomain(this);
        this->domainChange();
      }
    return true;
  }

//! @brief Agrega al dominio una constraint multipunto.
bool XC::Domain::addMP_Constraint(MP_Constraint *mpConstraint)
  {
    bool result= constraints.addMP_Constraint(mpConstraint);
    if(result)
      {
        mpConstraint->setDomain(this);
        this->domainChange();
      }
    return result;
  }

//! @brief Agrega al dominio una constraint multi retained node.
bool XC::Domain::addMRMP_Constraint(MRMP_Constraint *mrmpConstraint)
  {
    bool result= constraints.addMRMP_Constraint(mrmpConstraint);
    if(result)
      {
        mrmpConstraint->setDomain(this);
        this->domainChange();
      }
    return result;
  }

//! @brief Agrega al dominio una constraint monopunto.
bool XC::Domain::addSP_Constraint(SP_Constraint *spConstraint, int pattern)
  {
    bool result= constraints.addSP_Constraint(spConstraint,pattern);
    if(!result)
      {
        std::cerr << "Domain::addSP_Constraint - " << pattern
                  << " pattern could not add the XC::SP_Constraint\n";
        return false;
      }

    spConstraint->setDomain(this);
    this->domainChange();
    return true;
  }

//! @brief Agrega al caso being passed as parameter una carga sobre nodos.
bool XC::Domain::addNodalLoad(NodalLoad *load, int pattern)
  {
    bool result= constraints.addNodalLoad(load,pattern);
    if(result)
      {
        load->setDomain(this); // done in LoadPattern::addNodalLoad()
        this->domainChange();
      }
    return result;
  }

//! @brief Agrega al caso being passed as parameter una carga sobre elementos.
bool XC::Domain::addElementalLoad(ElementalLoad *load, int pattern)
  {
    bool result= constraints.addElementalLoad(load,pattern);
    if(result == false)
      {
        std::cerr << "Domain::addElementalLoad() - no pattern with tag " <<
        pattern << " in  the model, not adding the ele load" << *load << std::endl;
        return false;
      }

    // load->setDomain(this); // done in LoadPattern::addElementalLoad()
    this->domainChange();
    return result;
  }

//! @brief Borra el elemento cuyo identificador se pasa como parámetro.
bool XC::Domain::removeElement(int tag)
  { return mesh.removeElement(tag); }

//! @brief Elimina del dominio el nodo cuyo tag se pasa como parámetro.
bool XC::Domain::removeNode(int tag)
  { return mesh.removeNode(tag); }

bool XC::Domain::removeSP_Constraint(int theNode, int theDOF, int loadPatternTag)
  {
    bool retval= constraints.removeSP_Constraint(theNode,theDOF,loadPatternTag);
    if(retval)
      domainChange();
    return retval;
  }

//! @brief Elimina del dominio la constraint monopunto cuyo tag se pasa como parámetro.
bool XC::Domain::removeSP_Constraint(int tag)
  {
    bool retval= constraints.removeSP_Constraint(tag);
    if(retval)
      domainChange();
    return retval;
  }

//! @brief Elimina del dominio la constraint multipunto cuyo tag se pasa como parámetro.
bool XC::Domain::removeMP_Constraint(int tag)
  {
    bool result = constraints.removeMP_Constraint(tag);
    if(result)
      domainChange();
    return result;
  }


//! @brief Elimina del dominio la constraint multi retained node cuyo tag se pasa como parámetro.
bool XC::Domain::removeMRMP_Constraint(int tag)
  {
    bool result = constraints.removeMRMP_Constraint(tag);
    if(result)
      domainChange();
    return result;
  }

//! @brief Añade al modelo la hipótesis simple being passed as parameter.
bool XC::Domain::addLoadPattern(LoadPattern *load)
  {
    bool result= constraints.addLoadPattern(load);
    if(result)
      {
        load->setDomain(this);
        domainChange();
      }
    else
      {
        if(verborrea>3)
          std::cerr << "Domain::addLoadPattern; no se pudo agregar la acción de tag: "
                    << load->getTag() << "'\n";
      }
    return result;
  }

//! @brief Añade al modelo 
bool XC::Domain::addNodeLocker(NodeLocker *nl)
  {
    bool result= constraints.addNodeLocker(nl);
    if(result)
      {
        nl->setDomain(this);
        domainChange();
      }
    return result;
  }

//! @brief Añade al modelo la combinacion being passed as parameter.
bool XC::Domain::addCombinacion(Combinacion *comb)
  {      
    bool retval= false;
    if(comb)
      {
        if(nmbCombActual!= "")
	  std::clog << "Domain::addCombinacion; ¡ojo! "
                    << "se agrega la combinación: " << comb->getNombre()
                    << " sin haber quitado la combinación: " << nmbCombActual
                    << ".\n";
        nmbCombActual= comb->getNombre();
        if(comb->getDomain()!=this)
          comb->setDomain(this);
        retval= comb->addToDomain();
      }
    return retval;
  }

//! @brief Elimina del dominio el caso de carga cuyo tag se pasa como parámetro.
bool XC::Domain::removeLoadPattern(int tag)
  {
    int numSPs= 0;
    bool result= constraints.removeLoadPattern(tag,numSPs);
    if(result)
      {
        // mark the domain has having changed if numSPs > 0
        // as the constraint handlers have to be redone
        if(numSPs>0)
          domainChange();
      }
    // finally return the load pattern
    return result;
  }

//! @brief Elimina del dominio el 
bool XC::Domain::removeNodeLocker(int tag)
  {
    int numSPs= 0;
    bool result= constraints.removeNodeLocker(tag,numSPs);
    if(result)
      {
        // mark the domain has having changed if numSPs > 0
        // as the constraint handlers have to be redone
        if(numSPs>0)
          domainChange();
      }
    // finally return the node locker
    return result;
  }

//! @brief Elimina del dominio el caso de carga being passed as parameter.
bool XC::Domain::removeLoadPattern(LoadPattern *lp)
  {
    bool retval= false;
    if(lp)
      retval= removeLoadPattern(lp->getTag());
    return retval;
  }

//! @brief Elimina del dominio el caso de carga being passed as parameter.
bool XC::Domain::removeNodeLocker(NodeLocker *nl)
  {
    bool retval= false;
    if(nl)
      retval= removeNodeLocker(nl->getTag());
    return retval;
  }

//! @brief Devuelve el nombre de la combinación actual.
const std::string &XC::Domain::getNombreCombActual(void) const
  { return nmbCombActual; }

//! @brief Elimina del dominio la combinación being passed as parameter.
void XC::Domain::removeCombinacion(Combinacion *comb)
  {
    if(comb)
      {
        if((nmbCombActual!= comb->getNombre())&& (!nmbCombActual.empty()))
	  std::clog << "Domain::removeCombinacion; ¡ojo! "
                    << "se quita la combinación: " << comb->getNombre()
                    << " sin haber quitado la combinación: " << nmbCombActual
                    << ".\n";
        nmbCombActual= "";
        if(comb->getDomain()!=this)
          comb->setDomain(this);
        comb->removeFromDomain();
      }
  }

//! @brief Elimina del dominio todos los casos de carga.
void XC::Domain::removeLPs(void)
  {
    int numSPs= constraints.removeLPs();
    // mark the domain has having changed if numSPs > 0
    // as the constraint handlers have to be redone
    if(numSPs>0)
      domainChange();
  }

//! @brief Elimina del dominio todos los bloqueos de nodos.
void XC::Domain::removeNLs(void)
  {
    int numSPs= constraints.removeNLs();
    // mark the domain has having changed if numSPs > 0
    // as the constraint handlers have to be redone
    if(numSPs>0)
      domainChange();
  }

//! @brief Elimina del dominio la carga sobre nodo being passed as parameter.
//!
//! @param tag: Identificador de la carga sobre nodo a eliminar.
//! @param loadPattern: Puntero a la hipótesis a la que pertenece la carga.
bool XC::Domain::removeNodalLoad(int tag, int loadPattern)
  { return constraints.removeNodalLoad(tag,loadPattern); }


//! @brief Elimina del dominio la carga sobre elementos being passed as parameter.
//!
//! @param tag: Identificador de la carga sobre elementos a eliminar.
//! @param loadPattern: Puntero a la hipótesis a la que pertenece la carga.
bool XC::Domain::removeElementalLoad(int tag, int loadPattern)
  { return constraints.removeElementalLoad(tag,loadPattern); }

//! @brief Elimina del dominio la constraint monopunto being passed as parameter.
//!
//! @param tag: Identificador de la constraint monopunto a eliminar.
//! @param loadPattern: Puntero a la hipótesis a la que pertenece la carga.
bool XC::Domain::removeSP_Constraint(int tag, int loadPattern)
  {
    bool removed= constraints.removeSP_Constraint(tag,loadPattern);
    if(removed)
      this->domainChange();
    return removed;
  }

//! @brief Anula los punteros a grupo de grados de libertad de los nodos.
void XC::Domain::clearDOF_GroupPtr(void)
  { mesh.clearDOF_GroupPtr(); }

//! @brief Devuelve un iterador a los elementos del dominio.
XC::ElementIter &XC::Domain::getElements()
  { return mesh.getElements(); }

//! @brief Devuelve un iterador a los nodos del dominio.
XC::NodeIter &XC::Domain::getNodes()
  { return mesh.getNodes(); }

//! @brief Devuelve la malla del dominio.
const XC::Mesh &XC::Domain::getMesh(void) const
  { return mesh; }

//! @brief Devuelve la malla del dominio.
XC::Mesh &XC::Domain::getMesh(void)
  { return mesh; }

//! @brief Returns domain constraints.
const XC::ConstrContainer &XC::Domain::getConstraints(void) const
  { return constraints; }

//! @brief Returns domain constraints.
XC::ConstrContainer &XC::Domain::getConstraints(void)
  { return constraints; }


/* GENERAL NOTE ON RETRIEVAL OF COMPONENT PTRs:
**   downward casts (while bad) are o.k. as only the type
**   of components can be added to the storage objects, e.g.
**   only elements can be added to theElements
*/

//! @brief Devuelve verdadero si el elemento cuyo tag se pasa como parámetro
//! ya existe en el dominio.
bool XC::Domain::existElement(int tag)
 { return mesh.existElement(tag); }

//! @brief Devuelve un puntero al elemento cuyo tag se pasa como parámetro.
XC::Element *XC::Domain::getElement(int tag)
  { return mesh.getElement(tag); }

//! @brief Devuelve un puntero al elemento cuyo tag se pasa como parámetro.
const XC::Element *XC::Domain::getElement(int tag) const
  { return mesh.getElement(tag); }


//! @brief Devuelve verdadero si el nodo cuyo tag se pasa como parámetro
//! ya existe en la malla.
bool XC::Domain::existNode(int tag)
 { return mesh.existNode(tag); }


//! @brief Devuelve un puntero al nodo cuyo tag se pasa como parámetro.
XC::Node *XC::Domain::getNode(int tag)
  { return mesh.getNode(tag); }

//! @brief Devuelve un puntero al nodo cuyo tag se pasa como parámetro.
const XC::Node *XC::Domain::getNode(int tag) const
  { return mesh.getNode(tag); }

int XC::Domain::getCommitTag(void) const
  { return commitTag; }

//! @brief Devuelve el número de elementos.
int XC::Domain::getNumElements(void) const
  { return mesh.getNumElements(); }

//! @brief Devuelve el número de nodos.
int XC::Domain::getNumNodes(void) const
  { return mesh.getNumNodes(); }

//! @brief Devuelve el BND del modelo.
const XC::Vector &XC::Domain::getPhysicalBounds(void)
  { return mesh.getPhysicalBounds(); }

//! @brief Construye (si es necesario) el grafo de elementos del dominio y devuelve una referencia al mismo.
XC::Graph  &XC::Domain::getElementGraph(void)
  { return mesh.getElementGraph(); }

//! @brief Construye (si es necesario) el grafo de nodos del dominio y devuelve una referencia al mismo.
XC::Graph &XC::Domain::getNodeGraph(void)
  { return mesh.getNodeGraph(); }

void XC::Domain::setCommitTag(int newTag)
  { commitTag = newTag; }

void XC::Domain::setCurrentTime(double newTime)
  { timeTracker.setCurrentTime(newTime); }

void XC::Domain::setCommittedTime(double newTime)
  { timeTracker.setCommittedTime(newTime); }

void XC::Domain::setTime(double newTime)
  {
    setCurrentTime(newTime);
    setCommittedTime(newTime);
  }

void XC::Domain::applyLoad(double timeStep)
  {
    // set the current pseudo time in the domain to be newTime
    setCurrentTime(timeStep);

    // first loop over nodes and elements getting them to first zero their loads
    mesh.zeroLoads();

    constraints.applyLoad(timeStep);
  }

//! @brief Establece como constantes todas las cargas del dominio.
void XC::Domain::setLoadConstant(void)
  { constraints.setLoadConstant(); }

//! @brief Inicializa.
int XC::Domain::initialize(void)
  { return mesh.initialize(); }

//! @brief Asigna valores a los coeficientes de amortiguamiento de Rayleigh.
int XC::Domain::setRayleighDampingFactors(const RayleighDampingFactors &rF)
  { return mesh.setRayleighDampingFactors(rF); }

//! @brief Consuma el estado del modelo y llama al método record de todos los recorders definidos.
int XC::Domain::commit(void)
  {
    //
    // first invoke commit on all nodes and elements in the domain
    //
    mesh.commit();

    // set the new committed time in the domain
    setCommittedTime(timeTracker.getCurrentTime());

    // if(!CallbackCommit.empty())
    //   EjecutaBloque(CallbackCommit,"domain:callback_commit");

    ObjWithRecorders::record(commitTag,timeTracker.getCurrentTime()); //Llama al método record de todos los recorders.

    // update the commitTag
    commitTag++;
    return 0;
  }

//! @brief Devuelve el estado del modelo al último consumado.
int XC::Domain::revertToLastCommit(void)
  {
    //
    // first invoke revertToLastCommit  on all nodes and elements in the domain
    //
    mesh.revertToLastCommit();

    // set the current time and load factor in the domain to last committed
    setCurrentTime(timeTracker.getCommittedTime());

    // apply load for the last committed time
    applyLoad(timeTracker.getCurrentTime());

    return update();
  }

//! @brief Devuelve el estado del modelo al inicial del cálculo y llama
//! al método restart() de todos los recorders.
int XC::Domain::revertToStart(void)
  {
    //
    // first invoke revertToStart  on all nodes and
    // elements in the domain
    //
    mesh.revertToStart();

// ADDED BY TERJE //////////////////////////////////
    // invoke 'restart' on all recorders
    ObjWithRecorders::restart();
/////////////////////////////////////////////////////

    // set the current time and load factor in the domain to last committed
    timeTracker.Zero();

    // apply load for the last committed time
    applyLoad(timeTracker.getCurrentTime());

    return update();
  }

//! @brief Actualiza el estado de los elementos del modelo.
int XC::Domain::update(void)
  {
    // set the global constants
    ProblemaEF::theActiveDomain= this;
    return mesh.update();
  }

//! @brief Actualiza el estado del dominio.
int XC::Domain::update(double newTime, double dT)
  {
    applyLoad(newTime);
    update();
    return 0;
  }

int XC::Domain::newStep(double dT)
  { return 0; }


//! @brief Establece el valor del vector de autovalores.
int XC::Domain::setEigenvalues(const Vector &theValues)
  {
    theEigenvalues= theValues;

    // now set the time at which eigen values
    // were determined to be current domain time
    timeTracker.setEigenvalueTime();
    return 0;
  }

//! @brief Devuelve el autovalor que corresponde al modo being passed as parameter.
const double &XC::Domain::getEigenvalue(int mode) const
  { return theEigenvalues(mode-1); }

//! @brief Devuelve la pulsación correspondiente al modo
//! being passed as parameter.
double XC::Domain::getPulsacion(int mode) const
  { return sqrt(getEigenvalue(mode)); }

//! @brief Devuelve el período correspondiente al modo
//! being passed as parameter.
double XC::Domain::getPeriodo(int mode) const
  { return 2.0*M_PI/getPulsacion(mode); }

//! @brief Devuelve la frecuencia correspondiente al modo
//! being passed as parameter.
double XC::Domain::getFrecuencia(int mode) const
  { return 1./getPeriodo(mode); }

//! @brief Devuelve el vector de autovalores.
const XC::Vector &XC::Domain::getEigenvalues(void) const
  { return theEigenvalues; }

//! @brief Devuelve un vector con las pulsaciones calculadas.
XC::Vector XC::Domain::getPulsaciones(void) const
  {
    Vector retval= getEigenvalues();
    const int dim= retval.Size();
    for(int i= 0;i<dim;i++)
      retval[i]= sqrt(retval(i));
    return retval;
  }

//! @brief Devuelve un vector con las periodos calculados.
XC::Vector XC::Domain::getPeriodos(void) const
  {
    Vector retval= getPulsaciones();
    const int dim= retval.Size();
    for(int i= 0;i<dim;i++)
      retval[i]= 2.0*M_PI/retval(i);
    return retval;
  }

//! @brief Devuelve un vector con las frecuencias calculadas.
XC::Vector XC::Domain::getFrecuencias(void) const
  {
    Vector retval= getPeriodos();
    const int dim= retval.Size();
    for(int i= 0;i<dim;i++)
      retval[i]= 1.0/retval(i);
    return retval;
  }

//! @brief Devuelve el número de autovalores que se han calculado.
int XC::Domain::getNumModes(void) const
  { return getEigenvalues().Size(); }

//! @brief Establece el valor del vector de autovalores.
int XC::Domain::setModalParticipationFactors(const Vector &theValues)
  {
    modalParticipationFactors= theValues;
    return 0;
  }

//! @brief Devuelve el factor de participación modal
//! que corresponde al modo being passed as parameter.
const double &XC::Domain::getModalParticipationFactor(int mode) const
  { return modalParticipationFactors(mode-1); }

//! @brief Devuelve el vector de factores de participación modal.
const XC::Vector &XC::Domain::getModalParticipationFactors(void) const
  { return modalParticipationFactors; }

//! @brief Devuelve la masa modal efectiva correspondiente
//! al modo being passed as parameter.
const double XC::Domain::getEffectiveModalMass(int mode) const
  { return mesh.getEffectiveModalMass(mode); }

//! @brief Devuelve las masas modales efectivas en cada modo.
XC::Vector XC::Domain::getEffectiveModalMasses(void) const
  {
    const int nm= getNumModes();
    return mesh.getEffectiveModalMasses(nm);
  }

//! @brief Devuelve la masa total del nodo.
double XC::Domain::getTotalMass(void) const
  {
    double retval= 0;
    const Vector masas= getEffectiveModalMasses();
    const size_t sz= masas.Size();
    for(size_t i=0;i<sz;i++)
      retval+= masas(i);
    return retval;
  }

void XC::Domain::setDomainChangeStamp(int newStamp)
  { currentGeoTag= newStamp; }


//! @brief Establece que el modelo ha cambiado.
void XC::Domain::domainChange(void)
  { hasDomainChangedFlag= true; }

//! @brief Devuelve verdadero si el modelo ha cambiado.
int XC::Domain::hasDomainChanged(void)
  {
    // if the flag indicating the domain has changed since the
    // last call to this method has changed, increment the integer
    // and reset the flag
    bool result = hasDomainChangedFlag;
    hasDomainChangedFlag = false;
    if(result)
      {
        currentGeoTag++;
        mesh.setGraphBuiltFlags(false);
      }
    // return the integer so user can determine if domain has changed
    // since their last call to this method
    return currentGeoTag;
  }

//! @brief Imprime el dominio.
void XC::Domain::Print(std::ostream &s, int flag)
  {

    s << "Current Domain Information\n";
    s << timeTracker << std::endl;

    mesh.Print(s, flag);
    constraints.Print(s,flag);
  }

std::ostream &XC::operator<<(std::ostream &s, XC::Domain &M)
  {
    M.Print(s);
    return s;
  }

//! @brief Agrega un recorder al modelo.
int XC::Domain::addRecorder(Recorder &theRecorder)
  {
    if(theRecorder.setDomain(*this) != 0)
      {
        std::cerr << "Domain::addRecorder() - recorder could not be added\n";
        return -1;
      }
    else
      ObjWithRecorders::addRecorder(theRecorder);
    return 0;
  }

//! @brief Agrega una región.
int XC::Domain::addRegion(MeshRegion &theRegion)
  {
    if(!theRegions)
      theRegions= new DqMeshRegion();
    theRegions->add(theRegion);
    return 0;
  }

//! @brief Devuelve un puntero a la región cuyo tag se pasa como parámetro.
XC::MeshRegion *XC::Domain::getRegion(int tag)
  {
    MeshRegion *retval= nullptr;
    if(theRegions)
      retval= theRegions->getRegion(tag);
    return retval;
  }

//! @brief Construye el grafo de elementos.
int XC::Domain::buildEleGraph(Graph &theEleGraph)
  { return mesh.buildEleGraph(theEleGraph); }

//! @brief Construye el grafo de nodos.
int XC::Domain::buildNodeGraph(Graph &theNodeGraph)
  { return mesh.buildNodeGraph(theNodeGraph); }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::Domain::getDbTagData(void) const
  {
    static DbTagData retval(9);
    return retval;
  }

//! @brief Envía los datos through the channel being passed as parameter.
int XC::Domain::sendData(CommParameters &cp)
  {
    int res= ObjWithRecorders::sendData(cp);
    res+= cp.sendMovable(timeTracker,getDbTagData(),CommMetaData(2));
    res+= cp.sendInt(currentGeoTag,getDbTagData(),CommMetaData(3));
    const int tagChannel= cp.getChannel()->getTag();
    if(lastChannel != tagChannel || lastGeoSendTag != currentGeoTag)
      {
        lastChannel= tagChannel;
        //
        // into an ID we are gonna place the class and db tags for each node so can rebuild
        // this ID we then send to the channel
        //
        lastGeoSendTag= currentGeoTag;// now so that we don't do this next time if nothing in the domain has changed
      }
    res+= cp.sendMovable(mesh,getDbTagData(),CommMetaData(4));
    res+= cp.sendMovable(constraints,getDbTagData(),CommMetaData(5));
    res+= cp.sendVector(theEigenvalues,getDbTagData(),CommMetaData(6));
    res+= cp.sendVector(modalParticipationFactors,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Recibe los datos through the channel being passed as parameter.
int XC::Domain::recvData(const CommParameters &cp)
  {
    int res= ObjWithRecorders::recvData(cp);
    res+= cp.receiveMovable(timeTracker,getDbTagData(),CommMetaData(2));
    //
    // now if the currentGeoTag does not agree with whats in the domain
    // we must wipe everything in the domain and recreate the domain based on the info from the channel
    //
    const int tagChannel= cp.getChannel()->getTag();
    const int geoTag= getDbTagDataPos(3);
    if(currentGeoTag == 0 || lastChannel != tagChannel  || geoTag != currentGeoTag)
      {
        // set the currrentGeoTag
        lastChannel= tagChannel;
  
        // if receiving set lastGeoSendTag to be equal to currentGeoTag
        // at time all the data was sent if not we must clear out the objects and rebuild
        lastGeoSendTag= geoTag; currentGeoTag= geoTag;
  
        // mark domainChangeFlag as false
        // this way if restoring froma a database and domain has not changed for the analysis
        // the analysis will not have to to do a domainChanged() operation
        hasDomainChangedFlag= false;
      }

    res+= cp.receiveMovable(mesh,getDbTagData(),CommMetaData(4));
    res+= cp.receiveMovable(constraints,getDbTagData(),CommMetaData(5));
    // now set the domains lastGeoSendTag and currentDomainChangedFlag
    lastGeoSendTag = currentGeoTag;
    res+= cp.receiveVector(theEigenvalues,getDbTagData(),CommMetaData(6));
    res+= cp.receiveVector(modalParticipationFactors,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Domain::sendSelf(CommParameters &cp)
  {
    // update the commitTag and currentGeoTag
    commitTag= cp.getCommitTag();
    hasDomainChanged();

    inicComm(9);
    int retval= sendData(cp);

    if(dbTag==0)
      dbTag = cp.getChannel()->getDbTag();
    retval+= cp.sendIdData(getDbTagData(),dbTag);
    if(retval < 0)
      std::cerr << "Domain::sendSelf() - ch failed to send data.\n";
    return retval;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::Domain::recvSelf(const CommParameters &cp)
  {
    // set the commitTag in the domain to cTag & update the getTag if needed
    commitTag= cp.getCommitTag();
    hasDomainChanged();
  
    // first we get the data about the state of the domain for this commitTag
    inicComm(9);

    int retval= cp.receiveIdData(getDbTagData(),dbTag);
    if(retval < 0)
      std::cerr << "Domain::recvSelf() - ch failed to recv the initial ID\n";
    else
      retval+= recvData(cp);
    if(retval<0)
      std::cerr << "Domain::recvSelf() - data could not be received.\n" ;
    return retval;
  }
  
//! @brief Devuelve el desplazamiento en el grado de libertad dof del nodo cuyo tag se pasa como parámetro.
double XC::Domain::getNodeDisp(int nodeTag, int dof,int &errorFlag)
  { return mesh.getNodeDisp(nodeTag,dof,errorFlag); }

//! @brief Asigna la matriz de masas al nudo cuyo tag se pasa como parámetro.
int XC::Domain::setMass(const XC::Matrix &mass, int nodeTag)
  { return mesh.setMass(mass,nodeTag); }

//! @brief Asigna la excepción para comprobación de reacciones (ver Domain::checkNodalReactions).
void XC::Domain::setNodeReactionException(const int &n)
  { mesh.setNodeReactionException(n); }

void XC::Domain::checkNodalReactions(const double &tol)
  { mesh.checkNodalReactions(tol); }

//! @brief Calcula las reacciones en los nodos del dominio.
int XC::Domain::calculateNodalReactions(bool inclInertia,const double &tol)
  {
    int retval= mesh.calculateNodalReactions(inclInertia,tol);
    retval+= constraints.calculateNodalReactions(inclInertia,tol);
    return retval;
  }

//! @brief Devuelve (si puede) un puntero al preprocesador.
const XC::Preprocessor *XC::Domain::GetPreprocessor(void) const
  {
    const Preprocessor *retval= nullptr;
    const EntProp *owr= Owner();
    if(owr)
      retval= dynamic_cast<const Preprocessor *>(owr);
    return retval;
  }

//! @brief Devuelve (si puede) un puntero al preprocesador.
XC::Preprocessor *XC::Domain::GetPreprocessor(void)
  {
    Preprocessor *retval= nullptr;
    EntProp *owr= Owner();
    if(owr)
      retval= dynamic_cast<Preprocessor *>(owr);
    return retval;
  }

//! @brief Envía el dominio through the channelespecificado.
int XC::sendDomain(Domain &dom,int posDbTag,DbTagData &dt,CommParameters &cp)
  {
    int retval= dom.sendSelf(cp);
    retval+= cp.sendInt(dom.dbTag,dt,CommMetaData(posDbTag));
    return retval;
  }

//! @brief Recibe el dominio through the channelespecificado.
int XC::receiveDomain(Domain &dom,int posDbTag,DbTagData &dt,const CommParameters &cp)
  {
    int res= cp.receiveInt(dom.dbTag,dt,CommMetaData(posDbTag));
    res+= dom.recvSelf(cp);
    if(res < 0)
      std::cerr << "receiveDomain - failed to receive vector data\n";
    return res;
  }
