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
//SetBase.cc

#include "SetBase.h"

#include <boost/any.hpp>
#include "domain/constraints/SFreedom_Constraint.h"


#include "utility/matrix/ID.h"
#include "preprocessor/Preprocessor.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

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
      std::cerr << getClassName() << "::" << __FUNCTION__
                << " three components needed ([red,green,blue])."
		<< std::endl;
  }

//! @brief Return the color of the object [red,green,blue] as
//! integers from 0 to 255.
const XC::Vector &XC::SetBase::getColor(void) const
  { return color; }

//! @brief Generates a finite element mesh from the set components.
void XC::SetBase::genMesh(meshing_dir dm)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented." << std::endl;
  }

//! @brief Creates the inertia load that corresponds to the
//! acceleration argument.
void XC::SetBase::createInertiaLoads(const Vector &)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented." << std::endl;
  }

//! @brief Impone desplazamiento nulo for all the nodes of this set.
void XC::SetBase::fix(const SFreedom_Constraint &)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented." << std::endl;
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
	      std::cerr << getClassName() << "::" << __FUNCTION__
                        << " element identified by: "
                        << tag_elem << " not found." << std::endl;
          }
      }
    else
      std::cerr << "domain not set." << std::endl;    
  }
 
//! @brief Computes the tributary lengths that correspond to each
//! node of the element set.
void XC::SetBase::computeTributaryLengths(bool initialGeometry) const
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
              elem->computeTributaryLengths(initialGeometry);
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__
                        << " element identified by: "
                        << tag_elem << " not found." << std::endl;
          }
      }
    else
      std::cerr << "domain not set." << std::endl;
  }

//! @brief Computes the tributary areas that correspond to each
//! node of the element set.
void XC::SetBase::computeTributaryAreas(bool initialGeometry) const
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
              elem->computeTributaryAreas(initialGeometry);
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__
                        << " element identified by: "
                        << tag_elem << " not found." << std::endl;
          }
      }
    else
      std::cerr << "domain not set." << std::endl;
  }

//! @brief Computes the tributary volumes that correspond to each
//! node of the element set.
void XC::SetBase::computeTributaryVolumes(bool initialGeometry) const
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
              elem->computeTributaryVolumes(initialGeometry);
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__
                        << " element identified by: "
                        << tag_elem << " not found." << std::endl;
          }
      }
    else
      std::cerr << "domain not set." << std::endl;
  }


