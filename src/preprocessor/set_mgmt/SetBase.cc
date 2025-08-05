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
//SetBase.cc

#include "SetBase.h"

#include <boost/any.hpp>
#include "domain/constraints/SFreedom_Constraint.h"
#include "utility/utils/misc_utils/colormod.h"


#include "utility/matrix/ID.h"
#include "preprocessor/Preprocessor.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "utility/kernel/python_utils.h"

//! @brief Constructor.
XC::SetBase::SetBase(const std::string &nmb,Preprocessor *md)
  : EntMdlrBase(nmb,md), color(3)
  { setColorComponents(0.0,0.0,0.0); }

//! @brief Comparison operator.
bool XC::SetBase::operator==(const SetBase &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= EntMdlrBase::operator==(other);
        if(retval)
          retval= ((color[0]==other.color[0]) && (color[1]==other.color[1]) && (color[2]==other.color[2]));
      }
    return retval;
  }

//! @brief Set the color of the object (red,green,blue) as
//! integers from 0 to 255.
void XC::SetBase::setColorComponents(const double &r, const double &g, const double &b)
  { color[0]= r; color[1]= g; color[2]= b; }

//! @brief Set the color of the object [red,green,blue] as
//! integers from 0 to 255.
void XC::SetBase::setColor(const Vector &rgb)
  {
    if(rgb.Size()>2)
      { color[0]= rgb[0]; color[1]= rgb[1]; color[2]= rgb[2]; }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                << " three components needed ([red,green,blue])."
		<< Color::def << std::endl;
  }

//! @brief Return the color of the object [red,green,blue] as
//! integers from 0 to 255.
const XC::Vector &XC::SetBase::getColor(void) const
  { return color; }

//! @brief Generates a finite element mesh from the set components.
void XC::SetBase::genMesh(meshing_dir dm)
  {
    std::cerr << Color::red
	      << getClassName() << "::" << __FUNCTION__
              << " not implemented."
	      << Color::def << std::endl;
  }

//! @brief Creates the inertia load that corresponds to the
//! acceleration argument.
void XC::SetBase::createInertiaLoads(const Vector &)
  {
    std::cerr << Color::red
	      << getClassName() << "::" << __FUNCTION__
              << " not implemented."
	      << Color::def << std::endl;
  }

//! @brief Impone desplazamiento nulo for all the nodes of this set.
void XC::SetBase::fix(const SFreedom_Constraint &)
  {
    std::cerr << Color::red
	      << getClassName() << "::" << __FUNCTION__
              << " not implemented."
	      << Color::def << std::endl;
  }

//! @brief Returns the tags of the nodes en un vector de enteros.
const XC::ID &XC::SetBase::getIdNodeTags(void) const
  {
    static ID retval;
    const std::set<int> tmp= getNodeTags();
    if(!tmp.empty())
      {
        const size_t sz= tmp.size();
	retval.resize(sz);
        size_t conta= 0;
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++,conta++)
          retval[conta]= *i;
      }
    return retval;
  }

//! @brief Returns the tags of the elements en un vector de enteros.
const XC::ID &XC::SetBase::getIdElementTags(void) const
  {
    static ID retval;
    const std::set<int> tmp= getElementTags();
    if(!tmp.empty())
      {
        const size_t sz= tmp.size();
	retval.resize(sz);
        size_t conta= 0;
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++,conta++)
          retval[conta]= *i;
      }
    return retval;
  }

//! @brief Returns the tags of the nodes in a Python list.
boost::python::list XC::SetBase::getNodeTagsPy(void) const
  {
    boost::python::list retval;
    const std::set<int> tmp= getNodeTags();
    if(!tmp.empty())
      {
        size_t conta= 0;
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++,conta++)
          retval.append(*i);
      }
    return retval;
  }

//! @brief Returns the tags of the elements in a Python list.
boost::python::list XC::SetBase::getElementTagsPy(void) const
  {
    boost::python::list retval;
    const std::set<int> tmp= getElementTags();
    if(!tmp.empty())
      {
        size_t conta= 0;
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++,conta++)
          retval.append(*i);
      }
    return retval;
  }

//! @brief Returns true if the all the elements connected to the given node
//! belong to the set.
bool XC::SetBase::interiorNode(const Node *n) const
  {
    bool retval= true;
    if(n)
      {
	if(In(n))
	  {
            std::set<const Element *> connectedElements= n->getConnectedElements();
	    for(std::set<const Element *>::const_iterator i= connectedElements.begin();
		i!=connectedElements.end();
		i++)
	      {
		const Element *e= *i;
		if(!In(e))
		  {
		    retval= false;
		    break;
		  }
	      }
	  }
	else
	  retval= false;
      }
    else
      retval= false;
    return retval;
  }

//! @brief Returns true if the point belongs to the set.
bool XC::SetBase::In(const Pnt *) const
  { return false; }

//! @brief Returns true if the edge belongs to the set.
bool XC::SetBase::In(const Edge *) const
  { return false; }

//! @brief Returns true if the surface belongs to the set.
bool XC::SetBase::In(const Face *) const
  { return false; }

//! @brief Returns true if the body belongs to the set.
bool XC::SetBase::In(const Body *) const
  { return false; }

//! @brief Returns true if the «uniform grid» belongs to the set.
bool XC::SetBase::In(const UniformGrid *) const
  { return false; }

//! @brief Removes the given node from the set.
bool XC::SetBase::remove(Node *)
  {
    std::cerr << Color::red
	      << getClassName() << "::" << __FUNCTION__
	      << " not implemented yet."
	      << Color::def << std::endl;
    return false;
  }

//! @brief Removes the given element from the set.
bool XC::SetBase::remove(Element *)
  {
    std::cerr << Color::red
	      << getClassName() << "::" << __FUNCTION__
	      << " not implemented yet."
	      << Color::def << std::endl;
    return false;
  }

//! @brief Removes the given constraint from the set.
bool XC::SetBase::remove(Constraint *)
  {
    std::cerr << Color::red
	      << getClassName() << "::" << __FUNCTION__
	      << " not implemented yet."
	      << Color::def << std::endl;
    return false;
  }

//! @brief Reset tributary areas (or lengths, or volumes) for the nodes that
//! are connected to the set elements.
void XC::SetBase::resetTributaries(void) const
  {
    const std::set<int> tmp= getElementTags();
    const Domain *dom= getPreprocessor()->getDomain();
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_elem= *i;
            const Element *elem= dom->getElement(tag_elem);
            if(elem)
              elem->resetTributaries();
            else
	      std::cerr << Color::red
			<< getClassName() << "::" << __FUNCTION__
                        << " element identified by: "
                        << tag_elem << " not found."
			<< Color::def << std::endl;
          }
      }
    else
      std::cerr << Color::red
		<< getClassName() << "::" << __FUNCTION__
		<< ": domain not set."
		<< Color::def << std::endl;    
  }
 
//! @brief Check if the interior nodes of the set have already a tributary
//! area and warns the user about this circumstance.
bool XC::SetBase::checkTributaries(void) const
  {
    const std::set<int> tmp= getNodeTags();
    const Domain *dom= getPreprocessor()->getDomain();
    int count= 0;
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_node= *i;
            const Node *node= dom->getNode(tag_node);
            if(interiorNode(node))
	      {
                if(node->getTributary()!= 0.0)
		  count++;
	      }
          }
      }
    else
      std::cerr << Color::red
		<< getClassName() << "::" << __FUNCTION__
		<< ": domain not set."
		<< Color::def << std::endl;
    if(count>0)
      {
        std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	          << " " << count
	  	  << " of the set interior nodes out of a total of "
		  << tmp.size()
	          << " have already a tributary area."
	          << " Have you called the computation twice?"
	          << Color::def << std::endl;
      }
    return (count==0);
  }
 
//! @brief Computes the tributary lengths that correspond to each
//! node of the element set.
//! @param initialGeometry: if true compute lengths on the initial geometry
//!                         of the model. Otherwise use its current geometry.
void XC::SetBase::computeTributaryLengths(bool initialGeometry) const
  {
    checkTributaries();
    const std::set<int> tmp= getElementTags();
    const size_t sz= tmp.size();
    if(sz>0)
      {
	const Domain *dom= getPreprocessor()->getDomain();
	if(dom)
	  {
	    for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
	      {
		const int &tag_elem= *i;
		const Element *elem= dom->getElement(tag_elem);
		if(elem)
		  elem->computeTributaryLengths(initialGeometry);
		else
		  std::cerr << Color::red
			    << getClassName() << "::" << __FUNCTION__
			    << " element identified by: "
			    << tag_elem << " not found."
			    << Color::def << std::endl;
	      }
	  }
	else
	  std::cerr << Color::red
		    << getClassName() << "::" << __FUNCTION__
		    << "domain not set."
		    << Color::def << std::endl;
      }
    else
      std::clog << Color::yellow
                << getClassName() << "::" << __FUNCTION__
		<< "; this set (" << this->getName()
	        << ") has no elements. No tributary lengths computed."
		<< Color::def << std::endl;
  }

//! @brief Computes the tributary areas that correspond to each
//! node of the element set.
void XC::SetBase::computeTributaryAreas(bool initialGeometry) const
  {
    checkTributaries();
    const std::set<int> tmp= getElementTags();
    const size_t sz= tmp.size();
    if(sz>0)
      {
	const Domain *dom= getPreprocessor()->getDomain();
	if(dom)
	  {
	    for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
	      {
		const int &tag_elem= *i;
		const Element *elem= dom->getElement(tag_elem);
		if(elem)
		  elem->computeTributaryAreas(initialGeometry);
		else
		  std::cerr << Color::red
			    << getClassName() << "::" << __FUNCTION__
			    << " element identified by: "
			    << tag_elem << " not found."
			    << Color::def << std::endl;
	      }
	  }
	else
	  std::cerr << Color::red
		    << getClassName() << "::" << __FUNCTION__
		    << "domain not set."
		    << Color::def << std::endl;
      }
    else
      std::clog << Color::yellow
                << getClassName() << "::" << __FUNCTION__
		<< "; this set (" << this->getName()
	        << ") has no elements. No tributary areas computed."
		<< Color::def << std::endl;
  }

//! @brief Computes the tributary volumes that correspond to each
//! node of the element set.
void XC::SetBase::computeTributaryVolumes(bool initialGeometry) const
  {
    checkTributaries();
    const std::set<int> tmp= getElementTags();
    const size_t sz= tmp.size();
    if(sz>0)
      {
	const Domain *dom= getPreprocessor()->getDomain();
	if(dom)
	  {
	    for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
	      {
		const int &tag_elem= *i;
		const Element *elem= dom->getElement(tag_elem);
		if(elem)
		  elem->computeTributaryVolumes(initialGeometry);
		else
		  std::cerr << Color::red
			    << getClassName() << "::" << __FUNCTION__
			    << " element identified by: "
			    << tag_elem << " not found."
			    << Color::def << std::endl;
	      }
	  }
	else
	  std::cerr << Color::red
		    << getClassName() << "::" << __FUNCTION__
		    << "domain not set."
		    << Color::def << std::endl;
      }
    else
      std::clog << Color::yellow
                << getClassName() << "::" << __FUNCTION__
		<< "; this set (" << this->getName()
	        << ") has no elements. No tributary volumes computed."
		<< Color::def << std::endl;
  }

//! @brief Return a Python dictionary containing the object members values.
boost::python::dict XC::SetBase::getPyDict(void) const
  {
    boost::python::dict retval= EntMdlrBase::getPyDict();
    // Retrieve properties.
    retval["colors"]= color.getPyList();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::SetBase::setPyDict(const boost::python::dict &d)
  {
    EntMdlrBase::setPyDict(d);
    boost::python::list tmp= boost::python::extract<boost::python::list>(d["colors"]);
    color= Vector(tmp);
  }
