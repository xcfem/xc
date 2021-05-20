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
//FiberSets.cc

#include "FiberSets.h"
#include "utility/actor/actor/MovableMap.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"

//! @brief Constructor.
XC::FiberSets::FiberSets(void)
  : std::map<std::string,FiberSet>(), MovableObject(0)
  {}

//! @brief Return true if the set already exists.
bool XC::FiberSets::exists(const std::string &name)
  {
    bool retval= false;
    if(find(name) != end()) //Set exists
      retval= true;
    return retval;
  }

//! @brief Creates a new fiber set.
XC::FiberSet &XC::FiberSets::create_fiber_set(const std::string &name)
  {
    FiberSet *retval= nullptr;
    if(!exists(name)) //Set doesn't exists
      retval= &((*this)[name]= FiberSet());
    else
      {
	retval= &((*this)[name]);
	retval->clear(); //Set redefinition.
      }
    return *retval;
  }

//! @brief Creates a fiber set which name is being passed as parameter.
XC::FiberSets::iterator XC::FiberSets::get_fiber_set(const std::string &set_name)
  {
    iterator i= find(set_name);
    if(i == end())
      {
        if(verbosity>1)
          std::clog << "Fiber set: '" << set_name
                    << "' doesn't exists; it is created." << std::endl;
        create_fiber_set(set_name);
        i= find(set_name);
      }
    return i;
  }

//! @brief Creates a fiber set which material has the tag being passed as parameter.
XC::FiberSets::iterator XC::FiberSets::sel_mat_tag(FiberSet &fibers, const std::string &set_name,const int &matTag)
  {
    iterator i= get_fiber_set(set_name);
    fibers.SelMatTag(matTag,(*i).second);
    return i;
  }

//! @brief Creates a fiber set with the fibers that belongs to the set named
//! origin_set_name, and have the material identified with the tag being passed as parameter.
//! @param set_name: name of the new set.
//! @param origin_set_name: name of the set that contains the fibers.
//! @matTag: material tag.
XC::FiberSets::iterator XC::FiberSets::resel_mat_tag(const std::string &set_name,const std::string &origin_set_name,const int &matTag)
  {
    iterator i= end();
    if(set_name != origin_set_name)
      {
        i= get_fiber_set(set_name);
        iterator j= find(origin_set_name);
        if(j == end())
          {
            std::clog << "Origin fiber set: '" << origin_set_name
                      << "' doesn't exists; command ignored." << std::endl;
          }
        else
          (*j).second.SelMatTag(matTag,(*i).second);
      }
    return i;
  }

//! @brief Send object members through the communicator argument.
int XC::FiberSets::sendData(Communicator &comm)
  {
    const size_t sz= size();
    setDbTagDataPos(0,sz);
    int res= 0;
    if(sz>0)
      {
        DbTagData labelData(sz);
        DbTagData dbTags(sz);
        int loc= 0;
        for(iterator i=begin();i!=end();i++,loc++)
          {
	    const std::string &label= (*i).first;
            res+= comm.sendString(label,labelData,CommMetaData(loc));
	    FiberSet &object= (*i).second;
            res+= comm.sendMovable(object,dbTags,CommMetaData(loc));
          }
        res+= labelData.send(getDbTagData(),comm,CommMetaData(1));
        res+= dbTags.send(getDbTagData(),comm,CommMetaData(2));
      }
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::FiberSets::recvData(const Communicator &comm)
  {
    const size_t sz= getDbTagDataPos(0);
    int res= 0;
    if(sz>0)
      {
        DbTagData labelData(sz);
        int res= labelData.receive(getDbTagData(),comm,CommMetaData(1));
        DbTagData dbTags(sz);
        res+= dbTags.receive(getDbTagData(),comm,CommMetaData(2));
        std::string label;
        FiberSet tmp;
        for(size_t i= 0;i<sz;i++)
          {
            res+= comm.receiveString(label,labelData,CommMetaData(i));
            res+= comm.receiveMovable(tmp,dbTags,CommMetaData(i));
            (*this)[label]= tmp;
          }
      }
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::FiberSets::sendSelf(Communicator &comm)
  {
    inicComm(2);
    int res= sendData(comm);
    const int dataTag=getDbTag();
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << dataTag << " failed to send ID";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::FiberSets::recvSelf(const Communicator &comm)
  {
    const int dataTag= this->getDbTag();
    inicComm(2);
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << dataTag << " failed to receive ID\n";
    else
      res+= recvData(comm);
    return res;
  }
