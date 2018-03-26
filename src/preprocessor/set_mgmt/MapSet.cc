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
//MapSet.cc

#include "MapSet.h"
#include "Set.h"
#include "SetEstruct.h"
#include "preprocessor/cad/entidades/EntMdlr.h"
#include "preprocessor/Preprocessor.h"


#include "utility/actor/actor/MovableID.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

XC::ID XC::MapSet::setsDbTags;
std::deque<std::string> XC::MapSet::setsClassNames;

//! @brief Return true if the sets already exists.
bool XC::MapSet::exists(const std::string &nmb) const
  { return (sets.find(nmb)!=sets.end()); }

//! @brief Returns a pointer to the set which name is being passed as parameter.
XC::SetBase *XC::MapSet::busca_set(const std::string &nmb)
  {
    if(exists(nmb))
      return sets[nmb];
    else if(entidades.find(nmb)!=entidades.end())
      return entidades[nmb];
    else
      return nullptr;
  }

//! @brief Returns a pointer to the set which name is being passed as parameter.
const XC::SetBase *XC::MapSet::busca_set(const std::string &nmb) const
  {
    const_iterator i= sets.find(nmb);
    if(i!=sets.end())
      return (*i).second;
    else
      {
    	map_ent_mdlr::const_iterator j= entidades.find(nmb);
        if(j != entidades.end())
          return (*j).second;
        else
          return nullptr;
      }
  }

//! @brief Returns the set which name is being passed as parameter.
XC::SetBase &XC::MapSet::getSet(const std::string &nmb)
  {
    SetBase *retval= busca_set(nmb);
    if(!retval)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; set named '"
                  << nmb << "' not found. Total set returned." << std::endl;
        retval= total;
      }
    return *retval;
  }


XC::MapSet::const_iterator XC::MapSet::begin(void) const
  { return sets.begin(); }

XC::MapSet::const_iterator XC::MapSet::end(void) const
  { return sets.end(); }

XC::MapSet::iterator XC::MapSet::begin(void)
  { return sets.begin(); }

XC::MapSet::iterator XC::MapSet::end(void)
  { return sets.end(); }

//! @brief Creates a new set con el nombre que se le being passed as parameter.
XC::Set *XC::MapSet::create_set(const std::string &nmb)
  {
    Set *tmp =nullptr;
    if(!exists(nmb)) //Set is new.
      {
        tmp= new Set(nmb,getPreprocessor());
        sets[nmb]= tmp;
      }
    else //Set already exists
      tmp= dynamic_cast<Set *>(busca_set(nmb));
    return tmp;
  }

void XC::MapSet::abre_set(const std::string &nmb)
  {
    SetBase *tmp =nullptr;
    if(!exists(nmb)) //Set doesn't exists.
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; the set: '"
                << nmb << "' doesn't exists.\n";
    else //The set exists
      {
        tmp= busca_set(nmb);
        assert(tmp);
        abiertos[nmb]= tmp;
      }
  }
void XC::MapSet::cierra_set(const std::string &nmb)
  {
    if(!exists(nmb)) //Set doesn't exists.
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; the set: '"
                << nmb << "' doesn't exists.\n";
    else //The set exists
      {
	iterator i= abiertos.find(nmb);
        if(i!= abiertos.end())
          abiertos.erase(i);
        else
          if(verbosity>1)
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; the set: '"
                      << nmb << "' is already closed.\n";
      }
  }

//! @bref Creates a structured set copia del que se le being passed as parameter.
XC::SetEstruct *XC::MapSet::create_set_estruct(const SetEstruct &set_estruct)
  {
    const std::string nmb= set_estruct.getName();
    if(exists(nmb)) //The set exists
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the set: " << nmb
		  << " already exists. Doing nothing." << std::endl;
        return nullptr;
      }
    else //the set is new.
      {
	SetEstruct *retval= set_estruct.getCopy();
        if(retval) sets[nmb]= retval;
        return retval;
      }
  }

//! @bref Creates a copy of the structured set being passed as parameter.
XC::SetBase *XC::MapSet::broke_set(const std::string &nmb,const std::string &nmb_clase)
  {
    SetBase *retval= nullptr;
    if(exists(nmb)) //The set exists
      retval= busca_set(nmb);
    else //the set is new.
      {
        if(nmb_clase == "XC::Set")
          retval= new Set(nmb,getPreprocessor());
//         else if(nmb_clase == "XC::SetEstruct")
//           retval= new SetEstruct(nmb,getPreprocessor());
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; class name: '"
                    << nmb_clase << "' unknown." << std::endl;
      }
    return retval;
  }

//! @bref Inserts, as a set, a preprocessor entity.
XC::EntMdlr *XC::MapSet::inserta_ent_mdlr(EntMdlr *ent_mdlr)
  {
    assert(ent_mdlr);
    const std::string nmb= ent_mdlr->getName();
    if(exists(nmb)) //The set exists
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "the set: " << nmb
		  << " already exists. New set not inserted." << std::endl;
        return nullptr;
      }
    else //the set is new.
      {
        entidades[nmb]= ent_mdlr;
        return ent_mdlr;
      }
  }

//! @brief Returns a pointer to the set estructurado which name is being passed as parameter.
XC::SetEstruct *XC::MapSet::busca_set_estruct(const std::string &nmb)
  {
    SetBase *set= busca_set(nmb);
    SetEstruct *retval= nullptr;
    if(set)
      retval= dynamic_cast<SetEstruct *>(set);
    return retval;
  }

//! @brief Inicializa the set total.
void XC::MapSet::setup_total(void)
  {
    const std::string str_tot= "total";
    total= create_set(str_tot);
  }

//! @brief Default constructor.
XC::MapSet::MapSet(Preprocessor *prep)
  : PreprocessorContainer(prep), MovableObject(0), total(nullptr)
  { setup_total(); }

//! @brief Copy constructor (DOESN'T COPY SETS).
XC::MapSet::MapSet(const MapSet &otro)
  : PreprocessorContainer(otro), MovableObject(otro), total(nullptr)
  { setup_total(); }

//! @brief Assignment operator (DOESN'T COPY SETS).
XC::MapSet &XC::MapSet::operator=(const MapSet &otro)
  {
    PreprocessorContainer::operator=(otro);
    MovableObject::operator=(otro);
    setup_total();
    return *this;
  }

//! @brief Creates a new set with the name which is passed as a parameter.
XC::Set *XC::MapSet::defSet(const std::string &nmb)
  { return create_set(nmb); }

//! @brief Deletes the set and removes it from the sets map.
void XC::MapSet::removeSet(const std::string &nmb)
  {
    Set *tmp= dynamic_cast<Set *>(sets[nmb]);
    if(tmp) //Set exists.
      {
        sets[nmb]= nullptr;
        delete tmp;
        sets.erase(nmb);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ERROR set named: "
                << nmb << " not found." << std::endl;
  }

//! @brief Clears all defined sets.
void XC::MapSet::clearSets(void)
  {
    for(iterator i= begin();i!=end();i++)
      {
        delete (*i).second;
        (*i).second= nullptr;
      }
    total= nullptr;
    sets.clear();
  }

//! @brief Clears all.
void XC::MapSet::clearAll(void)
  {
    total= nullptr;
    entidades.clear();
    clearSets();
  }

//! @brief Clears all.
void XC::MapSet::reset(void)
  {
    clearAll();
    setup_total();
  }

//! @brief Destructor.
XC::MapSet::~MapSet(void)
  { clearAll(); }

//! @brief Returns the DBTags of the sets.
const XC::ID &XC::MapSet::getSetsDBTags(CommParameters &cp)
  {
    static ID retval;
    const int size= sets.size();
    if(size>0)
      {
        retval.resize(size);
        int loc =0;
        // loop over sets in map adding their dbTag to the ID
        for(const_iterator i= begin();i!=end();i++)
          {
            retval[loc]= (*i).second->getDbTag(cp);
            loc++;
          }        
      }
    return retval;
  }

//! @brief Returns the nombres de clase of the sets.
const std::deque<std::string> &XC::MapSet::getSetsClassNames(void)
  {
    const int size= sets.size();
    if(size>0)
      {
        setsClassNames.resize(size);
        int loc =0;
        // loop over sets in map adding their classTag to the ID
        for(const_iterator i= begin();i!=end();i++)
          {
            setsClassNames[loc]= (*i).second->getClassName();
            loc++;
          }
      }
    return setsClassNames;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::MapSet::getDbTagData(void) const
  {
    static DbTagData retval(6);
    return retval;
  }

//! @brief Envía los dbTags of the sets través del canal being passed as parameter.
int XC::MapSet::sendSetsDbTags(int posDbTag,CommParameters &cp)
  {
    const int size= sets.size();
    int res= 0;
    if(size>0)
      {
        setsDbTags= getSetsDBTags(cp);
        res+= cp.sendID(setsDbTags,getDbTagData(),CommMetaData(posDbTag));
      }
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ch failed to send the IDs.\n";
    return res;
  }

//! @brief Envía los nombres de clase of the sets través del canal being passed as parameter.
int XC::MapSet::sendSetsClassNames(int posDbTag,CommParameters &cp)
  {
    const int size= sets.size();
    int res= 0;
    if(size>0)
      {
        setsClassNames= getSetsClassNames();
        res+= cp.sendStrings(setsClassNames,getDbTagData(),CommMetaData(posDbTag));
      }
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ch failed to send the IDs.\n";
    return res;
  }

//! @brief Recibe los dbTags of the sets through the channel being passed as parameter.
int XC::MapSet::receiveSetsDbTags(int posDbTag,int size,const CommParameters &cp)
  {
    setsDbTags.resize(size);
    int res= 0;
    if(size>0)
      res= cp.receiveID(setsDbTags,getDbTagData(),CommMetaData(posDbTag));
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ch failed to receive the IDs.\n";
    return res;
  }

//! @brief Recibe los nombres de clase of the sets through the channel being passed as parameter.
int XC::MapSet::receiveSetsClassNames(int posDbTag,int size,const CommParameters &cp)
  {
    setsClassNames.resize(size);
    int res= 0;
    if(size>0)
      res= cp.receiveStrings(setsClassNames,getDbTagData(),CommMetaData(posDbTag));
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ch failed to receive the IDs.\n";
    return res;
  }

//! @brief Envía the sets definidos through the channel being passed as parameter.
int XC::MapSet::sendSets(int posDbTag1, int posDbTag2, int posDbTag3,CommParameters &cp)
  {
    std::deque<std::string> nombres;
    for(const_iterator i= begin();i!=end();i++)
      nombres.push_back((*i).first);
    DbTagData &dt= getDbTagData();
    int res= cp.sendStrings(nombres,dt,CommMetaData(posDbTag1));
    res+= sendSetsDbTags(posDbTag2,cp);
    res+= sendSetsClassNames(posDbTag3,cp);
    int loc= 0;
    DbTagData tags(setsDbTags);
    for(iterator i= begin();i!=end();i++,loc++)
      res+= cp.sendMovable(*(*i).second,tags,CommMetaData(loc));
    return res;
  }

//! @brief Recibe the sets definidos through the channel being passed as parameter.
int XC::MapSet::receiveSets(int posDbTag1, int posDbTag2, int posDbTag3,const int &sz,const CommParameters &cp)
  {
    std::deque<std::string> nombres;
    int res= cp.receiveStrings(nombres,getDbTagData(),CommMetaData(posDbTag1));
    res+= receiveSetsDbTags(posDbTag2,sz,cp);
    res+= receiveSetsClassNames(posDbTag3,sz,cp);
    int loc= 0;
    DbTagData tags(setsDbTags);
    for(std::deque<std::string>::iterator i= nombres.begin();i!=nombres.end();i++,loc++)
      {
        SetBase *tmp= broke_set(*i,setsClassNames[loc]);
        if(tmp)
          cp.receiveMovable(*tmp,tags,CommMetaData(loc));
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; error receiving set: '" << *i << "'.\n";
      }
    return res;
  }
//! @brief Send members through the channel being passed as parameter.
int XC::MapSet::sendAbiertos(int posDbTag1, int posDbTag2,CommParameters &cp)
  {
    const size_t sz= abiertos.size();
    int res= cp.sendInt(sz,getDbTagData(),CommMetaData(posDbTag1));
    if(!abiertos.empty())
      {
        std::deque<std::string> nmb_abiertos;
        for(const_iterator i= abiertos.begin();i!=abiertos.end();i++)
          nmb_abiertos.push_back((*i).first);
        res+= cp.sendStrings(nmb_abiertos,getDbTagData(),CommMetaData(posDbTag2));
      }
    return res;
  }
//! @brief Receives members through the channel being passed as parameter.
int XC::MapSet::receiveAbiertos(int posDbTag1, int posDbTag2,const CommParameters &cp)
  {
     int sz_abiertos= 0;
     int res= cp.receiveInt(sz_abiertos,getDbTagData(),CommMetaData(posDbTag1));
     if(sz_abiertos>0)
       {
         std::deque<std::string> nmb_abiertos;
         res+= cp.receiveStrings(nmb_abiertos,getDbTagData(),CommMetaData(posDbTag2));
         for(std::deque<std::string>::const_iterator i= nmb_abiertos.begin();i!=nmb_abiertos.end();i++)
           abre_set(*i);
       }
    return res;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::MapSet::sendData(CommParameters &cp)
  {
    const size_t sz= sets.size();
    int res= cp.sendInt(sz,getDbTagData(),CommMetaData(0)); //number of sets
    if(sz>0)
      {
        res+= sendSets(1,2,3,cp);
        res+= sendAbiertos(4,5,cp);
      }
    //XX Entities sending (points, lines, surfaces,...) pending.
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::MapSet::recvData(const CommParameters &cp)
  {
    int sz= 0;
    int res= cp.receiveInt(sz,getDbTagData(),CommMetaData(0)); //Number of sets.
    if(sz>0)
      {
        res+= receiveSets(1,2,3,sz,cp);
        res+= receiveAbiertos(4,5,cp);
      }
    //XX Entities receiving (points, lines, surfaces,...) pending.
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::MapSet::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(6);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::MapSet::recvSelf(const CommParameters &cp)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief Returns the sets que contienen the pointer to node
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSet::get_sets(const Node *n)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(n)) retval.insert((*i).second);
    return retval;
  }

//! @brief Returns the sets that containt the element pointer
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSet::get_sets(const Element *e)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(e)) retval.insert((*i).second);
    return retval;
  }

//! @brief Returns the sets que contienen the pointer a punto
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSet::get_sets(const Pnt *p)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(p)) retval.insert((*i).second);
    return retval;
  }

//! @brief Returns the sets que contienen the pointer a «edge»
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSet::get_sets(const Edge *e)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(e)) retval.insert((*i).second);
    return retval;
  }

//! @brief Returns the sets than contain a pointer to the face
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSet::get_sets(const Face *f)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(f)) retval.insert((*i).second);
    return retval;
  }

//! @brief Returns the sets que contienen the pointer a cuerpo
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSet::get_sets(const Body *b)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(b)) retval.insert((*i).second);
    return retval;
  }

//! @brief Returns the sets que contienen the pointer a «uniform grid»
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSet::get_sets(const UniformGrid *ug)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(ug)) retval.insert((*i).second);
    return retval;
  }
