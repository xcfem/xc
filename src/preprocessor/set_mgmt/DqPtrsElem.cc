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
//  Este software se distribuye con la esperanza de que sea útil pero 
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
#include "preprocessor/cad/trf/TrfGeom.h"
#include "xc_utils/src/geom/d1/Polilinea3d.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/MeshEdges.h"

//! @brief Constructor.
XC::DqPtrsElem::DqPtrsElem(EntCmd *owr)
  : DqPtrsKDTree<Element,KDTreeElements>(owr) {}

//! @brief Copy constructor.
XC::DqPtrsElem::DqPtrsElem(const DqPtrsElem &otro)
  : DqPtrsKDTree<Element,KDTreeElements>(otro)
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
XC::DqPtrsElem &XC::DqPtrsElem::operator=(const DqPtrsElem &otro)
  {
    DqPtrsKDTree<Element,KDTreeElements>::operator=(otro);
    kdtreeElements= otro.kdtreeElements;
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

//!  @brief Set indices to the objects to allow its use in VTK.
void XC::DqPtrsElem::numera(void)
  {
    size_t idx= 0;
    for(iterator i= begin();i!=end();i++,idx++)
      {
	Element *ptr= *i;
        ptr->set_indice(idx);
      }
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

//! @brief Calcula los esfuerzos en cada uno of the elements.
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

//! @brief Returns the element set contour.
std::deque<Polilinea3d> XC::DqPtrsElem::getContours(const double &factor) const
  {
    typedef std::set<const Element *> ElementConstPtrSet;
    const Element *elem= nullptr;
    MeshEdges edgesContour;
    for(const_iterator i= begin();i!=end();i++)
      {
        elem= *i;
        const int numEdges= elem->getNumEdges();
        for(int j= 0;j<numEdges;j++)
          {
	    MeshEdge meshEdge(elem->getNodesEdge(j));
            ElementConstPtrSet elementsShared= meshEdge.getConnectedElements(*this);
            if(elementsShared.size()==1) //border element.
              if(find(edgesContour.begin(), edgesContour.end(), meshEdge) == edgesContour.end())
                { edgesContour.push_back(meshEdge); }
          }
      }
    return edgesContour.getContours(factor);
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
        assert(n);
	if(e->In(geomObj,tol))
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
