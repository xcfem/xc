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
//LineMap.cc

#include "LineMap.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/multi_block_topology/entities/1d/Edge.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/multi_block_topology/entities/1d/Line.h"
#include "preprocessor/multi_block_topology/entities/1d/DividedLine.h"
#include "preprocessor/multi_block_topology/entities/1d/CmbEdge.h"
#include "preprocessor/multi_block_topology/entities/1d/CircularArc.h"
#include "preprocessor/set_mgmt/Set.h"
#include "utility/kernel/python_utils.h"

//! @brief Constructor.
XC::LineMap::LineMap(MultiBlockTopology *mbt)
  : EntityMap<Edge>(mbt) {}

//! @brief Creates a new edge with the given identifier and the given type.
XC::Edge *XC::LineMap::New(const size_t &tag, const std::string &className)
  {
    Edge *retval= nullptr;
    if(className=="XC::Line")
      retval= New<Line>(tag);
    else if(className=="XC::DividedLine")
      retval= New<DividedLine>(tag);
    else if(className=="XC::CmbEdge")
      retval= New<CmbEdge>(tag);
    else if(className=="XC::CircularArc")
      retval= New<CircularArc>(tag);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; unknown class: '" << className << "'\n";
    return retval;
  }

//! @brief Line segment.
XC::Line *XC::LineMap::newLine(const size_t &id_p1, const size_t &id_p2)
  {
    Preprocessor *preprocessor= getPreprocessor();
    assert(preprocessor);
    MultiBlockTopology &mbt= preprocessor->getMultiBlockTopology();
    Pnt *p1= mbt.getPoints().busca(id_p1);
    Pnt *p2= mbt.getPoints().busca(id_p2);
    Line *retval= dynamic_cast<Line *>(createLine(p1,p2));
    assert(retval);
    return retval;
  }

//! @brief Divided line.
XC::DividedLine *XC::LineMap::newDividedLine(const size_t &id_p1, const size_t &id_p2)
  {
    Preprocessor *preprocessor= getPreprocessor();
    assert(preprocessor);
    MultiBlockTopology &mbt= preprocessor->getMultiBlockTopology();
    Pnt *p1= mbt.getPoints().busca(id_p1);
    Pnt *p2= mbt.getPoints().busca(id_p2);
    DividedLine *retval= dynamic_cast<DividedLine *>(createDividedLine(p1,p2));
    assert(retval);
    return retval;
  }

//! @brief Circle arc.
XC::CircularArc *XC::LineMap::newCircleArc(const size_t &id_p1, const size_t &id_p2, const size_t &id_p3)
  {
    Preprocessor *preprocessor= getPreprocessor();
    assert(preprocessor);
    MultiBlockTopology &mbt= preprocessor->getMultiBlockTopology();
    Pnt *p1= mbt.getPoints().busca(id_p1);
    Pnt *p2= mbt.getPoints().busca(id_p2);
    Pnt *p3= mbt.getPoints().busca(id_p3);
    CircularArc *retval= dynamic_cast<CircularArc *>(createArc(p1,p2,p3));
    assert(retval);
    return retval;
  }


//! @brief Line sequence.
XC::CmbEdge *XC::LineMap::newLineSequence(void)
  {
    CmbEdge *retval= dynamic_cast<CmbEdge *>(createLineSequence());
    assert(retval);
    return retval;
  }

//! @brief Insert the new line in the total and the opened sets.
void XC::LineMap::updateSets(Edge *nueva_linea) const
  {
    Preprocessor *preprocessor= const_cast<Preprocessor *>(this->getPreprocessor());
    preprocessor->get_sets().get_set_total()->getLines().push_back(nueva_linea);
    preprocessor->get_sets().insert_ent_mdlr(nueva_linea);
    MapSet::map_sets &open_sets= preprocessor->get_sets().get_open_sets();
    for(MapSet::map_sets::iterator i= open_sets.begin();i!= open_sets.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->getLines().push_back(nueva_linea);
      }
  }

//! @brief Find a line between the points or creates a new one.
//! and inserts it on the container
//! @param pA: pointer to back end of the line.
//! @param pB: pointer to front end of the line.
XC::Edge *XC::LineMap::createLine(Pnt *pA,Pnt *pB)
  {
    Edge *tmp= nullptr;
    if(pA && pB)
      {
        if(pA==pB)
	  std::cerr << getClassName() << __FUNCTION__
	            << "; ends of the line: ("
                    << pA->getName() << ","
                    << pB->getName() 
                    << "), are the same." << std::endl;
        tmp= find_edge_ptr_by_endpoints(*pA,*pB);
        if(!tmp) //Line doesn't exists.
          {
            assert(getPreprocessor());
            tmp= New<Line>();
            assert(tmp);
            tmp->SetVertice(1,pA);
            tmp->SetVertice(2,pB);
          }
        if(!tmp)
	  std::cerr << getClassName() << __FUNCTION__
		    << "; can't get a line"
                    << " between points: " << pA->getName()
                    << " and " << pB->getName() << std::endl;
      }
    else
      std::cerr << getClassName() << __FUNCTION__
		<< "; error, null pointer to point (A, B or both)."
		<< std::endl;
    return tmp;
  }

//! @brief Creates a new line between the points being passed as parameters
//! and inserts it on the container
//! @param pA: pointer to back end of the line.
//! @param pB: pointer to front end of the line.
XC::Edge *XC::LineMap::createDividedLine(Pnt *pA,Pnt *pB)
  {
    Edge *tmp= nullptr;
    if(pA && pB)
      {
        if(pA==pB)
	  std::cerr << getClassName() << __FUNCTION__
	            << "; ends of the line: ("
                    << pA->getName() << ","
                    << pB->getName() 
                    << "), are the same." << std::endl;
        tmp= find_edge_ptr_by_endpoints(*pA,*pB);
        if(!tmp)
          {
            assert(getPreprocessor());
            tmp= New<DividedLine>();
            assert(tmp);
            tmp->SetVertice(1,pA);
            tmp->SetVertice(2,pB);
          }
        if(!tmp)
	  std::cerr << getClassName() << __FUNCTION__
		    << "; can't get a line"
                    << " between points: " << pA->getName()
                    << " and " << pB->getName() << std::endl;
      }
    else
      std::cerr << getClassName() << __FUNCTION__
		<< "; error, null pointer to point (A, B or both)."
		<< std::endl;
    return tmp;
  }

//! @brief Creates a new arc of circle between the points
//! being passed as parameters and inserts it in the edge set.
XC::Edge *XC::LineMap::createArc(Pnt *pA,Pnt *pB,Pnt *pC)
  {
    Edge *tmp= nullptr;
    if(pA && pB && pC)
      {
        tmp= find_edge_ptr_by_endpoints(*pA,*pB,*pC);
        if(!tmp)
          {
            assert(getPreprocessor());
            tmp= New<CircularArc>();
            assert(tmp);
            tmp->SetVertice(1,pA);
            tmp->SetVertice(2,pC);
            tmp->SetVertice(3,pB); //intermediate point.
          }
        if(!tmp)
	  std::cerr << getClassName() << __FUNCTION__
		    << "; can't get an arc"
                    << " between the points: "
		    << pA->getName() << ", " << pB->getName()
                    << " and " << pC->getName() << std::endl;
      }
    else
      std::cerr << getClassName() << __FUNCTION__
		<< "; error, null pointer to point (A, B and/or C)."
		<< std::endl;
    return tmp;
  }

//! @brief Creates a line sequence (polyline) with those being
//! passed as parameters and inserts it in the edge set.
XC::Edge *XC::LineMap::createLineSequence(void)
  {
    Edge *tmp= New<CmbEdge>();
    assert(tmp);
    return tmp;
  }

//! @brief Return a copy of the argument edge.
XC::Edge *XC::LineMap::createCopy(const Edge *l)
  {
    Edge *retval= busca(getTag());
    if(retval)
      std::cerr << getClassName() << __FUNCTION__
	        << "; line identified by: " 
                << getTag() << " already exist, do nothing." << std::endl;
    else //Line is new.
      {
        retval= dynamic_cast<Edge *>(l->getCopy());
        if(retval)
          {
            retval->setName("l"+boost::lexical_cast<std::string>(getTag()));
            (*this)[getTag()]= retval;
            updateSets(retval);
            tag++;
	  }
        else
	  std::cerr << getClassName() << __FUNCTION__
	            << "; memory exhausted or the object: '"
                    << l->getName() << "is not a line." << std::endl; 
      }
    return retval;
  }


//! @brief Return the average length of the surfaces.
double XC::LineMap::getAverageLength(void) const
  {
    double retval= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      retval+= (*i).second->getLength();
    retval/=(size());
    return retval;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::LineMap::getPyDict(void) const
  {
    boost::python::dict retval= EntityMap<Edge>::getPyDict();
    boost::python::dict edge_dict;
    for(const_iterator i= this->begin(); i!= this->end(); i++)
      {
	const Edge *edge= (*i).second;
	const int &tag= edge->getTag();
	const boost::python::dict item_dict= edge->getPyDict();
	edge_dict[tag]= item_dict;
      }
    retval["edges"]= edge_dict; 
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::LineMap::setPyDict(const boost::python::dict &d)
  {
    EntityMap<Edge>::setPyDict(d);
    const boost::python::dict &edge_dict= boost::python::extract<boost::python::dict>(d["edges"]);
    boost::python::list items= edge_dict.items();
    const boost::python::ssize_t sz= len(items);
    if(sz>0)
      {
	for(boost::python::ssize_t i=0; i<sz; i++)
	  {
	    const boost::python::object obj= boost::python::extract<boost::python::object>(items[i][0]);
	    const int tag= tag_integer_from_py_object(obj);
	    const boost::python::dict itemDict= boost::python::extract<boost::python::dict>(items[i][1]);
	    const std::string className= boost::python::extract<std::string>(itemDict["className"]);
	    Edge *tmp= New(tag, className);
	    if(tmp)
	      { tmp->setPyDict(itemDict); }
	    else
	      std::cerr << getClassName() << "::" << __FUNCTION__
		        << "; something went wrong when reading line: " << tag
		        << ". Command ignored." << std::endl;
	  }
      }
  }
