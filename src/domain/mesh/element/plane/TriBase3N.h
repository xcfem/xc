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
//TriBase3N.h
                                                                        
#include "PlaneElement.h"

#ifndef TriBase3N_h
#define TriBase3N_h

#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "preprocessor/multi_block_topology/aux_meshing.h"
#include "preprocessor/prep_handlers/LoadHandler.h"
#include "domain/load/plane/BidimStrainLoad.h"
#include "vtkCellType.h"

namespace XC {
//! @ingroup PlaneElements
//
//! @brief Base class for 3 node triangles.
template <class PhysProp> //3 Gauss point by default.
class TriBase3N: public PlaneElement<3,PhysProp>
  {
  protected:
    ElemPtrArray3d put_on_mesh(const NodePtrArray3d &,meshing_dir dm) const;
  public:

    TriBase3N(int classTag,const PhysProp &);
    TriBase3N(int tag, int classTag,const PhysProp &);
    TriBase3N(int tag, int classTag, int node1, int node2, int node3,const PhysProp &pp);

    Element::NodesEdge getNodesEdge(const size_t &i) const;
    ID getLocalIndexNodesEdge(const size_t &i) const;
    int getEdgeNodes(const Node *,const Node *) const;

    int getVtkCellType(void) const;

    void zeroLoad(void);	
    int addLoad(ElementalLoad *theLoad, double loadFactor);

  };

//! @brief Constructor
template <class PhysProp>
XC::TriBase3N<PhysProp>::TriBase3N(int classTag,const PhysProp &pp)
  : PlaneElement<3,PhysProp>(0,classTag,pp) {}

//! @brief Constructor.
template <class PhysProp>
XC::TriBase3N<PhysProp>::TriBase3N(int tag,int classTag,const PhysProp &physProp)
  :PlaneElement<3,PhysProp>(tag,classTag,physProp) {}

//! @brief Constructor.
template <class PhysProp>
XC::TriBase3N<PhysProp>::TriBase3N(int tag, int classTag, int node1, int node2, int node3,const PhysProp &pp)
  : PlaneElement<3,PhysProp>(tag,classTag,pp)
  {
    this->theNodes.set_id_nodes(node1,node2,node3);
  }

//! @brief Put the element on the mesh being passed as parameter.
template <class PhysProp>
XC::ElemPtrArray3d XC::TriBase3N<PhysProp>::put_on_mesh(const XC::NodePtrArray3d &nodes,meshing_dir dm) const
  {
    std::cerr << "ElemPtrArray3d XC::TriBase3N<PhysProp>::put_on_mesh not implemented" << std::endl; 
    ElemPtrArray3d retval;
    return retval;
  }

//! @brief Returns the nodes de un lado of the element.
template <class PhysProp>
XC::Element::NodesEdge XC::TriBase3N<PhysProp>::getNodesEdge(const size_t &i) const
  {
    Element::NodesEdge retval(2,static_cast<Node *>(nullptr));
    const NodePtrsWithIDs &nodes= PlaneElement<3,PhysProp>::getNodePtrs();
    const size_t sz= nodes.size();
    if(i<sz)
      {
        retval[0]= nodes(i);
        if(i<(sz-1))
          retval[1]= nodes(i+1);
        else
          retval[1]= nodes(0);
      }
    return retval;    
  }

//! @brief Returns the edge of the element
//! that ends in the nodes being passed as parameters.
template <class PhysProp>
int XC::TriBase3N<PhysProp>::getEdgeNodes(const Node *n1,const Node *n2) const
  {
    int retval= -1;
    const NodePtrsWithIDs &nodes= PlaneElement<3,PhysProp>::getNodePtrs();
    const int i1= nodes.find(n1);
    const int i2= nodes.find(n2);
    if((i1>=0) && (i2>=0))
      {
        const int dif= i2-i1;
        if(dif==1)
          retval= i1;
        else if(dif==-1)
          retval= i2;
        else if((i1==3) && (i2==0))
          retval= 3;
        else if((i1==0) && (i2==3))
          retval= 3;
      }
    return retval;
  }

//! @brief Returns the local indexes of the nodes that lies on the i-th edge.
template <class PhysProp>
ID XC::TriBase3N<PhysProp>::getLocalIndexNodesEdge(const size_t &i) const
  {
    ID retval(2);
    const NodePtrsWithIDs &nodes= PlaneElement<3,PhysProp>::getNodePtrs();
    const size_t sz= nodes.size();
    if(i<sz)
      {
        retval[0]= i;
        if(i<(sz-1))
          retval[1]= i+1;
        else
          retval[1]= 0;
      }
    return retval;
  }

//! @brief Zeroes loads on element.
template <class PhysProp>
void XC::TriBase3N<PhysProp>::zeroLoad(void)
  {
    PlaneElement<3,PhysProp>::zeroLoad();
    this->physicalProperties.getMaterialsVector().zeroInitialGeneralizedStrains(); //Removes initial deformations.
    return;
  }

//! @brief Adds to the element the load being passed as parameter.
template <class PhysProp>
int XC::TriBase3N<PhysProp>::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(this->isDead())
      std::cerr << this->getClassName() 
                << "; load over inactive element: "
                << this->getTag() << std::endl;
    else
      {
        if(const BidimStrainLoad *strainLoad= dynamic_cast<const BidimStrainLoad *>(theLoad)) //Prescribed deformations.
          {
            static std::vector<Vector> initStrains;
            initStrains= strainLoad->getStrains();
            for(std::vector<Vector>::iterator i= initStrains.begin();i!=initStrains.end();i++)
              (*i)*= loadFactor;
            this->physicalProperties.getMaterialsVector().addInitialGeneralizedStrains(initStrains);
          }
        else
          {
            std::cerr << "TriBase3N::addLoad -- load type unknown for element with tag: " <<
            this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

//! @brief Interfaz con VTK.
template <class PhysProp>
int XC::TriBase3N<PhysProp>::getVtkCellType(void) const
  { return VTK_TRIANGLE; }

} // end of XC namespace
#endif
