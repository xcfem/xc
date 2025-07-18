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
//EntMdlr.cc

#include "EntMdlr.h"
#include "utility/matrices/RangoIndice.h"
#include "utility/geom/pos_vec/Pos3dArray.h"
#include "utility/geom/pos_vec/Pos3dArray3d.h"
#include "utility/matrices/3d_arrays/Array3dRange.h"
#include "domain/mesh/node/Node.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/IRowSet.h"
#include "preprocessor/set_mgmt/JRowSet.h"
#include "preprocessor/set_mgmt/KRowSet.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "utility/utils/misc_utils/colormod.h"

#include "vtkCellType.h"

//! @brief Constructor.
XC::EntMdlr::EntMdlr(Preprocessor *m,const size_t &i)
  : SetEstruct("",m), idx(i), doGenMesh(true), ttzNodes(), ttzElements() {}

//! @brief Constructor.
//! @param name: Object identifier.
//! @param i: index to be used in VTK arrays.
//! @param m: Pointer to preprocessor.
XC::EntMdlr::EntMdlr(const std::string &name,const size_t &i,Preprocessor *m)
  : SetEstruct(name,m), idx(i), doGenMesh(true), ttzNodes(), ttzElements() {}


//! @brief Copy constructor.
XC::EntMdlr::EntMdlr(const EntMdlr &other)
  : SetEstruct(other), idx(other.idx), doGenMesh(true), ttzNodes(other.ttzNodes), ttzElements(other.ttzElements) {}

//! @brief Assignment operator.
XC::EntMdlr &XC::EntMdlr::operator=(const EntMdlr &other)
  {
    SetEstruct::operator=(other);
    idx= other.idx;
    doGenMesh= other.doGenMesh;
    ttzNodes= other.ttzNodes;
    ttzElements= other.ttzElements;
    return *this;
  }

//! @brief Comparison operator.
bool XC::EntMdlr::operator==(const EntMdlr &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= SetEstruct::operator==(other);
        if(retval)
          retval= (idx==other.idx);
	if(retval)
	  retval= (doGenMesh==other.doGenMesh);
	if(retval)
	  retval= (ttzNodes==other.ttzNodes);
	if(retval)
          retval= (ttzElements==other.ttzElements);
      } 
    return retval;
  }

//! @brief Assigns the objects index for its use in
//! VTK arrays (see numerate in Set).
void XC::EntMdlr::set_index(const size_t &i)
  { idx= i; }

//! @brief Return a description of the object.
//! This mimics the corresponding function in XC::Set and allows
//! the modeler entities behave as sets in the Python interface.
std::string XC::EntMdlr::getDescription(void) const
  {
    std::string retval= getClassName()+": ";
    retval+= getName();
    return retval;
  }

//! @brief Interfaz con VTK.
int XC::EntMdlr::getVtkCellType(void) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; must be overloaded in derived classes."
	      << Color::def << std::endl;
    return VTK_EMPTY_CELL;
  }

//! @brief Returns true if the object lies inside the
//! geometric object.
//!
//! @param geomObj: object to be contained in.
//! @param tol: tolerance.
bool XC::EntMdlr::In(const GeomObj3d &geomObj, const double &tol) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet."
	      << Color::def << std::endl;
    return false;
  }

//! @brief Returns true if the object lies outside the
//! geometric object.
//!
//! @param geomObj: object to be contained in.
//! @param tol: tolerance.
bool XC::EntMdlr::Out(const GeomObj3d &geomObj, const double &tol) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; not implemented yet."
	      << Color::def << std::endl;
    return false;
  }

//! @brief Clears object contents.
void XC::EntMdlr::clearAll(void)
  {
    SetEstruct::clearPyProps();
    BorraPtrNodElem();
  }

//! @brief Clears pointer to nodes and elements.
void XC::EntMdlr::BorraPtrNodElem(void)
  {
    ttzNodes.clearAll();
    ttzElements.clearAll();
  }

//! @brief Returns a pointer to the node which indexes are
//! being passed as parameters.
//!
//! @param i: index of the layer.
//! @param j: index of the row.
//! @param k: index of the column.
XC::Node *XC::EntMdlr::getNode(const size_t &i,const size_t &j,const size_t &k)
  {
    Node *retval= nullptr;
    if(!ttzNodes.Null())
      retval= ttzNodes(i,j,k);
    return retval;
  }

//! @brief Returns a pointer to the node which indexes are
//! being passed as parameters.
//!
//! @param i: index of the layer.
//! @param j: index of the row.
//! @param k: index of the column.
const XC::Node *XC::EntMdlr::getNode(const size_t &i,const size_t &j,const size_t &k) const
  {
    EntMdlr *this_no_const= const_cast<EntMdlr *>(this);
    const Node *retval= this_no_const->getNode(i,j,k);
    return retval;
  }

//! @brief Returns the pointers to the nodes of this array.
std::deque<const XC::Node *> XC::EntMdlr::getNodePtrs(void) const
  { return ttzNodes.getNodePtrs(); }

//! @brief Return the node closest to the point being passed as parameter.
XC::Node *XC::EntMdlr::getNearestNode(const Pos3d &p)
  { return ttzNodes.getNearestNode(p); }

//! @brief Return the node closest to the point being passed as parameter.
const XC::Node *XC::EntMdlr::getNearestNode(const Pos3d &p) const
  {
    EntMdlr *this_no_const= const_cast<EntMdlr *>(this);
    return this_no_const->getNearestNode(p);
  }

//! @brief Return the indexes of the given node.
XC::ID XC::EntMdlr::getNodeIndices(const Node *n) const
  { return ttzNodes.getNodeIndices(n); }

//! @brief Return the indexes of the nearest node to the given position.
XC::ID XC::EntMdlr::getIndicesOfNearestNode(const Pos3d &p) const
  {
    const Node *n= this->getNearestNode(p);
    return this->getNodeIndices(n);
  }

//! @brief Returns a pointer to the element which indexes
//! are being passed as parameters.
//!
//! @param i: index of the layer.
//! @param j: index of the row.
//! @param k: index of the column.
XC::Element *XC::EntMdlr::getElement(const size_t &i,const size_t &j,const size_t &k)
  {
    if(!ttzElements.Null())
      return ttzElements(i,j,k);
    else
      return nullptr;
  }

//! @brief Returns a pointer to the element which indices are being passed as parameters.
//!
//! @param i: index of the layer.
//! @param j: index of the row.
//! @param k: index of the column.
const XC::Element *XC::EntMdlr::getElement(const size_t &i,const size_t &j,const size_t &k) const
  {
    if(!ttzElements.Null())
      return ttzElements(i,j,k);
    else
      return nullptr;
  }

//! @brief Return the element closest to the point being passed as parameter.
XC::Element *XC::EntMdlr::getNearestElement(const Pos3d &p)
  { return ttzElements.getNearestElement(p); }

//! @brief Return the element closest to the point being passed as parameter.
const XC::Element *XC::EntMdlr::getNearestElement(const Pos3d &p) const
  {
    EntMdlr *this_no_const= const_cast<EntMdlr *>(this);
    return this_no_const->getNearestElement(p);
  }


//! @brief Return the elements of this entity connected to the given node.
std::set<const XC::Element *> XC::EntMdlr::getConnectedElements(const Node *n) const
  { return ttzElements.getConnectedElements(n); }

//! @brief Return the elements of this entity connected to the given node.
std::set<XC::Element *> XC::EntMdlr::getConnectedElements(const Node *n)
  { return ttzElements.getConnectedElements(n); }

//! @brief Return the elements of this entity connected to the given node.
boost::python::list XC::EntMdlr::getConnectedElementsPy(const Node *n)
  { return ttzElements.getConnectedElementsPy(n); }

//! @brief Return the elements connected to any of the nodes of this entity.
std::set<const XC::Element *> XC::EntMdlr::getConnectedElements(void) const
  { return ttzNodes.getConnectedElements(); }

//! @brief Return the elements connected to any of the nodes of this entity.
std::set<XC::Element *> XC::EntMdlr::getConnectedElements(void)
  { return ttzNodes.getConnectedElements(); }

//! @brief Return the elements connected to any of the nodes of this entity.
boost::python::list XC::EntMdlr::getConnectedElementsPy(void)
  { return ttzNodes.getConnectedElementsPy(); }

//! @brief Returns a pointer to the node cuyo identifier is being passed as parameter.
XC::Node *XC::EntMdlr::findNode(const int &tag)
  { return ttzNodes.findNode(tag); }

//! @brief Returns a pointer to the node cuyo identifier is being passed as parameter.
const XC::Node *XC::EntMdlr::findNode(const int &tag) const
  { return ttzNodes.findNode(tag); }

//! @brief Returns a pointer to the element
//! identified by the tag being passed as parameter.
XC::Element *XC::EntMdlr::findElement(const int &tag)
  { return ttzElements.findElement(tag); }

//! @brief Returns a pointer to the element
//! identified by the tag being passed as parameter.
const XC::Element *XC::EntMdlr::findElement(const int &tag) const
  { return ttzElements.findElement(tag); }

//! @brief Remove the given element from the array.
bool XC::EntMdlr::remove(Element *ePtr)
  { return ttzElements.removeElement(ePtr); }

//! @brief Remove the element identifie by the given tag from the array.
bool XC::EntMdlr::removeElement(const int &tag)
  { return ttzElements.removeElement(tag); }

//! @brief Remove the given node from the array.
bool XC::EntMdlr::remove(Node *nPtr)
  { return ttzNodes.removeNode(nPtr); }

//! @brief Remove the node identifie by the given tag from the array.
bool XC::EntMdlr::removeNode(const int &tag)
  { return ttzNodes.removeNode(tag); }

//! @brief Creates a set that corresponds to a row of nodes and elements.
XC::SetEstruct *XC::EntMdlr::create_row_set(const Array3dRange &rango,const std::string &nmb)
  {
    SetEstruct *retval= nullptr;
    if(getPreprocessor())
      {
        MapSet &map_set= getPreprocessor()->get_sets();
        if(rango.isIRow())
          {
            retval= map_set.create_set_estruct(getVarRefIRow(rango,nmb));
          }
        else if(rango.isJRow())
          {
            XC::JRowSet fj= getVarRefJRow(rango,nmb);
            retval= map_set.create_set_estruct(fj);
          }
        else if(rango.isKRow())
          {
            retval= map_set.create_set_estruct(getVarRefKRow(rango,nmb));
          }
        else
	  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	            << "; can't create row set."
		    << Color::def << std::endl;
      }
    else
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; preprocessor undefined."
		  << Color::def << std::endl;
        return nullptr;
      }
    return retval;
  }

//! @brief Return Simpson's weights.
XC::Vector XC::EntMdlr::getSimpsonWeights(const std::string &ijk,const std::string &strExpr,const size_t &f,const size_t &c, const size_t &n) const
  {
    Vector retval;
    const ExprAlgebra e(strExpr);
    if(ijk=="i")
      retval= ttzNodes.IRowSimpsonIntegration(f,c,e,n);
    else if(ijk=="j")
      retval= ttzNodes.JRowSimpsonIntegration(f,c,e,n);
    else if(ijk=="k")
      retval= ttzNodes.KRowSimpsonIntegration(f,c,e,n);
    return retval;
  }

//! @brief Creates a node at the position being passed as parameter.
XC::Node *XC::EntMdlr::create_node(const Pos3d &pos,size_t i,size_t j, size_t k)
  {
    Node *retval= nullptr;
    NodeHandler &nh= getPreprocessor()->getNodeHandler();
    const size_t dim= nh.getSpaceDim();
    if(dim==3)
      retval= nh.newNode(pos);
    else if(dim==2)
      retval= nh.newNode(pos.x(), pos.y());
    else if(dim==1)
      retval= nh.newNode(pos.x());
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; space dimension: " << dim
	        << " not implemented."
		<< Color::def << std::endl;
    ttzNodes(i,j,k)= retval;
    return retval;
  }
  
//! @brief Creates nodes at the positions being passed as parameters.
void XC::EntMdlr::create_nodes(const Pos3dArray3d &positions)
  {
    const size_t n_layers= positions.getNumberOfLayers();
    if(n_layers>0)
      {
	if(ttzNodes.Null())
	  {
	    const size_t n_rows= positions(1).getNumberOfRows();
	    const size_t n_cols= positions(1).getNumberOfColumns();
	    ttzNodes = NodePtrArray3d(n_layers,n_rows,n_cols);

	    if(getPreprocessor())
	      {
		for( size_t i= 1;i<=n_layers;i++)
		  for( size_t j= 1;j<=n_rows;j++)
		    for( size_t k= 1;k<=n_cols;k++)
		      create_node(positions(i,j,k),i,j,k);
		if(verbosity>5)
		  std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			    << "; created " << ttzNodes.NumPtrs() << " node(s)."
			    << Color::def << std::endl;
	      }
	  }
	else
	  if(verbosity>2)
	    std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
		      << "; nodes from entity: '" << getName()
		      << "' already exist."
		      << Color::def << std::endl;
      }
  }

//! @brief Set the (i,j,k) node. Issue a warning if there is already
//! a node there.
const XC::Node *XC::EntMdlr::set_node(size_t i,size_t j, size_t k, Node *nodePtr)
  {
    const Node *oldNode= nullptr;
    if(!ttzNodes.empty())
      {
        oldNode= this->getNode(i,j,k);
	if(oldNode)
	  {
	    std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
	              << "; node at position: ("
		      << i << ", " << j << ", " << k 
		      << ") already exist."
		      << Color::def << std::endl;
	  }
        ttzNodes(i,j,k)= nodePtr;
      }
    return oldNode;
  }

//! @brief Creates elements on the nodes created
//! in create_nodes.
bool XC::EntMdlr::create_elements(meshing_dir dm)
  {
    bool retval= false;
    if(!ttzNodes.empty())
      {
        if(ttzNodes.HasNull())
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	            << "; there are null pointers."
                    << " Elements were not created."
		    << Color::def << std::endl;
        else
          if(ttzElements.Null())
            {
              if(getPreprocessor())
                {
                  if(verbosity>4)
                    std::clog << "Creating elements of entity: '"
			      << getName() << "'...";   
                  const Element *seed= getPreprocessor()->getElementHandler().get_seed_element();
                  if(seed)
                    {
                      ttzElements= seed->put_on_mesh(ttzNodes,dm);
                      add_elements_to_handler(ttzElements);
                      retval= true;
                    }
                  else if(verbosity>0)
                    std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
		              << "; seed element not set."
			      << Color::def << std::endl;
                  if(verbosity>4)
                    std::clog << "created." << std::endl;
                }
              else
                std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		          << "; preprocessor undefined."
			  << Color::def << std::endl;
            }
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; there is no nodes for the elements."
		<< Color::def << std::endl;
    const size_t numElements= ttzElements.NumPtrs();
    if(numElements==0 && verbosity>0)
      {
	const int entDimension= this->GetDimension();
	int elementDimension= -1;
        const Element *seed= getPreprocessor()->getElementHandler().get_seed_element();
	if(seed)
	  elementDimension= seed->getDimension();
	if(elementDimension<=entDimension)
	  std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
		    << "; warning 0 elements created for entity: " << getName()
		    << Color::def << std::endl;
      }
    return retval;
  }

//! @brief Returns true if the points begins or ends in the line.
void XC::EntMdlr::setGenMesh(bool m)
  { doGenMesh= m; }

//! @brief Returns true if the points begins or ends in the line.
const bool &XC::EntMdlr::getGenMesh(void) const
  { return doGenMesh; }

//! @brief Creates a point at the position being passed as parameter.
XC::Pnt *XC::EntMdlr::create_point(const Pos3d &pos)
  { return getPreprocessor()->getMultiBlockTopology().getPoints().New(pos); }

//! @brief Creates a point if there is no other existing point at
//! a distance less that tol.
XC::Pnt *XC::EntMdlr::create_point_if_needed(const Pos3d &pos, const double &tol)
  {
    Pnt *retval= nullptr;
    MultiBlockTopology &topology= getPreprocessor()->getMultiBlockTopology();
    PntMap &points= topology.getPoints();
    if(!points.empty()) //There is at least one point.
       {
         retval= points.getNearest(pos);
         const double d2= dist2(pos,retval->getPos());
         if(d2>tol*tol) // Create new point.
           { retval= create_point(pos); }
       }
     else
       { retval= create_point(pos); }
    return retval;
  }

//! @brief Creates points at the positions being passed as parameters.
void XC::EntMdlr::create_points(const Pos3dArray &positions)
  {
    if(verbosity>4)
      std::clog << "Creating points for line: '" << getName() << "'...";   
    const size_t n_rows= positions.getNumberOfRows();
    const size_t cols= positions.getNumberOfColumns();

    if(getPreprocessor())
      {
        size_t cont= 0;
        for(size_t i= 1;i<=n_rows;i++)
          for(size_t j= 1;j<=cols;j++)
            {
              create_point(positions(i,j));
              cont++;
            }
      }
    else
      std::cerr << Color::red << getClassName() << __FUNCTION__
	        << "; preprocessor needed."
		<< Color::def << std::endl;
    if(verbosity>4)
      std::clog << "creados." << std::endl;
  }

XC::IRowSet XC::EntMdlr::getVarRefIRow(size_t f,size_t c,const std::string &nmb)
  { return IRowSet(*this,f,c,nmb,getPreprocessor()); }
XC::IRowSet XC::EntMdlr::getVarRefIRow(const RangoIndice &layer_range,size_t f,size_t c,const std::string &nmb)
  { return IRowSet(*this,layer_range,f,c,nmb,getPreprocessor()); }
XC::IRowSet XC::EntMdlr::getVarRefIRow(const Array3dRange &rango,const std::string &nmb)
  { return getVarRefIRow(rango.getLayerRange(),rango.getRowRange().Inf(),rango.getColumnRange().Inf(),nmb); }

XC::JRowSet XC::EntMdlr::getVarRefJRow(size_t layer,size_t c,const std::string &nmb)
  { return JRowSet(*this,layer,c,nmb,getPreprocessor()); }
XC::JRowSet XC::EntMdlr::getVarRefJRow(size_t layer,const RangoIndice &row_range,size_t c,const std::string &nmb)
  { return JRowSet(*this,layer,row_range,c,nmb,getPreprocessor()); }
XC::JRowSet XC::EntMdlr::getVarRefJRow(const Array3dRange &rango,const std::string &nmb)
  { return getVarRefJRow(rango.getLayerRange().Inf(),rango.getRowRange(),rango.getColumnRange().Inf(),nmb); }

XC::KRowSet XC::EntMdlr::getVarRefKRow(size_t layer,size_t f,const std::string &nmb)
  { return KRowSet(*this,layer,f,nmb,getPreprocessor()); }
XC::KRowSet XC::EntMdlr::getVarRefKRow(size_t layer,size_t f,const RangoIndice &column_range,const std::string &nmb)
  { return KRowSet(*this,layer,f,column_range,nmb,getPreprocessor()); }
XC::KRowSet XC::EntMdlr::getVarRefKRow(const Array3dRange &rango,const std::string &nmb)
  { return getVarRefKRow(rango.getLayerRange().Inf(),rango.getRowRange().Inf(),rango.getColumnRange(),nmb); }

//! @brief Return the squared distance to
//! the position being passed as parameter.
double XC::EntMdlr::getSquaredDistanceTo(const Pos3d &pt) const
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented."
	      << Color::def << std::endl;
    return 0.0;
  }

//! @brief Return the distance to the position being passed as parameter.
double XC::EntMdlr::getDistanceTo(const Pos3d &pt) const
  { return sqrt(getSquaredDistanceTo(pt)); }

//! @brief Fixes the nodes of the set.
void XC::EntMdlr::fix(const SFreedom_Constraint &spc)
  { ttzNodes.fix(spc); }


//! @brief Return the centroid of the object.
Pos3d XC::EntMdlr::getCentroid(void) const
  {
    Pos3d retval;
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented."
	      << Color::def << std::endl;
    return retval;
  }
//! @brief Destructor.
XC::EntMdlr::~EntMdlr(void)
  { clearAll(); }


//! @brief Return a Python dictionary containing the object members values.
boost::python::dict XC::EntMdlr::getPyDict(void) const
  {
    boost::python::dict retval= SetEstruct::getPyDict();
    // Retrieve properties.
    retval["doGenMesh"]= doGenMesh;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::EntMdlr::setPyDict(const boost::python::dict &d)
  {
    SetEstruct::setPyDict(d);
    doGenMesh= boost::python::extract<bool>(d["doGenMesh"]);
  }
