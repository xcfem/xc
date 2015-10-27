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

// $Revision: 1.13 $
// $Date: 2005/11/22 19:44:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/LoadPattern.cpp,v $

// Written: fmk 07/99
// Revised:
//
// Purpose: This file contains the method definitions for class XC::LoadPattern.
// LoadPattern is a container class.
//


#include "LoadPattern.h"
#include <cstdlib>
#include <utility/matrix/ID.h>
#include "domain/domain/Domain.h"
#include <domain/constraints/SP_Constraint.h>
#include <domain/load/pattern/TimeSeries.h>
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>
#include <domain/load/ElementalLoadIter.h>
#include <domain/load/NodalLoadIter.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/load/groundMotion/GroundMotion.h>

#include <domain/load/NodalLoad.h>
#include "domain/load/elem_load.h"

#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/ArrayCommMetaData.h"

#include "preprocessor/loaders/LoadLoader.h"

void XC::LoadPattern::libera_contenedores(void)
  {
    if(theNodalLoads)
      {
        delete theNodalLoads;
        theNodalLoads= nullptr;
      }
    
    if(theElementalLoads)
      {
        delete theElementalLoads;
        theElementalLoads= nullptr;
      }
  }

void XC::LoadPattern::libera_iteradores(void)
  {
    if(theEleIter)
      {
        delete theEleIter;
        theEleIter= nullptr;
      }
    if(theNodIter)
      {
        delete theNodIter;
        theNodIter= nullptr;
      }
  }

//! @brief Reserva memoria para almacenar las cargas.
void XC::LoadPattern::alloc_contenedores(void)
  {
    libera_contenedores();
    theNodalLoads = new ArrayOfTaggedObjects(this,32,"nodalLoad");
    theElementalLoads = new ArrayOfTaggedObjects(this,32,"elementLoad");

    if(!theNodalLoads || !theElementalLoads)
      {
        std::cerr << "LoadPattern::LoadPattern() - ran out of memory\n";
        exit(-1);
      }
  }

//! @brief Reserva memoria para almacenar los iteradores.
void XC::LoadPattern::alloc_iteradores(void)
  {
    libera_iteradores();
    theEleIter = new ElementalLoadIter(theElementalLoads);
    theNodIter = new NodalLoadIter(theNodalLoads);

    if(theEleIter == 0 || theNodIter == 0)
      {
        std::cerr << "LoadPattern::LoadPattern() - ran out of memory\n";
        exit(-1);
      }
  }

void XC::LoadPattern::libera(void)
  {
    setTimeSeries(nullptr);

    libera_contenedores();
    libera_iteradores();

    // AddingSensitivity:BEGIN /////////////////////////////
    if(randomLoads)
      {
        delete randomLoads;
        randomLoads= nullptr;
      }
    // AddingSensitivity:END ///////////////////////////////
  }

//! @brief Constructor
XC::LoadPattern::LoadPattern(int tag, int clasTag)
  :NodeLocker(tag,clasTag), loadFactor(0.0), gamma_f(1.0),
   theSeries(nullptr), theNodalLoads(nullptr), theElementalLoads(nullptr), 
   theNodIter(nullptr), theEleIter(nullptr), isConstant(1)
  {
    // constructor for subclass
    alloc_contenedores();
    alloc_iteradores();

    // AddingSensitivity:BEGIN /////////////////////////////
    randomLoads= nullptr;
    // AddingSensitivity:END ///////////////////////////////
  }


//! @brief Constructor por defecto.
XC::LoadPattern::LoadPattern(void)
  :NodeLocker(0,PATTERN_TAG_LoadPattern), loadFactor(0.0), gamma_f(1.0),
   theSeries(nullptr), theNodalLoads(nullptr), theElementalLoads(nullptr),
   theNodIter(nullptr), theEleIter(nullptr), isConstant(1)
  {
    alloc_contenedores();
    alloc_iteradores();

    // AddingSensitivity:BEGIN /////////////////////////////
    randomLoads= nullptr;
    // AddingSensitivity:END ///////////////////////////////
  }

//! @brief Constructor.
XC::LoadPattern::LoadPattern(int tag)
  :NodeLocker(tag,PATTERN_TAG_LoadPattern),loadFactor(0.0), gamma_f(1.0),
   theSeries(nullptr), theNodalLoads(nullptr), theElementalLoads(nullptr),
   theNodIter(nullptr), theEleIter(nullptr), isConstant(1)
  {
    alloc_contenedores();
    alloc_iteradores();

    // AddingSensitivity:BEGIN /////////////////////////////
    randomLoads = nullptr;
    // AddingSensitivity:END ///////////////////////////////
  }


XC::LoadPattern *XC::LoadPattern::getCopy(void)
  {
    std::cerr << "LoadPattern::getCopy, no implementada." << std::endl;
    return nullptr;
  }

//! @brief Lee un objeto XC::LoadPattern desde archivo
bool XC::LoadPattern::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(LoadPattern) Procesando comando: " << cmd << std::endl;
    if(cmd == "tseries")
      {
        if(theSeries)
          theSeries->LeeCmd(status);
        else
          {
            status.GetBloque(); //Ignora entrada.
            std::cerr << "El LoadPattern cuyo identificador es " << getTag()
                      << " no tiene modulación en el tiempo." << std::endl;
          }
        return true;
      }
    else if(cmd == "gamma_f")
      {
        gamma_f= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "nodal_load") //Carga sobre nodo.
      {
        std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
        if(fnc_indices.size()<1)
          std::cerr << "uso: " << cmd << "[idNodo,tag] " << std::endl;
        int tag_nodo= 0;
        if(fnc_indices.size()>0)
          tag_nodo= convert_to_int(fnc_indices[0]); //Identificador del nodo.
        if(fnc_indices.size()>1)
          nextTag= convert_to_int(fnc_indices[1]); //Tag de la carga.
        NodalLoad *theLoad= new NodalLoad(nextTag,tag_nodo,Vector(1));
        nextTag++;
        if(theLoad)
          theLoad->LeeCmd(status);
        addNodalLoad(theLoad);
        return true;
      }
    else if(procesa_comando_element_load(this,nextTag,cmd,status))
      { return true; }
    else if(cmd == "nodalLoads")
      {
        if(theNodalLoads)
          theNodalLoads->LeeCmd(status);
        else
          std::cerr << cmd << "; no existen cargas nodales." << std::endl;
        return true;
      }
    else if(cmd == "elementalLoads")
      {
        if(theElementalLoads)
          theElementalLoads->LeeCmd(status);
        else
          std::cerr << cmd << "; no existen cargas sobre elementos." << std::endl;
        return true;
      }
    else
      return NodeLocker::procesa_comando(status);
  }

//! @brief Destructor.
XC::LoadPattern::~LoadPattern(void)
  { libera(); }


void XC::LoadPattern::setTimeSeries(TimeSeries *theTimeSeries)
  {
    // invoke the destructor on the old TimeSeries
    if(theSeries)
      { 
        delete theSeries;
        theSeries= nullptr;
      }
    // set the pointer to the new series object
    if(theTimeSeries)
      theSeries= theTimeSeries->getCopy();
    else
      theSeries= nullptr;
  }


//! @brief Establece el dominio para las cargas.
void XC::LoadPattern::setDomain(Domain *theDomain)
  {
    NodeLocker::setDomain(theDomain);

    // if subclass does not implement .. check for 0 pointer
    if(theNodalLoads)
      {
        NodalLoad *nodLoad= nullptr;
        NodalLoadIter &theNodalIter= getNodalLoads();
        while((nodLoad = theNodalIter()) != 0)
          nodLoad->setDomain(theDomain);
      }
    if(theElementalLoads)
      {
        ElementalLoad *eleLoad= nullptr;
        ElementalLoadIter &theElementalIter= getElementalLoads();
        while((eleLoad = theElementalIter()) != 0)
          eleLoad->setDomain(theDomain);
      }
  }

//! @brief Apply load.
bool XC::LoadPattern::addToDomain(void)
  {
    bool retval= false;
    Domain *theDomain = this->getDomain();
    if(theDomain)
      retval= theDomain->addLoadPattern(this);
    else
      std::cerr << "WARNING: LoadPattern::addToDomain() - null pointer to the domain\n";
    return retval;
  }

//! @brief Removes load.
void XC::LoadPattern::removeFromDomain(void)
  {
    Domain *theDomain = this->getDomain();
    if(theDomain)
      theDomain->removeLoadPattern(this);
    else
      std::cerr << "WARNING: LoadPattern::removeFromDomain() - null pointer to the domain\n";
  }

//! @brief Agrega la carga sobre nodo que se pasa como parámetro.
bool XC::LoadPattern::addNodalLoad(NodalLoad *load)
  {
    Domain *theDomain = this->getDomain();

    bool result = theNodalLoads->addComponent(load);
    if(result == true)
      {
        if(theDomain)
          load->setDomain(theDomain);
        load->setLoadPatternTag(this->getTag());
        load->set_owner(this);
        currentGeoTag++;
      }
    else
      std::cerr << "WARNING: LoadPattern::addNodalLoad() - load could not be added\n";
    return result;
  }

//! @brief Crea una carga sobre nodo que se pasa como parámetro.
XC::NodalLoad *XC::LoadPattern::newNodalLoad(const int &tag_nodo,const Vector &f)
  {
    MapLoadPatterns *map= dynamic_cast<MapLoadPatterns *>(Owner());
    assert(map);
    int nextTag= map->getCurrentNodeLoadTag();
    NodalLoad *retval= new NodalLoad(nextTag,tag_nodo,f);
    if(addNodalLoad(retval))
      map->setCurrentNodeLoadTag(++nextTag);
    return retval;
  }

//! @brief Crea una carga sobre elementos.
XC::ElementalLoad *XC::LoadPattern::newElementalLoad(const std::string &cmd)
  {
    MapLoadPatterns *map= dynamic_cast<MapLoadPatterns *>(Owner());
    assert(map);
    int nextTag= map->getCurrentElementLoadTag();
    ElementalLoad *retval= procesa_element_load(this,nextTag,cmd);
    if(retval)
      map->setCurrentElementLoadTag(++nextTag);
    return retval;
  }

//! @brief Agrega la carga sobre elemento que se pasa como parámetro.
bool XC::LoadPattern::addElementalLoad(ElementalLoad *load)
  {
    Domain *theDomain= getDomain();

    const bool result= theElementalLoads->addComponent(load);
    if(result)
      {
        if(theDomain)
          load->setDomain(theDomain);
        load->setLoadPatternTag(this->getTag());
        load->set_owner(this);
        currentGeoTag++;
      }
    else
      std::cerr << "WARNING: LoadPattern::addElementalLoad() - load could not be added\n";
    return result;
  }

//! @brief Agrega la carga sobre elemento que se pasa como parámetro e incrementa el tag de cargas sobre elemento.
bool XC::LoadPattern::newElementalLoad(ElementalLoad *load)
  {
    MapLoadPatterns *map= dynamic_cast<MapLoadPatterns *>(Owner());
    int nextTag= map->getCurrentElementLoadTag();
    bool result= false;
    if(load)
      {
        result= addElementalLoad(load);
        if(result)
          map->setCurrentElementLoadTag(++nextTag);
      }
    else
      std::cerr  << "MapLoadPatterns::newElementalLoad; el puntero a la carga es nulo." << std::endl;
    return result;
  }

// //! @brief Crea la carga sobre elemento que se pasa como parámetro.
// XC::NodalLoad *XC::MapLoadPatterns::newElementLoad(const int &tag_elem,const Vector &f)
//   {
//     MapLoadPatterns *map= dynamic_cast<MapLoadPatterns *>(owner());
//     int nextTag= map->getCurrentNodeLoadTag();
//     NodalLoad *retval= new NodalLoad(nextTag,tag_nodo,f);
//     bool retval= addNodalLoad(theLoad);
//     if(addNodalLoad(theLoad))
//       map->setCurrentNodeLoadTag(++nextTag);
//     return retval;
//   }

//! @brief Agrega la coacción que se pasa como parámetro.
bool XC::LoadPattern::addSP_Constraint(SP_Constraint *theSp)
  { return NodeLocker::addSP_Constraint(theSp); }

//! @brief Devuelve un iterador a las cargas sobre nodo.
XC::NodalLoadIter &XC::LoadPattern::getNodalLoads(void)
  {
    theNodIter->reset();
    return *theNodIter;
  }

//! @brief Devuelve un iterador a las cargas sobre elemento.
XC::ElementalLoadIter &XC::LoadPattern::getElementalLoads(void)
  {
    theEleIter->reset();
    return *theEleIter;
  }

//! @brief Devuelve el número de cargas sobre nodos.
int XC::LoadPattern::getNumNodalLoads(void) const
  {
    int retval= 0;
    if(theNodalLoads)
      retval= theNodalLoads->getNumComponents();
    return retval;
  }

//! @brief Devuelve el número de cargas sobre elementos.
int XC::LoadPattern::getNumElementalLoads(void) const
  {
    int retval= 0;
    if(theElementalLoads)
      retval= theElementalLoads->getNumComponents();
    return retval;
  }

//! @brief Devuelve el número de cargas total.
int XC::LoadPattern::getNumLoads(void) const
  { return getNumNodalLoads()+getNumElementalLoads(); }

//! @brief Borra todas las cargas y coacciones.
void XC::LoadPattern::clearAll(void)
  {
    NodeLocker::clearAll();
    theElementalLoads->clearAll();
    theNodalLoads->clearAll();
    setTimeSeries(nullptr);
  }

//! @brief Elimina la carga sobre nodo cuyo tag se pasa como parámetro.
bool XC::LoadPattern::removeNodalLoad(int tag)
  {
    bool result= theNodalLoads->removeComponent(tag);
    if(result) currentGeoTag++;
    return result;
  }

//! @brief Elimina la carga sobre elemento cuyo tag se pasa como parámetro.
bool XC::LoadPattern::removeElementalLoad(int tag)
  {
    bool result= theElementalLoads->removeComponent(tag);
    if(result) currentGeoTag++;
    return result;
  }

//! @brief Aplica la carga en el instante que se pasa como parámetro.
void XC::LoadPattern::applyLoad(double pseudoTime)
  {
    assert(theSeries);
    // first determine the load factor
    if(isConstant != 0)
      loadFactor= theSeries->getFactor(pseudoTime);
    const double factor= loadFactor*gamma_f; //Ponderación de la hipótesis.

    NodalLoad *nodLoad= nullptr;
    NodalLoadIter &theNodalIter= getNodalLoads();
    while((nodLoad = theNodalIter()) != 0)
      nodLoad->applyLoad(factor);

    ElementalLoad *eleLoad= nullptr;
    ElementalLoadIter &theElementalIter= getElementalLoads();
    while((eleLoad = theElementalIter()) != 0)
      eleLoad->applyLoad(factor);

    NodeLocker::applyLoad(pseudoTime,factor);
  }

void XC::LoadPattern::setLoadConstant(void)
  { isConstant = 0; }

//! @brief Devuelve el factor de ponderación obtenido del objeto TimeSeries.
const double &XC::LoadPattern::getLoadFactor(void) const
  { return loadFactor; }

//! @brief Devuelve el factor de ponderación impuesto por la combinación de hipótesis.
const double &XC::LoadPattern::GammaF(void) const
  { return gamma_f; }
//! @brief Devuelve el factor de ponderación impuesto por la combinación de hipótesis.
double &XC::LoadPattern::GammaF(void)
  { return gamma_f; }

//! @brief Asigna el factor de ponderación impuesto por la combinación de hipótesis.
void XC::LoadPattern::setGammaF(const double &g)
  { gamma_f= g; }


//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::LoadPattern::getDbTagData(void) const
  {
    static DbTagData retval(16);
    return retval;
  }

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::LoadPattern::sendData(CommParameters &cp)
  {
    int res= NodeLocker::sendData(cp);
    res+= cp.sendDoubles(loadFactor,gamma_f,getDbTagData(),CommMetaData(7));
    res+= sendTimeSeriesPtr(theSeries,8,9,getDbTagData(),cp);
    res+= cp.sendMovable(*theNodalLoads,getDbTagData(),CommMetaData(10));
    res+= cp.sendMovable(*theElementalLoads,getDbTagData(),CommMetaData(11));

    res+= cp.sendVectorPtr(randomLoads,getDbTagData(),ArrayCommMetaData(12,13,14));
    res+= cp.sendBool(RVisRandomProcessDiscretizer,getDbTagData(),CommMetaData(15));
    return res;
  }

//! @brief Recibe los miembros a través del canal que se pasa como parámetro.
int XC::LoadPattern::recvData(const CommParameters &cp)
  {
    int res= NodeLocker::recvData(cp);
    res+= cp.receiveDoubles(loadFactor,gamma_f,getDbTagData(),CommMetaData(7));
    theSeries= receiveTimeSeriesPtr(theSeries,8,9,getDbTagData(),cp);

    theNodalLoads->clearAll();
    res+= theNodalLoads->recibe<NodalLoad>(getDbTagDataPos(10),cp,&FEM_ObjectBroker::getNewNodalLoad);

    theElementalLoads->clearAll();
    res+= theElementalLoads->recibe<ElementalLoad>(getDbTagDataPos(11),cp,&FEM_ObjectBroker::getNewElementalLoad);

    randomLoads= cp.receiveVectorPtr(randomLoads,getDbTagData(),ArrayCommMetaData(12,13,14));
    res+= cp.receiveBool(RVisRandomProcessDiscretizer,getDbTagData(),CommMetaData(15));
    return res;
  }

//! @brief Envía el objeto por el canal que se pasa como parámetro.
int XC::LoadPattern::sendSelf(CommParameters &cp)
  {
    inicComm(16);
    int res= sendData(cp);
    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "LoadPattern::sendSelf() - failed to send data.\n";    
    return res;
  }


//! @brief Recibe el objeto por el canal que se pasa como parámetro.
int XC::LoadPattern::recvSelf(const CommParameters &cp)
  {
    inicComm(16);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "LoadPattern::recvSelf() - data could not be received.\n" ;
    else
      res+= recvData(cp);
    return res;
  }

//! @brief Imprime el caso de carga.
void XC::LoadPattern::Print(std::ostream &s, int flag)
  {
    s << "Load Pattern: " << this->getTag() << "\n";
    s << "  load factor: " << loadFactor << "\n";
    s << "  gamma_f: " << gamma_f << "\n";
    if(theSeries)
      theSeries->Print(s,flag);
    std::cerr << "  Nodal Loads: \n";
    theNodalLoads->Print(s,flag);
    std::cerr << "\n  Elemental Loads: \n";
    theElementalLoads->Print(s, flag);
    NodeLocker::Print(s,flag);
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
//! nnlds: Número de cargas en los nodos.
any_const_ptr XC::LoadPattern::GetProp(const std::string &cod) const
  {
    if(cod=="tseries")
      return any_const_ptr(theSeries);
    else if(cod=="getNumCargasNod") //Número de cargas en los nodos.
      {
        tmp_gp_int= theNodalLoads->getNumComponents();
        return any_const_ptr(tmp_gp_int);
      }
    else if(cod=="getNumCargasElem") //Número de cargas en los elementos.
      {
        tmp_gp_int= theElementalLoads->getNumComponents();
        return any_const_ptr(tmp_gp_int);
      }
    else if(cod=="getNumCargas") //Número de cargas en los elementos.
      {
        tmp_gp_int= theNodalLoads->getNumComponents();
        tmp_gp_int+= theElementalLoads->getNumComponents();
        tmp_gp_int+= theSPs->getNumComponents();
        return any_const_ptr(tmp_gp_int);
      }
    else
      return NodeLocker::GetProp(cod);
  }

int XC::LoadPattern::addMotion(GroundMotion &theMotion, int tag)
  {
    std::cerr << "LoadPattern::addMotion() - cannot add GroundMotion - use MultiSupport Pattern instead\n";
    return -1;
  }

XC::GroundMotion *XC::LoadPattern::getMotion(int tag)
  { return 0; }

// AddingSensitivity:BEGIN ////////////////////////////////////
void XC::LoadPattern::applyLoadSensitivity(double pseudoTime)
  {
    if(theSeries && isConstant != 0)
      loadFactor= theSeries->getFactorSensitivity(pseudoTime);

    NodalLoad *nodLoad;
    NodalLoadIter &theNodalIter = this->getNodalLoads();
    const double factor= loadFactor*gamma_f;
    while((nodLoad = theNodalIter()) != 0)
      nodLoad->applyLoad(factor);


    // Don't inlude element loads and sp constraints for now
    /*
      ElementalLoad *eleLoad;
      ElementalLoadIter &theElementalIter = this->getElementalLoads();
      while((eleLoad = theElementalIter()) != 0)
      eleLoad->applyLoad(factor);

      SP_Constraint *sp;
      SP_ConstraintIter &theIter = this->getSPs();
      while((sp = theIter()) != 0)
      sp->applyConstraint(factor);
    */
  }

int XC::LoadPattern::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(!theSeries)
      { std::cerr << "set/update/activate parameter is illegaly called in LoadPattern " << std::endl; }

    //const int argc= argv.size();
    // Nodal load
    if(strstr(argv[0].c_str(),"loadAtNode") != 0)
      {
        if(argv.size() < 3)
          return -1;

        RVisRandomProcessDiscretizer = false;

        int nodeNumber = atoi(argv[1]);
        NodalLoad *thePossibleNodalLoad;
        NodalLoad *theNodalLoad = 0;
        NodalLoadIter &theNodalIter = this->getNodalLoads();

        while((thePossibleNodalLoad = theNodalIter()) != 0)
          {
            if( nodeNumber == thePossibleNodalLoad->getNodeTag() )
              { theNodalLoad = thePossibleNodalLoad; }
          }

        if(theNodalLoad)
          {
	    std::vector<std::string> argv2(argv);
            argv2.erase(argv2.begin(),argv2.begin()+2);
            return theNodalLoad->setParameter(argv2, param);
          }
        else
          return -1;
    }

    else if(strstr(argv[0].c_str(),"elementPointLoad") != 0)
      {
        if(argv.size() < 3)
          return -1;
        std::cerr << "elementPointLoad no implementada." << std::endl;
//       RVisRandomProcessDiscretizer = false;

//       int eleNumber = atoi(argv[1]);
//       ElementalLoad *theEleLoad = 0;
//       ElementalLoadIter &theEleLoadIter = this->getElementalLoads();
//       while ((theEleLoad = theEleLoadIter()) != 0) {
//         int eleTag = theEleLoad->getElementTag();
//         if (eleNumber == eleTag) {
//           return theEleLoad->setParameter(&argv[2], argc-2, param);
//         }
//       }

      return -1;
    }

    else if(strstr(argv[0].c_str(),"randomProcessDiscretizer") != 0)
      {
        if(argv.size() < 2)
          return -1;
        RVisRandomProcessDiscretizer = true;
        std::vector<std::string> argv2(argv);
        argv2.erase(argv2.begin(),argv2.begin()+1);
        return theSeries->setParameter(argv2, param);
      }

    // Unknown parameter
    else
        return -1;
  }

int XC::LoadPattern::updateParameter(int parameterID, Information &info)
  {
    if(!theSeries)
      { std::cerr << "set/update/activate parameter is illegaly called in LoadPattern " << std::endl; }


    if(RVisRandomProcessDiscretizer)
      { return theSeries->updateParameter(parameterID,info); }
    else
      {
        NodalLoad *thePossibleNodalLoad = 0;
        NodalLoad *theNodalLoad = 0;
        NodalLoadIter &theNodalIter = this->getNodalLoads();

        switch (parameterID)
          {
          case 1: case -1:  // Not implemented.
            return -1;
          default:
            if(parameterID > 1000  &&  parameterID < 2000)
              {
                int nodeNumber = parameterID-1000;
                while((thePossibleNodalLoad = theNodalIter()) != 0)
                  {
                    if(nodeNumber == thePossibleNodalLoad->getNodeTag() )
                      { theNodalLoad = thePossibleNodalLoad; }
                  }
                return theNodalLoad->updateParameter(1, info);
              }
            else if(parameterID > 2000  &&  parameterID < 3000)
              {
                int nodeNumber = parameterID-2000;
                while((thePossibleNodalLoad = theNodalIter()) != 0)
                  {
                    if(nodeNumber == thePossibleNodalLoad->getNodeTag() )
                      { theNodalLoad = thePossibleNodalLoad; }
                  }
                return theNodalLoad->updateParameter(2, info);
              }
            else if(parameterID > 3000  &&  parameterID < 4000)
              {
                int nodeNumber = parameterID-3000;
                while((thePossibleNodalLoad = theNodalIter()) != 0)
                  {
                    if(nodeNumber == thePossibleNodalLoad->getNodeTag() )
                      { theNodalLoad = thePossibleNodalLoad; }
                  }
                return theNodalLoad->updateParameter(3, info);
              }
            else
              return -1;
          }
      }
  }


int XC::LoadPattern::activateParameter(int parameterID)
  {
    if(!theSeries)
      std::cerr << "set/update/activate parameter is illegaly called in LoadPattern " << std::endl;

    if(RVisRandomProcessDiscretizer)
      { return theSeries->activateParameter(parameterID); }
    else
      {
        // Don't set flag here in the load pattern itself.
        // (Assume there always may be random loads)

        NodalLoad *theNodalLoad = 0;
        NodalLoadIter &theNodalIter = this->getNodalLoads();

        if(parameterID == 0)
          {
            // Go through all nodal loads and zero out gradientIdentifier
            // (Remember: the identifier is only zero if we are in
            // the process of zeroing out all sensitivity flags).
            while((theNodalLoad = theNodalIter()) != 0)
              { theNodalLoad->activateParameter(parameterID); }
          }
        else
          {
            // Find the right nodal load and set the flag
            if(parameterID > 1000  &&  parameterID < 2000)
              {
                int nodeNumber = parameterID-1000;
                while((theNodalLoad = theNodalIter()) != 0)
                  {
                    if(nodeNumber == theNodalLoad->getNodeTag() )
                      { theNodalLoad->activateParameter(1); }
                  }
              }
            else if(parameterID > 2000  &&  parameterID < 3000)
              {
                int nodeNumber = parameterID-2000;
                while((theNodalLoad = theNodalIter()) != 0)
                  {
                    if(nodeNumber == theNodalLoad->getNodeTag() )
                      { theNodalLoad->activateParameter(2); }
                  }
              }
            else if(parameterID > 3000  &&  parameterID < 4000)
              {
                int nodeNumber = parameterID-3000;
                while((theNodalLoad = theNodalIter()) != 0)
                  {
                    if(nodeNumber == theNodalLoad->getNodeTag() )
                      { theNodalLoad->activateParameter(3); }
                  }
              }
            else
              { std::cerr << "XC::LoadPattern::gradient() -- error in identifier. " << std::endl; }
          }
      }
    return 0;
  }

const XC::Vector &XC::LoadPattern::getExternalForceSensitivity(int gradNumber)
  {

    // THIS METHOD IS CURRENTLY ONLY USED FOR THE STATIC CASE
    // IT SHOULD BE DELETED AND REPLACED BY THE DYNAMIC CASE

    // Initial declarations
    Vector tempRandomLoads(1);
    int sizeRandomLoads;

    // Start with a fresh return vector
    if(randomLoads == 0)
      { randomLoads = new XC::Vector(1); }
    else
      {
        delete randomLoads;
        randomLoads = new XC::Vector(1);
      }

    // Prepare the vector identifying which loads are random.
    NodalLoad *theNodalLoad = 0;
    NodalLoadIter &theNodalIter = this->getNodalLoads();
    int i;

    // Loop through the nodal loads to pick up possible contributions
    int nodeNumber;
    int dofNumber;
    while((theNodalLoad = theNodalIter()) != 0)
      {
        const XC::Vector &gradientVector = theNodalLoad->getExternalForceSensitivity(gradNumber);
        if(gradientVector(0) != 0.0 )
          {
            // Found a random load! Get nodeNumber and dofNumber
            nodeNumber = theNodalLoad->getNodeTag();
            dofNumber = (int)gradientVector(0);

            // Update the randomLoads vector
            sizeRandomLoads = randomLoads->Size();
            if(sizeRandomLoads == 1)
              {
                delete randomLoads;
                randomLoads = new XC::Vector(2);
                (*randomLoads)(0) = (double)nodeNumber;
                (*randomLoads)(1) = (double)dofNumber;
              }
            else
              {
                tempRandomLoads = (*randomLoads);
                delete randomLoads;
                randomLoads = new XC::Vector(sizeRandomLoads+2);
                for(i=0; i<sizeRandomLoads; i++)
                  { (*randomLoads)(i) = tempRandomLoads(i); }
                (*randomLoads)(sizeRandomLoads) = nodeNumber;
                (*randomLoads)(sizeRandomLoads+1) = dofNumber;
              }
          }
      }
    return (*randomLoads);
  }

// AddingSensitivity:END //////////////////////////////////////
