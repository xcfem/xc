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
//NodePtrs.cpp

#include "NodePtrs.h"
#include "domain/mesh/element/Element.h"
#include "utility/matrix/ID.h"
#include "utility/matrix/Matrix.h"
#include "domain/mesh/node/Node.h"
#include "domain/domain/Domain.h"

#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"

#include "xc_utils/src/geom/d2/Plane.h"
#include "xc_utils/src/geom/d3/HalfSpace3d.h"


//! @brief Constructor.
XC::NodePtrs::NodePtrs(Element *owr,const size_t &sz)
  : EntCmd(owr), vector_ptr_nodes(sz,nullptr) {}

//! @brief Copy constructor.
XC::NodePtrs::NodePtrs(const NodePtrs &other)
  : EntCmd(other), vector_ptr_nodes(other.size(),nullptr) 
  {
    const size_t sz= size();
    if(sz>0)
      {
        const ContinuaReprComponent *owr= dynamic_cast<const ContinuaReprComponent *>(Owner());
        assert(owr);
        Domain *dom= owr->getDomain();
        if(dom)
          setPtrs(dom,ID(other.getTags()));
      }
  }

XC::NodePtrs &XC::NodePtrs::operator=(const NodePtrs &other)
  {
    inic();
    EntCmd::operator=(other);
    resize(other.size(),nullptr);
    const size_t sz= size();
    if(sz>0)
      {
        const ContinuaReprComponent *owr= dynamic_cast<const ContinuaReprComponent *>(Owner());
        assert(owr);
        Domain *dom= owr->getDomain();
        if(dom)
          setPtrs(dom,ID(other.getTags()));
      }
    return *this;
  }

//! @brief Destructor.
XC::NodePtrs::~NodePtrs(void)
  {
    inic();
    clear();
  }

void XC::NodePtrs::disconnect(void)
  {
    ContinuaReprComponent *owr= dynamic_cast<ContinuaReprComponent *>(Owner());
    assert(owr);
    for(iterator i= begin();i!= end();i++)
      {
        Node *tmp= *i;
        if(tmp)
          tmp->disconnect(owr);
      }
  }

//! @brief Pone a cero los pointers to node.
void XC::NodePtrs::inic(void)
  {
    disconnect();
    for(iterator i= begin();i!=end();i++)
      (*i)= nullptr;
  }

//! @brief Returns true ifalguno de los pointers
//! es nulo.
bool XC::NodePtrs::hasNull(void) const
  {
    bool retval= false;
    const size_t sz= size();
    for(size_t i=0; i<sz; i++)
      {
        if(!(*this)[i])
          {
            retval= true;
            break;
          }
      }
    return retval;
  }

//! @brief Sets the node pointers from the node identifiers.
void XC::NodePtrs::setPtrs(Domain *theDomain, const ID &theNodeTags)
  {
    inic();
    const size_t sz= theNodeTags.Size();
    resize(sz,nullptr);
    ContinuaReprComponent *owr= dynamic_cast<ContinuaReprComponent *>(Owner());
    assert(owr);
    for(size_t i=0; i<sz; i++)
      {
        (*this)[i]= theDomain->getNode(theNodeTags(i));
        if((*this)[i])
          (*this)[i]->connect(owr);
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; node with tag " << theNodeTags(i)
		      << " does not exist in the domain.\n";
          }
      }
  }

//! @brief Returns an iterator to the node identified by the
//! tag being passed as parameter.
XC::NodePtrs::iterator XC::NodePtrs::find(const int &tag)
  {
    iterator retval= end();
    for(iterator i= begin();i!=end();i++)
      if((*i)->getTag() == tag)
        retval= i;
    return retval;
  }

//! @brief Returns an iterator to the node identified by the
//! tag being passed as parameter.
XC::NodePtrs::const_iterator XC::NodePtrs::find(const int &tag) const
  {
    const_iterator retval= end();
    for(const_iterator i= begin();i!=end();i++)
      if((*i)->getTag() == tag)
        retval= i;
    return retval;
  }

//! @brief Returns the index of the node whose pointer is being passed as parameter.
int XC::NodePtrs::find(const Node *nPtr) const
  {
    int retval= -1, conta=0;
    for(const_iterator i= begin();i!=end();i++,conta++)
      if((*i) == nPtr)
        {
          retval= conta;
          break;
        }
    return retval;
  }

//! @brief Returns a pointer to the node at the position
//! being passed as parameter.
XC::Node *XC::NodePtrs::getNodePtr(const size_t &i)
  { return (*this)[i]; }

//! @brief Returns a const reference to the node at the position
//! being passed as parameter.
XC::NodePtrs::const_reference XC::NodePtrs::operator()(const size_t &i) const
  { return (*this)[i]; }

//!@brief Asigna the pointer to node i.
void XC::NodePtrs::set_node(const size_t &i,Node *n)
  {
    ContinuaReprComponent *owr= dynamic_cast<ContinuaReprComponent *>(Owner());
    if((*this)[i])
      {
        if((*this)[i]!=n)
          {
            (*this)[i]->disconnect(owr);
            (*this)[i]= n;
            if(n)
              (*this)[i]->connect(owr);
          }
      }
    else
      {
        (*this)[i]= n;
        if(n)
          (*this)[i]->connect(owr);
      }
  }

//! @brief Returns a vector with the number of DOFs for each node.
XC::ID XC::NodePtrs::getNumDOFs(void) const
  {
    const int numNodes= size();
    ID retval(numNodes);
    for(int i=0; i<numNodes; i++)
      retval(i)= (*this)[i]->getNumberDOF();
    return retval;
  }

//! @brief Return the total number of DOFs.
int XC::NodePtrs::getTotalDOFs(void) const
  {
    const int numNodes= size();
    int retval= 0;
    for(int i=0; i<numNodes; i++)
      retval+= (*this)[i]->getNumberDOF();
    return retval;
  }

//! @brief Returns the maximum value de la coordenada i of the nodes.
double XC::NodePtrs::MaxCooNod(int icoo) const
  {
    assert(!empty());
    const_iterator i= begin();
    double retval= (*i)->getCrds()[icoo];
    i++;
    for(;i!=end();i++)
      retval= std::max(retval,(*i)->getCrds()[icoo]);
    return retval;
  }

//! @brief Returns the minimum value de la coordenada i of the nodes.
double XC::NodePtrs::MinCooNod(int icoo) const
  {
    assert(!empty());
    const_iterator i= begin();
    double retval= (*i)->getCrds()[icoo];
    i++;
    for(;i!=end();i++)
      retval= std::min(retval,(*i)->getCrds()[icoo]);
    return retval;
  }

//! @brief Returns a vector con los tags of the nodes.
const std::vector<int> &XC::NodePtrs::getTags(void) const
  {
    static std::vector<int> retval;
    const size_t sz= size();
    retval.resize(sz);
    for(size_t i=0; i<sz; i++)
      if((*this)[i]) retval[i]= (*this)[i]->getTag();
    return retval;    
  }

std::vector<int> XC::NodePtrs::getIdx(void) const
  {
    const size_t sz= size();
    std::vector<int> retval(sz,-1);
    for(size_t i=0; i<sz; i++)
      if((*this)[i])
        retval[i]= (*this)[i]->getIdx();
    return retval;     
  }

//! @brief Checks that the dimensions of the node coordinates
//! are all equal to the number being passed as parameter.
bool XC::NodePtrs::checkDimension(const size_t &dim) const
  {
    bool retval= true;
    const size_t sz= size();
    for(size_t i=0; i<sz; i++)
      {
        if((*this)[i])
	  {
            if(dim!= (*this)[i]->getDim())
	      {
	        std::cerr << getClassName() << "::" << __FUNCTION__
	                  << " wrong dimension of node coordinates in node: "
	                  << i << " was: " << (*this)[i]->getDim()
		          << ", must be: " << dim
	                  << std::endl;
	        retval= false;
	        break;
              }
	  }
	else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; pointer to node 0 is null."
	            << std::endl;
      }
    return retval;
  }

//! @brief Returns the dimension of the node coordinates.
size_t XC::NodePtrs::getDimension(void) const
  {
    size_t retval= 0;
    if(!empty())
      {
        if((*this)[0])
	  { retval= (*this)[0]->getDim(); }
        else
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; pointer to node 0 is null."
	              << std::endl;
	  }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; node pointers container is empty."
                << std::endl;
    return retval;
  }

//! @brief Returns a matrix with the coordinates of the nodes by rows.
const XC::Matrix &XC::NodePtrs::getCoordinates(void) const
  {
    static Matrix retval;
    const size_t sz= size();
    const size_t dim= getDimension();
    retval= Matrix(sz,dim);
    for(size_t i=0; i<sz; i++)
      {
        if((*this)[i])
          {
            const Vector &coo= (*this)[i]->getCrds();
	    for(size_t j= 0;j<dim;j++)
              retval(i,j)= coo[j];
          }
      }
    return retval;
  }

//! @brief Return the position of the i-th node.
Pos3d XC::NodePtrs::getPosNode(const size_t &i,bool initialGeometry) const
  {
    if(initialGeometry)
      return (*this)[i]->getInitialPosition3d();
    else
      return (*this)[i]->getCurrentPosition3d();
  }

//! @brief Returns a list with the node positions.
std::list<Pos3d> XC::NodePtrs::getPositions(bool initialGeometry) const
  {
    std::list<Pos3d> retval;
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      retval.push_back(getPosNode(i,initialGeometry));
    return retval;
  }

//! @brief Returns the centroid of the node positions.
Pos3d XC::NodePtrs::getCenterOfMassPosition(bool initialGeometry) const
  {
    Pos3d retval;
    const size_t sz= size();
    if(sz>0)
      {
        Vector3d tmp= getPosNode(0,initialGeometry).VectorPos();;
        for(size_t i=1;i<sz;i++)
          tmp+= getPosNode(i,initialGeometry).VectorPos();
        tmp= tmp * 1.0/sz;
        retval= Pos3d()+tmp;
      }
    return retval;
  }

//! @brief Returns true if all the nodes are inside the object.
bool XC::NodePtrs::In(const GeomObj3d &obj,const double &factor,const double &tol) const
  {
    bool retval= true;
    for(const_iterator i= begin();i!= end();i++)
      {
        const Node *tmp= *i;
        if(tmp)
          if(!tmp->In(obj,factor,tol))
            { retval= false; break; }
      }
    return retval;
  }

//! @brief Returns true if the node are outside the object.
bool XC::NodePtrs::Out(const GeomObj3d &obj,const double &factor,const double &tol) const
  { return !In(obj,factor,tol); }

//! @brief Returns true if all the nodes are inside the object.
bool XC::NodePtrs::In(const GeomObj2d &obj,const double &factor,const double &tol) const
  {
    bool retval= true;
    for(const_iterator i= begin();i!= end();i++)
      {
        const Node *tmp= *i;
        if(tmp)
          if(!tmp->In(obj,factor,tol))
            { retval= false; break; }
      }
    return retval;
  }

//! @brief Returns true if the node are outside the object.
bool XC::NodePtrs::Out(const GeomObj2d &obj,const double &factor,const double &tol) const
  { return !In(obj,factor,tol); }

//! @brief Returns true if there are nodes in both sides of the plane.
bool XC::NodePtrs::Corta(const Plane &plane,bool initialGeometry) const
  {
    double factor= 1.0;
    if(initialGeometry)
      factor= 0.0;
    HalfSpace3d halfSpace(plane);
    bool in= In(halfSpace,factor,0.0);
    HalfSpace3d complementario(halfSpace);
    complementario.Swap();
    bool out= In(complementario,factor,0.0);
    return (!in && !out);
  }

//! @brief Returns the node closest to the point being passed as parameter.
XC::Node *XC::NodePtrs::getNearestNode(const Pos3d &p,bool initialGeometry)
  {
    Node *retval= nullptr;
    double d= DBL_MAX;
    const size_t sz= size();
    if(!empty() && !hasNull())
      {
        d= (*this)[0]->getDist2(p,initialGeometry);
        retval= (*this)[0];
        double tmp;
        for(size_t i=1;i<sz;i++)
          {
            tmp= (*this)[i]->getDist2(p,initialGeometry);
            if(tmp<d)
              {
                d= tmp;
                retval= (*this)[i];
              }
          }
      }
    return retval;
  }

//! @brief Returns the node closest to the point being passed as parameter.
const XC::Node *XC::NodePtrs::getNearestNode(const Pos3d &p,bool initialGeometry) const
  {
    NodePtrs *this_no_const= const_cast<NodePtrs *>(this);
    return this_no_const->getNearestNode(p,initialGeometry);
  }

//! @brief Return the index of the node pointer in the array.
int XC::NodePtrs::getNodeIndex(const Node *ptrNod) const
  {
    int retval= -1;
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      if((*this)[i]==ptrNod)
        {
          retval= i;
          break;
        }
    return retval;
  }


//! @brief Resets tributary areas (or lengths or volumes) of connected nodes.
void XC::NodePtrs::resetTributaries(void) const
  {
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      (*this)[i]->resetTributary();    
  }

//! @brief Adds to the tributary quantity of each node i
//! the i component of the vector being passed as parameter.
void XC::NodePtrs::dumpTributaries(const std::vector<double> &t) const
  {
    const size_t sz= size();
    assert(sz== t.size());
    for(size_t i=0;i<sz;i++)
      (*this)[i]->addTributary(t[i]);
  }

//! @brief Returns a vector that contains the distribution factors
//! of each node for the mode being passed as parameter.
XC::Vector XC::NodePtrs::getDistributionFactor(int mode) const
  {
    const int nrows= getTotalDOFs();
    Vector retval(nrows);
    // Distribution factor computation
    const int numNodes= size();
    int loc = 0;
    for(int i=0; i<numNodes; i++)
      {
        const Vector df= (*this)[i]->getDistributionFactor(mode);
        for(int j=0; j<df.Size(); j++)
          { retval(loc++)= df[j]; }
      }
    return retval;
  }

//! @brief Returns the vectors correspondientes a each node disposed in rows.
XC::Matrix XC::NodePtrs::getNodeVectors(const Vector &v) const
  {
    const int numNodes= size();
    const ID dofs= getNumDOFs();
    const int ndof_max= dofs.max();
    Matrix retval(numNodes,ndof_max);
    int loc = 0;
    for(int i=0; i<numNodes; i++)
      {
        const int nc= (*this)[i]->getNumberDOF();
        for(int j=0; j<nc; j++)
          { retval(i,j)= v[loc++]; }
      }
    return retval;
    
  }
