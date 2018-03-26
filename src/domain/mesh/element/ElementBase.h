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
//ElementBase.h

#ifndef ElementBase_h
#define ElementBase_h

#include "Element.h"
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include "material/Material.h"
#include "domain/domain/Domain.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

namespace XC {
class Node;

//! \ingroup Elem
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

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    ElementBase(int tag, int classTag);
    ElementBase(const ElementBase &otro);
    ElementBase &operator=(const ElementBase &otro);

    //! @brief Returns the element number of nodes.
    static inline const int numNodes(void)
      { return NNODES; }
    // public methods to obtain inforrmation about dof & connectivity    
    int getNumExternalNodes(void) const;
    NodePtrsWithIDs &getNodePtrs(void);
    const NodePtrsWithIDs &getNodePtrs(void) const;
    Pos3d getPosCdg(bool initialGeometry= true) const;
  };



//! @brief Default constructor.
template <int NNODES>
XC::ElementBase<NNODES>::ElementBase(int tag, int classTag)
  : Element(tag,classTag), theNodes(this,NNODES) {}

//! @brief Constructor.
template <int NNODES>
XC::ElementBase<NNODES>::ElementBase(const ElementBase<NNODES> &otro)
  : Element(otro), theNodes(otro.theNodes) 
  { theNodes.set_owner(this); }

//! @brief Assignment operator.
template <int NNODES>
XC::ElementBase<NNODES> &XC::ElementBase<NNODES>::operator=(const ElementBase &otro)
  {
    Element::operator=(otro);
    theNodes= otro.theNodes;
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
    const TIPOMAT *tmp = dynamic_cast<const TIPOMAT *>(ptr_mat);
    if(tmp)
      retval= tmp->getCopy();
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; on element: " << getTag() 
                  << " the material " << ptr_mat->getTag()
                  << " has not a suitable type." << std::endl;
        abort();
      }
    if(!retval)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; on element: " << getTag()
		  << "can't get a copy of the material with tag: "
		  << ptr_mat->getTag() << std::endl;
        abort();
      }
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
template <int NNODES>
int XC::ElementBase<NNODES>::sendData(CommParameters &cp)
  {
    int res= Element::sendData(cp);
    res+= cp.sendMovable(theNodes,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
template <int NNODES>
int XC::ElementBase<NNODES>::recvData(const CommParameters &cp)
  {
    int res= Element::recvData(cp);
    res+= cp.receiveMovable(theNodes,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Return position of the element centroid.
template <int NNODES>
Pos3d XC::ElementBase<NNODES>::getPosCdg(bool initialGeometry) const
  { return theNodes.getPosCdg(initialGeometry); }

} //end of XC namespace
#endif
