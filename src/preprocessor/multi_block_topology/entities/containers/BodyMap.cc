//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//BodyMap.cc

#include "BodyMap.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/multi_block_topology/entities/3d/Body.h"
#include "preprocessor/multi_block_topology/entities/3d/Block.h"
#include "preprocessor/multi_block_topology/entities/2d/QuadSurface.h"
#include "preprocessor/set_mgmt/Set.h"
#include "utility/kernel/python_utils.h"

//! @brief Constructor.
XC::BodyMap::BodyMap(MultiBlockTopology *mbt)
  : EntityMap<Body>(mbt) {}

//! @brief Creates a new body with the given identifier and the given type.
XC::Body *XC::BodyMap::New(const size_t &tag, const std::string &className)
  {
    Body *retval= nullptr;
    if(className=="XC::Block")
      retval= New<Block>(tag);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; unknown class: '" << className << "'\n";
    return retval;
  }

//! @brief Inserts the new body in the total set and in the set
//! that are open.
void XC::BodyMap::updateSets(Body *newBody) const
  {
    Preprocessor *preprocessor= const_cast<Preprocessor *>(getPreprocessor());
    preprocessor->get_sets().get_set_total()->getBodies().push_back(newBody);
    preprocessor->get_sets().insert_ent_mdlr(newBody);
    MapSet::map_sets &open_sets= preprocessor->get_sets().get_open_sets();
    for(MapSet::map_sets::iterator i= open_sets.begin();i!= open_sets.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->getBodies().push_back(newBody);
      }
  }

//! @brief Conciliates the number of divisions of the lines.
bool XC::BodyMap::conciliaNDivs(void)
  {
    getMultiBlockTopology()->conciliaNDivs();
    return checkNDivs();
  }

//! @brief Checks that the number of divisions of the edges
//! are compatible.
bool XC::BodyMap::checkNDivs(void) const
  {
    size_t conta= 0;
    if(!empty())
      for(const_iterator i= begin();i!=end();i++)
        if(!(*i).second->checkNDivs()) conta++;
    return (conta==0);
  }

//! @brief New block.
XC::Block *XC::BodyMap::newBlockPts(const size_t &id_p1, const size_t &id_p2, const size_t &id_p3, const size_t &id_p4, const size_t &id_p5, const size_t &id_p6, const size_t &id_p7, const size_t &id_p8)
  {
    Block *retval= dynamic_cast<Block *>(this->New<Block>());
    assert(retval);
    ID tmp(8);
    tmp[0]= id_p1; tmp[1]= id_p2; tmp[2]= id_p3; tmp[3]= id_p4;
    tmp[4]= id_p5; tmp[5]= id_p6; tmp[6]= id_p7; tmp[7]= id_p8;
    retval->setPoints(tmp);
    return retval;
  }

//! @brief Return the average volume of the bodies.
double XC::BodyMap::getAverageVolume(void) const
  {
    double retval= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      retval+= (*i).second->getVolume();
    retval/=(size());
    return retval;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::BodyMap::getPyDict(void) const
  {
    boost::python::dict retval= EntityMap<Body>::getPyDict();
    boost::python::dict body_dict;
    for(const_iterator i= this->begin(); i!= this->end(); i++)
      {
	const Body *body= (*i).second;
	const int &tag= body->getTag();
	const boost::python::dict item_dict= body->getPyDict();
	body_dict[tag]= item_dict;
      }
    retval["bodies"]= body_dict; 
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::BodyMap::setPyDict(const boost::python::dict &d)
  {
    EntityMap<Body>::setPyDict(d);
    const boost::python::dict &body_dict= boost::python::extract<boost::python::dict>(d["bodies"]);
    boost::python::list items= body_dict.items();
    const boost::python::ssize_t sz= len(items);
    if(sz>0)
      {
	for(boost::python::ssize_t i=0; i<sz; i++)
	  {
	    const boost::python::object obj= boost::python::extract<boost::python::object>(items[i][0]);
	    const int tag= tag_integer_from_py_object(obj);
	    const boost::python::dict itemDict= boost::python::extract<boost::python::dict>(items[i][1]);
	    const std::string className= boost::python::extract<std::string>(itemDict["className"]);
	    Body *tmp= New(tag, className);
	    if(tmp)
	      { tmp->setPyDict(itemDict); }
	    else
	      std::cerr << getClassName() << "::" << __FUNCTION__
		        << "; something went wrong when reading body: " << tag
		        << ". Command ignored." << std::endl;
	  }
      }
  }
