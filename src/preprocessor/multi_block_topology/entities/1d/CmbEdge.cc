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
//CmbEdge.cc

#include "CmbEdge.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "preprocessor/multi_block_topology/entities/1d/Line.h"
#include "CircularArc.h"
#include "preprocessor/Preprocessor.h"
#include "utility/geom/d3/BND3d.h"
#include "utility/geom/d1/Segment3d.h"
#include "utility/geom/d1/Polyline3d.h"

#include "utility/geom/pos_vec/Pos3dArray.h"
#include "utility/utils/text/text_string.h"
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
    CmbEdge::Side *this_no_const= const_cast<CmbEdge::Side *>(this);
    return this_no_const->P1();
  }

//! @brief Returns a constant pointer to the front end of the edge.
const XC::Pnt *XC::CmbEdge::Side::P2(void) const
  {
    CmbEdge::Side *this_no_const= const_cast<CmbEdge::Side *>(this);
    return this_no_const->P2();
  }

//! @brief Returns a constant pointer to the back end of the edge.
XC::Pnt *XC::CmbEdge::Side::P1(void)
  {
    if(!edge) return nullptr;
    if(forward)
      return edge->P1();
    else
      return edge->P2();
  }

//! @brief Returns a constant pointer to the front end of the edge.
XC::Pnt *XC::CmbEdge::Side::P2(void)
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

//! @brief Returns the distance to the
//! position being passed as parameter.
double XC::CmbEdge::Side::getDist(const Pos3d &p) const
  { return edge->getDist(p); }

//! @brief Return the positions on the line.
Pos3dArray XC::CmbEdge::Side::get_positions(void) const
  {
    if(edge)
      return edge->get_positions();
    else
      return Pos3dArray();
  }

//! @brief Return the pointers to the nodes of the forward sequence.
std::vector<XC::Node *> XC::CmbEdge::Side::getNodesForward(void)
  {
    if(forward)
      return edge->getNodesForward();
    else
      return edge->getNodesReverse();
  }

//! @brief Return the pointers to the nodes of the reverse sequence.
std::vector<XC::Node *> XC::CmbEdge::Side::getNodesReverse(void)
  {
    if(forward)
      return this->edge->getNodesReverse();
    else
      return this->edge->getNodesForward();
  }

//! @brief Return the pointers to the nodes of the forward sequence.
std::vector<const XC::Node *> XC::CmbEdge::Side::getNodesForward(void) const
  {
    const Edge *e= this->edge;
    if(forward)
      return e->getNodesForward();
    else
      return e->getNodesReverse();
  }

//! @brief Return the pointers to the nodes of the reverse sequence.
std::vector<const XC::Node *> XC::CmbEdge::Side::getNodesReverse(void) const
  {
    const Edge *e= this->edge;
    if(forward)
      return e->getNodesReverse();
    else
      return e->getNodesForward();
  }

//! @brief Return the identifiers of the nodes of the forward sequence.
std::vector<int> XC::CmbEdge::Side::getTagsNodesForward(void) const
  {
    if(forward)
      return this->edge->getTagsNodesForward();
    else
      return this->edge->getTagsNodesReverse();
  }

//! @brief Return the identifiers of the nodes of the reverse sequence.
std::vector<int> XC::CmbEdge::Side::getTagsNodesReverse(void) const
  {
    if(forward)
      return this->edge->getTagsNodesReverse();
    else
      return this->edge->getTagsNodesForward();
  }

//! @brief Return the positions of the nodes of the forward sequence.
Pos3dArray XC::CmbEdge::Side::getNodePosForward(void) const
  {
    if(forward)
      return this->edge->getNodePosForward();
    else
      return this->edge->getNodePosReverse();
  }

//! @brief Return the positions of the nodes of the reverse sequence.
Pos3dArray XC::CmbEdge::Side::getNodePosReverse(void) const
  {
    if(forward)
      return this->edge->getNodePosReverse();
    else
      return this->edge->getNodePosForward();
  }

//! @brief Return the node which index is being passed as parameter empezando por el principio.
XC::Node *XC::CmbEdge::Side::getNodeForward(const size_t &i)
  {
    if(!edge)
      return nullptr;
    else
      if(forward)
        return this->edge->getNodeForward(i);
      else
        return this->edge->getNodeReverse(i);
  }

//! @brief Return the node which index is being passed as parameter empezando por el final.
XC::Node *XC::CmbEdge::Side::getNodeReverse(const size_t &i)
  {
    if(!edge)
      return nullptr;
    else
      if(forward)
        return this->edge->getNodeReverse(i);
      else
        return this->edge->getNodeForward(i);
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

//! @brief Return the element size.
double XC::CmbEdge::Side::getElemSize(void) const
  { return edge->getElemSize(); }

//! @brief Return the identifier of the line.
size_t XC::CmbEdge::Side::getTag(void) const
  { return edge->getTag(); }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::CmbEdge::Side::getPyDict(void) const
  {
    boost::python::dict retval= CommandEntity::getPyDict();
    int tmp= -1;
    if(edge)
      tmp= edge->getTag();
    retval["edge"]= tmp;
    retval["forward"]= forward;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::CmbEdge::Side::setPyDict(LineMap &lines, const boost::python::dict &d)
  {
    CommandEntity::setPyDict(d);
    const int tagEdge= boost::python::extract<int>(d["edge"]);
    if(tagEdge>=0)
      edge= lines.busca(tagEdge);
    forward= boost::python::extract<bool>(d["forward"]);
  }


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

//! @brief Reverse the vertex sequence.
//! 
//! Changes the orientation of the object (I->J->K->L => L->K->J->I)
void XC::CmbEdge::reverse(void)
  {
    ID tags= getKPoints();
    lines.clear();
    std::reverse(tags.begin(), tags.end());
    addPoints(tags);
  }

//! @brief Move forward or backwards the sequence of vertexes.
//! 
//! Move forward or backwards the sequence of vertexes. For example if
//! the given advance is 2. Changes the vertex sequence as follows
//! newI= K, newJ= L, newK= I, newL= J.
void XC::CmbEdge::revolve(const int &advance)
  {
    if(advance!=0)
      {
        ID tags= getKPoints();
        if(advance>0) // rotate left.
	  std::rotate(tags.begin(), tags.begin()+advance, tags.end());
        else // rotate right.
	  std::rotate(tags.begin(), tags.begin()+tags.size()+advance, tags.end());
        lines.clear();
        addPoints(tags);
      }
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
//! @brief Returns the distance to the
//! position being passed as parameter.
double XC::CmbEdge::getDist(const Pos3d &p) const
  {
    double retval= DBL_MAX;
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      {
	const double d= (*i).getDist(p);
	if(d<retval)
	  { retval= d; }
      }
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

//! @brief Return the average element size.
double XC::CmbEdge::getAvgElemSize(void) const
  {
    double retval= 0.0;
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      retval+= (*i).getElemSize();
    retval/= lines.size();
    return retval;
  }  

//! @brief Return the maximum element size.
double XC::CmbEdge::getMaxElemSize(void) const
  {
    std::deque<Side>::const_iterator i=lines.begin();
    double retval= (*i).getElemSize();
    i++;
    for(;i!=lines.end();i++)
      retval= std::max(retval,(*i).getElemSize());
    return retval;
  }

//! @brief Return the minimum element size.
double XC::CmbEdge::getMinElemSize(void) const
  {
    std::deque<Side>::const_iterator i=lines.begin();
    double retval= (*i).getElemSize();
    i++;
    for(;i!=lines.end();i++)
      retval= std::min(retval,(*i).getElemSize());
    return retval;
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
        retval(npos)= last_point()->getPos();
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

//! @brief Return true if the given edge makes part of this compound line.
bool XC::CmbEdge::hasEdge(const Edge *l) const
  {
    bool retval= false;
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      {
        if((*i).getEdge() == l)
	  {	    
            retval= true;
	    break;
	  }
      }
    return retval;
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

//! @brief Return the lines that define the edges in a Python list.
boost::python::list XC::CmbEdge::getEdgesPy(void) const
  {
    std::deque<const Edge *> edges= getEdges();
    boost::python::list retval;
    for(std::deque<const Edge *>::const_iterator i= edges.begin(); i!= edges.end(); i++)
      {
	const Edge *pEdge= *i;	
        boost::python::object pyObj(boost::ref(*pEdge));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Return the pointers to the sides at the position
//! argument. If not found returns an empty container.
std::deque<XC::CmbEdge::Side *> XC::CmbEdge::findSides(const Pos3d &pos)
  {
    std::deque<XC::CmbEdge::Side *> retval;
    const double elemSize= getAvgElemSize();
    double tolerance= elemSize/1e4; // Default value.
    for(std::deque<Side>::iterator i=lines.begin();i!=lines.end();i++)
      {
	Side *s= &(*i);
	const Pos3d p1= s->P1()->getPos();
	const Pos3d p2= s->P2()->getPos();
	const double d= dist(Segment3d(p1,p2), pos);
        if(d<tolerance)
	  { retval.push_back(s); }
      }
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
  
//! @brief Return the i-th vertex.
XC::Pnt *XC::CmbEdge::getVertex(const size_t &i)
  {
    if(i<getNumberOfVertices())
      return getSide(i)->P1();
    else
      return P2();
  }

//! @brief Return the vertices.
std::deque<XC::Pnt *> XC::CmbEdge::getVertices(void)
  {
    std::deque<Pnt *> retval;
    for(std::deque<Side>::iterator i=lines.begin();i!=lines.end();i++)
      retval.push_back((*i).P1());
    return retval;
  }

//! @brief Return the vertices.
std::deque<const XC::Pnt *> XC::CmbEdge::getVertices(void) const
  {
    CmbEdge *this_no_const= const_cast<CmbEdge *>(this);
    std::deque<Pnt *> tmp= this_no_const->getVertices();
    std::deque<const Pnt *> retval;
    for(std::deque<Pnt *>::const_iterator i=tmp.begin();i!=tmp.end();i++)
      retval.push_back((*i));
    return retval;
  }

//! @brief Return the vertices.
std::set<const XC::Pnt *> XC::CmbEdge::getVertexSet(void) const
  {
    std::set<const Pnt *> retval;
    std::deque<const Pnt *> tmp= getVertices();
    for(std::deque<const XC::Pnt *>::const_iterator i=tmp.begin();i!=tmp.end();i++)
      retval.insert((*i));
    return retval;
  }

//! @brief Sets the i-th vertex.
void XC::CmbEdge::SetVertice(const size_t &,Pnt *)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
  }

//! @brief Return a pointer to the vertex at the position
//! argument. If not found returns nullptr.
//! @param pos: position to search for the vertex.
XC::Pnt *XC::CmbEdge::findVertex(const Pos3d &pos)
  {
    XC::Pnt *retval= nullptr;
    const double elemSize= getAvgElemSize();
    double tol2= elemSize/1e6;
    std::deque<Pnt *> vertices= getVertices();
    for(std::deque<Pnt *>::iterator i= vertices.begin();i!=vertices.end();i++)
      {
	Pnt *p= *i;
        const double d2= dist2(p->getPos(), pos);
        if(d2<tol2)
	  {
	    retval= p;
	    break;
	  }
      }
    return retval;
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

//! @brief Returns true if this edge is connected to the point.
bool XC::CmbEdge::isConnectedTo(const Pnt &p) const
  {
    bool retval= false;
    if((this->first_point()==&p) || (this->last_point()==&p))
      retval= true;
    return retval;
  }

//! @brief Returns true if this edge is connected to the given one.
bool XC::CmbEdge::isConnectedTo(const CmbEdge &edge) const
  {
    bool retval= false;
    if(isConnectedTo(*edge.first_point()) || isConnectedTo(*edge.last_point()))
      retval= true;
    return retval;
  }

//! @brief Return the surfaces that touch this edge.
std::set<const XC::Face *> XC::CmbEdge::getConnectedSurfaces(void) const
  {
    std::set<const Face *> retval;
    std::deque<const Edge *> edges= this->getEdges();
    for(std::deque<const Edge *>::const_iterator i= edges.begin(); i!= edges.end(); i++)
      {
	const Edge *edge= *i;
	std::set<const Face *> tmp= edge->getConnectedSurfaces();
	if(!tmp.empty())
	  {
	    std::set<const Face *>::const_iterator j= tmp.begin();
	    for(;j!=tmp.end();j++)
	      {
		const Face *f= *j;
                retval.insert(f);
	      }
	  }
      }
    return retval;
  }

//! @brief Get the surfaces connected to this one that belong to the
//! given set.
std::set<const XC::Face *> XC::CmbEdge::getConnectedSurfaces(const SetBase *s) const
  {
    std::set<const Face *> retval;
    std::deque<const Edge *> edges= this->getEdges();
    for(std::deque<const Edge *>::const_iterator i= edges.begin(); i!= edges.end(); i++)
      {
	const Edge *edge= *i;
	std::set<const Face *> tmp= edge->getConnectedSurfaces();
	if(!tmp.empty())
	  {
	    std::set<const Face *>::const_iterator j= tmp.begin();
	    for(;j!=tmp.end();j++)
	      {
		const Face *f= *j;
		if(s->In(f))
		  retval.insert(f);
	      }
	  }
      }
    return retval;
  }

//! @brief Return the surfaces that touch this point (neighbors).
boost::python::list XC::CmbEdge::getConnectedSurfacesPy(void) const
  {
    const std::set<const Face *> tmp= this->getConnectedSurfaces();
    boost::python::list retval;
    for(std::set<const Face *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      {
	const Face *pFace= *i;	
        boost::python::object pyObj(boost::ref(*pFace));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Return the surfaces that touch this point and belong to
//! the given set.
boost::python::list XC::CmbEdge::getConnectedSurfacesPy(const SetBase *s) const
  {
    const std::set<const Face *> tmp= this->getConnectedSurfaces(s);
    boost::python::list retval;
    for(std::set<const Face *>::const_iterator i= tmp.begin(); i!= tmp.end(); i++)
      {
	const Face *pFace= *i;	
        boost::python::object pyObj(boost::ref(*pFace));
	retval.append(pyObj);
      }
    return retval;
  }

//! @brief Returns a polyline representation of the object.
Polyline3d XC::CmbEdge::getPolyline(void) const
  {
    Polyline3d retval;
    retval.push_back(P1()->getPos());
    for(std::deque<Side>::const_iterator i=lines.begin();i!=lines.end();i++)
      retval.push_back((*i).P2()->getPos());
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
        retval= BND3d(getVertex(1)->getPos(),getVertex(1)->getPos());
	if(nv<2)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; the polyline has only a point." << std::endl;
	  }
	else
	  {
  	    for(size_t i=2;i<=nv;i++)
	      retval+= getVertex(i)->getPos();
	  }
      }
    return retval;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::CmbEdge::getPyDict(void) const
  {
    boost::python::dict retval= Edge::getPyDict();
    boost::python::list sideList;
    for(std::deque<Side>::const_iterator i= lines.begin(); i!= lines.end(); i++)
      sideList.append((*i).getPyDict());
    retval["sides"]= sideList; 
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::CmbEdge::setPyDict(const boost::python::dict &d)
  {
    Edge::setPyDict(d);
    boost::python::list sideList= boost::python::extract<boost::python::list>(d["sides"]);
    const size_t sz= boost::python::len(sideList);
    Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
	MultiBlockTopology &mbt= preprocessor->getMultiBlockTopology();
	LineMap &lines= mbt.getLines();
	for(size_t i= 0; i<sz; i++)
	  {
	    Side s;
	    const boost::python::dict sideData= boost::python::extract<boost::python::dict>(sideList[i]);
	    s.setPyDict(lines, sideData);
	    this->lines.push_back(s);
	  }
      }
    else
      std::cerr << getClassName() << __FUNCTION__
	        << "; preprocessor needed." << std::endl;
  }

