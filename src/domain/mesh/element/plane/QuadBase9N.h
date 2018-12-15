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
//QuadBase9N.h
                                                                        
#include "PlaneElement.h"

#ifndef QuadBase9N_h
#define QuadBase9N_h

#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "preprocessor/multi_block_topology/aux_meshing.h"
#include "preprocessor/prep_handlers/LoadHandler.h"
#include "domain/load/plane/BidimStrainLoad.h"
#include "vtkCellType.h"

namespace XC {
//! @ingroup PlaneElements
//! @brief Base class for nine node quads.
template <class PhysProp>
class QuadBase9N : public PlaneElement<9,PhysProp>
  {
  protected:
    ElemPtrArray3d put_on_mesh(const NodePtrArray3d &,meshing_dir dm) const;

  public:
    QuadBase9N(int classTag);
    QuadBase9N(int tag, int classTag,const PhysProp &);

    BoolArray3d getNodePattern(void) const;
    Element::NodesEdge getNodesEdge(const size_t &i) const;
    ID getLocalIndexNodesEdge(const size_t &i) const;
    int getEdgeNodes(const Node *,const Node *) const;
    Polygon3d getPolygon(bool initialGeometry= true) const;
    Segment3d getSide(const size_t &i,bool initialGeometry= true) const;

    int getVtkCellType(void) const;

    void zeroLoad(void);	
    int addLoad(ElementalLoad *theLoad, double loadFactor);
  };

//! @brief Constructor
template <class PhysProp>
XC::QuadBase9N<PhysProp>::QuadBase9N(int classTag)
  : PlaneElement<9,PhysProp>(0,classTag) {}

//! @brief Constructor.
template <class PhysProp>
XC::QuadBase9N<PhysProp>::QuadBase9N(int tag,int classTag,const PhysProp &pp)
  :PlaneElement<9,PhysProp>(tag,classTag,pp) {}

//! @brief Return a grid of booleans, one for each of the
//! element nodes. If there is a node that doesn't exist
//! for a position the correspondin value will be false.
template <class PhysProp>
BoolArray3d XC::QuadBase9N<PhysProp>::getNodePattern(void) const
  {
    BoolArray3d retval(1,3,3,true); //One layer, three rows, three columns.
    return retval;
  }

//! @brief Put the element on the mesh being passed as parameter.
template <class PhysProp>
XC::ElemPtrArray3d XC::QuadBase9N<PhysProp>::put_on_mesh(const NodePtrArray3d &nodes,meshing_dir dm) const
  { return put_quad9N_on_mesh(*this,nodes,dm); }

//! @brief Returns the element contour as a polygon.
template <class PhysProp>
Polygon3d XC::QuadBase9N<PhysProp>::getPolygon(bool initialGeometry) const
  {
    Polygon3d retval;
    const NodePtrsWithIDs &nodePtrs= this->getNodePtrs();
    retval.push_back(nodePtrs.getPosNode(0,initialGeometry));
    retval.push_back(nodePtrs.getPosNode(4,initialGeometry));
    retval.push_back(nodePtrs.getPosNode(1,initialGeometry));
    retval.push_back(nodePtrs.getPosNode(5,initialGeometry));
    retval.push_back(nodePtrs.getPosNode(2,initialGeometry));
    retval.push_back(nodePtrs.getPosNode(6,initialGeometry));
    retval.push_back(nodePtrs.getPosNode(3,initialGeometry));
    retval.push_back(nodePtrs.getPosNode(7,initialGeometry));
    return retval;
  }

//! @brief Returns the element edge.
template <class PhysProp>
Segment3d XC::QuadBase9N<PhysProp>::getSide(const size_t &i,bool initialGeometry) const
  {
    Segment3d retval;
    std::cerr << this->getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Returns the nodes of the element edge.
template <class PhysProp>
XC::Element::NodesEdge XC::QuadBase9N<PhysProp>::getNodesEdge(const size_t &i) const
  {
    Element::NodesEdge retval(3,static_cast<Node *>(nullptr));
    std::cerr << this->getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;    
  }

//! @brief Returns the edge of the element that ends 
//! in the nodes being passed as parameters.
template <class PhysProp>
int XC::QuadBase9N<PhysProp>::getEdgeNodes(const Node *n1,const Node *n2) const
  {
    int retval= -1;
    std::cerr << this->getClassName() << "::" << __FUNCTION__
              << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Returns the local indexes of the nodes that lie on the i-th edge.
template <class PhysProp>
ID XC::QuadBase9N<PhysProp>::getLocalIndexNodesEdge(const size_t &i) const
  {
    ID retval(2);
    std::cerr << this->getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Zeroes loads on element.
template <class PhysProp>
void XC::QuadBase9N<PhysProp>::zeroLoad(void)
  {
    PlaneElement<9,PhysProp>::zeroLoad();
    this->physicalProperties.getMaterialsVector().zeroInitialGeneralizedStrains(); //Removes initial deformations.
    return;
  }
 
//! @brief Adds to the element the load being passed as parameter.
template <class PhysProp>
int XC::QuadBase9N<PhysProp>::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(this->isDead())
      std::cerr << this->getClassName() 
                << "; load over inactive elements: "
                << this->getTag() << std::endl;
    else
      {
        if(const BidimStrainLoad *strainLoad= dynamic_cast<const BidimStrainLoad *>(theLoad)) //Prescribed strains.
          {
            static std::vector<Vector> initStrains;
            initStrains= strainLoad->getStrains();
            for(std::vector<Vector>::iterator i= initStrains.begin();i!=initStrains.end();i++)
              (*i)*= loadFactor;
            this->physicalProperties.getMaterialsVector().addInitialGeneralizedStrains(initStrains);
          }
        else
          {
            std::cerr << this->getClassName() << "::" << __FUNCTION__
		      << "; load type unknown for element with tag: " <<
            this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

//! @brief Interfaz con Vtk.
template <class PhysProp>
int XC::QuadBase9N<PhysProp>::getVtkCellType(void) const
  { return VTK_QUADRATIC_QUAD; }

} // end of XC namespace
#endif
