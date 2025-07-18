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
//NodePtrArray3d.cc

#include "NodePtrArray3d.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/plane/PlaneElement.h"
#include "utility/functions/algebra/num_integration.h"

#include "domain/domain/Domain.h"
#include "domain/constraints/SFreedom_Constraint.h"


#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/d2/Polygon3d.h"

#include "boost/lexical_cast.hpp"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Default constructor.
XC::NodePtrArray3d::NodePtrArray3d(const size_t n_layers)
  : PtrArray3dBase<NodePtrArray>(n_layers) {}
//! @brief Constructor.
XC::NodePtrArray3d::NodePtrArray3d(const size_t n_layers,const size_t n_rows,const size_t cols)
  : PtrArray3dBase<NodePtrArray>(n_layers)
  {
    for(size_t i=0;i<n_layers;i++)
      (*this)[i]= NodePtrArray(n_rows,cols);
  }

//! @brief Returns (if it exists) a pointer to the node
//! which tag is being passed as parameter.
XC::Node *XC::NodePtrArray3d::findNode(const int &tag)
  {
    Node *retval= nullptr;
    const size_t numberOfLayers= getNumberOfLayers();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        NodePtrArray &layer= operator()(i);
        retval= layer.findNode(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Returns the node closest to the point being passed as parameter.
const XC::Node *XC::NodePtrArray3d::getNearestNode(const Pos3d &p) const
  {
    NodePtrArray3d *this_no_const= const_cast<NodePtrArray3d *>(this);
    return this_no_const->getNearestNode(p);
  }

//! @brief Returns the node closest to the point being passed as parameter.
XC::Node *XC::NodePtrArray3d::getNearestNode(const Pos3d &p)
  {
    Node *retval= nullptr, *ptrNod= nullptr;
    const size_t numberOfLayers= getNumberOfLayers();
    double d= DBL_MAX;
    double tmp= 0;
    if(numberOfLayers>100)
      std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	        << "; node pointers array has "
                << numberOfLayers << " layers. "
                << "It is better to search by coordinates in the associated set."
                << Color::def << std::endl;
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        NodePtrArray &layer= operator()(i);
        ptrNod= layer.getNearestNode(p);
        tmp= ptrNod->getDist2(p);
        if(tmp<d)
          {
            d= tmp;
            retval= ptrNod;
          }
      }
    return retval;
  }

//! @brief Returns the indexes of the node identified by the pointer
//! being passed as parameter.
XC::ID XC::NodePtrArray3d::getNodeIndices(const Node *n) const
  {
    ID retval(3);
    retval[0]= -1; retval[1]= -1; retval[2]= -1;
    const size_t numberOfLayers= getNumberOfLayers();
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    Node *ptrNod= nullptr;
    for(size_t i=1;i<=numberOfLayers;i++)
      for(size_t j=1;j<=numberOfRows;j++)
        for(size_t k=1;k<=numberOfColumns;k++)
          {
            ptrNod= (*this)(i,j,k);
            if(ptrNod==n)
              {
                retval[0]= i; retval[1]= j; retval[2]= k;
                break;
              }
          }
    if(retval[0]==-1) // not found. 
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; node "
		  << n->getTag() << " in position: "
		  << n->getInitialPosition3d() << " not found. "
		  << Color::def << std::endl;
	std::exit(EXIT_FAILURE);
      }
    return retval;
  }

//! @brief Returns (if it exists) a pointer to the node
//! which tag is being passed as parameter.
const XC::Node *XC::NodePtrArray3d::findNode(const int &tag) const
  {
    const Node *retval= nullptr;
    const size_t numberOfLayers= getNumberOfLayers();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        const NodePtrArray &layer= operator()(i);
        retval= layer.findNode(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Returns the pointers to the nodes of this array.
std::deque<const XC::Node *> XC::NodePtrArray3d::getNodePtrs(void) const
  {
    std::deque<const Node *> retval;
    const size_t numberOfLayers= getNumberOfLayers();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        const NodePtrArray &layer= operator()(i);
	std::deque<const Node *> tmp= layer.getNodePtrs();
	retval.insert(retval.end(), tmp.begin(), tmp.end());
      }
    return retval;
  }

//! @brief Returns a Python list containing the nodes of this array.
boost::python::list XC::NodePtrArray3d::getPyNodeList(void) const
  {
    boost::python::list retval;
    const std::deque<const Node *> tmp= this->getNodePtrs();
    const size_t sz= tmp.size();
    for(size_t j= 0;j<sz;j++)
      {
	const Node *node= tmp[j];
	retval.append(node);
      }
    return retval;
  }

//! @brief Remove the given node from the array.
//! @param ePtr: pointer to the node to remove.
bool XC::NodePtrArray3d::removeNode(Node *ePtr)
  {
    bool retval= false;
    if(ePtr)
      {
	const size_t numberOfLayers= getNumberOfLayers();
	for(size_t i=1;i<=numberOfLayers;i++)
	  {
	    NodePtrArray &layer= operator()(i);
	    retval= layer.removeNode(ePtr);
	    if(retval) break;
	  }
      }
    return retval;
  }

//! @brief Remove the given node from the array.
//! @param tag: identifier of the node to remove.
bool XC::NodePtrArray3d::removeNode(const int &tag)
  {
    Node *ePtr= findNode(tag);
    return this->removeNode(ePtr);
  }

//! @brief Return the elements connected to any of the nodes of this entity.
std::set<const XC::Element *> XC::NodePtrArray3d::getConnectedElements(void) const
  {
    std::set<const Element *> retval;
    const size_t numberOfLayers= getNumberOfLayers();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
	const NodePtrArray &layer= operator()(i);
	std::set<const Element *> tmp= layer.getConnectedElements();
	retval.insert(tmp.begin(), tmp.end());
      }
    return retval;
  }

//! @brief Return the elements connected to any of the nodes of this entity.
std::set<XC::Element *> XC::NodePtrArray3d::getConnectedElements(void)
  {
    std::set<Element *> retval;
    const size_t numberOfLayers= getNumberOfLayers();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
	NodePtrArray &layer= operator()(i);
	std::set<Element *> tmp= layer.getConnectedElements();
	retval.insert(tmp.begin(), tmp.end());
      }
    return retval;
  }


//! @brief Return the elements connected to any of the nodes of this entity.
boost::python::list XC::NodePtrArray3d::getConnectedElementsPy(void)
  {
    boost::python::list retval;
    const std::set<Element *> tmp= this->getConnectedElements();
    for(std::set<Element *>::iterator i= tmp.begin(); i!=tmp.end(); i++)
      {
	Element *element= *i;
	boost::python::object pyObj(boost::ref(*element));
	retval.append(pyObj);
      }
    return retval;
  }

XC::Vector XC::NodePtrArray3d::IRowSimpsonIntegration(const size_t &f,const size_t &c,const ExprAlgebra &e,const size_t &n) const
  {
    const_ref_i_row iRow= getIRowConstRef(f,c);
    const std::deque<double> dq_retval= RowSimpsonIntegration(iRow,"z",2,e,n);
    const size_t szr= dq_retval.size();
    Vector retval(szr);
    for(size_t i= 0;i<szr;i++)
      retval[i]= dq_retval[i];
    return retval;
  }

XC::Vector XC::NodePtrArray3d::JRowSimpsonIntegration(const size_t &layer, const size_t &c,const ExprAlgebra &e,const size_t &n) const
  {
    const_ref_j_row iRow= getJRowConstRef(layer,c);
    const std::deque<double> dq_retval= RowSimpsonIntegration(iRow,"x",0,e,n);
    const size_t szr= dq_retval.size();
    Vector retval(szr);
    for(size_t i= 0;i<szr;i++)
      retval[i]= dq_retval[i];
    return retval;
  }

XC::Vector XC::NodePtrArray3d::KRowSimpsonIntegration(const size_t &layer,const size_t &f,const ExprAlgebra &e,const size_t &n) const
  {
    const_ref_k_row iRow= getKRowConstRef(layer,f);
    const std::deque<double> dq_retval= RowSimpsonIntegration(iRow,"y",1,e,n);
    const size_t szr= dq_retval.size();
    Vector retval(szr);
    for(size_t i= 0;i<szr;i++)
      retval[i]= dq_retval[i];
    return retval;
  }

//! @brief Impone desplazamiento nulo en the nodes de this set.
void XC::NodePtrArray3d::fix(const SFreedom_Constraint &spc) const
  {
    if(Null()) return;
    const size_t numberOfLayers= getNumberOfLayers();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        const NodePtrArray &layer= operator()(i);
        layer.fix(spc);
      }
  }

std::vector<int> XC::NodePtrArray3d::getTags(void) const
  {
    const size_t numberOfLayers= getNumberOfLayers();
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    std::vector<int> retval(numberOfLayers*numberOfRows*numberOfColumns,-1);
    size_t conta= 0;
    for(size_t i=1;i<=numberOfLayers;i++)
      for(size_t j=1;j<=numberOfRows;j++)
        for(size_t k=1;k<=numberOfColumns;k++)
          retval[conta++]= (*this)(i,j,k)->getTag();
    return retval;
  }

void XC::NodePtrArray3d::Print(std::ostream &os) const
  {
    const size_t numberOfLayers= getNumberOfLayers();
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        for(size_t j=1;j<=numberOfRows;j++)
          {
            for(size_t k=1;k<=numberOfColumns;k++)
	      os << (*this)(i,j,k)->getTag() << " ";
	    os << std::endl;
          }
        os << std::endl;
      }
  }

std::ostream &XC::operator<<(std::ostream &os, const NodePtrArray3d &t)
  {
    t.Print(os);
    return os;
  }

void XC::fix(const XC::NodePtrArray3d::box_var_ref &box_ref,const XC::SFreedom_Constraint &spc)
  {
    if(box_ref.Empty()) return;
    const size_t numberOfLayers= box_ref.getNumberOfLayers();
    const size_t numberOfRows= box_ref.getNumberOfRows();
    const size_t numberOfColumns= box_ref.getNumberOfColumns();

    // Obtenemos el domain.
    const Node *n= box_ref(1,1,1);
    Domain *dom= nullptr;
    dom= n->getDomain();
    if(dom)
      {
        for(size_t i=1;i<=numberOfLayers;i++)
          for(size_t j=1;j<=numberOfRows;j++)
            for(size_t k=1;k<=numberOfColumns;k++)
              {
                const Node *nod= box_ref(i,j,k);
                if(nod)
                  {
                    const int tag_nod= nod->getTag();
                    SFreedom_Constraint *sp= spc.getCopy();
                    sp->setNodeTag(tag_nod); 
                    dom->addSFreedom_Constraint(sp);
                  }
              }
      } 
  }

//! @brief Returns the indexes of the nodes (j,k),(j+1,k),(j+1,k+1),(j,k+1). 
std::vector<XC::Node *> getNodePtrs(const XC::NodePtrArray3d::constant_i_layer_const_ref &nodes, const size_t &j, const size_t &k, const std::vector< std::pair<int,int> > &indices)
  {
    const size_t sz= indices.size();
    std::vector<XC::Node *> retval(sz,nullptr);
    const size_t numberOfRows= nodes.getNumberOfRows();
    const size_t numberOfColumns= nodes.getNumberOfColumns();
    if(j>=numberOfRows)
      {
        std::cerr << Color::red << __FUNCTION__ << "; row index j= " << j
		  << " out of range."
	          << Color::def << std::endl;
        return retval;
      }
    if(k>=numberOfColumns)
      {
        std::cerr << Color::red << __FUNCTION__ << "; column index k= " << k
		  << " out of range."
		  << Color::def << std::endl;
        return retval;
      }

    size_t cont= 0;
    for(std::vector< std::pair<int,int>>::const_iterator i= indices.begin();i!=indices.end();i++)
      {
	std::pair<int,int> v= *i;
	const size_t jj= j+v.first;
	const size_t kk= k+v.second;
	XC::Node *ptr= nodes(jj,kk);
        if(ptr)
          retval[cont]= ptr;
	else
          std::cerr << Color::red << __FUNCTION__
		    << "; error obtaining node at ("
		    << j << ',' << k << ")."
		    << Color::def << std::endl;
	cont++;
      }
    return retval;
  }

//! @brief Returns the indexes of the nodes (j,k),(j+1,k),(j+1,k+1),(j,k+1). 
std::vector<int> XC::getNodeIdsQuad4N(const XC::NodePtrArray3d::constant_i_layer_const_ref &nodes,const size_t &j,const size_t &k)
  {
    std::vector<int> retval(4,-1);
    std::vector< std::pair<int,int> > indices(4);
    indices[0]= std::pair<int,int>(0,0);
    indices[1]= std::pair<int,int>(0,1);
    indices[2]= std::pair<int,int>(1,1);
    indices[3]= std::pair<int,int>(1,0);
    const std::vector<XC::Node *> node_pointers= getNodePtrs(nodes,j,k,indices);
    std::list<Pos3d> positions;
    size_t cont= 0; 
    for(std::vector<XC::Node *>::const_iterator i= node_pointers.begin();i!=node_pointers.end();i++)
      {
	const Node *n= *i;
        retval[cont]= n->getTag();
        positions.push_back(n->getInitialPosition3d());
	cont++;
      }	

//     const Vector3d v2= p2-p1;
//     const Vector3d v3= p3-p2;
//     const Vector3d v4= p4-p3;
//     const Vector3d v1= p1-p4;
//     const double d1= dot(v1,v3);
//     const double d2= dot(v2,v4);
//     if((d1>0))
//       {
//         std::swap(p3,p2);
//         std::swap(retval[2],retval[1]);
//       }
//     if((d2>0))
//       {
// 	std::swap(p3,p4);
//         std::swap(retval[2],retval[3]);
//       }

    Polygon3d tmp(positions.begin(),positions.end());
    const double area= tmp.getArea();
    if(area<elem_warning_area)
      {
        std::clog << Color::yellow << __FUNCTION__
		  << "; area for (" << j << ',' << k
                  << ") cell is too small (" << area << ").\n"
		  << " polygon " << tmp
		  << Color::def << std::endl;
      }
    return retval;
  }

//! @brief Returns the indexes of the nodes (j,k),(j+1,k),(j+1,k+1),(j,k+1). 
std::vector<int> XC::getNodeIdsQuad9N(const XC::NodePtrArray3d::constant_i_layer_const_ref &nodes,const size_t &j,const size_t &k)
  {
    std::vector<int> retval(9,-1);
    std::vector< std::pair<int,int> > indices(9);
    indices[0]= std::pair<int,int>(0,0);
    indices[1]= std::pair<int,int>(0,2);
    indices[2]= std::pair<int,int>(2,2);
    indices[3]= std::pair<int,int>(2,0);

    indices[4]= std::pair<int,int>(0,1);
    indices[5]= std::pair<int,int>(1,2);
    indices[6]= std::pair<int,int>(2,1);
    indices[7]= std::pair<int,int>(1,0);
    indices[8]= std::pair<int,int>(1,1);
    const std::vector<XC::Node *> node_pointers= getNodePtrs(nodes,j,k,indices);
    std::list<Pos3d> positions;
    size_t cont= 0; 
    for(std::vector<XC::Node *>::const_iterator i= node_pointers.begin();i!=node_pointers.end();i++)
      {
	const XC::Node *n= *i;
        retval[cont]= n->getTag();
	if(cont<8)
          positions.push_back(n->getInitialPosition3d());
	cont++;
      }	
    Polygon3d tmp(positions.begin(),positions.end());
    const double area= tmp.getArea();
    if(area<1e-3)
      {
        std::cerr << Color::red << __FUNCTION__
		  << "Area for (" << j << ',' << k
                  << ") cell is too small (" << area << ").\n"
		  << " polygon " << tmp << Color::def
		  << std::endl;
      }
    return retval;
  }
