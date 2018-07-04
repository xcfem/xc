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
//SetEstruct.cc

#include "SetEstruct.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/multi_block_topology/matrices/TritrizPtrElem.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/utils/ElementEdges.h"





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

//! @brief Returns the element edges with both end nodes
//! belong to the nodes of the set.
XC::ElementEdges XC::SetEstruct::getElementEdges(void)
  {
    const NodePtrSet nodes= getNodePtrSet();
    return getElementEdgesBetweenNodes(nodes);
  }

//! @brief Adds to the model the elements being passed as parameters.
void XC::SetEstruct::add_elements(const TritrizPtrElem &elements)
  {
    const size_t n_layers= elements.getNumberOfLayers();
    if(n_layers<1) return;
    const size_t numberOfRows= elements(1).getNumberOfRows();
    const size_t cols= elements(1).getNumberOfColumns();
    for(register size_t i= 1;i<=n_layers;i++)
      for(register size_t j= 1;j<=numberOfRows;j++)
        for(register size_t k= 1;k<=cols;k++)
          getPreprocessor()->getElementHandler().Add(elements(i,j,k));
  }

//! @brief Returns the tags of the nodes.
std::set<int> XC::SetEstruct::getNodeTags(void) const
  {
    std::set<int> retval;
    const size_t numberOfLayers= getNumNodeLayers();
    if(numberOfLayers>0)
      {
        const size_t numberOfRows= getNumNodeRows();
        const size_t numberOfColumns= getNumNodeColumns();
        for(size_t i= 1;i<=numberOfLayers;i++)
          for(size_t j= 1;j<=numberOfRows;j++)
            for(size_t k= 1;k<=numberOfColumns;k++)
              retval.insert(getNode(i,j,k)->getTag());
      }
    return retval;
  }

boost::python::list XC::SetEstruct::getNodes(void)
  {
    boost::python::list retval;
    const size_t numberOfLayers= getNumNodeLayers();
    if(numberOfLayers>0)
      {
        const size_t numberOfRows= getNumNodeRows();
        const size_t numberOfColumns= getNumNodeColumns();
        for(size_t i= 1;i<=numberOfLayers;i++)
          for(size_t j= 1;j<=numberOfRows;j++)
            for(size_t k= 1;k<=numberOfColumns;k++)
              {
                Node *tmp= getNode(i,j,k);
                boost::python::object pyObj(boost::ref(*tmp));
                retval.append(pyObj);
              }
      }
    return retval;
  }


//! @brief Returns the tags of the elements.
std::set<int> XC::SetEstruct::getElementTags(void) const
  {
    std::set<int> retval;
    const size_t numberOfLayers= getNumElementLayers();
    if(numberOfLayers>0)
      {
        const size_t numberOfRows= getNumElementRows();
        const size_t numberOfColumns= getNumElementColumns();
        for(size_t i= 1;i<=numberOfLayers;i++)
          for(size_t j= 1;j<=numberOfRows;j++)
            for(size_t k= 1;k<=numberOfColumns;k++)
              retval.insert(getElement(i,j,k)->getTag());
      }
    return retval;
  }

boost::python::list XC::SetEstruct::getElements(void)
  {
    boost::python::list retval;
    const size_t numberOfLayers= getNumElementLayers();
    if(numberOfLayers>0)
      {
        const size_t numberOfRows= getNumElementRows();
        const size_t numberOfColumns= getNumElementColumns();
        for(size_t i= 1;i<=numberOfLayers;i++)
          for(size_t j= 1;j<=numberOfRows;j++)
            for(size_t k= 1;k<=numberOfColumns;k++)
              {
                Element *tmp= getElement(i,j,k);
                boost::python::object pyObj(boost::ref(*tmp));
                retval.append(pyObj);
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
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; node set is not one-dimensional." << std::endl;
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
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the node set is not bidimensional." << std::endl;
        return nullptr;
      } 
  }
