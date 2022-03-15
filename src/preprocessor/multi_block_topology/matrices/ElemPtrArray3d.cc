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
//ElemPtrArray3d.cc

#include "ElemPtrArray3d.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/Element1D.h"
//#include <boost/any.hpp>
#include "utility/geom/pos_vec/Pos3d.h"
#include "boost/lexical_cast.hpp"

//! @brief Default constructor.
XC::ElemPtrArray3d::ElemPtrArray3d(const size_t n_layers,const ElemPtrArray &m)
  : PtrArray3dBase<ElemPtrArray>(n_layers,m) {}
//! @brief Constructor.
XC::ElemPtrArray3d::ElemPtrArray3d(const size_t n_layers,const size_t iRows,const size_t cols)
  : PtrArray3dBase<ElemPtrArray>(n_layers)
  {
    for(size_t i=0;i<n_layers;i++)
      (*this)[i]= ElemPtrArray(iRows,cols);
  }

//! @brief Returns (if it exists) a pointer to the element
//! identified by the tag being passed as parameter.
XC::Element *XC::ElemPtrArray3d::findElement(const int &tag)
  {
    Element *retval= nullptr;
    const size_t numberOfLayers= getNumberOfLayers();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        ElemPtrArray &layer= operator()(i);
        retval= layer.findElement(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Returns (if it exists) a pointer to the element
//! identified by the tag being passed as parameter.
const XC::Element *XC::ElemPtrArray3d::findElement(const int &tag) const
  {
    const Element *retval= nullptr;
    const size_t numberOfLayers= getNumberOfLayers();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        const ElemPtrArray &layer= operator()(i);
        retval= layer.findElement(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Returns the element closest to the point being passed as parameter.
XC::Element *XC::ElemPtrArray3d::getNearestElement(const Pos3d &p)
  {
    Element *retval= nullptr, *ptrElem= nullptr;
    const size_t numberOfLayers= getNumberOfLayers();
    double d= DBL_MAX;
    double tmp;
    if(numberOfLayers>100)
      std::clog << getClassName() << "::" << __FUNCTION__
	        << "; the element pointer array has"
                << numberOfLayers << " layers. "
                << " It is better to search by coordinates in the associated set."
                << std::endl;
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        ElemPtrArray &layer= operator()(i);
        ptrElem= layer.getNearestElement(p);
        tmp= ptrElem->getDist2(p);
        if(tmp<d)
          {
            d= tmp;
            retval= ptrElem;
          }
      }
    return retval;
  }

//! @brief Returns the element closest to the point being passed as parameter.
const XC::Element *XC::ElemPtrArray3d::getNearestElement(const Pos3d &p) const
  {
    ElemPtrArray3d *this_no_const= const_cast<ElemPtrArray3d *>(this);
    return this_no_const->getNearestElement(p);
  }

//! @brief Return the nearest element if it's a 1D element
//! otherwise returns null.
XC::Element1D *XC::ElemPtrArray3d::get_nearest_element_1d(const Vector &p, const std::string &callingMethod)
  {
    Element1D *retval= nullptr;
    const double x= p[0];
    const double y= p[1];
    double z= 0.0;
    if(p.Size()>2)
      z= p[2];
    const Pos3d pos(x,y,z);
    Element *elem= getNearestElement(pos);
    if(elem)
      {
	retval= dynamic_cast<Element1D *>(elem);
	if(retval)
	  {
	    const double d= retval->getDist(pos);
	    const double l= retval->getLength();
	    if(d>l)
	      std::clog << getClassName() << "::" << __FUNCTION__
		        << "; the element: " << retval->getTag()
		        << " is quite far (d= " << d
		        << ") from the point: "
		        << p << std::endl;
	  }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; method: " << callingMethod
		<< " not defined for elements of class: "
		<< elem->getClassName() << " yet."
		<< std::endl;
    return retval;
  }

// Loads on elements.
//! @brief Define an elemental concentrated load at position p with value v
//! expressed in global coordinates.
//! @param p: position of the load.
//! @param v: value of the load vector expressed in global coordinates.
void XC::ElemPtrArray3d::vector2dPointLoadGlobal(const Vector &p,const Vector &v)
  {
    Element1D *elem1d= get_nearest_element_1d(p, __FUNCTION__);
    if(elem1d)
      elem1d->vector2dPointLoadGlobal(p,v);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no element found near to position: " << p
		<< std::endl;      
  }

//! @brief Define an elemental concentrated load at position p with value v
//! expressed in local (element) coordinates.
//! @param p: position of the load.
//! @param v: value of the load vector expressed in local (element) coordinates.
void XC::ElemPtrArray3d::vector2dPointLoadLocal(const Vector &p,const Vector &v)
  {
    Element1D *elem1d= get_nearest_element_1d(p, __FUNCTION__);
    if(elem1d)
      elem1d->vector2dPointLoadLocal(p,v);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no element found near to position: " << p
		<< std::endl;      
  }

//! @brief Define an elemental concentrated load at position p with value v
//! expressed in global coordinates.
//! @param p: position of the load.
//! @param v: value of the load vector expressed in global coordinates.
void XC::ElemPtrArray3d::vector3dPointLoadGlobal(const Vector &p,const Vector &v)
  {
    Element1D *elem1d= get_nearest_element_1d(p, __FUNCTION__);
    if(elem1d)
      elem1d->vector3dPointLoadGlobal(p,v);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no element found near to position: " << p
		<< std::endl;      
  }

//! @brief Define an elemental concentrated load at position p with value v
//! expressed in local (element) coordinates.
//! @param p: position of the load.
//! @param v: value of the load vector expressed in local (element) coordinates.
void XC::ElemPtrArray3d::vector3dPointLoadLocal(const Vector &p,const Vector &v)
  {
    Element1D *elem1d= get_nearest_element_1d(p, __FUNCTION__);
    if(elem1d)
      elem1d->vector2dPointLoadLocal(p,v);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no element found near to position: " << p
		<< std::endl;      
  }

//! @brief Returns a Python list containing the elements of this array.
boost::python::list XC::ElemPtrArray3d::getPyElementList(void) const
  {
    boost::python::list retval;
    const size_t numberOfLayers= getNumberOfLayers();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        const ElemPtrArray &layer= operator()(i);
	boost::python::list tmp= layer.getPyElementList();
	const size_t sz= len(tmp);
        for(size_t i= 0;i<sz;i++)
          {
	    const Element *elem= boost::python::extract<const Element *>(tmp[i]);
	    retval.append(elem);
	  }
      }
    return retval;
  }

//! @brief Return the elements connected to the node being passed as parameter.
std::set<const XC::Element *> XC::ElemPtrArray3d::getConnectedElements(const Node *n) const
  {
    ElemPtrArray3d *this_no_const= const_cast<ElemPtrArray3d *>(this);
    std::set<Element *> tmp= this_no_const->getConnectedElements(n);
    std::set<const XC::Element *> retval(tmp.begin(), tmp.end());
    return retval;
  }

//! @brief Return the elements connected to the node being passed as parameter.
std::set<XC::Element *> XC::ElemPtrArray3d::getConnectedElements(const Node *n)
  {
    std::set<Element *> retval;
    const size_t numberOfLayers= getNumberOfLayers();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        ElemPtrArray &layer= operator()(i);
	const std::set<Element *> tmp= layer.getConnectedElements(n);
	retval.insert(tmp.begin(), tmp.end());
      }
    return retval;
  }

//! @brief Return the elements connected to the node being passed as parameter.
boost::python::list XC::ElemPtrArray3d::getConnectedElementsPy(const Node *n)
  {
    boost::python::list retval;
    std::set<XC::Element *> elements= getConnectedElements(n);
    for(std::set<XC::Element *>::iterator i= elements.begin(); i!= elements.end(); i++)
      {
        Element *ptrElem= *i;
	boost::python::object pyObj(boost::ref(*ptrElem));
	retval.append(pyObj);
      }
    return retval;
  }
