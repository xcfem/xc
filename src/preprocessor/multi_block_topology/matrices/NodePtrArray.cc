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
//NodePtrArray.cc

#include "NodePtrArray.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "domain/domain/Domain.h"
#include "domain/constraints/SFreedom_Constraint.h"


#include "utility/geom/pos_vec/Pos3d.h"

#include "boost/lexical_cast.hpp"

//! @brief Returns (if it exists) a pointer to the node
//! which tag is being passed as parameter.
XC::Node *XC::NodePtrArray::findNode(const int &tag)
  {
    Node *retval= nullptr;
    Node *tmp= nullptr;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          tmp= operator()(j,k);
          if(tmp)
            {
              if(tag == tmp->getTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Returns (if it exists) a pointer to the node
//! which tag is being passed as parameter.
const XC::Node *XC::NodePtrArray::findNode(const int &tag) const
  {
    const Node *retval= nullptr;
    const Node *tmp= nullptr;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          tmp= operator()(j,k);
          if(tmp)
            {
              if(tag == tmp->getTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Returns the node closest to the point being passed as parameter.
XC::Node *XC::NodePtrArray::getNearestNode(const Pos3d &p)
  {
    Node *retval= nullptr, *ptrNod= nullptr;
    double d= DBL_MAX;
    double tmp;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    if(numberOfRows*numberOfColumns>500)
      std::clog << getClassName() << "::" << __FUNCTION__
		<< "; node matrix has "
                << numberOfRows*numberOfColumns << " elements. "
                << "It is better to look by coordinates in the associated set."
                << std::endl;
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          ptrNod= operator()(j,k);
          tmp= ptrNod->getDist2(p);
          if(tmp<d)
            {
              d= tmp;
              retval= ptrNod;
            }
        }
    return retval;
  }

//! @brief Returns the node closest to the point being passed as parameter.
const XC::Node *XC::NodePtrArray::getNearestNode(const Pos3d &p) const
  {
    NodePtrArray *this_no_const= const_cast<NodePtrArray *>(this);
    return this_no_const->getNearestNode(p);
  }

//! @brief Remove the given node from the array (set the corresponding
//! pointer to null).
//! @param nPtr: pointer to the node to remove.
bool XC::NodePtrArray::removeNode(Node *nPtr)
  {
    bool retval= false;
    if(nPtr)
      {
	const Node *tmp= nullptr;
	const size_t numberOfRows= getNumberOfRows();
	const size_t numberOfColumns= getNumberOfColumns();
	for(size_t j= 1;j<=numberOfRows;j++)
	  for(size_t k= 1;k<=numberOfColumns;k++)
	    {
	      tmp= operator()(j,k);
	      if(tmp)
		{
		  if(nPtr == tmp)
		    {
		      (*this)(j,k)= nullptr;
		      retval= true;
		      break;
		    }
		}
	    }
      }
    return retval;
  }

//! @brief Remove the node identifie by the given tag from the array (set
//! the corresponding pointer to null).
//! @param tag: identifier of the node to remove.
bool XC::NodePtrArray::removeNode(const int &tag)
  {
    Node *nPtr= findNode(tag);
    return removeNode(nPtr);
  }

//! @brief Returns the pointers to the nodes of this array.
std::deque<const XC::Node *> XC::NodePtrArray::getNodePtrs(void) const
  {
    std::deque<const Node *> retval;
    if(!Null())
      {
	const size_t numberOfRows= getNumberOfRows();
	const size_t numberOfColumns= getNumberOfColumns();
	for(size_t j= 1;j<=numberOfRows;j++)
	  for(size_t k= 1;k<=numberOfColumns;k++)
	    {
	      const Node *node= operator()(j,k);
   	      retval.push_back(node);
	    }
      }
    return retval;
  }

//! @brief Returns the pointers to the nodes of this array.
std::deque<XC::Node *> XC::NodePtrArray::getNodePtrs(void)
  {
    std::deque<Node *> retval;
    if(!Null())
      {
	const size_t numberOfRows= getNumberOfRows();
	const size_t numberOfColumns= getNumberOfColumns();
	for(size_t j= 1;j<=numberOfRows;j++)
	  for(size_t k= 1;k<=numberOfColumns;k++)
	    {
	      Node *node= operator()(j,k);
   	      retval.push_back(node);
	    }
      }
    return retval;
  }

//! @brief Returns a Python list containing the nodes of this array.
boost::python::list XC::NodePtrArray::getPyNodeList(void) const
  {
    boost::python::list retval;
    if(!Null())
      {
	const std::deque<const Node *> tmp= this->getNodePtrs();
	const size_t sz= tmp.size();
	for(size_t j= 0;j<sz;j++)
	  {
	    const Node *node= tmp[j];
   	    retval.append(node);
	  }
      }
    return retval;
  }

//! @brief Return the elements connected to any of the nodes of this entity.
std::set<const XC::Element *> XC::NodePtrArray::getConnectedElements(void) const
  {
    std::set<const Element *> retval;
    if(!Null())
      {
	const std::deque<const Node *> tmp= this->getNodePtrs();
	const size_t sz= tmp.size();
	for(size_t j= 0;j<sz;j++)
	  {
	    const Node *node= tmp[j];
	    std::set<const Element *> tmp=  node->getConnectedElements();
   	    retval.insert(tmp.begin(), tmp.end());
	  }
      }
    return retval;
  }

//! @brief Return the elements connected to any of the nodes of this entity.
std::set<XC::Element *> XC::NodePtrArray::getConnectedElements(void)
  {
    std::set<Element *> retval;
    if(!Null())
      {
	const std::deque<Node *> tmp= this->getNodePtrs();
	const size_t sz= tmp.size();
	for(size_t j= 0;j<sz;j++)
	  {
	    Node *node= tmp[j];
	    std::set<Element *> tmp= node->getConnectedElements();
   	    retval.insert(tmp.begin(), tmp.end());
	  }
      }
    return retval;
  }


//! @brief Return the elements connected to any of the nodes of this entity.
boost::python::list XC::NodePtrArray::getConnectedElementsPy(void)
  {
    boost::python::list retval;
    const std::set<Element *> tmp= this->getConnectedElements();
    for(std::set<Element *>::iterator i= tmp.begin(); i!=tmp.end(); i++)
      {
	Element *element= *i;
	boost::python::object pyObj(boost::ref(*element));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Impone desplazamiento nulo en the nodes de this set.
void XC::NodePtrArray::fix(const SFreedom_Constraint &seed) const
  {
    if(Null()) return;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        operator()(j,k)->fix(seed);
  }

//! @brief Returns an array with the identifiers of the nodes.
m_int XC::NodePtrArray::getTags(void) const
  {
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    m_int retval(numberOfRows,numberOfColumns,-1);
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          const Node *ptr= operator()(j,k);
          if(ptr)
            retval(j,k)= ptr->getTag();
        }
    return retval;
  }
