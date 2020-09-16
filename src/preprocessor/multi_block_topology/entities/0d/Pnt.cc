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
//Pnt.cc

#include "Pnt.h"
#include "preprocessor/multi_block_topology/entities/1d/Edge.h"
#include "preprocessor/multi_block_topology/trf/TrfGeom.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/utils/text/text_string.h"
#include "xc_utils/src/geom/pos_vec/Pos3dArray.h"
#include "xc_utils/src/geom/pos_vec/Pos3dArray3d.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "preprocessor/multi_block_topology/ReferenceFrame.h"


//! @brief Constructor.
XC::Pnt::Pnt(Preprocessor *m,const Pos3d &pto)
  : EntMdlr(m), p(pto) {}

//! @brief Constructor.
XC::Pnt::Pnt(const std::string &name, Preprocessor *m,const Pos3d &pto)
  : EntMdlr(name,0,m), p(pto) {}

//! @brief Comparison operator.
bool XC::Pnt::operator==(const Pnt &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= EntMdlr::operator==(other);
        if(retval)
          retval= (p==other.p);
      } 
    return retval;
  }

//! @brief Virtual constructor.
XC::SetEstruct *XC::Pnt::getCopy(void) const
  { return new Pnt(*this); }

//! Inserts the line being passed as parameter in the list of lines
//! that begin or end in the point.
void XC::Pnt::insert_line(Edge *l) const
  { lines_pt.insert(l); }

//! Erases the line being passed as parameter in the list of lines
//! that begin or end in the point.
void XC::Pnt::erase_line(Edge *l) const
  {
    std::set<const Edge *>::iterator i= lines_pt.find(l);
    if(i!= lines_pt.end()) //La ha encontrado.
      lines_pt.erase(i);
  }

//! Return the position vector of the point.
Vector3d XC::Pnt::VectorPos(void) const
  { return p.VectorPos();  }

//! @brief Updates topology.
void XC::Pnt::update_topology(void)
  {}

//! @brief Return the object BND.
BND3d XC::Pnt::Bnd(void) const
  { return BND3d(p,p);  }

boost::python::list XC::Pnt::getConnectedEdgesTags(void) const
  {
    boost::python::list retval;
    if(!lines_pt.empty())
      {
        std::set<const Edge *>::const_iterator i= lines_pt.begin();
        for(;i!=lines_pt.end();i++)
          {
            const Edge *l= *i;
            retval.append(l->getTag());
          }
      }
    return retval;
  }

boost::python::list XC::Pnt::getConnectedEdgesPy(void)
  {
    boost::python::list retval;
    if(!lines_pt.empty())
      {
        std::set<const Edge *>::const_iterator i= lines_pt.begin();
        for(;i!=lines_pt.end();i++)
          {
            const Edge *tmp= *i;
	    boost::python::object pyObj(boost::ref(*tmp));
	    retval.append(pyObj);
          }
      }
    return retval;
  }


//! @brief Return the lines that start o finish in this point.
std::set<const XC::Edge *> XC::Pnt::getEdgesThatEndOnThisPoint(void) const
  {
    std::set<const Edge *> retval;
    if(!lines_pt.empty())
      {
        std::set<const Edge *>::const_iterator i= lines_pt.begin();
        for(;i!=lines_pt.end();i++)
          {
            const Edge *l= *i;
            if(isEndPoint(*l))
              retval.insert(l);
          }
      }
    return retval;
  }

//! @brief Return the names of the lines that touch the point.
const std::string &XC::Pnt::getConnectedEdgesNames(void) const
  {
    static std::string retval;
    retval= "";
    if(!lines_pt.empty())
      {
        std::set<const Edge *>::const_iterator i= lines_pt.begin();
        retval+= (*i)->getName();
        for(;i!=lines_pt.end();i++)
          retval+= "," + (*i)->getName();
      }
    return retval;
  }

//! @brief Returns true if the line starts or ends in this point.
bool XC::Pnt::isConnectedTo(const Edge &l) const
  {
    std::set<const Edge *>::const_iterator i= lines_pt.find(&l);
    return (i!=lines_pt.end());
  }

//! @brief Returns true if the point is an end of the edge.
bool XC::Pnt::isEndPoint(const Edge &l) const
  { return l.isEndPoint(*this); }

//! @brief Returns true if the points touches the surface.
bool XC::Pnt::isConnectedTo(const Face &s) const
  {
    for(std::set<const Edge *>::const_iterator i= lines_pt.begin(); i!=lines_pt.end();i++)
      { if((*i)->isConnectedTo(s)) return true; }
    return false;
  }

//! @brief Returns true if the points touch the body.
bool XC::Pnt::isConnectedTo(const Body &b) const
  {
    for(std::set<const Edge *>::const_iterator i= lines_pt.begin(); i!=lines_pt.end();i++)
      { if((*i)->isConnectedTo(b)) return true; }
    return false;
  }

//! @brief Returns the faces connected with the point.
const std::set<const XC::Face *> XC::Pnt::getConnectedSurfaces(void) const
  {
    std::set<const Face *> retval;
    for(std::set<const Edge *>::const_iterator i= lines_pt.begin(); i!=lines_pt.end();i++)
      {
	const std::set<const Face *> tmp= (*i)->getConnectedSurfaces();
        retval.insert(tmp.begin(), tmp.end());
      }
    return retval;
  }

//! @brief Return the squared distance to the position
//! being passed as parameter.
double XC::Pnt::getSquaredDistanceTo(const Pos3d &pt) const
  { return dist2(p,pt);  }

//! @brief Returns true if the point lies inside the
//! geometric object.
bool XC::Pnt::In(const GeomObj3d &geomObj, const double &tol) const
  { return geomObj.In(GetPos(),tol); }

//! @brief Returns true if the point lies outside the
//! geometric object.
bool XC::Pnt::Out(const GeomObj3d &geomObj, const double &tol) const
  { return !geomObj.In(GetPos(),tol); }

//! @brief Creates nodes.
void XC::Pnt::create_nodes(void)
  {
    if(getGenMesh() && (getNumberOfNodes()==0))
      {
        Pos3dArray tmp(1,1,GetPos());
        Pos3dArray3d ptos(1,tmp);
        EntMdlr::create_nodes(ptos);
      }
  }

//! @brief Creates mesh.
void XC::Pnt::genMesh(meshing_dir dm)
  {
    create_nodes();
  }

//! @brief Returns true if the point owns a node (is meshed).
bool XC::Pnt::hasNode(void) const
  {
    bool retval= false;
    if(getNumberOfNodes()>0)
      retval= (getNode()!= nullptr);
    return retval;      
  }

//! @brief Return node's tag.
int XC::Pnt::getTagNode(void) const
  {
    int retval= -1;
    const Node *nod= getNode();
    if(nod)
      retval= nod->getTag();
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; the point: '" << getName()
                << "' has not a node (is not meshed)." << std::endl;
    return retval;
  }


//! @brief Return point's node.
XC::Node *XC::Pnt::getNode(void)
  {
    Node *nod= EntMdlr::getNode();
    if(!nod)
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; the point: '"
		<< getName()
                << "' has not a node (is not meshed)." << std::endl;
    return nod;
  }

//! @brief Return point's node.
const XC::Node *XC::Pnt::getNode(void) const
  {
    const Node *nod= EntMdlr::getNode();
    if(!nod)
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; the point: '"
		<< getName()
                << "' has not a node (is not meshed)." << std::endl;
    return nod;
  }

//! @brief Return the sets to which this point belongs.
std::set<XC::SetBase *> XC::Pnt::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        MapSet &sets= const_cast<MapSet &>(preprocessor->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << getClassName() << __FUNCTION__
	        << "; preprocessor needed." << std::endl;
    return retval;
  }

//! @brief Adds the point to the set being passed as parameters.
void XC::Pnt::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        Set *s= dynamic_cast<Set *>(*i);
        if(s) s->getPoints().push_back(this);
      }
  }

//! @brief Moves the point (used by XC::Set only).
//! @param desplaz: displacement vector.
void XC::Pnt::Move(const Vector3d &desplaz)
  {
    p+=desplaz;
    return;
  }

//! @brief Applies to the point the transformation being passed as parameter.
void XC::Pnt::Transform(const TrfGeom &trf)
  { p= trf.Transform(p); }

XC::Vector &XC::operator-(const Pnt &b,const Pnt &a)
  {
    static Vector retval(3);
    const Pos3d A= a.GetPos();
    const Pos3d B= b.GetPos();
    retval[0]= B.x()-A.x();
    retval[1]= B.y()-A.y();
    retval[2]= B.z()-A.z();
    return retval;
  }

//! @brief Search for a line that connects the points.
const XC::Edge *XC::find_connected_edge_const_ptr(const Pnt &pA,const Pnt &pB)
  {
    const Edge *retval= nullptr;
    const std::set<const Edge *> pA_lines= pA.getEdgesThatEndOnThisPoint();
    for(std::set<const Edge *>::const_iterator i= pA_lines.begin();i!=pA_lines.end();i++)
      if(pB.isConnectedTo(**i))
        {
          retval= *i;
          break;
        }
    return retval;
  }

//! @brief Search for a line that connects the points.
const XC::Edge *XC::find_connected_edge_const_ptr(const Pnt &pA,const Pnt &pB, const Pnt &pC)
  {
    const Edge *retval= nullptr;
    const std::set<const Edge *> pA_lines= pA.getEdgesThatEndOnThisPoint();
    for(std::set<const Edge *>::const_iterator i= pA_lines.begin();i!=pA_lines.end();i++)
      if(pB.isConnectedTo(**i) && pC.isConnectedTo(**i))
        {
          retval= *i;
          break;
        }
    return retval;
  }

//! @brief Search for a line that connects the points.
XC::Edge *XC::find_connected_edge_ptr(const Pnt &pA,const Pnt &pB)
  {
    Edge *retval= nullptr;
    std::set<const Edge *> pA_lines= pA.getEdgesThatEndOnThisPoint();
    for(std::set<const Edge *>::iterator i= pA_lines.begin();i!=pA_lines.end();i++)
      if(pB.isConnectedTo(**i))
        {
          retval= const_cast<Edge *>(*i);
          break;
        }
    return retval;
  }

//! @brief Search for a line that connects the points.
XC::Edge *XC::find_connected_edge_ptr(const Pnt &pA,const Pnt &pB, const Pnt &pC)
  {
    Edge *retval= nullptr;
    std::set<const Edge *> pA_lines= pA.getEdgesThatEndOnThisPoint();
    for(std::set<const Edge *>::iterator i= pA_lines.begin();i!=pA_lines.end();i++)
      if(pB.isConnectedTo(**i) && pC.isConnectedTo(**i))
        {
          retval= const_cast<Edge *>(*i);
          break;
        }
    return retval;
  }

//! @brief Search for a line that connects the points.
XC::Edge *XC::find_edge_ptr_by_endpoints(const Pnt &pA,const Pnt &pB)
  {
    Edge *retval= nullptr;
    std::set<const Edge *> pA_lines= pA.getEdgesThatEndOnThisPoint();
    for(std::set<const Edge *>::iterator i= pA_lines.begin();i!=pA_lines.end();i++)
      if(pB.isEndPoint(**i))
        {
          retval= const_cast<Edge *>(*i);
          break;
        }
    return retval;
  }

//! @brief Search for a line that connects the points.
XC::Edge *XC::find_edge_ptr_by_endpoints(const Pnt &pA,const Pnt &pB, const Pnt &pC)
  {
    Edge *retval= nullptr;
    std::set<const Edge *> pA_lines= pA.getEdgesThatEndOnThisPoint();
    for(std::set<const Edge *>::iterator i= pA_lines.begin();i!=pA_lines.end();i++)
      if(pB.isConnectedTo(**i) && pC.isEndPoint(**i))
        {
          retval= const_cast<Edge *>(*i);
          break;
        }
    return retval;
  }

//! @brief Search for a line that connects the points.
const XC::Edge *XC::find_edge_const_ptr_by_endpoints(const Pnt &pA,const Pnt &pB)
  {
    const Edge *retval= nullptr;
    std::set<const Edge *> pA_lines= pA.getEdgesThatEndOnThisPoint();
    for(std::set<const Edge *>::iterator i= pA_lines.begin();i!=pA_lines.end();i++)
      if(pB.isEndPoint(**i))
        {
          retval= *i;
          break;
        }
    return retval;
  }

//! @brief Search for a line that connects the points.
const XC::Edge *XC::find_edge_const_ptr_by_endpoints(const Pnt &pA,const Pnt &pB, const Pnt &pC)
  {
    const Edge *retval= nullptr;
    std::set<const Edge *> pA_lines= pA.getEdgesThatEndOnThisPoint();
    for(std::set<const Edge *>::iterator i= pA_lines.begin();i!=pA_lines.end();i++)
      if(pB.isConnectedTo(**i) && pC.isEndPoint(**i))
        {
          retval= (*i);
          break;
        }
    return retval;
  }

XC::Face *XC::find_face_ptr_by_vertices(const Pnt &pA,const Pnt &pB,const Pnt &pC,const Pnt &pD)
  {
    const Face *retval= find_face_const_ptr_by_vertices(pA,pB,pC,pD);
    return const_cast<Face *>(retval);
  }
const XC::Face *XC::find_face_const_ptr_by_vertices(const Pnt &pA,const Pnt &pB,const Pnt &pC,const Pnt &pD)
  {
    const Face *retval=nullptr;
    std::set<const Face *> pA_faces= pA.getConnectedSurfaces();
    std::set<const Face *> pB_faces= pB.getConnectedSurfaces();
    std::set<const Face *> tmp1;
    std::set_intersection(pA_faces.begin(),pA_faces.end(),pB_faces.begin(),pB_faces.end(), std::inserter(tmp1, tmp1.begin()));
    std::set<const Face *> pC_faces= pC.getConnectedSurfaces();
    std::set<const Face *> tmp2;
    std::set_intersection(tmp1.begin(),tmp1.end(),pC_faces.begin(),pC_faces.end(), std::inserter(tmp2, tmp2.begin()));
    std::set<const Face *> pD_faces= pD.getConnectedSurfaces();
    std::set<const Face *> tmp3;
    std::set_intersection(tmp2.begin(),tmp2.end(),pD_faces.begin(),pD_faces.end(), std::inserter(tmp3,tmp3.begin()));
    if(tmp3.size()>1)
      std::cerr << __FUNCTION__
	        << "; there are two surfaces that connect the points."
		<< std::endl;
    if(tmp3.size()>0)
      retval= *tmp3.begin();
    return retval;
  }
