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
//Preprocessor.cc

#include "Preprocessor.h"
#include "ProblemaEF.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "preprocessor/set_mgmt/SetEstruct.h"
#include "preprocessor/set_mgmt/Set.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "boost/lexical_cast.hpp"


#include "utility/matrix/ID.h"

//! @brief Default constructor.
XC::Preprocessor::Preprocessor(EntCmd *owr,DataOutputHandler::map_output_handlers *oh)
  : EntCmd(owr), MovableObject(0), domain(nullptr), materialHandler(this), transf(this), beamIntegrators(this), 
    nodes(this), elements(this), loads(this), constraints(this),
    cad(this),sets(this)
  { domain= new Domain(this,oh); }

//! @brief Copy constructor (prohibited).
XC::Preprocessor::Preprocessor(const Preprocessor &otro)
  : EntCmd(otro), MovableObject(otro), domain(nullptr), materialHandler(this), transf(this), beamIntegrators(this),
    nodes(this), elements(this), loads(this), constraints(this),
    cad(this),sets(this)
  {
    std::cerr << "No se puede copiar un objeto de clase Preprocessor." << std::endl;
  }

//! @brief Assignment operator (prohibited).
XC::Preprocessor &XC::Preprocessor::operator=(const Preprocessor &otro)
  {
    std::cerr << "No se puede asignar un objeto de clase Preprocessor." << std::endl;
    return *this;
  }

//! @brief Insert the pointer to the node in the "total" set and in the 
//! sets that are currently opened.
void XC::Preprocessor::UpdateSets(Node *new_node)
  {
    sets.get_set_total()->addNode(new_node);
    MapSet::map_sets &abiertos= sets.get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->addNode(new_node);
      }
  }

//! @brief Insert the pointer to the element in the "total" set and in the 
//! sets that are currently opened.
void XC::Preprocessor::UpdateSets(Element *new_elem)
  {
    sets.get_set_total()->addElement(new_elem);
    MapSet::map_sets &abiertos= sets.get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->addElement(new_elem);
      }
  }

//! @brief Insert the pointer to the constraint in the "total" set and in the 
//! sets that are currently opened.
void XC::Preprocessor::UpdateSets(Constraint *new_constraint)
  {
    sets.get_set_total()->GetConstraints().push_back(new_constraint);
    MapSet::map_sets &abiertos= sets.get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->GetConstraints().push_back(new_constraint);
      }
  }

//! @brief Destructor.
XC::Preprocessor::~Preprocessor(void)
  {
    cad.clearAll();
    if(domain) delete domain;
    domain= nullptr;
  }


//! @brief Assign Stress Reduction Factor for element deactivation.
void XC::Preprocessor::setDeadSRF(const double &d)
  { Element::setDeadSRF(d); }

//! @brief Return a pointer to the set or geometric entity with
//! the name being passed as a parameter.
XC::SetEstruct *XC::Preprocessor::busca_set_estruct(const std::string &nmb)
  {
    SetEstruct *retval= sets.busca_set_estruct(nmb);
    if(!retval)
      retval= cad.busca_set_estruct(boost::lexical_cast<size_t>(nmb));
    return retval;
  }

//! @brief Domain setup to solve for a new load pattern.
void XC::Preprocessor::resetLoadCase(void)
  { 
    getLoadLoader().removeAllFromDomain();
    if(domain)
      domain->resetLoadCase();
    else
      std::cerr << "Preprocessor::resetLoadCase; domain not defined (null ptr)."
                << std::endl;
  }


//! @brief Delete all preprocessor entities.
void XC::Preprocessor::clearAll(void)
  {
    sets.reset();
    cad.clearAll();
    transf.clearAll();
    beamIntegrators.clearAll();
    nodes.clearAll();
    elements.clearAll();
    if(domain)
      domain->clearAll();
    loads.clearAll();
    constraints.clearAll();
    materialHandler.clearAll();
  }

//! @brief Return a pointer to the database.
XC::FE_Datastore *XC::Preprocessor::getDataBase(void)
  {
    FE_Datastore *retval= nullptr;
    ProblemaEF *prb= dynamic_cast<ProblemaEF *>(Owner());
    if(prb)
      retval= prb->getDataBase();
    return retval;
  }

//! @brief Return a vector to store the dbTags
//! of the class members
XC::DbTagData &XC::Preprocessor::getDbTagData(void) const
  {
    static DbTagData retval(10);
    return retval;
  }

//! @brief Send data through the channel being passed as parameter.
int XC::Preprocessor::sendData(CommParameters &cp)
  {
    //res+= cp.sendMovable(materialHandler,getDbTagData(),CommMetaData(0));
    //res+= cp.sendMovable(transf,getDbTagData(),CommMetaData(1));
    //res+= cp.sendMovable(beamIntegrators,getDbTagData(),CommMetaData(2));
    //res+= cp.sendMovable(nodes,getDbTagData(),CommMetaData(3));
    //res+= cp.sendMovable(elements,getDbTagData(),CommMetaData(4));
    int res= cp.sendMovable(loads,getDbTagData(),CommMetaData(5));
    //res+= cp.sendMovable(constraints,getDbTagData(),CommMetaData(6));
    //res+= cp.sendMovable(cad,getDbTagData(),CommMetaData(7));
    assert(domain);
    res+= sendDomain(*domain,8,getDbTagData(),cp);
    res+= cp.sendMovable(sets,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::Preprocessor::recvData(const CommParameters &cp)
  {
    //res+= cp.receiveMovable(materialHandler,getDbTagData(),CommMetaData(0));
    //res+= cp.receiveMovable(transf,getDbTagData(),CommMetaData(1));
    //res+= cp.receiveMovable(beamIntegrators,getDbTagData(),CommMetaData(2));
    //res+= cp.receiveMovable(nodes,getDbTagData(),CommMetaData(3));
    //res+= cp.receiveMovable(elements,getDbTagData(),CommMetaData(4));
    int res= cp.receiveMovable(loads,getDbTagData(),CommMetaData(5));
    //res+= cp.receiveMovable(constraints,getDbTagData(),CommMetaData(6));
    //res+= cp.receiveMovable(cad,getDbTagData(),CommMetaData(7));
    assert(domain);
    res+= receiveDomain(*domain,8,getDbTagData(),cp);
    res+= cp.receiveMovable(sets,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Send object through the channel being passed as parameter.
int XC::Preprocessor::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(10);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receive object through the channel being passed as parameter.
int XC::Preprocessor::recvSelf(const CommParameters &cp)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

