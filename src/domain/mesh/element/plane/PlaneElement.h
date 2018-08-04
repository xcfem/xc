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
//PlaneElement.h

#ifndef PlaneElement_h
#define PlaneElement_h
                                                                        
#include <domain/mesh/element/ElemWithMaterial.h>
#include "xc_utils/src/geom/d2/Polygon3d.h"
#include "xc_utils/src/geom/d1/Segment3d.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "preprocessor/Preprocessor.h"

#include "domain/mesh/node/Node.h"

namespace XC {
//! @ingroup Elem
//!
//! @defgroup PlaneElements Bidimensional elements (plane problems, shells,...).
//
//! @ingroup PlaneElements
//! @brief Base class for plane elements.
template <int NNODES,class PhysProp>
class PlaneElement : public ElemWithMaterial<NNODES, PhysProp>
  {
  protected:
    mutable std::vector<double> tributaryAreas;
  public:
    PlaneElement(int tag, int classTag,const PhysProp &);
    void checkElem(void);
    void setDomain(Domain *theDomain);

    virtual Polygon3d getPolygon(bool initialGeometry= true) const;
    virtual Segment3d getSide(const size_t &i,bool initialGeometry= true) const;
    Pos3d getCenterOfMassPosition(bool initialGeometry= true) const;
    double getPerimeter(bool initialGeometry= true) const;
    double getArea(bool initialGeometry= true) const;
    virtual void computeTributaryAreas(bool initialGeometry= true) const;
    double getTributaryArea(const Node *) const;

    double getDist2(const Pos2d &p,bool initialGeometry= true) const;
    double getDist(const Pos2d &p,bool initialGeometry= true) const;
    double getDist2(const Pos3d &p,bool initialGeometry= true) const;
    double getDist(const Pos3d &p,bool initialGeometry= true) const;

    size_t getDimension(void) const;

  };

//! @brief Constructor.
template <int NNODES,class PhysProp>
XC::PlaneElement<NNODES, PhysProp>::PlaneElement(int tag,int classTag,const PhysProp &physProp)
  :ElemWithMaterial<NNODES, PhysProp>(tag,classTag,physProp), tributaryAreas(NNODES,0.0)
  {}

//! @brief Sets nodes and checks the element.
template <int NNODES,class PhysProp>
void XC::PlaneElement<NNODES, PhysProp>::checkElem(void)
  {
    if(this->getNodePtrs().hasNull())
      std::cerr << "the element: " << this->getTag()
                << " tiene pointers to node, nulos." << std::endl;
    else
      {
        const double area= this->getArea();
        if(area<1e-3)
          {
            std::cerr << "Element: " << this->getTag() << " with nodes: [";
            const std::vector<int> inodes= this->getNodePtrs().getTags();
            std::vector<int>::const_iterator i= inodes.begin();
            std::cerr << *i;
            i++;
            for(;i!=inodes.end();i++)
	      std::cerr << "," << *i;
            std::cerr << "] has a very little area (" << area << ").\n";
          }
      }
  }

//! @brief Sets the element domain.
template <int NNODES,class PhysProp>
void XC::PlaneElement<NNODES, PhysProp>::setDomain(Domain *theDomain)
  {
    ElemWithMaterial<NNODES, PhysProp>::setDomain(theDomain);
    if(theDomain)
      checkElem();
    else
      std::cerr << "PlaneElement::setDomain -- Domain is null\n";
  }

//! @brief Return the position of the element centroid.
template <int NNODES,class PhysProp>
Pos3d XC::PlaneElement<NNODES, PhysProp>::getCenterOfMassPosition(bool initialGeometry) const
  { return getPolygon(initialGeometry).getCenterOfMass(); }

//! @brief Return the element dimension (0, 1, 2 o3 3).
template <int NNODES,class PhysProp>
size_t XC::PlaneElement<NNODES, PhysProp>::getDimension(void) const
  { return 2; }

//! @brief Returns the perimeter of the element.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getPerimeter(bool initialGeometry) const
  { return getPolygon(initialGeometry).getPerimeter(); }

//! @brief Returns element area.
//!
//! @param initialGeometry: if true returns the area of the undeformed geometry.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getArea(bool initialGeometry) const
  { return getPolygon(initialGeometry).getArea(); }

//! @brief Computes tributary areas that correspond to each node.
template <int NNODES,class PhysProp>
void XC::PlaneElement<NNODES, PhysProp>::computeTributaryAreas(bool initialGeometry) const
  {
    tributaryAreas= getPolygon(initialGeometry).getTributaryAreas();
    this->dumpTributaries(tributaryAreas);
  }

//! @brief Returns tributary area for the node being passed as parameter.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getTributaryArea(const Node *nod) const
  {
    double retval= 0.0;
    const int i= this->theNodes.find(nod);
    if(i>=0) //The node belongs to this element.
      retval= tributaryAreas[i];
    return retval;
  }

//! @brief Returns the element contour as a polygon.
template <int NNODES,class PhysProp>
Polygon3d XC::PlaneElement<NNODES, PhysProp>::getPolygon(bool initialGeometry) const
  {
    const std::list<Pos3d> positions= this->getPosNodes(initialGeometry);
    return Polygon3d(positions.begin(),positions.end());
  }

//! @brief Returns a lado of the element. 
// Redefine for elements with more than two nodes by face.
template <int NNODES,class PhysProp>
Segment3d XC::PlaneElement<NNODES, PhysProp>::getSide(const size_t &i,bool initialGeometry) const
  {
    Segment3d retval;
    const NodePtrsWithIDs &nodes= this->getNodePtrs();
    const size_t sz= nodes.size();
    if(i<sz)
      {
        const Pos3d p1= nodes.getPosNode(i,initialGeometry);
        if(i<(sz-1))
          retval= Segment3d(p1,nodes.getPosNode(i+1,initialGeometry));
        else
          retval= Segment3d(p1,nodes.getPosNode(0,initialGeometry));
      }
    return retval;
  }

//! @brief Returns the squared distance from the element to the point
//! being passed as parameter.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getDist2(const Pos2d &p,bool initialGeometry) const
  { return getDist2(To3dXY2d(p),initialGeometry); }

//! @brief Return the distance from the element to the point
//! being passed as parameter.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getDist(const Pos2d &p,bool initialGeometry) const
  { return getDist(To3dXY2d(p),initialGeometry); }

//! @brief Returns the squared distance from the element to the point
//! being passed as parameter.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getDist2(const Pos3d &p,bool initialGeometry) const
  { return getPolygon(initialGeometry).dist2(p); }

//! @brief Return the distance from the element to the point
//! being passed as parameter.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getDist(const Pos3d &p,bool initialGeometry) const
  { return getPolygon(initialGeometry).dist(p); }

} // end of XC namespace
#endif
