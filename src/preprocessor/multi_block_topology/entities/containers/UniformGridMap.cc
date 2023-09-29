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
//UniformGridMap.cc

#include "UniformGridMap.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/multi_block_topology/entities/3d/UniformGrid.h"
#include "preprocessor/multi_block_topology/entities/2d/QuadSurface.h"
#include "preprocessor/set_mgmt/Set.h"
#include "utility/kernel/python_utils.h"

//! @brief Constructor.
XC::UniformGridMap::UniformGridMap(MultiBlockTopology *mbt)
  : EntityMap<UniformGrid>(mbt) {}

//! @brief Insert the new line in the total and the opened sets.
void XC::UniformGridMap::updateSets(UniformGrid *new_unif_grid) const
  {
    Preprocessor *preprocessor= const_cast<Preprocessor *>(getPreprocessor());
    preprocessor->get_sets().get_set_total()->getUniformGrids().push_back(new_unif_grid);
    preprocessor->get_sets().insert_ent_mdlr(new_unif_grid);
    MapSet::map_sets &open_sets= preprocessor->get_sets().get_open_sets();
    for(MapSet::map_sets::iterator i= open_sets.begin();i!= open_sets.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->getUniformGrids().push_back(new_unif_grid);
      }
  }

//! @brief New uniform grid.
XC::UniformGrid *XC::UniformGridMap::New(void)
  {
    UniformGrid *retval= busca(getTag());
    if(!retval) //new grid.
      {
        Preprocessor *preprocessor= getPreprocessor();
        retval= new UniformGrid(preprocessor);
        (*this)[getTag()]= retval;
        updateSets(retval);
        tag++;
      }
    return retval;
  }

//! @brief Creates a new point with the tag being passed as parameter.
XC::UniformGrid *XC::UniformGridMap::New(const size_t &tag)
  {
    size_t old_tag= getTag();
    UniformGrid *retval= nullptr;
    setTag(tag); //Point identifier.
    retval= busca(getTag());
    if(retval)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; point with identifier: " << tag
		  << " already exists. Command ignored." << std::endl;
      }
    else
      {	retval= New(); }
    setTag(old_tag);
    return retval;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::UniformGridMap::getPyDict(void) const
  {
    boost::python::dict retval= EntityMap<UniformGrid>::getPyDict();
    boost::python::dict point_dict;
    for(const_iterator i= this->begin(); i!= this->end(); i++)
      {
	const UniformGrid *pt= (*i).second;
	const int &tag= pt->getTag();
	const boost::python::dict item_dict= pt->getPyDict();
	point_dict[tag]= item_dict;
      }
    retval["points"]= point_dict; 
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::UniformGridMap::setPyDict(const boost::python::dict &d)
  {
    EntityMap<UniformGrid>::setPyDict(d);
    const boost::python::dict &point_dict= boost::python::extract<boost::python::dict>(d["points"]);
    boost::python::list items= point_dict.items();
    const boost::python::ssize_t sz= len(items);
    if(sz>0)
      {
	for(boost::python::ssize_t i=0; i<sz; i++)
	  {
	    const boost::python::object obj= boost::python::extract<boost::python::object>(items[i][0]);
	    const int tag= tag_integer_from_py_object(obj);
	    const boost::python::dict itemDict= boost::python::extract<boost::python::dict>(items[i][1]);
	    UniformGrid *tmp= New(tag);
	    if(tmp)
	      { tmp->setPyDict(itemDict); }
	    else
	      std::cerr << getClassName() << "::" << __FUNCTION__
		        << "; something went wrong when reading point: " << tag
		        << ". Command ignored." << std::endl;
	  }
      }
  }
