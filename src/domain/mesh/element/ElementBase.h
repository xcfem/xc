// -*-c++-*-
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
//ElementBase.h

#ifndef ElementBase_h
#define ElementBase_h

#include "Element.h"
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include "material/Material.h"
#include "domain/domain/Domain.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/utils/misc_utils/colormod.h"

namespace XC {
class Node;

//! @ingroup Elem
//
//! @brief Base class for finite element with
//! pointer to nodes container
template <int NNODES>
class ElementBase: public Element
  {
  protected:
    NodePtrsWithIDs theNodes;  //!< pointers to node.

    template <class TIPOMAT>
    TIPOMAT *cast_material(const Material *ptr_mat);

    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);

  public:
    ElementBase(int tag, int classTag);
    ElementBase(const ElementBase &);
    ElementBase &operator=(const ElementBase &);

    //! @brief Returns the element number of nodes.
    static inline const int numNodes(void)
      { return NNODES; }
    // public methods to obtain information about dof & connectivity    
    int getNumExternalNodes(void) const;
    NodePtrsWithIDs &getNodePtrs(void);
    const NodePtrsWithIDs &getNodePtrs(void) const;
    Pos3d getCenterOfMassPosition(bool initialGeometry= true) const;
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
  };



//! @brief Default constructor.
template <int NNODES>
XC::ElementBase<NNODES>::ElementBase(int tag, int classTag)
  : Element(tag,classTag), theNodes(this,NNODES) {}

//! @brief Copy constructor.
template <int NNODES>
XC::ElementBase<NNODES>::ElementBase(const ElementBase<NNODES> &other)
  : Element(other), theNodes(other.theNodes) 
  { theNodes.set_owner(this); }

//! @brief Assignment operator.
template <int NNODES>
XC::ElementBase<NNODES> &XC::ElementBase<NNODES>::operator=(const ElementBase &other)
  {
    Element::operator=(other);
    theNodes= other.theNodes;
    theNodes.set_owner(this);
    return *this;
  }

//! @brief Return the number of external nodes.
template <int NNODES>
int XC::ElementBase<NNODES>::getNumExternalNodes(void) const
  { return theNodes.size(); }

//! @brief Returns a pointer to the node vector.
template <int NNODES>
const XC::NodePtrsWithIDs &XC::ElementBase<NNODES>::getNodePtrs(void) const
  { return theNodes; }

//! @brief Returns a pointer to the node vector.
template <int NNODES>
XC::NodePtrsWithIDs &XC::ElementBase<NNODES>::getNodePtrs(void)
  { return theNodes; }

//! @brief Casts the material pointer to a suitable type.
template <int NNODES> template <class TIPOMAT>
TIPOMAT *ElementBase<NNODES>::cast_material(const Material *ptr_mat)
  {
    TIPOMAT *retval= nullptr;
    if(ptr_mat)
      {
	const TIPOMAT *tmp = dynamic_cast<const TIPOMAT *>(ptr_mat);
	if(tmp)
	  retval= tmp->getCopy();
	else
	  {
	    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		      << "; on element: " << getTag() 
		      << " the material " << ptr_mat->getTag()
		      << " with name: " << ptr_mat->getName()
		      << " and type: " << ptr_mat->getClassName()
		      << " has not a suitable type."
		      << Color::def << std::endl;
	    abort();
	  }
	if(!retval)
	  {
	    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		      << "; on element: " << getTag()
		      << " can't get a copy of the material with tag: "
		      << ptr_mat->getTag()
		      << Color::def << std::endl;
	    abort();
	  }
      }
    else
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; on element: " << getTag()
		  << " pointer to material is null. "
		  << Color::def << std::endl;
	abort();
      }
    return retval;
  }

//! @brief Send members through the communicator argument.
template <int NNODES>
int XC::ElementBase<NNODES>::sendData(Communicator &comm)
  {
    int res= Element::sendData(comm);
    res+= comm.sendMovable(theNodes,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives members through the communicator argument.
template <int NNODES>
int XC::ElementBase<NNODES>::recvData(const Communicator &comm)
  {
    int res= Element::recvData(comm);
    res+= comm.receiveMovable(theNodes,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Return a Python dictionary with the object members values.
template <int NNODES>
boost::python::dict XC::ElementBase<NNODES>::getPyDict(void) const
  {
    boost::python::dict retval= Element::getPyDict();
    retval["nodes"]= theNodes.getPyDict();
    return retval;
  }
//! @brief Set the values of the object members from a Python dictionary.
template <int NNODES>
void XC::ElementBase<NNODES>::setPyDict(const boost::python::dict &d)
  {
    Element::setPyDict(d);
    theNodes.setPyDict(boost::python::extract<boost::python::dict>(d["nodes"]));
  }
  
//! @brief Return position of the element centroid.
template <int NNODES>
Pos3d XC::ElementBase<NNODES>::getCenterOfMassPosition(bool initialGeometry) const
  { return theNodes.getCenterOfMassPosition(initialGeometry); }

} //end of XC namespace
#endif
