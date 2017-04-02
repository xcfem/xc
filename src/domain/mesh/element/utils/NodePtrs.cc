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

#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/geom/d3/SemiEspacio3d.h"


//! @brief Constructor.
XC::NodePtrs::NodePtrs(Element *owr,const size_t &sz)
  : EntCmd(owr), theNodes(sz,nullptr) {}

//! @brief Constructor de copia.
XC::NodePtrs::NodePtrs(const NodePtrs &otro)
  : EntCmd(otro), theNodes(otro.theNodes.size(),nullptr) 
  {
    const size_t sz= theNodes.size();
    if(sz>0)
      {
        const ContinuaReprComponent *owr= dynamic_cast<const ContinuaReprComponent *>(Owner());
        assert(owr);
        Domain *dom= owr->getDomain();
        if(dom)
          setPtrs(dom,ID(otro.getTags()));
      }
  }

XC::NodePtrs &XC::NodePtrs::operator=(const NodePtrs &otro)
  {
    inic();
    EntCmd::operator=(otro);
    theNodes= vector_ptr_nodes(otro.theNodes.size(),nullptr);
    const size_t sz= theNodes.size();
    if(sz>0)
      {
        const ContinuaReprComponent *owr= dynamic_cast<const ContinuaReprComponent *>(Owner());
        assert(owr);
        Domain *dom= owr->getDomain();
        if(dom)
          setPtrs(dom,ID(otro.getTags()));
      }
    return *this;
  }

//! @brief Destructor.
XC::NodePtrs::~NodePtrs(void)
  {
    inic();
    theNodes.clear();
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
    const size_t sz= theNodes.size();
    for(size_t i=0; i<sz; i++)
      {
        if(!theNodes[i])
          {
            retval= true;
            break;
          }
      }
    return retval;
  }

//! @brief Asigna los pointers to partir de los identificadores de elemento.
void XC::NodePtrs::setPtrs(Domain *theDomain, const ID &theNodeTags)
  {
    inic();
    const size_t sz= theNodeTags.Size();
    theNodes.resize(sz,nullptr);
    ContinuaReprComponent *owr= dynamic_cast<ContinuaReprComponent *>(Owner());
    assert(owr);
    for(size_t i=0; i<sz; i++)
      {
        theNodes[i]= theDomain->getNode(theNodeTags(i));
        if(theNodes[i])
          theNodes[i]->connect(owr);
        else
          {
            std::cerr << "WARNING - NodePtrs::setDomain - node with tag ";
	    std::cerr << theNodeTags(i) << " does not exist in the domain\n";
          }
      }
  }

//! @brief Returns an iterator al elemento cuyo tag is being passed as parameter.
XC::NodePtrs::iterator XC::NodePtrs::find(const int &tag)
  {
    iterator retval= end();
    for(iterator i= begin();i!=end();i++)
      if((*i)->getTag() == tag)
        retval= i;
    return retval;
  }

//! @brief Returns an iterator al elemento cuyo tag is being passed as parameter.
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
  { return theNodes[i]; }

//! @brief Returns a const reference to the node at the position
//! being passed as parameter.
XC::NodePtrs::const_reference XC::NodePtrs::operator()(const size_t &i) const
  { return theNodes[i]; }

//! @brief Returns a const reference to the node at the position
//! being passed as parameter.
XC::NodePtrs::const_reference XC::NodePtrs::operator[](const size_t &i) const
  { return theNodes[i]; }

//!@brief Asigna the pointer to node i.
void XC::NodePtrs::set_node(const size_t &i,Node *n)
  {
    ContinuaReprComponent *owr= dynamic_cast<ContinuaReprComponent *>(Owner());
    if(theNodes[i])
      {
        if(theNodes[i]!=n)
          {
            theNodes[i]->disconnect(owr);
            theNodes[i]= n;
            if(n)
              theNodes[i]->connect(owr);
          }
      }
    else
      {
        theNodes[i]= n;
        if(n)
          theNodes[i]->connect(owr);
      }
  }

//! @brief Returns a vector with the number of DOFs for each node.
XC::ID XC::NodePtrs::getNumDOFs(void) const
  {
    const int numNodes= size();
    ID retval(numNodes);
    for(int i=0; i<numNodes; i++)
      retval(i)= theNodes[i]->getNumberDOF();
    return retval;
  }

//! @brief Return the total number of DOFs.
int XC::NodePtrs::getTotalDOFs(void) const
  {
    const int numNodes= size();
    int retval= 0;
    for(int i=0; i<numNodes; i++)
      retval+= theNodes[i]->getNumberDOF();
    return retval;
  }

//! @brief Returns the valor máximo de la coordenada i de los nodos.
double XC::NodePtrs::MaxCooNod(int icoo) const
  {
    assert(!theNodes.empty());
    const_iterator i= begin();
    double retval= (*i)->getCrds()[icoo];
    i++;
    for(;i!=end();i++)
      retval= std::max(retval,(*i)->getCrds()[icoo]);
    return retval;
  }

//! @brief Returns the valor mínimo de la coordenada i de los nodos.
double XC::NodePtrs::MinCooNod(int icoo) const
  {
    assert(!theNodes.empty());
    const_iterator i= begin();
    double retval= (*i)->getCrds()[icoo];
    i++;
    for(;i!=end();i++)
      retval= std::min(retval,(*i)->getCrds()[icoo]);
    return retval;
  }

//! @brief Returns a vector con los tags de los nodos.
const std::vector<int> &XC::NodePtrs::getTags(void) const
  {
    static std::vector<int> retval;
    const size_t sz= theNodes.size();
    retval.resize(sz);
    for(size_t i=0; i<sz; i++)
      if(theNodes[i]) retval[i]= theNodes[i]->getTag();
    return retval;    
  }

std::vector<int> XC::NodePtrs::getIdx(void) const
  {
    const size_t sz= theNodes.size();
    std::vector<int> retval(sz,-1);
    for(size_t i=0; i<sz; i++)
      if(theNodes[i])
        retval[i]= theNodes[i]->getIdx();
    return retval;     
  }

//! @brief Returns a matriz con las coordenadas de los nodos.
const XC::Matrix &XC::NodePtrs::getCoordinates(void) const
  {
    static Matrix retval;
    const size_t sz= theNodes.size();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        if(theNodes[i])
          {
            const Vector &coo= theNodes[i]->getCrds();
            retval(i,0)= coo[0];
            retval(i,1)= coo[1];
            retval(i,2)= coo[2];
          }
      }
    return retval;
  }

//! @brief Return the position of the i-th node.
Pos3d XC::NodePtrs::getPosNodo(const size_t &i,bool initialGeometry) const
  {
    if(initialGeometry)
      return theNodes[i]->getPosInicial3d();
    else
      return theNodes[i]->getPosFinal3d();
  }

//! @brief Returns a matriz con las posiciones de los nodos.
std::list<Pos3d> XC::NodePtrs::getPosiciones(bool initialGeometry) const
  {
    std::list<Pos3d> retval;
    const size_t sz= theNodes.size();
    for(size_t i=0;i<sz;i++)
      retval.push_back(getPosNodo(i,initialGeometry));
    return retval;
  }

//! @brief Returns the centro de gravedad de las posiciones de los nodos.
Pos3d XC::NodePtrs::getPosCdg(bool initialGeometry) const
  {
    Pos3d retval;
    const size_t sz= theNodes.size();
    if(sz>0)
      {
        Vector3d tmp= getPosNodo(0,initialGeometry).VectorPos();;
        for(size_t i=1;i<sz;i++)
          tmp+= getPosNodo(i,initialGeometry).VectorPos();
        tmp= tmp * 1.0/sz;
        retval= Pos3d()+tmp;
      }
    return retval;
  }

//! @brief Returns true if the node are in the halfspace.
bool XC::NodePtrs::In(const SemiEspacio3d &semiEsp,const double &tol,bool initialGeometry) const
  {
    bool retval= true;
    std::list<Pos3d> posiciones= getPosiciones(initialGeometry);
    for(std::list<Pos3d>::const_iterator i= posiciones.begin();i!=posiciones.end();i++)
      if(!semiEsp.In(*i))
        { retval= false; break; }
    return retval;
  }

//! @brief Returns true if the node are outside the halfspace.
bool XC::NodePtrs::Out(const SemiEspacio3d &semiEsp,const double &tol,bool initialGeometry) const
  {
    SemiEspacio3d complementario(semiEsp);
    complementario.Swap();
    return In(complementario,tol,initialGeometry);
  }

//! @brief Returns true if there are nodes in both sides of the plane.
bool XC::NodePtrs::Corta(const Plano3d &plano,bool initialGeometry) const
  {
    bool in= In(plano,0.0,initialGeometry);
    bool out= Out(plano,0.0,initialGeometry);
    return (!in && !out);
  }

//! @brief Returns the node closest to the point being passed as parameter.
XC::Node *XC::NodePtrs::getNearestNode(const Pos3d &p,bool initialGeometry)
  {
    Node *retval= nullptr;
    double d= DBL_MAX;
    const size_t sz= theNodes.size();
    if(!theNodes.empty() && !hasNull())
      {
        d= theNodes[0]->getDist2(p,initialGeometry);
        retval= theNodes[0];
        double tmp;
        for(size_t i=1;i<sz;i++)
          {
            tmp= theNodes[i]->getDist2(p,initialGeometry);
            if(tmp<d)
              {
                d= tmp;
                retval= theNodes[i];
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
int XC::NodePtrs::getIndiceNodo(const Node *ptrNod) const
  {
    int retval= -1;
    const size_t sz= theNodes.size();
    for(size_t i=0;i<sz;i++)
      if(theNodes[i]==ptrNod)
        {
          retval= i;
          break;
        }
    return retval;
  }


//! @brief Resets tributary areas (or lengths or volumes) of connected nodes.
void XC::NodePtrs::resetTributarias(void) const
  {
    const size_t sz= theNodes.size();
    for(size_t i=0;i<sz;i++)
      theNodes[i]->resetTributaria();    
  }

//! @brief Agrega al la magnitud tributaria de cada node i
//! la componente i del vector being passed as parameter.
void XC::NodePtrs::vuelcaTributarias(const std::vector<double> &t) const
  {
    const size_t sz= theNodes.size();
    assert(sz== t.size());
    for(size_t i=0;i<sz;i++)
      theNodes[i]->addTributaria(t[i]);
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
        const Vector df= theNodes[i]->getDistributionFactor(mode);
        for(int j=0; j<df.Size(); j++)
          { retval(loc++)= df[j]; }
      }
    return retval;
  }

//! @brief Returns the vectors correspondientes a cada node colocados por filas.
XC::Matrix XC::NodePtrs::getNodeVectors(const Vector &v) const
  {
    const int numNodes= size();
    const ID dofs= getNumDOFs();
    const int ndof_max= dofs.max();
    Matrix retval(numNodes,ndof_max);
    int loc = 0;
    for(int i=0; i<numNodes; i++)
      {
        const int nc= theNodes[i]->getNumberDOF();
        for(int j=0; j<nc; j++)
          { retval(i,j)= v[loc++]; }
      }
    return retval;
    
  }
