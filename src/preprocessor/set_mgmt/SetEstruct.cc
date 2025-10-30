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
//SetEstruct.cc

#include "SetEstruct.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "domain/mesh/node/Node.h"
#include "domain/load/NodalLoad.h"
#include "domain/mesh/element/Element.h"
#include "domain/load/ElementalLoad.h"
#include "utility/utils/misc_utils/colormod.h"


//! @brief Constructor.
XC::SetEstruct::SetEstruct(const std::string &nmb,Preprocessor *md)
  : SetBase(nmb,md) {}

//! @brief Copy constructor.
XC::SetEstruct::SetEstruct(const SetEstruct &other)
  : SetBase(other) {}

//! @brief Assignment operator.
XC::SetEstruct &XC::SetEstruct::operator=(const SetEstruct &other)
  {
    SetBase::operator=(other);
    return *this;
  }

size_t XC::SetEstruct::getNumberOfNodes(void) const
  { return getNumNodeLayers()*getNumNodeRows()*getNumNodeColumns(); }
size_t XC::SetEstruct::getNumberOfElements(void) const
  { return getNumElementLayers()*getNumElementRows()*getNumElementColumns(); }

//! @brief Returns true if only the I index varies.
bool XC::SetEstruct::isIRow(void) const
  {
    if((getNumNodeRows()>1) || (getNumNodeColumns()>1)) return false;
    return true;
  }

//! @brief Returns true if only the J index varies.
bool XC::SetEstruct::isJRow(void) const
  {
    if((getNumNodeLayers()>1) || (getNumNodeColumns()>1)) return false;
    return true;
  }

//! @brief Returns true if only the K index varies.
bool XC::SetEstruct::isKRow(void) const
  {
    if((getNumNodeLayers()>1) || (getNumNodeRows()>1)) return false;
    return true;
  }

//! @brief Returns true if only J and K indices varies.
bool XC::SetEstruct::isIConstantLayer(void) const
  { return (getNumNodeLayers()==1); }

//! @brief Returns true if only J and K indices varies.
bool XC::SetEstruct::isJConstantLayer(void) const
  { return (getNumNodeRows()==1); }

//! @brief Returns true if only I and J indices varies.
bool XC::SetEstruct::isKConstantLayer(void) const
  { return (getNumNodeColumns()==1); }

//! @brief Returns the set type i_row, j_row, k_row, constantILayer,...
std::string XC::SetEstruct::getStrType(void) const
  {
    if(isIRow()) return "i_row";
    if(isJRow()) return "j_row";
    if(isKRow()) return "k_row";
    if(isIConstantLayer()) return "constantILayer";
    if(isJConstantLayer()) return "constantJLayer";
    if(isKConstantLayer()) return "constantKLayer";
    return "bloqueIJK";
  }

//! @brief Return the dimension of the range (0,1,2 or 3).
//! Zero if the range is empty, 1 if there is more than one layer, 2
//! if there are more than one row and three if there are more
//! than one column.
size_t XC::SetEstruct::Dimension(void) const
  {
    size_t retval= 0;
    if(getNumNodeLayers()>1) retval++;
    if(getNumNodeRows()>1) retval++;
    if(getNumNodeColumns()> 1) retval++;
    return retval;
  }

//! @brief Returns the pointers to the set nodes.
XC::NodePtrSet XC::SetEstruct::getNodePtrSet(void)
  {
    NodePtrSet retval;
    const size_t numberOfLayers= getNumNodeLayers();
    const size_t numberOfRows= getNumNodeRows();
    const size_t numberOfColumns= getNumNodeColumns();
    for(size_t i= 1;i<=numberOfLayers;i++)
      for(size_t j= 1;j<=numberOfRows;j++)
        for(size_t k= 1;k<=numberOfColumns;k++)
          retval.insert(getNode(i,j,k));
    return retval;    
  }

//! @brief Adds the elements to the element handler.
void XC::SetEstruct::add_elements_to_handler(const ElemPtrArray3d &elements)
  {
    const size_t n_layers= elements.getNumberOfLayers();
    if(n_layers>0)
      {
	const size_t numberOfRows= elements(1).getNumberOfRows();
	const size_t cols= elements(1).getNumberOfColumns();
	ElementHandler &eHandler= getPreprocessor()->getElementHandler();
	for( size_t i= 1;i<=n_layers;i++)
	  for( size_t j= 1;j<=numberOfRows;j++)
	    for( size_t k= 1;k<=cols;k++)
	      {
		eHandler.Add(elements(i,j,k));
	      }
      }
  }

//! @brief Return the nodes of the set in a list.
std::list<const XC::Node *> XC::SetEstruct::getNodes(void) const
  {
    std::list<const Node *> retval;
    const size_t numberOfLayers= getNumNodeLayers();
    if(numberOfLayers>0)
      {
        const size_t numberOfRows= getNumNodeRows();
        const size_t numberOfColumns= getNumNodeColumns();
        for(size_t i= 1;i<=numberOfLayers;i++)
          for(size_t j= 1;j<=numberOfRows;j++)
            for(size_t k= 1;k<=numberOfColumns;k++)
              retval.push_back(getNode(i,j,k));
      }
    return retval;
  }

//! @brief Return the nodes of the set in a list.
std::list<XC::Node *> XC::SetEstruct::getNodes(void)
  {
    std::list<Node *> retval;
    const size_t numberOfLayers= getNumNodeLayers();
    if(numberOfLayers>0)
      {
        const size_t numberOfRows= getNumNodeRows();
        const size_t numberOfColumns= getNumNodeColumns();
        for(size_t i= 1;i<=numberOfLayers;i++)
          for(size_t j= 1;j<=numberOfRows;j++)
            for(size_t k= 1;k<=numberOfColumns;k++)
              retval.push_back(getNode(i,j,k));
      }
    return retval;
  }

//! @brief Returns the tags of the nodes.
std::set<int> XC::SetEstruct::getNodeTags(void) const
  {
    std::set<int> retval;
    const std::list<const Node *> tmp= this->getNodes();
    for(std::list<const Node *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      retval.insert((*i)->getTag());
    return retval;
  }

//! @brief Return the nodes of the set in a Python list.
boost::python::list XC::SetEstruct::getNodesPy(void)
  {
    boost::python::list retval;
    const std::list<Node *> tmp= this->getNodes();
    for(std::list<Node *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      {
	Node *tmp= *i;
	boost::python::object pyObj(boost::ref(*tmp));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Returns the elements in a list.
std::list<const XC::Element *> XC::SetEstruct::getElements(void) const
  {
    std::list<const Element *> retval;
    const size_t numberOfLayers= getNumElementLayers();
    if(numberOfLayers>0)
      {
        const size_t numberOfRows= getNumElementRows();
        const size_t numberOfColumns= getNumElementColumns();
        for(size_t i= 1;i<=numberOfLayers;i++)
          for(size_t j= 1;j<=numberOfRows;j++)
            for(size_t k= 1;k<=numberOfColumns;k++)
              retval.push_back(getElement(i,j,k));
      }
    return retval;
  }

//! @brief Returns the elements in a list.
std::list<XC::Element *> XC::SetEstruct::getElements(void)
  {
    std::list<Element *> retval;
    const size_t numberOfLayers= getNumElementLayers();
    if(numberOfLayers>0)
      {
        const size_t numberOfRows= getNumElementRows();
        const size_t numberOfColumns= getNumElementColumns();
        for(size_t i= 1;i<=numberOfLayers;i++)
          for(size_t j= 1;j<=numberOfRows;j++)
            for(size_t k= 1;k<=numberOfColumns;k++)
              retval.push_back(getElement(i,j,k));
      }
    return retval;
  }

//! @brief Returns the tags of the elements.
std::set<int> XC::SetEstruct::getElementTags(void) const
  {
    std::set<int> retval;
    const std::list<const XC::Element *> tmp= this->getElements();
    for(std::list<const XC::Element *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      retval.insert((*i)->getTag());
    return retval;
  }

boost::python::list XC::SetEstruct::getElementsPy(void)
  {
    boost::python::list retval;
    const std::list<Element *> tmp= this->getElements();
    for(std::list<Element *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      {
	Element *tmp= *i;
	boost::python::object pyObj(boost::ref(*tmp));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Creates the inertia loads that corresponds to the
//! acceleration argument.
boost::python::list XC::SetEstruct::createInertiaLoads(const Vector &accel)
  {
    boost::python::list retval;
    size_t numberOfLayers= getNumNodeLayers();
    if(numberOfLayers>0)
      {
        const size_t numberOfRows= getNumNodeRows();
        const size_t numberOfColumns= getNumNodeColumns();
        for(size_t i= 1;i<=numberOfLayers;i++)
          for(size_t j= 1;j<=numberOfRows;j++)
            for(size_t k= 1;k<=numberOfColumns;k++)
	      {
		Node *nodePtr= getNode(i,j,k);
		const NodalLoad *loadPtr= nodePtr->createInertiaLoad(accel);
		if(loadPtr)
		  {
		    boost::python::object pyObj(boost::ref(*loadPtr));
		    retval.append(pyObj);
		  }
		else
		  {
		    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			      << "; null pointer returned by createInertiaLoad"
			      << " for node: " << nodePtr->getTag()
			      << " of type: " << nodePtr->getClassName()
			      << Color::def << std::endl;
		  }
		
	      }
      }
    numberOfLayers= getNumElementLayers();
    if(numberOfLayers>0)
      {
        const size_t numberOfRows= getNumElementRows();
        const size_t numberOfColumns= getNumElementColumns();
        for(size_t i= 1;i<=numberOfLayers;i++)
          for(size_t j= 1;j<=numberOfRows;j++)
            for(size_t k= 1;k<=numberOfColumns;k++)
	      {
		Element *elemPtr= getElement(i,j,k);
		ElementalLoad *loadPtr= elemPtr->createInertiaLoad(accel);
		if(loadPtr)
		  {
		    boost::python::object pyObj(boost::ref(*loadPtr));
		    retval.append(pyObj);
		  }
		else
		  {
		    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			      << "; null pointer returned by createInertiaLoad"
			      << " for element: " << elemPtr->getTag()
			      << " of type: " << elemPtr->getClassName()
			      << Color::def << std::endl;
		  }		
	      }
      }
    return retval;
  }

//! @brief Returns true if the node belongs to the set.
bool XC::SetEstruct::In(const Node *n) const
  {
    bool retval= false;
    const size_t numberOfLayers= getNumNodeLayers();
    if(numberOfLayers>0)
      {
        const size_t numberOfRows= getNumNodeRows();
        const size_t numberOfColumns= getNumNodeColumns();
        for(size_t i= 1;i<=numberOfLayers;i++)
          for(size_t j= 1;j<=numberOfRows;j++)
            for(size_t k= 1;k<=numberOfColumns;k++)
              if(getNode(i,j,k)==n)
                {
                  retval= true;
                  break;
                }
      }
    return retval;
  }

//! @brief Returns true if the element belongs to the set.
bool XC::SetEstruct::In(const Element *e) const
  {
    bool retval= false;
    const size_t numberOfLayers= getNumElementLayers();
    if(numberOfLayers>0)
      {
        const size_t numberOfRows= getNumElementRows();
        const size_t numberOfColumns= getNumElementColumns();
        for(size_t i= 1;i<=numberOfLayers;i++)
          for(size_t j= 1;j<=numberOfRows;j++)
            for(size_t k= 1;k<=numberOfColumns;k++)
              if(getElement(i,j,k)==e)
                {
                  retval= true;
                  break;
                }
      }
    return retval;
  }

//! @brief Returns true if the constraint belongs to the set.
bool XC::SetEstruct::In(const Constraint *) const
  { return false; }

XC::Node *XC::SetEstruct::getNodeI(const size_t &i)
  {
    if(isIRow())
      return getNode(i,1,1);
    else if(isJRow())
      return getNode(1,i,1);
    else if(isKRow())
      return getNode(1,1,i);
    else
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; node set is not one-dimensional."
		  << Color::def << std::endl;
        return nullptr;
      }
  }

XC::Node *XC::SetEstruct::getNodeIJ(const size_t &i,const size_t &j)
  {
    //XXX Must get the layer (it is not necessarily the first one),
    if(isIConstantLayer())
      return getNode(1,i,j);
    else if(isJConstantLayer()) 
      return getNode(i,1,j);
    else if(isKConstantLayer()) 
      return getNode(i,j,1);
    else
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; the node set is not bidimensional."
		  << Color::def << std::endl;
        return nullptr;
      } 
  }
