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

#include "DqPtrsElem.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/Element1D.h"
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include "preprocessor/multi_block_topology/trf/TrfGeom.h"
#include "utility/geom/d1/Polyline3d.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/MeshEdges.h"
#include "domain/load/ElementalLoad.h"
#include <boost/algorithm/string/find.hpp>
#include "utility/geom/d3/BND3d.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor.
XC::DqPtrsElem::DqPtrsElem(CommandEntity *owr)
  : DqPtrsKDTree<Element,KDTreeElements>(owr) {}

//! @brief Copy constructor.
XC::DqPtrsElem::DqPtrsElem(const DqPtrsElem &other)
  : DqPtrsKDTree<Element,KDTreeElements>(other)
  {}

//! @brief Copy constructor.
XC::DqPtrsElem::DqPtrsElem(const std::deque<Element *> &ts)
  : DqPtrsKDTree<Element,KDTreeElements>(ts)
  {}

//! @brief Copy constructor.
XC::DqPtrsElem::DqPtrsElem(const std::set<const Element *> &st)
  : DqPtrsKDTree<Element,KDTreeElements>(st)
  {}

//! @brief Assignment operator.
XC::DqPtrsElem &XC::DqPtrsElem::operator=(const DqPtrsElem &other)
  {
    DqPtrsKDTree<Element,KDTreeElements>::operator=(other);
    return *this;
  }

//! @brief Returns (if it exists) a pointer to the element
//! identified by the tag being passed as parameter.
XC::Element *XC::DqPtrsElem::findElement(const int &tag)
  {
    Element *retval= nullptr;
    Element *tmp= nullptr;
    for(iterator i= begin();i!=end();i++)
      {
        tmp= *i;
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

//! @brief Returns (if it exists) a pointer to the element
//! identified by the tag being passed as parameter.
const XC::Element *XC::DqPtrsElem::findElement(const int &tag) const
  {
    const Element *retval= nullptr;
    const Element *tmp= nullptr;
    for(const_iterator i= begin();i!=end();i++)
      {
        tmp= *i;
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

//! @brief Returns the number of elements of the set which are active.
size_t XC::DqPtrsElem::getNumLiveElements(void) const
  {
    size_t retval= 0;
    const Element *tmp= nullptr;
    for(const_iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          if(tmp->isAlive()) retval++;
      }
    return retval;
  }

//! @brief Returns the number of elements of the set which are inactive.
size_t XC::DqPtrsElem::getNumDeadElements(void) const
  {
    size_t retval= 0;
    const Element *tmp= nullptr;
    for(const_iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          if(tmp->isDead()) retval++;
      }
    return retval;
  }

//! @brief Set indices to the objects to allow its use in VTK.
void XC::DqPtrsElem::numerate(void)
  {
    size_t idx= 0;
    for(iterator i= begin();i!=end();i++,idx++)
      {
	Element *ptr= *i;
        ptr->set_index(idx);
      }
  }

//! @brief Return the average size of the elements.
double XC::DqPtrsElem::getAverageSize(bool initialGeometry) const
  {
    double retval= 0.0;
    int dim= 0;
    double fictitiousLength= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
	const Element *ptr= *i;
        dim= ptr->getDimension();
	if((dim>0) and (dim<=3))
	  {
	    fictitiousLength= 0.0;
	    if(dim==1)
	      fictitiousLength= ptr->getLength(initialGeometry);
	    else if(dim==2)
	      fictitiousLength= sqrt(ptr->getArea(initialGeometry));
	    else if(dim==3)
	      fictitiousLength= pow(ptr->getVolume(initialGeometry),1.0/3.0);
	    retval+= fictitiousLength;
	  }
      }
    retval/=(size());
    return retval;
  }

// Loads on elements.

//! @brief Creates the inertia load that corresponds to the
//! acceleration argument.
boost::python::list XC::DqPtrsElem::createInertiaLoads(const Vector &accel)
  {
    boost::python::list retval;
    for(iterator i= begin();i!=end();i++)
      {
        ElementalLoad *loadPtr= (*i)->createInertiaLoad(accel);
	if(loadPtr)
	  {
	    boost::python::object pyObj(boost::ref(*loadPtr));
	    retval.append(pyObj);
	  }
	else
          {
            std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	              << "; null pointer returned by createInertiaLoad"
	              << " for element: " << (*i)->getTag()
	              << " of type: " << (*i)->getClassName()
	              << Color::def << std::endl;
          }
      }
    return retval;
  }

//! @brief Return the nearest element if it's a 1D element
//! otherwise returns null.
XC::Element1D *XC::DqPtrsElem::get_nearest_element_1d(const Vector &p, const std::string &callingMethod)
  {
    Element1D *retval= nullptr;
    const double x= p[0];
    const double y= p[1];
    double z= 0.0;
    if(p.Size()>2)
      z= p[2];
    const Pos3d pos(x,y,z);
    Element *elem= getNearest(pos);
    if(elem)
      {
	retval= dynamic_cast<Element1D *>(elem);
	if(retval)
	  {
	    const double d= retval->getDist(pos);
	    const double l= retval->getLength();
	    if(d>l)
	      std::clog << Color::yellow << this->getClassName() << "::" << __FUNCTION__
		        << "; the element: " << retval->getTag()
		        << " is quite far (d= " << d
		        << ") from the point: "
		        << p
			<< Color::def << std::endl;
	  }
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; no nearest element to the point: " << p
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Define an elemental concentrated load at position p with value v
//! expressed in global coordinates.
//! @param p: position of the load.
//! @param v: value of the load vector expressed in global coordinates.
XC::ElementalLoad *XC::DqPtrsElem::vector2dPointLoadGlobal(const Vector &p,const Vector &v)
  {
    ElementalLoad *retval= nullptr;
    Element1D *elem1d= get_nearest_element_1d(p, __FUNCTION__);
    if(elem1d)
      { retval= elem1d->vector2dPointLoadGlobal(p,v); }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__ 
		<< "; no element found near to position: " << p
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Define an elemental concentrated load at position p with value v
//! expressed in local (element) coordinates.
//! @param p: position of the load.
//! @param v: value of the load vector expressed in local (element) coordinates.
XC::ElementalLoad *XC::DqPtrsElem::vector2dPointLoadLocal(const Vector &p,const Vector &v)
  {
    ElementalLoad *retval= nullptr;
    Element1D *elem1d= get_nearest_element_1d(p, __FUNCTION__);
    if(elem1d)
      retval= elem1d->vector2dPointLoadLocal(p,v);
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; no element found near to position: " << p
		<< Color::def << std::endl;      
    return retval;
  }

//! @brief Define an elemental concentrated load at position p with value v
//! expressed in global coordinates.
//! @param p: position of the load.
//! @param v: value of the load vector expressed in global coordinates.
XC::ElementalLoad *XC::DqPtrsElem::vector3dPointLoadGlobal(const Vector &p,const Vector &v)
  {
    ElementalLoad *retval= nullptr;
    Element1D *elem1d= get_nearest_element_1d(p, __FUNCTION__);
    if(elem1d)
      retval= elem1d->vector3dPointLoadGlobal(p,v);
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; no element found near to position: " << p
		<< Color::def << std::endl;      
    return retval;
  }

//! @brief Define an elemental concentrated load at position p with value v
//! expressed in local (element) coordinates.
//! @param p: position of the load.
//! @param v: value of the load vector expressed in local (element) coordinates.
XC::ElementalLoad *XC::DqPtrsElem::vector3dPointLoadLocal(const Vector &p,const Vector &v)
  {
    ElementalLoad *retval= nullptr;
    Element1D *elem1d= get_nearest_element_1d(p, __FUNCTION__);
    if(elem1d)
      elem1d->vector2dPointLoadLocal(p,v);
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; no element found near to position: " << p
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Return the total mass matrix.
XC::Matrix XC::DqPtrsElem::getTotalMass(void) const
  {
    Matrix retval;
    if(!empty())
      {
	const_iterator i= begin();
	retval= (*i)->getTotalMass();
	i++;
	for(;i!=end();i++)
	  { retval+= (*i)->getTotalMass(); }
      }
    return retval;
  }

//! @brief Return the total mass matrix component for the DOF argument.
double XC::DqPtrsElem::getTotalMassComponent(const int &dof) const
  {
    const Matrix totalMass= getTotalMass();
    const size_t sz= totalMass.noRows();
    Vector J(sz);
    J(dof)= 1.0;
    Vector tmp(sz);
    tmp.addMatrixVector(1.0, totalMass, J, 1.0);
    const double retval= dot(J,tmp);
    return retval;
  }

//! @brief Deactivates the elements.
void XC::DqPtrsElem::kill_elements(void)
  {
    Element *tmp= nullptr;
    for(iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          tmp->kill();
      }
  }

//! @brief Activates the elements.
void XC::DqPtrsElem::alive_elements(void)
  {
    Element *tmp= nullptr;
    for(iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          tmp->alive();
      }
  }

//! @brief Calcula los esfuerzos on each uno of the elements.
void XC::DqPtrsElem::calc_resisting_force(void)
  {
    Element *tmp= nullptr;
    for(iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          tmp->getResistingForce();
      }
  }

//! @brief Returns the tags of the elements.
std::set<int> XC::DqPtrsElem::getTags(void) const
  {
    std::set<int> retval;
    for(const_iterator i= begin();i!=end();i++)
      retval.insert((*i)->getTag());
    return retval;
  }

//! @brief Returns the element pointers in a set.
std::set<const XC::Element *>XC::DqPtrsElem::getConstPtrSet(void) const
  {
    std::set<const Element *> retval;
    for(const_iterator i= begin();i!=end();i++)
      retval.insert(*i);
    return retval;
  }

//! @brief Returns the boundary of the element set.
BND3d XC::DqPtrsElem::Bnd(const double &factor) const
  {
    const Element *elem= nullptr;
    BND3d retval;
    if(!empty())
      {
	const_iterator i= begin();
        elem= *i;
	retval= elem->getNodePtrs().Bnd(factor);
	i++;
	for(;i!=end();i++)
	  {
	    elem= *i;
	    retval+= elem->getNodePtrs().Bnd(factor);
	  }
      }
    return retval;
  }

//! @brief Returns edges that form  the element set contour.
XC::MeshEdges XC::DqPtrsElem::getEdgesContour(void) const
  {
    typedef std::set<const Element *> ElementConstPtrSet;
    const Element *elem= nullptr;
    MeshEdges retval;
    for(const_iterator i= begin();i!=end();i++)
      {
        elem= *i;
        const int numEdges= elem->getNumEdges();
        for(int j= 0;j<numEdges;j++)
          {
	    MeshEdge meshEdge(elem->getNodesEdge(j));
            ElementConstPtrSet elementsShared= meshEdge.getConnectedElements(*this);
            if(elementsShared.size()==1) //border element.
              if(find(retval.begin(), retval.end(), meshEdge) == retval.end())
                { retval.push_back(meshEdge); }
          }
      }
    return retval;
  }

//! @brief Returns the element set contour.
std::deque<Polyline3d> XC::DqPtrsElem::getContours(const double &factor) const
  {
    std::deque<Polyline3d> retval;
    if(!empty())
      {
	const MeshEdges edgesContour= this->getEdgesContour();
	retval= edgesContour.getContours(factor);
      }
    return retval;
  }

//! @brief Returns the node sequences of the element set contour.
boost::python::list XC::DqPtrsElem::getContoursNodeSequences(void) const
  {
    boost::python::list retval;
    if(!empty())
      {
	const MeshEdges edgesContour= this->getEdgesContour();
	retval= edgesContour.getContoursNodeSequencesPy();
      }
    return retval;
  }


//! @brief Return a container with the elements that lie inside the
//! geometric object.
//!
//! @param geomObj: geometric object that must contain the elements.
//! @param tol: tolerance for "In" function.
XC::DqPtrsElem XC::DqPtrsElem::pickElemsInside(const GeomObj3d &geomObj, const double &tol)
  {
    DqPtrsElem retval;
    for(iterator i= begin();i!=end();i++)
      {
        Element *e= (*i);
        assert(e);
	if(e->In(geomObj,tol))
	  retval.push_back(e);
      }
    return retval;    
  }

//! @brief Return a container with the elements that lie inside the
//! geometric object.
//!
//! @param geomObj: geometric object that must contain the elements.
//! @param tol: tolerance for "In" function.
XC::DqPtrsElem XC::DqPtrsElem::pickElemsInside(const GeomObj2d &geomObj, const double &tol)
  {
    DqPtrsElem retval;
    for(iterator i= begin();i!=end();i++)
      {
        Element *e= (*i);
        assert(e);
	if(e->In(geomObj,tol))
	  retval.push_back(e);
      }
    return retval;    
  }

//! @brief Return a container with the elements that cross (i.e. have nodes
//! inside and outside) the given geometric object.
//!
//! @param geomObj: geometric object that must contain the elements.
//! @param tol: tolerance for "In" function.
XC::DqPtrsElem XC::DqPtrsElem::pickElemsCrossing(const GeomObj3d &geomObj, const double &tol)
  {
    DqPtrsElem retval;
    for(iterator i= begin();i!=end();i++)
      {
        Element *e= (*i);
        assert(e);
	if(e->Crosses(geomObj, 1.0, tol))
	  retval.push_back(e);
      }
    return retval;    
  }

//! @brief Return a container with the elements that cross (i.e. have nodes
//! inside and outside) the given geometric object.
//!
//! @param geomObj: geometric object that must contain the elements.
//! @param tol: tolerance for "In" function.
XC::DqPtrsElem XC::DqPtrsElem::pickElemsCrossing(const GeomObj2d &geomObj, const double &tol)
  {
    DqPtrsElem retval;
    for(iterator i= begin();i!=end();i++)
      {
        Element *e= (*i);
        assert(e);
	if(e->Crosses(geomObj, 1.0, tol))
	  retval.push_back(e);
      }
    return retval;    
  }

//! @brief Return the names of the materials.
std::set<std::string> XC::DqPtrsElem::getMaterialNames(void) const
  {
    std::set<std::string> retval;
    for(const_iterator i= begin();i!=end();i++)
      {
	std::set<std::string> names= (*i)->getMaterialNames();
        retval.insert(names.begin(),names.end());
      }
    return retval;    
  }
  
//! @brief Return a container with the elements whose material(s) name(s)
//! contains the string.
//!
//! @param targetName: name of the target material.
XC::DqPtrsElem XC::DqPtrsElem::pickElemsOfMaterial(const std::string &targetName)
  {
    DqPtrsElem retval;
    for(iterator i= begin();i!=end();i++)
      {
        Element *e= (*i);
        assert(e);
        const std::set<std::string> materialNames= e->getMaterialNames();
	if(materialNames.find(targetName)!=materialNames.end())
          retval.push_back(e);
      }
    return retval;
  }

//! @brief Return the names of the materials in a python list.
boost::python::list XC::DqPtrsElem::getMaterialNamesPy(void) const
  {
    boost::python::list retval;
    std::set<std::string> tmp= getMaterialNames();
    for(std::set<std::string>::const_iterator i= tmp.begin();i!=tmp.end();i++)
        retval.append(*i);
    return retval;
  }

//! @brief Return the types (class names) of the elements.
std::set<std::string> XC::DqPtrsElem::getTypes(void) const
  {
    std::set<std::string> retval;
    for(const_iterator i= begin();i!=end();i++)
      {
        const std::string className= (*i)->getClassName();
	if(retval.find(className)==retval.end())
	  retval.insert(className);
      }
    return retval;    
  }

//! @brief Return the types (class names) of the elements.
boost::python::list XC::DqPtrsElem::getTypesPy(void) const
  {
    boost::python::list retval;
    std::set<std::string> tmp= getTypes();
    for(std::set<std::string>::const_iterator i= tmp.begin();i!=tmp.end();i++)
        retval.append(*i);
    return retval;
  }


//! @brief Return the dimensions of the elements.
std::set<size_t> XC::DqPtrsElem::getDimensions(void) const
  {
    std::set<size_t> retval;
    for(const_iterator i= begin();i!=end();i++)
      {
        const size_t dim= (*i)->getDimension();
	if(retval.find(dim)==retval.end())
	  retval.insert(dim);
      }
    return retval;    
  }

//! @brief Return the dimensions of the elements.
boost::python::list XC::DqPtrsElem::getDimensionsPy(void) const
  {
    boost::python::list retval;
    std::set<size_t> tmp= getDimensions();
    for(std::set<size_t>::const_iterator i= tmp.begin();i!=tmp.end();i++)
        retval.append(*i);
    return retval;
  }

//! @brief Return true if the distance to the given point is smaller
//! than the given one.
//! @param p: point to measure the distance to.
//! @param d: distance threshold.
bool XC::DqPtrsElem::isCloserThan(const Pos3d &p, const double &d) const
  {
    bool retval= false;
    const Element *e= this->getNearest(p);
    if(e)
      {
	const double dist= e->getDist(p);
        retval= (dist<=d);
      }
    return retval;
  }

//! @brief Return true if the distance to all the vertices of the given sequence
//! is smaller than the given one.
//! @param s: segment to measure the distance to.
//! @param d: distance threshold.
bool XC::DqPtrsElem::isCloserThan(const GeomObj::list_Pos3d &vertices, const double &d) const
  {
    bool retval= false;
    if(!vertices.empty())
      {
	GeomObj::list_Pos3d::const_iterator i= vertices.begin();
        const Pos3d &pi= *i;
	retval= this->isCloserThan(pi, d);
	if(!retval)
	  {
	    i++;
	    for(;i!=vertices.end();i++)
	      {
		const Pos3d &pj= *i;
		retval= this->isCloserThan(pj, d);
		if(retval)
		  break;
	      }
	  }
      }
    return retval;
  }

//! @brief Return a container with the elements whose class name
//! contains the string.
//!
//! @param typeName: string that must be contained in the class name.
XC::DqPtrsElem XC::DqPtrsElem::pickElemsOfType(const std::string &typeName)
  {
    DqPtrsElem retval;
    for(iterator i= begin();i!=end();i++)
      {
        Element *e= (*i);
        assert(e);
        const std::string className= e->getClassName();
	if(boost::algorithm::ifind_first(className,typeName))
	  retval.push_back(e);
      }
    return retval;
  }

//! @brief Return a container with the elements of the specified
//! dimension.
//!
//! @param dim: element dimension (point: 0, line: 1, surface: 2, volume: 3)
XC::DqPtrsElem XC::DqPtrsElem::pickElemsOfDimension(const size_t &dim)
  {
    DqPtrsElem retval;
    for(iterator i= begin();i!=end();i++)
      {
        Element *e= (*i);
        assert(e);
	if(e->getDimension()==dim)
	  retval.push_back(e);
      }
    return retval;
  }

//! @brief Return the union of both containers.
XC::DqPtrsElem XC::operator+(const DqPtrsElem &a,const DqPtrsElem &b)
  {
    DqPtrsElem retval(a);
    retval+=b;
    return retval;
  }

//! @brief Return the nodes in a that are not in b.
XC::DqPtrsElem XC::operator-(const DqPtrsElem &a,const DqPtrsElem &b)
  {
    DqPtrsElem retval;
    for(DqPtrsElem::const_iterator i= a.begin();i!=a.end();i++)
      {
        Element *n= (*i);
        assert(n);
	if(!b.in(n)) //If not in b.
	  retval.push_back(n);
      }
    return retval;
  }

//! @brief Return the nodes in a that are also in b.
XC::DqPtrsElem XC::operator*(const DqPtrsElem &a,const DqPtrsElem &b)
  {
    DqPtrsElem retval;
    for(DqPtrsElem::const_iterator i= a.begin();i!=a.end();i++)
      {
        Element *n= (*i);
        assert(n);
	if(b.in(n)) //If also in b.
	  retval.push_back(n);
      }
    return retval;    
  }
