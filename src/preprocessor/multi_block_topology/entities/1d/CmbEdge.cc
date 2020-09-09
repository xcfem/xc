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
//CmbEdge.cc

#include "CmbEdge.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/multi_block_topology/entities/1d/Line.h"
#include "CircularArc.h"
#include "preprocessor/Preprocessor.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/d1/Segment3d.h"
#include "xc_utils/src/geom/d1/Polyline3d.h"

#include "xc_utils/src/geom/pos_vec/Pos3dArray.h"
#include "xc_utils/src/utils/text/text_string.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::CmbEdge::Side::Side(Edge *ptr,const bool &s)
  : edge(ptr), forward(s) {}

//! @brief Comparison operator.
bool XC::CmbEdge::Side::equal(const XC::CmbEdge::Side &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= (edge==other.edge);
        if(retval)
          retval= (forward==other.forward);
       }
    return retval;
  }

//! @brief Returns a pointer to the edge.
XC::Edge *XC::CmbEdge::Side::getEdge(void)
  { return edge; }
//! @brief Returns a constant pointer to the edge.
const XC::Edge *XC::CmbEdge::Side::getEdge(void) const
  { return edge; }
//! @brief Assigns the line.
void XC::CmbEdge::Side::SetEdge(Edge *l)
  { edge= l; }
//! @brief Returns a constant pointer to the back end of the edge.
const XC::Pnt *XC::CmbEdge::Side::P1(void) const
  {
    if(!edge) return nullptr;
    if(forward)
      return edge->P1();
    else
      return edge->P2();
  }
//! @brief Returns a constant pointer to the front end of the edge.
const XC::Pnt *XC::CmbEdge::Side::P2(void) const
  {
    if(!edge) return nullptr;
    if(forward)
      return edge->P2();
    else
      return edge->P1();
  }


//! @brief Returns true if the points being passed as parameters
//! are the endpoints of the side.
bool XC::CmbEdge::Side::areEndPoints(const Pnt *p1,const Pnt *p2) const
  {
    bool retval= false;
    if(edge)
      retval= edge->areEndPoints(p1,p2);
    const Pnt *e1= P1();
    const Pnt *e2= P2();
    if(p1==e1)
      retval= (p2==e2);
    else if(p1==e2)
      retval= (p2==e1);
    return retval;
  }

//! @brief Return the name of the line.
const std::string &XC::CmbEdge::Side::getName(void) const
  { return edge->getName(); }

//! @brief Return the side length.
double XC::CmbEdge::Side::getLength(void) const
  { return edge->getLength(); }

//! @brief Returns true if this object lies inside the
//! geometric object.
//!
//! @param geomObj: object to be contained in.
//! @param tol: tolerance.
bool XC::CmbEdge::Side::In(const GeomObj3d &geomObj, const double &tol) const
  { return edge->In(geomObj,tol); }

//! @brief Returns true if this object lies outside the
//! geometric object.
//!
//! @param geomObj: object to be contained in.
//! @param tol: tolerance.
bool XC::CmbEdge::Side::Out(const GeomObj3d &geomObj, const double &tol) const
  { return !In(geomObj,tol); }

//! @brief Return the segment than links both ends.
Pos3d XC::CmbEdge::Side::getCentroid(void) const
  { return edge->getCentroid(); }

//! @brief Returns a vector tangent to the side at the point s.
const XC::Vector &XC::CmbEdge::Side::getTang(const double &s) const
  { return edge->getTang(s); }

//! @brief Return the positions on the line.
Pos3dArray XC::CmbEdge::Side::get_positions(void) const
  {
    if(edge)
      return edge->get_positions();
    else
      return Pos3dArray();
  }

//! @brief Return the identifiers of the nodes of the forward sequence.
std::vector<int> XC::CmbEdge::Side::getTagsNodesForward(void) const
  {
    if(forward)
      return edge->getTagsNodesForward();
    else
      return edge->getTagsNodesReverse();
  }

//! @brief Return the identifiers of the nodes of the reverse sequence.
std::vector<int> XC::CmbEdge::Side::getTagsNodesReverse(void) const
  {
    if(forward)
      return edge->getTagsNodesReverse();
    else
      return edge->getTagsNodesForward();
  }

//! @brief Return the positions of the nodes of the forward sequence.
Pos3dArray XC::CmbEdge::Side::getNodePosForward(void) const
  {
    if(forward)
      return edge->getNodePosForward();
    else
      return edge->getNodePosReverse();
  }

//! @brief Return the positions of the nodes of the reverse sequence.
Pos3dArray XC::CmbEdge::Side::getNodePosReverse(void) const
  {
    if(forward)
      return edge->getNodePosReverse();
    else
      return edge->getNodePosForward();
  }

//! @brief Return the node which index is being passed as parameter empezando por el principio.
XC::Node *XC::CmbEdge::Side::getNodeForward(const size_t &i)
  {
    if(!edge)
      return nullptr;
    else
      if(forward)
        return edge->getNodeForward(i);
      else
        return edge->getNodeReverse(i);
  }

//! @brief Return the node which index is being passed as parameter empezando por el final.
XC::Node *XC::CmbEdge::Side::getNodeReverse(const size_t &i)
  {
    if(!edge)
      return nullptr;
    else
      if(forward)
        return edge->getNodeReverse(i);
      else
        return edge->getNodeForward(i);
  }

//! @brief Return the node which index is being passed as parameter.
XC::Node *XC::CmbEdge::Side::getNode(const size_t &i)
  {
    XC::Node *retval= nullptr;
    if(edge)
      {
        if(forward)
          retval=  edge->getNode(i);
        else
          {
            const size_t n= edge->getNumberOfNodes();
            retval=  edge->getNode(n-i+1);
          }
      }
    return retval;
  }

//! @brief Return the node which index is being passed as parameter.
const XC::Node *XC::CmbEdge::Side::getNode(const size_t &i) const
  {
    if(!edge) return nullptr;
    const size_t n= edge->getNumberOfNodes();
    if(forward)
      return edge->getNode(i);
    else
      return edge->getNode(n-i);
  }

//! @brief Genertes a mesh from the corresponding line.
void XC::CmbEdge::Side::genMesh(meshing_dir dm)
  { edge->genMesh(dm); }

//! @brief Set the number of divisions of the side
//! and the number of divisions of its homologous
//! edges.
void XC::CmbEdge::Side::setNDivHomologousEdges(const size_t &nd)
  {
    edge->setNDivHomologousEdges(nd);
  }

//! @brief Set the number of divisions of the side.
void XC::CmbEdge::Side::setNDiv(const size_t &nd)
  {
    edge->setNDiv(nd);
  }

//! @brief Return the number of divisions of the line.
size_t XC::CmbEdge::Side::NDiv(void) const
  { return edge->NDiv(); }

//! @brief Return the identifier of the line.
size_t XC::CmbEdge::Side::getTag(void) const
  { return edge->getTag(); }

//! @brief Constructor.
XC::CmbEdge::CmbEdge(void)
  : Edge(),lines(0) {}

//! @brief Constructor.
XC::CmbEdge::CmbEdge(Preprocessor *m,const size_t &nd)
  : Edge(m,nd), lines(0) {}

//! @brief Constructor.
//! @param name: Object identifier.
//! @param m: Pointer to preprocessor.
//! @param nd: number of divisions.
XC::CmbEdge::CmbEdge(const std::string &name,Preprocessor *m,const size_t &nd)
  : Edge(name,m,nd), lines(0) {}

//! @brief Comparison operator.
bool XC::CmbEdge::operator==(const CmbEdge &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= Edge::operator==(other);
        if(retval)
          retval= (lines==other.lines);
       }
    return retval;
  }

//! @brief Virtual constructor.
XC::SetEstruct *XC::CmbEdge::getCopy(void) const
  { return new CmbEdge(*this); }


//! @brief Returns a pointer to the first lado.
XC::CmbEdge::Side *XC::CmbEdge::first_line(void)
  {
    if(lines.empty())
      return nullptr;
    else
      return &(*lines.begin());
  }
//! @brief Returns a pointer to the first lado.
const XC::CmbEdge::Side *XC::CmbEdge::first_line(void) const
  {
    if(lines.empty())
      return nullptr;
    else
      return &(*lines.begin());
  }
//! @brief Returns a pointer to the last edge.
XC::CmbEdge::Side *XC::CmbEdge::last_line(void)
  {
    if(lines.empty())
      return nullptr;
    else
      return &(*lines.rbegin());
  }
//! @brief Returns a pointer to the last edge.
const XC::CmbEdge::Side *XC::CmbEdge::last_line(void) const
  {
    if(lines.empty())
      return nullptr;
    else
      return &(*lines.rbegin());
  }

//! @brief Returns a pointer to the first point.
const XC::Pnt *XC::CmbEdge::first_point(void) const
  {
    const Side *pl= first_line();
    if(pl)
      return pl->P1();
    else
      return nullptr;
   }

//! @brief Returns a pointer to the last point.
const XC::Pnt *XC::CmbEdge::last_point(void) const
  {
    const Side *ul= last_line();
    if(ul)
      return ul->P2();
    else
      return nullptr;
   }

//! @brief Returns a constant pointer to start point.
const XC::Pnt *XC::CmbEdge::P1(void) const
  { return first_point(); }
//! @brief Returns a constant pointer to end point.
const XC::Pnt *XC::CmbEdge::P2(void) const
  { return last_point(); }

XC::Pnt *XC::CmbEdge::P1(void)
  { return const_cast<Pnt *>(first_point()); }
XC::Pnt *XC::CmbEdge::P2(void)
  { return const_cast<Pnt *>(last_point()); }

//! @brief Return the reverse vertex sequence.
XC::CmbEdge XC::CmbEdge::getReversed(void) const
  {
    CmbEdge retval(*this);
    retval.reverse();
    return retval;
  }

//! @brief Reverse the verrtex sequence.
//! 
//! Changes the orientation of the object (I->J->K->L => L->K->J->I)
void XC::CmbEdge::reverse(void)
  {
    ID tags= getKPoints();
    lines.clear();
    std::reverse(tags.begin(), tags.end());
    addPoints(tags);
  }

//! @brief Return the length of the line.
double XC::CmbEdge::getLength(void) const
  {
    double retval= 0;
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      retval+= (*i).getLength();
    return retval;
  }

//! @brief Return the centroid of the line.
Pos3d XC::CmbEdge::getCentroid(void) const
  {
    Pos3d retval(0.0,0.0,0.0);
    Vector3d v(0.0,0.0,0.0);
    double totalLength= 0.0;
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      {
	const double l= (*i).getLength();
	v+= l*(*i).getCentroid().VectorPos();
        totalLength+= l;
      }
    retval+= v*1.0/totalLength;
    return retval;
  }

//! @brief Returns true if this object lies inside the
//! geometric object.
//!
//! @param geomObj: object to be contained in.
//! @param tol: tolerance.
bool XC::CmbEdge::In(const GeomObj3d &geomObj, const double &tol) const
  {
    bool retval= true;
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      if(!(*i).In(geomObj,tol))
        { retval= false; break; }
    return retval;
  }

//! @brief Returns true if this object lies outside the
//! geometric object.
//!
//! @param geomObj: object to be contained in.
//! @param tol: tolerance.
bool XC::CmbEdge::Out(const GeomObj3d &geomObj, const double &tol) const
  { return !In(geomObj,tol); }

//! @brief Return the number of divisions for the whole object.
size_t XC::CmbEdge::NDiv(void) const
  {
    size_t &nd= const_cast<size_t &>(ndiv);
    nd= 0;
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      nd+= (*i).NDiv();
    return ndiv;
  }

//! @brief Sets the number of divisions for the whole object.
//!
//! @param nd: number of divisions.
void XC::CmbEdge::setNDiv(const size_t &nd)
  {
    const size_t nl= getNumberOfEdges();
    if(nl>0)
      {
        const size_t resto= nd % nl;
        if(resto != 0)
          std::clog << getClassName() << "::" << __FUNCTION__
	            << "; the number of divisions ("
                    << nd << ") is not a multiple of the number of segments ("
                    << nl << ")." << std::endl;
        const size_t q= nd/nl;
        for(std::deque<Side>::iterator i=lines.begin();i!=lines.end();i++)
          (*i).setNDiv(q);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no segments defined." << std::endl;
  }

//! @brief Return positions evenly distributed along the object.
Pos3dArray XC::CmbEdge::get_positions(void) const
  {
    const size_t npos= NDiv()+1; //Number of positions.
    Pos3dArray retval(npos);
    if(!lines.empty())
      {
        size_t cont= 1;
        for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
          {
            const Edge *e= (*i).getEdge();
            Pos3dArray tmp= e->get_positions();
            const size_t sz= tmp.size()-1; //The last one is not added.
            for(size_t i=1;i<sz;i++)
              {
                retval(cont)= tmp(i);
                cont++;
              }
          }
        retval(npos)= last_point()->GetPos();
      }
    return retval;
  }


//! @brief Triggers node creation on the edges.
void XC::CmbEdge::create_line_nodes(void)
  {
    for(std::deque<Side>::iterator i=lines.begin();i!=lines.end();i++)
      (*i).getEdge()->create_nodes();
  }

//! @brief Triggers meshing of lines.
void XC::CmbEdge::line_meshing(meshing_dir dm)
  {
    for(std::deque<Side>::iterator i=lines.begin();i!=lines.end();i++)
      (*i).genMesh(dm);
  }

//! @brief Creates the mesh.
void XC::CmbEdge::genMesh(meshing_dir dm)
  {
    if(verbosity>3)
      std::clog << "Meshing CmbEdge...(" << getName() << ")...";
    line_meshing(dm);
    
    //pointers to nodes.
    ttzNodes= NodePtrArray3d(1,NDiv()+1,1);
    size_t offset_j= 0;// initial column.
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      {
        ttzNodes.putBox(0,offset_j,0,(*i).getEdge()->getTtzNodes());
        offset_j+= (*i).getEdge()->getNumNodeRows()-1;
      }
    //pointers to elements.
    ttzElements= ElemPtrArray3d(1,NDiv(),1);
    offset_j= 0;// initial column.
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      {
        ttzElements.putBox(0,offset_j,0,(*i).getEdge()->getTtzElements());
        offset_j+= (*i).getEdge()->getNumElementRows()-1;
      }
    if(verbosity>3)
      std::clog << "done." << std::endl;
  }

//! @brief Creates and inserts the edges from the indexes
//! being passed as parameter.
void XC::CmbEdge::addLines(const ID &line_indexes)
  {
    const size_t nl= line_indexes.Size(); //Number of indexes.
    for(size_t i= 0;i<nl;i++)
      insert(line_indexes(i));
  }
 
//! @brief Creates and inserts the the edges that link the points
//! from the indexes being passed as parameter.
void XC::CmbEdge::addPoints(const ID &point_indexes)
  {
    const size_t np= point_indexes.Size(); //Number of indexes.
    if(np>=2)
      for(size_t i= 1;i<np;i++)
        {
          Pnt *pA= BuscaPnt(point_indexes(i-1));
          if(!pA)
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; point: " << point_indexes(i-1)
		      << " not found in definition of surface: '"
                      << getName() << "'" << std::endl;
          Pnt *pB= BuscaPnt(point_indexes(i));
          if(!pB)
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; point: " << point_indexes(i)
		      << " not found in definition of surface: '"
                      << getName() << "'" << std::endl;
          newLine(pA,pB);
        }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; at least two points are needed." << std::endl;
  }

//! @brief Close the line.
void XC::CmbEdge::close(void)
  {
    //Closes the polyline.
    Pnt *pA= P2();
    if(!pA)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; last point not found." << std::endl;
    Pnt *pB= P1();
    if(!pB)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; first point not found." << std::endl;
    if(pA!=pB)
      newLine(pA,pB);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; line is already closed." << std::endl;
  }

//! @brief Creates a new line between the points being passed as parameters
//! and inserts it on the edges set.
XC::Edge *XC::CmbEdge::newLine(Pnt *pA,Pnt *pB)
  {
    Line *retval= nullptr;
    assert(getPreprocessor());
    retval= dynamic_cast<Line *>(getPreprocessor()->getMultiBlockTopology().getLines().createLine(pA,pB));
    if(retval)
      {
        insert(retval);
        if(lines.size()==1) //Is the first one.
          {
            if(pA!=lines[0].P1())
              lines[0].reverse();
          }
      }    
    else
       std::cerr << getClassName() << "::" << __FUNCTION__
		 << "; line between points: "
                 << pA->getName() << " and " << pB->getName()
		 << " not found in definition of surface: '"
                 << getName() << "'" << std::endl;
    return retval;
  }

//! @brief Creates a new circle arc between the points being passed
//! as parameters and put it in the edge set.
XC::Edge *XC::CmbEdge::newLine(Pnt *pA,Pnt *pB,Pnt *pC)
  {
    CircularArc *retval= nullptr;
    assert(getPreprocessor());
    retval= dynamic_cast<CircularArc *>(getPreprocessor()->getMultiBlockTopology().getLines().createArc(pA,pB,pC));
    if(retval)
      insert(retval);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; arc between points: "
                << pA->getName() << ", " << pB->getName()
                << " and " << pC->getName()
		<< " not found in definition of surface: '"
                << getName() << "'" << std::endl;
    return retval;
  }

//! @brief Inserts (if found) the line which index is being passed as parameter.
void XC::CmbEdge::insert(const size_t &i)
  {
    Edge *tmp= BuscaEdge(i);
    if(!tmp)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; line identified by: '" 
                << i << "' not found.\n";
    else
      insert(tmp);
  }

//! @brief Inserts the line which pointer is being passed as parameter.
void XC::CmbEdge::insert(Edge *l)
  {
    if(IndiceEdge(l)!= 0) //Line already belongs to the set.
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; line already belongs to the set, insertion ignored.\n";
    else
      {
        if(lines.empty())
          lines.push_back(Side(l,true));
        else
          {
            if(l->P1()== P2()) //forward.
              lines.push_back(Side(l,true));
            else if(l->P2()== P2()) //reverse.
              lines.push_back(Side(l,false));
            else if(l->P1()== P1()) //reverse
              lines.push_front(Side(l,false));
            else if(l->P2()== P1()) //forward
              lines.push_front(Side(l,true));
            else
              std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; line: '" << l->getName()
			<< "' which ends are: '" << l->P1()->getName() << "' y '"
                        << l->P2()->getName() 
                        << "' can't connect to the combination: " << getName() 
			<< "' which ends are: '" << P1()->getName() << "' y '"
                        << P2()->getName() 
                        << " because they don't have shared ends." << std::endl;
          }
        update_topology();
      }
  }

//! @brief Searches the line l from those of this compound line.
size_t XC::CmbEdge::IndiceEdge(const Edge *l) const
  {
    size_t retval= 1;
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      {
        if((*i).getEdge() == l)
          return retval;
        else
          retval++;
      }
    return 0; //No la encuentra.
  }

//! @brief Returns a lado of the line compuesta.
const XC::CmbEdge::Side *XC::CmbEdge::getSide(const size_t &i) const
  { return &lines.at(i-1); }

//! @brief Returns a lado of the line compuesta.
XC::CmbEdge::Side *XC::CmbEdge::getSide(const size_t &i)
  { return &lines.at(i-1); }

//! @brief Return the side whose end points are the points
//! being passed as parameters.
const XC::CmbEdge::Side *XC::CmbEdge::getSideByPoints(const Pnt *p1,const Pnt *p2) const
  {
    const Side *retval= nullptr;
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      if((*i).areEndPoints(p1,p2))
        {
          retval= &(*i);
          break;
        }
    return retval; //Not found.
  }

//! @brief Return the side whose endpoints are the points
//! being passed as parameters.
XC::CmbEdge::Side *XC::CmbEdge::getSideByPoints(const Pnt *p1,const Pnt *p2)
  {
    Side *retval= nullptr;
    for(std::deque<Side>::iterator i=lines.begin();i!=lines.end();i++)
      if((*i).areEndPoints(p1,p2))
        {
          retval= &(*i);
          break;
        }
    return retval;
  }

//! @brief Return the side whose endpoints are the points
//! being passed as parameters.
const XC::CmbEdge::Side *XC::CmbEdge::getSideByPoints(const size_t &idP1,const size_t &idP2) const
  {
    const Pnt *p1= BuscaPnt(idP1);
    const Pnt *p2= BuscaPnt(idP2);
    return getSideByPoints(p1,p2);
  }

//! @brief Return the edge that has its vertices at the points
//! being passed as parameters.
XC::CmbEdge::Side *XC::CmbEdge::getSideByPoints(const size_t &idP1,const size_t &idP2)
  {
    const Pnt *p1= BuscaPnt(idP1);
    const Pnt *p2= BuscaPnt(idP2);
    return getSideByPoints(p1,p2);
  }

//! @brief Return the deque of lines that define the edges.
std::deque<XC::Edge *> XC::CmbEdge::getEdges(void)
  {
    std::deque<XC::Edge *> retval;
    for(std::deque<Side>::iterator i=lines.begin();i!=lines.end();i++)
      retval.push_back((*i).getEdge());
    return retval;
  }

//! @brief Return the deque of lines that define the edges.
std::deque<const XC::Edge *> XC::CmbEdge::getEdges(void) const
  {
    std::deque<const XC::Edge *> retval;
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      retval.push_back((*i).getEdge());
    return retval;
  }

  
//! @brief Return the i-th vertex.
const XC::Pnt *XC::CmbEdge::getVertex(const size_t &i) const
  {
    if(i<getNumberOfVertices())
      return getSide(i)->P1();
    else
      return P2();
  }

//! @brief Return the vertices.
std::set<const XC::Pnt *> XC::CmbEdge::getVertices(void) const
  {
    std::set<const XC::Pnt *> retval;
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      retval.insert((*i).P1());
    retval.insert(P2()); //Last vertex.
    return retval;
  }

//! @brief Sets the i-th vertex.
void XC::CmbEdge::SetVertice(const size_t &,Pnt *)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
  }

//! @brief Returns object k-points.
XC::ID XC::CmbEdge::getKPoints(void) const
  {
    const size_t numVertices= getNumberOfVertices();
    ID retval(numVertices);
    for(size_t i= 0;i<numVertices;i++)
      retval[i]= getSide(i+1)->P1()->getTag();
    return retval;
  }

//! @brief Returns a polyline representation of the object.
Polyline3d XC::CmbEdge::getPolyline(void) const
  {
    Polyline3d retval;
    retval.push_back(P1()->GetPos());
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      retval.push_back((*i).P2()->GetPos());
    return retval;
  }


//! @brief Returns object BND.
BND3d XC::CmbEdge::Bnd(void) const
  { 
    BND3d retval;
    const size_t nv= getNumberOfVertices();
    if(nv<1) //the set is empty.
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; polyline empty." << std::endl;
        return retval;
      }
    else
      {
        retval= BND3d(getVertex(1)->GetPos(),getVertex(1)->GetPos());
	if(nv<2)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; the polyline has only a point." << std::endl;
	  }
	else
	  {
  	    for(size_t i=2;i<=nv;i++)
	      retval+= getVertex(i)->GetPos();
	  }
      }
    return retval;
  }
