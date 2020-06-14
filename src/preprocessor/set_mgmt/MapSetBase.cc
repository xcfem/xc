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
//MapSetBase.cc

#include "MapSetBase.h"
#include "Set.h"
#include "SetEstruct.h"
#include "preprocessor/multi_block_topology/entities/EntMdlr.h"


#include "utility/actor/actor/MovableID.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

std::deque<std::string> XC::MapSetBase::setsClassNames;

//! @brief Return true if the sets already exists.
bool XC::MapSetBase::exists(const std::string &name) const
  { return (map_sets::find(name)!=map_sets::end()); }

//! @brief Returns a pointer to the set which name is being passed as parameter.
XC::SetBase *XC::MapSetBase::find(const std::string &name) const
  {
    XC::SetBase *retval= nullptr;
    if(exists(name))
      {
    	const_iterator j= map_sets::find(name);
        if(j != end())
          retval= (*j).second;
        else
          retval= nullptr;
      }
    return retval;
  }

//! @bref Returns a copy of the argument.
XC::SetEstruct *XC::MapSetBase::create_set_estruct(const SetEstruct &set_estruct)
  {
    const std::string name= set_estruct.getName();
    if(exists(name)) //The set exists
      {
	std::cerr << "MapSetBase::" << __FUNCTION__
		  << "; the set: " << name
		  << " already exists. Doing nothing." << std::endl;
        return nullptr;
      }
    else //the set is new.
      {
	SetEstruct *retval= set_estruct.getCopy();
        if(retval) (*this)[name]= retval;
        return retval;
      }
  }

//! @brief Returns a pointer to the structured set named as the parameter.
XC::SetEstruct *XC::MapSetBase::find_set_estruct(const std::string &name)
  {
    SetBase *set= find(name);
    SetEstruct *retval= nullptr;
    if(set)
      retval= dynamic_cast<SetEstruct *>(set);
    return retval;
  }

//! @brief Copy constructor (DOESN'T COPY SETS).
XC::MapSetBase::MapSetBase(const MapSetBase &other)
  : map_sets()
  {}

//! @brief Assignment operator (DOESN'T COPY SETS).
XC::MapSetBase &XC::MapSetBase::operator=(const MapSetBase &other)
  {
    return *this;
  }

//! @brief Deletes the set and removes it from the sets map.
void XC::MapSetBase::remove(const std::string &name)
  {
    Set *tmp= dynamic_cast<Set *>((*this)[name]);
    if(tmp) //Set exists.
      {
        (*this)[name]= nullptr;
        delete tmp;
        map_sets::erase(name);
      }
    else
      std::cerr << "MapSetBase::" << __FUNCTION__
		<< "; ERROR set named: "
                << name << " not found." << std::endl;
  }

//! @brief Rename the set.
void XC::MapSetBase::rename(const std::string &oldKey, const std::string &newKey)
  {
    if(oldKey!=newKey)
      {
	const iterator itOld= map_sets::find(oldKey);
	if(itOld != end())
	  {
	    // Swap value from oldKey to newKey.
	    SetBase *set_ptr= itOld->second;
	    auto const value= std::move(set_ptr);
	    const iterator itNew= map_sets::find(newKey);
	    if(itNew==end())
	      {
		// Erase old key-value from map
		erase(itOld);
		(*this).insert({newKey, std::move(value)});
		set_ptr->setName(newKey);
	      }
	    else
	      std::cerr << "MapSetBase::" << __FUNCTION__
			<< " key: '" << newKey << "' already exists."
			<< " Can't rename the set." << std::endl;

	  }
	else
	  std::cerr << "MapSetBase::" << __FUNCTION__
		    << " key: '" << oldKey << "' not found."
		    << std::endl;
      }
  }

//! @brief Clears all defined sets.
void XC::MapSetBase::clearSets(void)
  {
    for(iterator i= begin();i!=end();i++)
      {
        delete (*i).second;
        (*i).second= nullptr;
      }
    map_sets::clear();
  }

//! @brief Destructor.
XC::MapSetBase::~MapSetBase(void)
  { clearSets(); }

//! @brief Return the DBTags of the sets.
const XC::ID &XC::MapSetBase::getSetsDBTags(Communicator &comm)
  {
    static ID retval;
    const int sz= size();
    if(sz>0)
      {
        retval.resize(sz);
        int loc =0;
        // loop over sets in map adding their dbTag to the ID
        for(const_iterator i= begin();i!=end();i++)
          {
            retval[loc]= (*i).second->getDbTag(comm);
            loc++;
          }        
      }
    return retval;
  }

//! @brief Return the class names of the sets.
const std::deque<std::string> &XC::MapSetBase::getSetsNames(void) const
  {
    const int sz= size();
    if(sz>0)
      {
        setsClassNames.resize(sz);
        int loc =0;
        // loop over sets in map adding their classTag to the ID
        for(const_iterator i= begin();i!=end();i++)
          {
            setsClassNames[loc]= (*i).second->getName();
            loc++;
          }
      }
    return setsClassNames;
  }

//! @brief Return the class names of the sets.
boost::python::list XC::MapSetBase::getSetsNamesPy(void) const
  {
    boost::python::list retval;
    const std::deque<std::string> &tmp= getSetsNames();
    for(std::deque<std::string>::const_iterator i= tmp.begin();i!=tmp.end();i++)
      retval.append(*i);
    return retval;
  }

//! @brief Return the map keys.
const std::deque<std::string> &XC::MapSetBase::getKeys(void) const
  {
    const int sz= size();
    if(sz>0)
      {
        setsClassNames.resize(sz);
        int loc =0;
	for(const_iterator i= begin();i!=end();i++)
	  {
	    const std::string &key= (*i).first;
            setsClassNames[loc]= key;
	    loc++;
	    const std::string name= (*i).second->getName();
	    if(key!=name)
	      std::cerr << "MapSetBase::" << __FUNCTION__
			<< " key: '" << key << "' and name: '"
			<< name << "' doesn't match." << std::endl;
	  }
      }
    return setsClassNames;
  }

//! @brief Return the map keys in a Python list.
boost::python::list XC::MapSetBase::getKeysPy(void) const
  {
    boost::python::list retval;
    const std::deque<std::string> &tmp= getKeys();
    for(std::deque<std::string>::const_iterator i= tmp.begin();i!=tmp.end();i++)
      retval.append(*i);
    return retval;
  }

//! @brief Return the class names of the sets.
const std::deque<std::string> &XC::MapSetBase::getSetsClassNames(void) const
  {
    const int sz= size();
    if(sz>0)
      {
        setsClassNames.resize(sz);
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

//! @brief Return the sets that contain the pointer to node
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSetBase::get_sets(const Node *n)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(n)) retval.insert((*i).second);
    return retval;
  }

//! @brief Return the sets that contain the element pointer
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSetBase::get_sets(const Element *e)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(e)) retval.insert((*i).second);
    return retval;
  }

//! @brief Return the sets that contain the pointer to the ponint
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSetBase::get_sets(const Pnt *p)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(p)) retval.insert((*i).second);
    return retval;
  }

//! @brief Return the sets that contain the pointer to edge
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSetBase::get_sets(const Edge *e)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(e)) retval.insert((*i).second);
    return retval;
  }

//! @brief Return the sets than contain a pointer to face
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSetBase::get_sets(const Face *f)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(f)) retval.insert((*i).second);
    return retval;
  }

//! @brief Return the sets that contain the pointer to body
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSetBase::get_sets(const Body *b)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(b)) retval.insert((*i).second);
    return retval;
  }

//! @brief Return the sets that contain the pointer to uniform grid
//! being passed as parameter.
std::set<XC::SetBase *> XC::MapSetBase::get_sets(const UniformGrid *ug)
  {
    std::set<SetBase *> retval;
    for(iterator i= begin();i!=end();i++)
      if((*i).second->In(ug)) retval.insert((*i).second);
    return retval;
  }
