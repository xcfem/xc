// -*-c++-*-
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
//QuadBase4N.h
                                                                        
#include "PlaneElement.h"

#ifndef QuadBase4N_h
#define QuadBase4N_h

#include "preprocessor/multi_block_topology/matrices/ElemPtrArray3d.h"
#include "preprocessor/multi_block_topology/aux_meshing.h"
#include "preprocessor/prep_handlers/LoadHandler.h"
#include "domain/load/plane/BidimStrainLoad.h"
#include "vtkCellType.h"

namespace XC {
//! @ingroup PlaneElements
//! @brief Base class for 4 node quads.
template <class PhysProp>
class QuadBase4N: public PlaneElement<4,PhysProp>
  {
    static Matrix &compute_extrapolation_matrix(void);
  protected:
    ElemPtrArray3d put_on_mesh(const NodePtrArray3d &,meshing_dir dm) const;
  public:
    QuadBase4N(int classTag,const PhysProp &pp);
    QuadBase4N(int tag, int classTag,const PhysProp &);
    QuadBase4N(int tag, int classTag, int node1, int node2, int node3, int node4,const PhysProp &pp);

    BoolArray3d getNodePattern(void) const;
    Element::NodesEdge getNodesEdge(const size_t &i) const;
    ID getLocalIndexNodesEdge(const size_t &i) const;
    int getEdgeNodes(const Node *,const Node *) const;

    double getRho(void) const;
    void setRho(const double &);
    double getThickness(void) const;
    void setThickness(const double &);
    
    int getVtkCellType(void) const;

    void zeroLoad(void);	
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    
    Matrix getTrfMatrix(void) const;
    Matrix getLocalAxes(bool initialGeometry= true) const;
    const Matrix &getExtrapolationMatrix(void) const;
  };

//! @brief Constructor
template <class PhysProp>
  XC::QuadBase4N<PhysProp>::QuadBase4N(int classTag,const PhysProp &pp)
  : PlaneElement<4,PhysProp>(0,classTag,pp) {}

//! @brief Constructor.
template <class PhysProp>
XC::QuadBase4N<PhysProp>::QuadBase4N(int tag,int classTag,const PhysProp &pp)
  :PlaneElement<4,PhysProp>(tag,classTag,pp) {}

//! @brief Constructor.
template <class PhysProp>
XC::QuadBase4N<PhysProp>::QuadBase4N(int tag, int classTag, int node1, int node2, int node3, int node4,const PhysProp &pp)
  : PlaneElement<4,PhysProp>(tag,classTag,pp)
  {
    this->theNodes.set_id_nodes(node1,node2,node3,node4);
  }

//! @brief Return a grid of booleans, one for each of the
//! element nodes. If there is a node that doesn't exist
//! for a position the correspondin value will be false.
template <class PhysProp>
BoolArray3d XC::QuadBase4N<PhysProp>::getNodePattern(void) const
  {
    BoolArray3d retval(1,2,2,true); //One layer, two rows, two columns.
    return retval;
  }

//! @brief Put the element on the mesh being passed as parameter.
template <class PhysProp>
XC::ElemPtrArray3d XC::QuadBase4N<PhysProp>::put_on_mesh(const NodePtrArray3d &nodes, meshing_dir dm) const
  { return put_quad4N_on_mesh(*this,nodes,dm); }

//! @brief Returns the nodes de un lado of the element.
template <class PhysProp>
XC::Element::NodesEdge XC::QuadBase4N<PhysProp>::getNodesEdge(const size_t &i) const
  {
    Element::NodesEdge retval(2,static_cast<Node *>(nullptr));
    const NodePtrsWithIDs &nodes= PlaneElement<4,PhysProp>::getNodePtrs();
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
int XC::QuadBase4N<PhysProp>::getEdgeNodes(const Node *n1,const Node *n2) const
  {
    int retval= -1;
    const NodePtrsWithIDs &nodes= PlaneElement<4,PhysProp>::getNodePtrs();
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

//! @brief Returns the local indexes of the nodes that lie on the i-th edge.
template <class PhysProp>
ID XC::QuadBase4N<PhysProp>::getLocalIndexNodesEdge(const size_t &i) const
  {
    ID retval(2);
    const NodePtrsWithIDs &nodes= PlaneElement<4,PhysProp>::getNodePtrs();
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

//! @brief Return material density.
template <class PhysProp>
double XC::QuadBase4N<PhysProp>::getRho(void) const
  { return this->physicalProperties.getRho(); }
  
//! @brief Return material density.
template <class PhysProp>
void XC::QuadBase4N<PhysProp>::setRho(const double &r)
  { this->physicalProperties.setRho(r); }
  
//! @brief Return material density.
template <class PhysProp>
double XC::QuadBase4N<PhysProp>::getThickness(void) const
  { return this->physicalProperties.getThickness(); }
  
//! @brief Return material density.
template <class PhysProp>
void XC::QuadBase4N<PhysProp>::setThickness(const double &t)
  { this->physicalProperties.setThickness(t); }

//! @brief Zeroes loads on element.
template <class PhysProp>
void XC::QuadBase4N<PhysProp>::zeroLoad(void)
  {
    PlaneElement<4,PhysProp>::zeroLoad();
    this->physicalProperties.getMaterialsVector().zeroInitialGeneralizedStrains(); //Removes initial deformations.
    return;
  }

//! @brief Adds to the element the load being passed as parameter.
template <class PhysProp>
int XC::QuadBase4N<PhysProp>::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(this->isDead())
      std::cerr << this->getClassName() 
                << "; load over inactiva element: "
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

//! @brief Interfaz con VTK.
template <class PhysProp>
int XC::QuadBase4N<PhysProp>::getVtkCellType(void) const
  { return VTK_QUAD; }

//! @brief Returns a matrix with the axes of the element as matrix rows
//! [[x1,y1,z1],[x2,y2,z2],...·]
template <class PhysProp>
XC::Matrix XC::QuadBase4N<PhysProp>::getTrfMatrix(void) const
  {
    static Vector v1(2);
    static Vector v2(2);

    //get two vectors (v1, v2) in plane of shell by
    // nodal coordinate differences

    const NodePtrsWithIDs &theNodes= PlaneElement<4,PhysProp>::getNodePtrs();
    const Vector &coor0= theNodes[0]->getCrds();
    const Vector &coor1= theNodes[1]->getCrds();
    const Vector &coor2= theNodes[2]->getCrds();
    const Vector &coor3= theNodes[3]->getCrds();

    v1.Zero( );
    //v1= 0.5 * ( coor2 + coor1 - coor3 - coor0 );
    v1= coor2;
    v1+= coor1;
    v1-= coor3;
    v1-= coor0;
    v1*= 0.50;

    //normalize v1
    //double length= LovelyNorm( v1 );
    double length= v1.Norm( );
    v1/= length;

    v2.Zero( );
    //v2= 0.5 * ( coor3 + coor2 - coor1 - coor0 );
    v2= coor3;
    v2+= coor2;
    v2-= coor1;
    v2-= coor0;
    v2*= 0.50;

    //Gram-Schmidt process for v2

    //double alpha= LovelyInnerProduct( v2, v1 );
    const double alpha= v2^v1;

    //v2 -= alpha*v1;
    static Vector temp(3);
    temp= v1;
    temp*= alpha;
    v2-= temp;


    //normalize v2
    //length= LovelyNorm( v2 );
    length= v2.Norm( );
    v2/= length;

    Matrix R(2,2);
    // Fill in transformation matrix
    R(0,0)= v1(0); R(0,1)= v1(1);
    R(1,0)= v2(0); R(1,1)= v2(1);
    return R;
  }
 
//! @brief Returns a matrix with the axes of the element as matrix rows
//! [[x1,y1,z1],[x2,y2,z2],...·]
template <class PhysProp>
XC::Matrix XC::QuadBase4N<PhysProp>::getLocalAxes(bool initialGeometry) const
  {
    if(!initialGeometry)
      std::cerr << this->getClassName() << "::" << __FUNCTION__
	        << "; for deformed geometry not implemented."
                << std::endl;
    return getTrfMatrix();
  }


//! @brief Compute the matrix that can be used to extrapolate
//! the results from the Gauss points to the element nodes.
template <class PhysProp>
XC::Matrix &XC::QuadBase4N<PhysProp>::compute_extrapolation_matrix(void)
  {
    const double r3div2= sqrt(3)/2.0;
    const double dg= 1.0+r3div2;
    const double m= 1.0-r3div2;
    static Matrix retval(4,4); // 4 nodes 4 gauss points
    // Fill in transformation matrix
    retval(0,0)= dg;   retval(0,1)= -0.5; retval(0,2)= m;  retval(0,3)= -0.5;  
    retval(1,0)= -0.5; retval(1,1)= dg; retval(1,2)= -0.5; retval(1,3)= m;  
    retval(2,0)= m;    retval(2,1)= -0.5; retval(2,2)= dg; retval(2,3)= -0.5;       retval(3,0)= -0.5; retval(3,1)= m; retval(3,2)= -0.5;  retval(3,3)= dg;  
    return retval;
    
  }
 
//! @brief Return the matrix that can be used to extrapolate
//! the results from the Gauss points to the element nodes.
template <class PhysProp>
const XC::Matrix &XC::QuadBase4N<PhysProp>::getExtrapolationMatrix(void) const
  {
    static const Matrix retval= compute_extrapolation_matrix();
    return retval;
  }
 
} // end of XC namespace
#endif
