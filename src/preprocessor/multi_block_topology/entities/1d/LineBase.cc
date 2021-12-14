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
//LineBase.cc

#include "LineBase.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "utility/geom/d3/BND3d.h"
#include "utility/geom/pos_vec/Pos3dArray.h"
#include "utility/matrix/util_matrix.h"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"


//! @brief Constructor.
XC::LineBase::LineBase(Preprocessor *m,const size_t &nd)
  : Edge(m,nd), p1(nullptr), p2(nullptr) {}

//! @brief Constructor.
XC::LineBase::LineBase(const std::string &name,Preprocessor *m,const size_t &nd)
  : Edge(name,m,nd), p1(nullptr), p2(nullptr) {}

//! @brief Comparison operator.
bool XC::LineBase::operator==(const LineBase &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= Edge::operator==(other);
        if(retval)
          retval= (p1==other.p1);
        if(retval)
          retval= (p2==other.p2);
       } 
    return retval;
  }

//! @brief Returns a constant pointer to start point.
const XC::Pnt *XC::LineBase::P1(void) const
  { return p1; }

//! @brief Returns a constant pointer to end point.
const XC::Pnt *XC::LineBase::P2(void) const
  { return p2; }

//! @brief Return the i-th vertex.
const XC::Pnt *XC::LineBase::getVertex(const size_t &i) const
  {
    if(i==1)
      return p1;
    else
      return p2;
  }

//! @brief Return the parameter of the point on the line (distance to the
//! line first point measured over the line).
double XC::LineBase::getLambda(const Node &n) const
  {
    const Pos3d p= n.getInitialPosition3d();
    return getLambda(p);
  }

//! @brief Return true if lambda value (see getLambda) of point a
//! is smaller than lambda value of point b.
bool XC::LineBase::less(const Pos3d &a, const Pos3d &b) const
  { return (getLambda(a)<getLambda(b)); }

//! @brief Return true if lambda value (see getLambda) of node a
//! is smaller than lambda value of node b.
bool XC::LineBase::less(const Node &a, const Node &b) const
  { return (getLambda(a)<getLambda(b)); }

//! @brief Return the positions sorted by its distance to the first
//! point of the line.
std::vector<std::pair<double,Pos3d> >XC::LineBase::sort(const std::deque<Pos3d> &positions) const
  {
    const size_t sz= positions.size();
    std::vector<std::pair<double, size_t> > tmp(sz);
    for(size_t i= 0;i<sz;i++)
      {
	Pos3d p= positions[i]; 
	tmp[i]= std::pair(getLambda(p), i);
      }
    std::sort(tmp.begin(), tmp.end());
    std::vector<std::pair<double,Pos3d> > retval(sz);
    for(size_t i= 0;i<sz;i++)
      {
	Pos3d p= positions[tmp[i].second]; 
	retval[i]= std::pair(getLambda(p), p);
      }
    return retval;
  }

//! @brief Return the nodes sorted by its distance to the line
//! first point.
std::vector<std::pair<double,XC::Node *> >XC::LineBase::sort(const std::deque<Node *> &nodes) const
  {
    const size_t sz= nodes.size();
    std::vector<std::pair<double, Node *> > retval(sz);
    for(size_t i= 0;i<sz;i++)
      {
	Node *n= nodes[i]; 
	retval[i]= std::pair(getLambda(*n), n);
      }
    std::sort(retval.begin(), retval.end());
    return retval;
  }

//! @brief Create nodes on the positions argument.
//!
//! @param Positions to create nodes on.
void XC::LineBase::create_nodes(const std::deque<Pos3d> &positions)
  {
    std::vector<std::pair<double, Pos3d> > sortedNodes= sort(positions);
    const size_t sz= sortedNodes.size();
    Pos3dArray sortedPositions(sz);
    for(size_t i= 0; i<sz;i++)
      sortedPositions(i+1,1)= sortedNodes[i].second;
    Edge::create_nodes(sortedPositions);
  }

//! @brief Checks that the points are defined.
bool XC::LineBase::check_points(void) const
  {
    bool retval= false;
    if(p1 && p2)
      retval= true;
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; entity: '" << getName()
                << " is not defined." << std::endl;
    return retval;
  }

//! @brief Sets the i-th vertex.
void XC::LineBase::SetVertice(const size_t &i,Pnt *p)
  {
    if((i==1) && (p1!=p))
      {
        if(p1) p1->erase_line(this);
        p1= p;
      }
    else if(p2!=p)
      {
        if(p2) p2->erase_line(this);
        p2= p;
      }
    update_topology();
  }

//! @brief Sets the i-th vertex.
void XC::LineBase::SetVertice(const size_t &i,const size_t &id_point)
  {
    SetVertice(i,BuscaPnt(id_point));
    if(!getVertex(i))
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; point: '" << id_point << "' not found \n";
  }

//! @brief Assigns line ends.
void XC::LineBase::SetVertices(Pnt *pA,Pnt *pB)
  {
    if(pA==pB)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< " degenerated line (starts and end in the same point): '"
                << getName() << "'." << std::endl;
    SetVertice(1,pA);
    SetVertice(2,pB);
    if(p1==p2)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< " degenerated line (starts and end in the same point): '"
                << getName() << "'." << std::endl;
    update_topology();
  }
