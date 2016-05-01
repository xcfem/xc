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
//ConstrContainer.cc

#include <cstdlib>

#include "ConstrContainer.h"
#include <domain/domain/Domain.h>
#include <domain/load/ElementalLoadIter.h>
#include <domain/load/NodalLoadIter.h>
#include <domain/constraints/SP_Constraint.h>
#include <domain/constraints/MP_Constraint.h>
#include <domain/constraints/MRMP_Constraint.h>
#include <domain/mesh/node/Node.h>
#include "domain/mesh/element/Element.h"
#include <domain/load/NodalLoad.h>
#include <domain/load/ElementalLoad.h>
#include <domain/load/pattern/LoadPattern.h>
#include <domain/load/pattern/NodeLocker.h>

#include <utility/tagged/storage/MapOfTaggedObjects.h>
#include <utility/tagged/storage/MapOfTaggedObjectsIter.h>

#include <domain/domain/single/SingleDomSP_Iter.h>
#include <domain/domain/single/SingleDomMP_Iter.h>
#include <domain/domain/single/SingleDomMRMP_Iter.h>
#include <domain/load/pattern/LoadPatternIter.h>
#include <domain/load/pattern/NodeLockerIter.h>
#include <domain/domain/single/SingleDomAllSP_Iter.h>
#include "utility/matrix/ID.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>


#include "utility/actor/actor/MovableID.h"

#include "preprocessor/Preprocessor.h"
#include "preprocessor/loaders/LoadLoader.h"

void XC::ConstrContainer::libera(void)
  {
    //delete the objects in the domain
    clearAll();

    // delete all the storage objects
    // SEGMENT FAULT WILL OCCUR IF THESE OBJECTS WERE NOT CONSTRUCTED
    // USING NEW
    if(theSPs) delete theSPs;
    theSPs= nullptr;
    if(theMPs) delete theMPs;
    theMPs= nullptr;
    if(theSP_Iter) delete theSP_Iter;
    theSP_Iter= nullptr;
    if(theMP_Iter) delete theMP_Iter;
    theMP_Iter= nullptr;
    if(theMRMP_Iter) delete theMRMP_Iter;
    theMRMP_Iter= nullptr;
    if(allSP_Iter) delete allSP_Iter;
    allSP_Iter= nullptr;
  }

//! @brief Reserva memoria para los contenedores.
void XC::ConstrContainer::alloc_contenedores(int numSPs, int numMPs,int numNodeLockers,int numLoadPatterns)
  {
    // init the arrays for storing the constraints components
    theSPs= new MapOfTaggedObjects(this,"SPs");
    theMPs= new MapOfTaggedObjects(this,"MPs");
    theMRMPs= new MapOfTaggedObjects(this,"MRMPs");
  }

//! @brief Reserva memoria para los iteradores.
void XC::ConstrContainer::alloc_iters(void)
  {
    // init the iters
    theSP_Iter= new SingleDomSP_Iter(theSPs);
    theMP_Iter= new SingleDomMP_Iter(theMPs);
    theMRMP_Iter= new SingleDomMRMP_Iter(theMRMPs);
    allSP_Iter= new SingleDomAllSP_Iter(*getDomain());
  }

//! @brief Comprueba que se ha podido reservar memoria para los contenedores.
bool XC::ConstrContainer::check_contenedores(void) const
  {
    // check that there was space to create the data structures
    if((theSPs == 0) || (theMPs == 0) || (theMRMPs == 0) ||
       (theMP_Iter == 0) || (theSP_Iter == 0) || (theMRMP_Iter == 0))
      {
        std::cerr << "ConstrContainer::ConstrContainer() - out of memory\n";
        return false;
      }
    else
      return true;
  }

//! @brief Constructor.
XC::ConstrContainer::ConstrContainer(Domain *owr)
  :MeshComponentContainer(owr,DOMAIN_TAG_ConstrContainer), activeNodeLockers(this), activeLoadPatterns(this)
  {
    // init the arrays for storing the domain components
    alloc_contenedores();
    alloc_iters();
    if(!check_contenedores())
      { exit(-1); }
  }

//! @brief Constructor.
XC::ConstrContainer::ConstrContainer(Domain *owr,int numSPs, int numMPs, int numNodeLockers, int numLoadPatterns)
  :MeshComponentContainer(owr,DOMAIN_TAG_ConstrContainer), activeNodeLockers(this), activeLoadPatterns(this)
  {
    alloc_contenedores(numSPs,numMPs,numNodeLockers,numLoadPatterns);
    alloc_iters();
    check_contenedores();
  }

//! @brief Elimina del dominio todos los componentes
void XC::ConstrContainer::clearAll(void)
  {
    // clear the loads and constraints from any load pattern
    for(MapCasosActivos<LoadPattern>::iterator i= activeLoadPatterns.begin();
        i!= activeLoadPatterns.end();i++)
      (*i).second->clearAll();

    for(MapCasosActivos<NodeLocker>::iterator i= activeNodeLockers.begin();
        i!= activeNodeLockers.end();i++)
      (*i).second->clearAll();

    // clean out the containers
    if(theSPs) theSPs->clearAll();
    if(theMPs) theMPs->clearAll();
    activeNodeLockers.clear();
    activeLoadPatterns.clear(); //y estos en LoadLoader::clearAll
  }

//! @brief Destructor.
XC::ConstrContainer::~ConstrContainer(void)
  { libera(); }


//! @brief Agrega una constraint monopunto.
bool XC::ConstrContainer::addSP_Constraint(SP_Constraint *spConstraint)
  {
    bool retval= false;

    // check that no other object with similar tag exists in model
    const int tag= spConstraint->getTag();
    TaggedObject *other= theSPs->getComponentPtr(tag);
    if(other)
      {
        std::clog << "ConstrContainer::addSP_Constraint - cannot add a constraint with tag "
                  << tag << " already exists in model\n";
        retval= false;
      }
    else
      retval= theSPs->addComponent(spConstraint);
    return retval;
  }


//! @brief Agrega al dominio una constraint multipunto.
bool XC::ConstrContainer::addMP_Constraint(MP_Constraint *mpConstraint)
  {
    bool retval= false;

    // check that no other object with similar tag exists in model
    const int tag= mpConstraint->getTag();
    TaggedObject *other= theMPs->getComponentPtr(tag);
    if(other)
      {
        std::clog << "ConstrContainer::addMP_Constraint - cannot add as constraint with tag "
                  << tag << " already exists in model";
      }
    else
      retval= theMPs->addComponent(mpConstraint);
    return retval;
  }

//! @brief Agrega al dominio una constraint multi retained nodes.
bool XC::ConstrContainer::addMRMP_Constraint(MRMP_Constraint *mrmpConstraint)
  {
    bool retval= false;

    // check that no other object with similar tag exists in model
    const int tag= mrmpConstraint->getTag();
    TaggedObject *other= theMRMPs->getComponentPtr(tag);
    if(other)
      {
        std::clog << "ConstrContainer::addMRMP_Constraint - cannot add as constraint with tag "
                  << tag << " already exists in model";
      }
    else
      retval= theMRMPs->addComponent(mrmpConstraint);
    return retval;
  }

//! @brief Añade al modelo la hipótesis simple que se pasa como parámetro.
bool XC::ConstrContainer::addLoadPattern(LoadPattern *load)
  {
    bool retval= true;
    // first check if a load pattern with a similar tag exists in model
    const int tag= load->getTag();
    MapCasosActivos<LoadPattern>::const_iterator i= activeLoadPatterns.find(tag);
    if(i!=activeLoadPatterns.end())
      {
        if(verborrea>3)
          std::cerr << "ConstrContainer::addLoadPattern - cannot add as LoadPattern with tag "
                    << tag << " already exists in model\n";
        retval= false;
      }
    else
      activeLoadPatterns[tag]= load;// now we add the load pattern
    return retval;
  }

//! @brief Añade al modelo
bool XC::ConstrContainer::addNodeLocker(NodeLocker *nl)
  {
    bool retval= true;
    // first check if a locker with a similar tag exists in model
    const int tag= nl->getTag();
    MapCasosActivos<NodeLocker>::const_iterator i= activeNodeLockers.find(tag);
    if(i!=activeNodeLockers.end())
      {
        std::clog << "ConstrContainer::addNodeLocker - cannot add as LoadPattern with tag "
                  << tag << " already exists in model\n";
        retval= false;
      }
    else
      activeNodeLockers[tag]= nl;
    return retval;
  }

//! @brief Agraga al caso de carga una constraint monopunto.
bool XC::ConstrContainer::addSP_Constraint(SP_Constraint *spConstraint, int pattern)
  {
    bool retval= false;
    // now add it to the pattern
    LoadPattern *caso= getLoadPattern(pattern);
    if(caso)
      retval= caso->addSP_Constraint(spConstraint);
    else
      std::cerr << "ConstrContainer::addSP_Constraint - cannot add as pattern with tag "
                << pattern << " does not exist in domain\n";
    return retval;
  }

//! @brief Agrega al dominio una carga sobre nodo.
bool XC::ConstrContainer::addNodalLoad(NodalLoad *load, int pattern)
  {
    bool retval= false;
    int nodTag= load->getNodeTag();
    const Node *res= getDomain()->getNode(nodTag);
    if(res == 0)
      {
        std::cerr << "ConstrContainer::addNodalLoad() HI - no node with tag " << nodTag <<
          " exists in  the model, not adding the nodal load"  << *load << std::endl;
      }
    else
      {
        // now add it to the pattern
        LoadPattern *caso= getLoadPattern(pattern);
        if(caso)
          retval= caso->addNodalLoad(load);
        else
          {
            std::cerr << "ConstrContainer::addNodalLoad() - no pattern with tag " <<
            pattern << " in  the model, not adding the nodal load"  << *load << std::endl;
          }
      }
    return retval;
  }

//! @brief Agrega al dominio una carga sobre elementos.
bool XC::ConstrContainer::addElementalLoad(ElementalLoad *load, int pattern)
  {
    bool retval= false;
    // now add it to the pattern
    LoadPattern *caso= getLoadPattern(pattern);
    if(caso)
      retval= caso->addElementalLoad(load);
    else
      {
        std::cerr << "ConstrContainer::addElementalLoad() - no pattern with tag " << pattern <<
        "exits in  the model, not adding the ele load " << *load << std::endl;
      }
    // load->setDomain(this); // done in LoadPattern::addElementalLoad()
    //this->domainChange();
    return retval;
  }


bool XC::ConstrContainer::removeSP_Constraint(int theNode, int theDOF, int loadPatternTag)
  {
    SP_Constraint *theSP =nullptr;
    bool found= false;
    int spTag= 0;

    if(loadPatternTag == -1)
      {
        SP_ConstraintIter &theSPs= getSPs();
        while((found == false) && ((theSP= theSPs()) != 0))
          {
            int nodeTag= theSP->getNodeTag();
            int dof= theSP->getDOF_Number();
            if(nodeTag == theNode && dof == theDOF)
              {
	        spTag= theSP->getTag();
	        found= true;
              }
          }
        if(found == true)
          return this->removeSP_Constraint(spTag);
      }
    else
      {
        LoadPattern *thePattern= this->getLoadPattern(loadPatternTag);
        if(thePattern)
          {
            SP_ConstraintIter &theSPs= thePattern->getSPs();
            while((found == false) && ((theSP= theSPs()) != 0))
              {
	        int nodeTag= theSP->getNodeTag();
	        int dof= theSP->getDOF_Number();
	        if(nodeTag == theNode && dof == theDOF)
                  {
	            spTag= theSP->getTag();
	            found= true;
	          }
              }
            if(found == true)
	      return thePattern->removeSP_Constraint(spTag);
          }
      }
    return 0;
  }

//! @brief Elimina del dominio la constraint monopunto cuyo tag se pasa como parámetro.
bool XC::ConstrContainer::removeSP_Constraint(int tag)
  {
    SP_Constraint *theSP= dynamic_cast<SP_Constraint *>(theSPs->getComponentPtr(tag));
    if(theSP)
      theSP->setDomain(nullptr);
    return theSPs->removeComponent(tag);
  }

//! @brief Elimina del dominio la constraint multipunto cuyo tag se pasa como parámetro.
bool XC::ConstrContainer::removeMP_Constraint(int tag)
  {
    MP_Constraint *theMP= dynamic_cast<MP_Constraint *>(theMPs->getComponentPtr(tag));
    if(theMP)
      theMP->setDomain(nullptr);
    return theMPs->removeComponent(tag);
  }

//! @brief Elimina del dominio la constraint multi retained nodes cuyo tag se pasa como parámetro.
bool XC::ConstrContainer::removeMRMP_Constraint(int tag)
  {
    MRMP_Constraint *theMRMP= dynamic_cast<MRMP_Constraint *>(theMRMPs->getComponentPtr(tag));
    if(theMRMP)
      theMRMP->setDomain(nullptr);
    return theMRMPs->removeComponent(tag);
  }

//! @brief Elimina del dominio el caso de carga cuyo tag se pasa como parámetro.
bool XC::ConstrContainer::removeLoadPattern(int tag,int &numSPs)
  { 
    MapCasosActivos<LoadPattern>::iterator i= activeLoadPatterns.find(tag);

    bool retval= false;
    numSPs= 0;

    if(i!=activeLoadPatterns.end())
      {
        // perform a downward cast, set the objects domain pointer to 0
        // and return the result of the cast
        const LoadPattern *result= (*i).second;
        if(result)
          {
            numSPs= result->getNumSPs();
            activeLoadPatterns.erase(i);
            retval= true;
          }
        else
	  std::cerr << "error en ConstrContainer::removeLoadPattern" << std::endl;
      }
    // finally return the load pattern
    return retval;
  }

//! @brief Elimina del dominio el
bool XC::ConstrContainer::removeNodeLocker(int tag,int &numSPs)
  {
    // remove the object from the container
    MapCasosActivos<NodeLocker>::iterator i= activeNodeLockers.find(tag);

    bool retval= false;
    numSPs= 0;

    if(i!=activeNodeLockers.end())
      {
        // perform a downward cast, set the objects domain pointer to 0
        // and return the result of the cast
        const NodeLocker *result= (*i).second;
        if(result)
          {
            numSPs= result->getNumSPs();
            activeNodeLockers.erase(i);
            retval= true;
          }
        else
	  std::cerr << "error en ConstrContainer::removeNodeLocker" << std::endl;
      }
    // finally return the node locker
    return retval;
  }

int XC::ConstrContainer::removeLPs(void)
  {
    const std::deque<int> tags= getTagsLPs();
    int numSPs= 0;
    int tmp;
    for(std::deque<int>::const_iterator i= tags.begin(); i!=tags.end();i++)
      {
	removeLoadPattern(*i,tmp);
        numSPs+= tmp;
      }
    return numSPs;
  }

int XC::ConstrContainer::removeNLs(void)
  {
    const std::deque<int> tags= getTagsNLs();
    int numSPs= 0;
    int tmp;
    for(std::deque<int>::const_iterator i= tags.begin(); i!=tags.end();i++)
      {
	removeNodeLocker(*i,tmp);
        numSPs+= tmp;
      }
    return numSPs;
  }

//! @brief Elimina del dominio la carga sobre nodo que se pasa como parámetro.
//!
//! @param tag: Identificador de la carga sobre nodo a eliminar.
//! @param loadPattern: Puntero a la hipótesis a la que pertenece la carga.
bool XC::ConstrContainer::removeNodalLoad(int tag, int loadPattern)
  {
    bool retval= false;
    // remove the object from the container
    LoadPattern *theLoadPattern= getLoadPattern(loadPattern);

    // if not there return 0
    if(theLoadPattern)
      retval= theLoadPattern->removeNodalLoad(tag);
    return retval;
  }

//! @brief Elimina del dominio la carga sobre elementos que se pasa como parámetro.
//!
//! @param tag: Identificador de la carga sobre elementos a eliminar.
//! @param loadPattern: Identificador de la hipótesis a la que pertenece la carga.
bool XC::ConstrContainer::removeElementalLoad(int tag, int loadPattern)
  {
    bool retval= false;
    // remove the object from the container
    LoadPattern *theLoadPattern= this->getLoadPattern(loadPattern);
    // if not there return 0
    if(theLoadPattern)
      retval= theLoadPattern->removeElementalLoad(tag);
    return retval;
  }

//! @brief Elimina del dominio la constraint monopunto que se pasa como parámetro.
//!
//! @param tag: Identificador de la constraint monopunto a eliminar.
//! @param loadPattern: Identificador de la hipótesis a la que pertenece la carga.
bool XC::ConstrContainer::removeSP_Constraint(int tag, int loadPattern)
  {
    bool retval= false;
    LoadPattern *theLoadPattern= this->getLoadPattern(loadPattern);// remove the object from the container
    if(theLoadPattern)
      retval= theLoadPattern->removeSP_Constraint(tag);
    return retval;
  }

//! @brief Domain single point constraints iterator.
XC::SP_ConstraintIter &XC::ConstrContainer::getSPs(void)
  {
    theSP_Iter->reset();
    return *theSP_Iter;
  }

//! @brief All (domain and load cases) single point constraints iterator.
XC::SP_ConstraintIter &XC::ConstrContainer::getDomainAndLoadPatternSPs(void)
  {
    allSP_Iter->reset();
    return *allSP_Iter;
  }

//! @brief Domain multi-point constraints iterator.
XC::MP_ConstraintIter &XC::ConstrContainer::getMPs(void)
  {
    theMP_Iter->reset();
    return *theMP_Iter;
  }

//! @brief Domain multi-row multi-point constraints iterator.
XC::MRMP_ConstraintIter &XC::ConstrContainer::getMRMPs(void)
  {
    theMRMP_Iter->reset();
    return *theMRMP_Iter;
  }


//! @brief Devuelve un iterador a los casos de carga del dominio.
std::map<int,XC::LoadPattern *> &XC::ConstrContainer::getLoadPatterns(void)
  { return activeLoadPatterns; }

//! @brief Devuelve un iterador a los casos de carga del dominio.
std::map<int,XC::NodeLocker *> &XC::ConstrContainer::getNodeLockers(void)
  { return activeNodeLockers; }

//! @brief Devuelve un iterador a los casos de carga del dominio.
const std::map<int,XC::LoadPattern *> &XC::ConstrContainer::getLoadPatterns(void) const
  { return activeLoadPatterns; }

//! @brief Devuelve un iterador a los casos de carga del dominio.
const std::map<int,XC::NodeLocker *> &XC::ConstrContainer::getNodeLockers(void) const
  { return activeNodeLockers; }


/* GENERAL NOTE ON RETRIEVAL OF COMPONENT PTRs:
**   downward casts (while bad) are o.k. as only the type
**   of components can be added to the storage objects, e.g.
**   only elements can be added to theElements
*/

//! @brief Devuelve un puntero a la constraint monopunto cuyo tag se pasa como parámetro.
XC::SP_Constraint *XC::ConstrContainer::getSP_Constraint(int tag)
  {
    TaggedObject *mc= theSPs->getComponentPtr(tag);
    // if not there return 0 otherwise perform a cast and return that
    if(!mc) return nullptr;
    SP_Constraint *result= dynamic_cast<SP_Constraint *>(mc);
    return result;
  }

//! @brief Devuelve un puntero a la constraint multipunto cuyo tag se pasa como parámetro.
XC::MP_Constraint *XC::ConstrContainer::getMP_Constraint(int tag)
  {
    TaggedObject *mc= theMPs->getComponentPtr(tag);
    // if not there return 0 otherwise perform a cast and return that
    if(!mc) return nullptr;
    MP_Constraint *result= dynamic_cast<MP_Constraint *>(mc);
    return result;
  }

//! @brief Devuelve un puntero a la constraint multi retained node cuyo tag se pasa como parámetro.
XC::MRMP_Constraint *XC::ConstrContainer::getMRMP_Constraint(int tag)
  {
    TaggedObject *mc= theMRMPs->getComponentPtr(tag);
    // if not there return 0 otherwise perform a cast and return that
    if(!mc) return nullptr;
    MRMP_Constraint *result= dynamic_cast<MRMP_Constraint *>(mc);
    return result;
  }

//! @brief Devuelve un puntero al caso de carga cuyo tag se pasa como parámetro.
XC::LoadPattern *XC::ConstrContainer::getLoadPattern(const int &tag)
  {
    LoadPattern *retval= nullptr;
    MapCasosActivos<LoadPattern>::iterator i= activeLoadPatterns.find(tag);
    if(i==activeLoadPatterns.end())
      std::cerr << "ConstrContainer::getLoadPattern - no se encontró el caso de carga de tag "
                << tag << "\n";
    else
      retval= (*i).second;
    return retval;
  }

//! @brief Devuelve un puntero al caso de carga cuyo tag se pasa como parámetro.
const XC::LoadPattern *XC::ConstrContainer::getLoadPattern(const int &tag) const
  {
    const LoadPattern *retval= nullptr;
    MapCasosActivos<LoadPattern>::const_iterator i= activeLoadPatterns.find(tag);
    if(i==activeLoadPatterns.end())
      std::cerr << "ConstrContainer::getLoadPattern - no se encontró el caso de carga de tag "
                << tag << "\n";
    else
      retval= (*i).second;
    return retval;
  }

//! @brief Devuelve un puntero al bloqueador de nodos cuyo tag se pasa como parámetro.
XC::NodeLocker *XC::ConstrContainer::getNodeLocker(const int &tag)
  {
    NodeLocker *retval= nullptr;
    MapCasosActivos<NodeLocker>::iterator i= activeNodeLockers.find(tag);
    if(i==activeNodeLockers.end())
      std::cerr << "ConstrContainer::getNodeLocker - no se encontró el caso de carga de tag "
                << tag << "\n";
    else
      retval= (*i).second;
    return retval;
  }

//! @brief Devuelve un puntero al bloqueador de nodos cuyo tag se pasa como parámetro.
const XC::NodeLocker *XC::ConstrContainer::getNodeLocker(const int &tag) const
  {
    const NodeLocker *retval= nullptr;
    MapCasosActivos<NodeLocker>::const_iterator i= activeNodeLockers.find(tag);
    if(i==activeNodeLockers.end())
      std::cerr << "ConstrContainer::getNodeLocker - no se encontró el caso de carga de tag "
                << tag << "\n";
    else
      retval= (*i).second;
    return retval;
  }


//! @brief Devuelve el número de coacciones mononodales.
int XC::ConstrContainer::getNumSPs(void) const
  { return theSPs->getNumComponents(); }

//! @brief Devuelve el número de coacciones multinodales.
int XC::ConstrContainer::getNumMPs(void) const
  { return theMPs->getNumComponents(); }

//! @brief Devuelve el número de coacciones multinodales.
int XC::ConstrContainer::getNumMRMPs(void) const
  { return theMRMPs->getNumComponents(); }

//! @brief Devuelve el número de casos de carga.
int XC::ConstrContainer::getNumLoadPatterns(void) const
  { return activeLoadPatterns.size(); }

//! @brief Devuelve el número
int XC::ConstrContainer::getNumNodeLockers(void) const
  { return activeNodeLockers.size(); }

void XC::ConstrContainer::applyLoad(double timeStep)
  {
    // now loop over node lockers, invoking applyLoad on them
    for(MapCasosActivos<NodeLocker>::iterator i= activeNodeLockers.begin();
        i!= activeNodeLockers.end();i++)
      i->second->applyLoad();



    // now loop over load patterns, invoking applyLoad on them
    for(MapCasosActivos<LoadPattern>::iterator i= activeLoadPatterns.begin();
        i!= activeLoadPatterns.end();i++)
      i->second->applyLoad(timeStep);

    //
    // finally loop over the MP_Constraints and SP_Constraints of the domain
    //

    MP_ConstraintIter &theMPs= getMPs();
    MP_Constraint *theMP;
    while((theMP= theMPs()) != 0)
        theMP->applyConstraint(timeStep);

    MRMP_ConstraintIter &theMRMPs= getMRMPs();
    MRMP_Constraint *theMRMP;
    while((theMRMP= theMRMPs()) != 0)
        theMRMP->applyConstraint(timeStep);

    SP_ConstraintIter &theSPs= getSPs();
    SP_Constraint *theSP;
    while((theSP= theSPs()) != 0)
      theSP->applyConstraint(timeStep);
  }

//! @brief Devuelve las coacciones correspondientes al nodo
//! y grado de libertad que se pasan como parámetro.
std::deque<int> XC::ConstrContainer::getTagsSPsNodo(int theNode, int theDOF) const
  {
    std::deque<int> retval; 

    int nodeTag= 0;
    int dof= 0;
    ConstrContainer *this_no_const= const_cast<ConstrContainer *>(this);
    SP_ConstraintIter &theSPs= this_no_const->getSPs();
    SP_Constraint *theSP;
    while(((theSP= theSPs()) != 0))
      {
        nodeTag= theSP->getNodeTag();
        dof= theSP->getDOF_Number();
        if(nodeTag == theNode && dof == theDOF)
          retval.push_back(theSP->getTag());
      }
    return retval;
  }

//! @brief Devuelve las coacciones correspondientes al nodo
//! que se pasan como parámetro.
std::deque<int> XC::ConstrContainer::getTagsSPsNodo(int theNode) const
  {
    std::deque<int> retval; 

    int nodeTag= 0;
    ConstrContainer *this_no_const= const_cast<ConstrContainer *>(this);
    SP_ConstraintIter &theSPs= this_no_const->getSPs();
    SP_Constraint *theSP;
    while(((theSP= theSPs()) != 0))
      {
        nodeTag= theSP->getNodeTag();
        if(nodeTag == theNode)
          retval.push_back(theSP->getTag());
      }
    return retval;
  }

//! @brief Devuelve las coacciones correspondientes al nodo
//! y grado de libertad que se pasan como parámetro.
std::deque<int> XC::ConstrContainer::getTagsMPsNodo(int theNode, int theDOF) const
  {
    std::deque<int> retval; 

    std::cerr << "XC::ConstrContainer::getTagsMPsNodo no está implementada." << std::endl;
    return retval;
  }

//! @brief Devuelve las coacciones correspondientes al nodo
//! y grado de libertad que se pasan como parámetro.
std::deque<int> XC::ConstrContainer::getTagsMPsNodo(int theNode) const
  {
    std::deque<int> retval; 

    std::cerr << "XC::ConstrContainer::getTagsMPsNodo no está implementada." << std::endl;
    return retval;
  }

std::deque<int> XC::ConstrContainer::getTagsLPs(void) const
  {
    // loop over all the load patterns that are currently added to the domain
    // getting their tag
    std::deque<int> retval;   
    for(MapCasosActivos<LoadPattern>::const_iterator i= activeLoadPatterns.begin();
        i!= activeLoadPatterns.end();i++)
      retval.push_back(i->second->getTag());
    return retval;
  }

std::string XC::ConstrContainer::getNombresLPs(void) const
  {
    // loop over all the load patterns that are currently added to the domain
    // getting their name.
    std::string retval;
    const size_t sz= activeLoadPatterns.size();
    if(sz>0)
      {
        const Domain *dom= getDomain();
        if(dom)
          {
            const Preprocessor *preprocessor= dom->GetPreprocessor();
            if(preprocessor)
              {
                const LoadLoader &loadLoader= preprocessor->getLoadLoader();
                const MapLoadPatterns &casos= loadLoader.getLoadPatterns();
                for(MapCasosActivos<LoadPattern>::const_iterator i= activeLoadPatterns.begin();
                    i!= activeLoadPatterns.end();i++)
                  retval+= casos.getNombreLoadPattern((*i).second) + " ";
              }
            else
	      std::cerr << "ConstrContainer::getNombresLPs; necesito un preprocesador." << std::endl;
          }
        else
          std::cerr << "ConstrContainer::getNombresLPs; necesito un dominio." << std::endl;
      }
    return retval;
  }

std::deque<int> XC::ConstrContainer::getTagsNLs(void) const
  {
    // loop over all the load patterns that are currently added to the domain
    // getting theit tag
    std::deque<int> retval;   
    for(MapCasosActivos<NodeLocker>::const_iterator i= activeNodeLockers.begin();
        i!= activeNodeLockers.end();i++)
      retval.push_back(i->second->getTag());
    return retval;
  }

//! @brief Devuelve verdadero si las SPs afectan al nodo que se pasa como parámetro.
bool XC::ConstrContainer::nodoAfectadoSPs(int tagNodo) const
  {
    bool retval= false;
    ConstrContainer *this_no_const= const_cast<ConstrContainer *>(this);
    SP_ConstraintIter &theSPs= this_no_const->getDomainAndLoadPatternSPs();
    SP_Constraint *theSP;
    while((theSP= theSPs()) != 0)
      if(theSP->getNodeTag() == tagNodo)
        {
          retval= true;
          break;
        }
    return retval;
  }

//! @brief Devuelve verdadero si las MPs afectan al nodo que se pasa como parámetro.
bool XC::ConstrContainer::nodoAfectadoMPs(int tagNodo) const
  {
    bool retval= false;
    ConstrContainer *this_no_const= const_cast<ConstrContainer *>(this);
    MP_ConstraintIter &theMPs= this_no_const->getMPs();
    MP_Constraint *theMP;
    while((theMP= theMPs()) != 0)
      if(theMP->afectaANodo(tagNodo))
        {
          retval= true;
          break;
        }
    return retval;
  }

//! @brief Devuelve verdadero si las MRMPs afectan al nodo que se pasa como parámetro.
bool XC::ConstrContainer::nodoAfectadoMRMPs(int tagNodo) const
  {
    bool retval= false;
    ConstrContainer *this_no_const= const_cast<ConstrContainer *>(this);
    MRMP_ConstraintIter &theMRMPs= this_no_const->getMRMPs();
    MRMP_Constraint *theMRMP;
    while((theMRMP= theMRMPs()) != 0)
      if(theMRMP->afectaANodo(tagNodo))
        {
          retval= true;
          break;
        }
    return retval;
  }

//! @brief Devuelve verdadero si MPs o SPs afectan al nodo que se pasa como parámetro.
bool XC::ConstrContainer::nodoAfectadoSPsOMPs(int nodeTag) const
  {
    bool retval= nodoAfectadoSPs(nodeTag);
    if(!retval)
      retval= nodoAfectadoMPs(nodeTag);
    if(!retval)
      retval= nodoAfectadoMRMPs(nodeTag);
    return retval;
  }

//! @brief Establece como constantes todas las cargas del dominio.
void XC::ConstrContainer::setLoadConstant(void)
  {
    // loop over all the load patterns that are currently added to the domain
    // getting them to set their loads as now constant
    for(MapCasosActivos<LoadPattern>::const_iterator i= activeLoadPatterns.begin();
        i!= activeLoadPatterns.end();i++)
      i->second->setLoadConstant();
  }

//! @brief Reactions due to constraints.
int XC::ConstrContainer::calculateNodalReactions(bool inclInertia, const double &tol)
  {
    MP_ConstraintIter &theMPs= this->getMPs();
    MP_Constraint *theMP;
    while((theMP= theMPs()) != 0)
      theMP->addResistingForceToNodalReaction(inclInertia);

    MRMP_ConstraintIter &theMRMPs= this->getMRMPs();
    MRMP_Constraint *theMRMP;
    while((theMRMP= theMRMPs()) != 0)
      theMRMP->addResistingForceToNodalReaction(inclInertia);

    return 0;
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::ConstrContainer::getDbTagData(void) const
  {
    static DbTagData retval(7);
    return retval;
  }

//! @brief Envía los tags de los casos de carga activos por el canal que se pasa como parámetro.
int XC::ConstrContainer::sendLPatternsTags(const int &posFlag,const int &posDbTag,CommParameters &cp)
  {
    int res= 0;
    static ID loadPatternsTags;
    const std::deque<int> tags= getTagsLPs();
    const size_t sz= tags.size();
    if(sz>0)
      {
        setDbTagDataPos(posFlag,1);
        loadPatternsTags.resize(sz);
        for(size_t i= 0; i<sz;i++)
          loadPatternsTags[i]= tags[i];
        res+= cp.sendID(loadPatternsTags,getDbTagData(),CommMetaData(posDbTag));
      }
    else
      setDbTagDataPos(posFlag,0);
    return res;
  }

//! @brief Recibe los tags de los casos de carga activos por el canal que se pasa como parámetro.
int XC::ConstrContainer::recvLPatternsTags(const int &posFlag,const int &posDbTag,const CommParameters &cp)
  {
    Domain *dom= getDomain();
    int res= 0;
    static ID loadPatternsTags;
    const int flag= getDbTagDataPos(posFlag);
    if(flag != 0)
      {
        res= cp.receiveID(loadPatternsTags,getDbTagData(),CommMetaData(posDbTag));
    
        assert(dom);
        Preprocessor *preprocessor= dom->GetPreprocessor();
        if(preprocessor)
          {
            LoadLoader &loadLoader= preprocessor->getLoadLoader();
            LoadPattern *load= nullptr;
            const size_t sz= loadPatternsTags.Size();
            for(size_t i=0;i<sz;i++)
              {
                load= loadLoader.getLoadPatterns().buscaLoadPattern(loadPatternsTags[i]);
                if(load)
                  {
                    if(addLoadPattern(load))
                      load->setDomain(dom);
                    else
                      {
                        if(verborrea>3)
                          {
                            const MapLoadPatterns &casos= loadLoader.getLoadPatterns();
	                    std::cerr << "ConstrContainer::recvLPatternsTags; no se pudo agregar la acción: '"
                                      << casos.getNombreLoadPattern(load)
                                      << "' de tag: " << load->getTag() << std::endl;
                          }
                      }
                  }
                else
	          std::cerr << "ConstrContainer::recvLPatternsTags; no se encontró el caso de carga de tag: "
                            << loadPatternsTags[i] << std::endl;
              }
          }
      }
    return res;
  }

//! @brief Envía los tags de los bloqueadores de nodo activos por el canal que se pasa como parámetro.
int XC::ConstrContainer::sendNLockersTags(const int &posFlag,const int &posDbTag,CommParameters &cp)
  {
    int res= 0;
    static ID nLockersTags;
    const std::deque<int> tags= getTagsNLs();
    const size_t sz= tags.size();
    if(sz>0)
      {
        setDbTagDataPos(posFlag,1);
        nLockersTags.resize(sz);
        for(size_t i= 0; i<sz;i++)
          nLockersTags[i]= tags[i];
        res+= cp.sendID(nLockersTags,getDbTagData(),CommMetaData(posDbTag));
      }
    else
      setDbTagDataPos(posFlag,0);
    return res;
  }

//! @brief Recibe los tags de los casos de carga activos por el canal que se pasa como parámetro.
int XC::ConstrContainer::recvNLockersTags(const int &posFlag,const int &posDbTag,const CommParameters &cp)
  {
    Domain *dom= getDomain();
    int res= 0;
    static ID nLockersTags;
    const int flag= getDbTagDataPos(posFlag);
    if(flag != 0)
      {
        res= cp.receiveID(nLockersTags,getDbTagData(),CommMetaData(posDbTag));
    
        assert(dom);
        Mesh &mesh= dom->getMesh();
        NodeLocker *nl= nullptr;
        const size_t sz= nLockersTags.Size();
        for(size_t i=0;i<sz;i++)
          {
            nl= mesh.getNodeLockers().buscaNodeLocker(nLockersTags[i]);
            if(nl)
              {
                if(addNodeLocker(nl))
                  nl->setDomain(dom);
              }
            else
              std::cerr << "ConstrContainer::recvNLockersTags; no se encontró el bloqueador de nodos de tag: "
                        << nLockersTags[i] << std::endl;
          }
      }
    return res;
  }

//! @brief Envía los miembros del objeto por el canal que se pasa como parámetro.
int XC::ConstrContainer::sendData(CommParameters &cp)
  {
    int res= cp.sendMovable(*theSPs,getDbTagData(),CommMetaData(0));
    res+= cp.sendMovable(*theMPs,getDbTagData(),CommMetaData(1));
    res+= cp.sendMovable(*theMRMPs,getDbTagData(),CommMetaData(2));
    
    res+= sendNLockersTags(3,4,cp); //Guardamos los bloqueadores de nodo "activos".
    res+= sendLPatternsTags(5,6,cp); //Guardamos los casos de carga "activos".
    return res;
  }

//! @brief Recibe los miembros del objeto por el canal que se pasa como parámetro.
int XC::ConstrContainer::recvData(const CommParameters &cp)
  {
    int res= theSPs->recibe<SP_Constraint>(getDbTagDataPos(0),cp,&FEM_ObjectBroker::getNewSP);
    res+= theMPs->recibe<MP_Constraint>(getDbTagDataPos(1),cp,&FEM_ObjectBroker::getNewMP);
    res+= theMRMPs->recibe<MRMP_Constraint>(getDbTagDataPos(2),cp,&FEM_ObjectBroker::getNewMRMP);
    res+= recvNLockersTags(3,4,cp);
    res+= recvLPatternsTags(5,6,cp);
    return res;
  }

//! @brief Envía el objeto por el canal que se pasa como parámetro.
int XC::ConstrContainer::sendSelf(CommParameters &cp)
  {
    inicComm(7);
    int res= sendData(cp);

    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ConstrContainer::send - ch failed to send the initial ID\n";
    return res;
  }


int XC::ConstrContainer::recvSelf(const CommParameters &cp)
  {
    // first we get the data about the state of the cc for this cTag
    inicComm(7);
    int res= cp.receiveIdData(getDbTagData(),getDbTag());
    if(res<0)
      std::cerr << "ConstrContainer::recv - ch failed to recv the initial ID\n";
    else
      res+= recvData(cp);
    return res;
  }

//! @brief Imprime el dominio.
void XC::ConstrContainer::Print(std::ostream &s, int flag)
  {

    s << "Current ConstrContainer Information\n";

    s << "\nSP_Constraints: numConstraints: ";
    s << theSPs->getNumComponents() << "\n";
    theSPs->Print(s, flag);

    s << "\nMP_Constraints: numConstraints: ";
    s << theMPs->getNumComponents() << "\n";
    theMPs->Print(s, flag);

    s << "\nMRMP_Constraints: numConstraints: ";
    s << theMRMPs->getNumComponents() << "\n";
    theMRMPs->Print(s, flag);

    s << "\nLOAD PATTERNS: num Patterns: ";
    s << activeLoadPatterns.size() << "\n\n";
    activeLoadPatterns.Print(s, flag);

    s << "\nNode lockers: num lockers: ";
    s << activeNodeLockers.size() << "\n\n";
    activeNodeLockers.Print(s, flag);
  }

std::ostream &XC::operator<<(std::ostream &s, XC::ConstrContainer &M)
  {
    M.Print(s);
    return s;
  }

