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
//PlaneElement.h

#ifndef PlaneElement_h
#define PlaneElement_h
                                                                        
#include <domain/mesh/element/ElemWithMaterial.h>
#include "xc_utils/src/geom/d2/Polygon3d.h"
#include "xc_utils/src/geom/d1/Segment3d.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/lists/utils_list_pos3d.h"
#include "preprocessor/Preprocessor.h"

#include "domain/mesh/node/Node.h"

namespace XC {

const double elem_warning_area= 1e-6; // If area smaller than this trig a warning.
//! @ingroup Elem
//!
//! @defgroup PlaneElements Bidimensional elements (plane problems, shells,...).
//
//! @ingroup PlaneElements
//! @brief Base class for plane elements.
//! @details
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
class PlaneElement: public ElemWithMaterial<NNODES, PhysProp>
  {
  protected:
    mutable std::vector<double> tributaryAreas;
  public:
    PlaneElement(int tag, int classTag,const PhysProp &);
    virtual void checkElem(void);
    void setDomain(Domain *theDomain);

    std::deque<Pos3d> getNodePositions(bool initialGeometry= true) const;
    Plane getPlane(bool initialGeometry= true) const;
    virtual Polygon3d getPolygon(bool initialGeometry= true) const;
    bool clockwise(const Pos3d &, bool initialGeometry= true) const;
    bool counterclockwise(const Pos3d &, bool initialGeometry= true) const;
    std::string orientation(const Pos3d &, bool initialGeometry= true) const;
    virtual Segment3d getSide(const size_t &i,bool initialGeometry= true) const;
    double getMaximumCornerAngle(bool initialGeometry= true) const;
    Pos3d getCenterOfMassPosition(bool initialGeometry= true) const;
    virtual double getPerimeter(bool initialGeometry= true) const;
    virtual double getArea(bool initialGeometry= true) const;
    virtual void computeTributaryAreas(bool initialGeometry= true) const;
    const std::vector<double> &getTributaryAreas(void) const;
    double getTributaryArea(const int &) const;
    double getTributaryArea(const Node *) const;

    double getDist2(const Pos2d &p,bool initialGeometry= true) const;
    double getDist(const Pos2d &p,bool initialGeometry= true) const;
    double getDist2(const Pos3d &p,bool initialGeometry= true) const;
    double getDist(const Pos3d &p,bool initialGeometry= true) const;
    Pos2d getProjection(const Pos2d &p,bool initialGeometry= true) const;
    Pos3d getProjection(const Pos3d &p,bool initialGeometry= true) const;

    size_t getDimension(void) const;
  };

//! @brief Constructor.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
XC::PlaneElement<NNODES, PhysProp>::PlaneElement(int tag,int classTag,const PhysProp &physProp)
  :ElemWithMaterial<NNODES, PhysProp>(tag,classTag,physProp), tributaryAreas(NNODES,0.0)
  {}

//! @brief Sets nodes and checks the element.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
void XC::PlaneElement<NNODES, PhysProp>::checkElem(void)
  {
    if(this->getNodePtrs().hasNull())
      std::cerr << this->getClassName() << "::" << __FUNCTION__
		<< "; the element: " << this->getTag()
                << " pointers to nodes not set." << std::endl;
    else
      {
        const double area= this->getArea();
        if(area<elem_warning_area)
          {
            std::clog << this->getClassName() << "::" << __FUNCTION__
		      << "; element: " << this->getTag() << " with nodes: [";
            const std::vector<int> inodes= this->getNodePtrs().getTags();
            std::vector<int>::const_iterator i= inodes.begin();
            std::clog << *i;
            i++;
            for(;i!=inodes.end();i++)
	      std::clog << "," << *i;
            std::clog << "] has a very little area (" << area << ").\n";
          }
      }
  }

//! @brief Sets the element domain.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
void XC::PlaneElement<NNODES, PhysProp>::setDomain(Domain *theDomain)
  {
    ElemWithMaterial<NNODES, PhysProp>::setDomain(theDomain);
    if(theDomain)
      checkElem();
    else
      std::cerr << this->getClassName() << "::" << __FUNCTION__
	        << "; Domain is null\n";
  }

//! @brief Return the position of the element centroid.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
Pos3d XC::PlaneElement<NNODES, PhysProp>::getCenterOfMassPosition(bool initialGeometry) const
  { return getPolygon(initialGeometry).getCenterOfMass(); }

//! @brief Return the element dimension (0, 1, 2 o3 3).
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
size_t XC::PlaneElement<NNODES, PhysProp>::getDimension(void) const
  { return 2; }

//! @brief Returns the perimeter of the element.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getPerimeter(bool initialGeometry) const
  { return getPolygon(initialGeometry).getPerimeter(); }

//! @brief Returns element area.
//!
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
//! @param initialGeometry: if true returns the area of the undeformed geometry.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getArea(bool initialGeometry) const
  { return getPolygon(initialGeometry).getArea(); }

//! @brief Computes tributary areas that correspond to each node.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
void XC::PlaneElement<NNODES, PhysProp>::computeTributaryAreas(bool initialGeometry) const
  {
    tributaryAreas= getPolygon(initialGeometry).getTributaryAreas();
    this->dumpTributaries(tributaryAreas);
  }

//! @brief Return tributary areas that correspond to each node.
template <int NNODES,class PhysProp>
const std::vector<double> &XC::PlaneElement<NNODES, PhysProp>::getTributaryAreas(void) const
  { return this->tributaryAreas; }
  

//! @brief Returns tributary area for the i-th node.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getTributaryArea(const int &i) const
  {
    double retval= 0;
    if((i>0) && (i<NNODES)) 
      retval= tributaryAreas[i];
    return retval;
  }
  
//! @brief Returns tributary area for the node being passed as parameter.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getTributaryArea(const Node *nod) const
  {
    const int i= this->theNodes.find(nod);
    return getTributaryArea(i);
  }

//! @brief Returns the maximum corner angle quality parameter.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getMaximumCornerAngle(bool initialGeometry) const
  {
    const std::deque<Pos3d> positions= getNodePositions(initialGeometry);
    return getMaxCornerAngle(positions.begin(),positions.end());
  }
  
//! @brief Returns the positions of the element nodes
//! (without duplicates corresponding to degenerated elements).
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
std::deque<Pos3d> XC::PlaneElement<NNODES, PhysProp>::getNodePositions(bool initialGeometry) const
  {
    const std::deque<Pos3d> tmp= this->getPosNodes(initialGeometry);
    // Filter collapsed sides.
    std::deque<Pos3d> retval;
    std::deque<Pos3d>::const_iterator i= tmp.begin();
    Pos3d p0= *i;
    i++;
    retval.push_back(p0);
    for(;i!= tmp.end();i++)
      {
	Pos3d p1= *i;
	if(dist2(p0,p1)>1e-12)
	  retval.push_back(p1);
	p0= p1;
      }
    // Repeated nodes removed (if any).
    return retval;
  }

//! @brief Returns the plane that contains the element.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
Plane XC::PlaneElement<NNODES, PhysProp>::getPlane(bool initialGeometry) const
  {
    const std::deque<Pos3d> positions= getNodePositions(initialGeometry);
    return Plane(positions.begin(),positions.end());
  }
  
//! @brief Returns the element contour as a polygon.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
Polygon3d XC::PlaneElement<NNODES, PhysProp>::getPolygon(bool initialGeometry) const
  {
    const std::deque<Pos3d> positions= getNodePositions(initialGeometry);
    Polygon3d retval(positions.begin(),positions.end());
    const double maxAngle= getMaximumCornerAngle();
    if((PlaneElement<NNODES, PhysProp>::verbosity>1) && (abs(maxAngle-M_PI)<1e-3))
      {
        std::cerr << this->getClassName() << "::" << __FUNCTION__
                  << " element with tag: " << this->getTag()
	          << " has maximum corner angle of "
		  << RadToDeg(maxAngle)
		  << " degrees (too distorted)."
		  << " Returning empty polygon."
		  << std::endl;
      }
    return retval;
  }

//! @brief Return the projection of the argument on the element.
//! @param p: position to project.
//! @param initialGeometry: if true use undeformed element geometry.
template <int NNODES,class PhysProp>
Pos2d XC::PlaneElement<NNODES, PhysProp>::getProjection(const Pos2d &p,bool initialGeometry) const
  {
    const Pos3d tmp(getProjection(Pos3d(p.x(),p.y(),0.0)));
    return tmp.XY2DProjection();    
  }

//! @brief Return the projection of the argument on the element.
//! @param p: position to project.
//! @param initialGeometry: if true use undeformed element geometry.
template <int NNODES,class PhysProp>
Pos3d XC::PlaneElement<NNODES, PhysProp>::getProjection(const Pos3d &p,bool initialGeometry) const
  {
    const Plane plane= getPlane(initialGeometry);
    return plane.Projection(p);
  }
  
//! @brief Return true if the nodes are clockwise ordered
//! with respect to the element.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
bool XC::PlaneElement<NNODES, PhysProp>::clockwise(const Pos3d &vPoint, bool initialGeometry) const
  {
    const Polygon3d plg= this->getPolygon(initialGeometry);
    return plg.clockwise(vPoint);
  }
  
//! @brief Return true if the nodes are counterclockwise ordered
//! with respect to the element.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
bool XC::PlaneElement<NNODES, PhysProp>::counterclockwise(const Pos3d &vPoint, bool initialGeometry) const
{ return !this->clockwise(vPoint, initialGeometry); }

//! @brief Return the orientation of the element
//! (clockwise or counterclockwise).
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
std::string XC::PlaneElement<NNODES, PhysProp>::orientation(const Pos3d &vPoint, bool initialGeometry) const
  {
    std::string retval= "counterclockwise";
    if(this->clockwise(vPoint, initialGeometry))
      { retval= "clockwise"; }
    return retval;
  }


//! @brief Returns a lado of the element. 
// Redefine for elements with more than two nodes by face.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
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
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getDist2(const Pos2d &p,bool initialGeometry) const
  { return getDist2(To3dXY2d(p),initialGeometry); }

//! @brief Return the distance from the element to the point
//! being passed as parameter.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getDist(const Pos2d &p,bool initialGeometry) const
  { return getDist(To3dXY2d(p),initialGeometry); }

//! @brief Returns the squared distance from the element to the point
//! being passed as parameter.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getDist2(const Pos3d &p,bool initialGeometry) const
  { return getPolygon(initialGeometry).dist2(p); }

//! @brief Return the distance from the element to the point
//! being passed as parameter.
//! @tparam NNODES number of nodes.
//! @tparam PhysProp material properties.
template <int NNODES,class PhysProp>
double XC::PlaneElement<NNODES, PhysProp>::getDist(const Pos3d &p,bool initialGeometry) const
  { return getPolygon(initialGeometry).dist(p); }

} // end of XC namespace
#endif
