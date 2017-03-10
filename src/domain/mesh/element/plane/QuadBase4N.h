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
//QuadBase4N.h
                                                                        
#include "ElemPlano.h"

#ifndef QuadBase4N_h
#define QuadBase4N_h

#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "preprocessor/cad/aux_meshing.h"
#include "preprocessor/loaders/LoadLoader.h"
#include "domain/load/plane/BidimStrainLoad.h"
#include "med.h"
#include "vtkCellType.h"

namespace XC {
//! \ingroup Elem
//
//! \ingroup ElemPlanos
//
//! @brief Base class for 4 node quads.
template <class PhysProp>
class QuadBase4N : public ElemPlano<4,PhysProp>
  {
  protected:
    TritrizPtrElem put_on_mesh(const TritrizPtrNod &,meshing_dir dm) const;
  public:

    QuadBase4N(int classTag,const PhysProp &pp);
    QuadBase4N(int tag, int classTag,const PhysProp &);
    QuadBase4N(int tag, int classTag, int node1, int node2, int node3, int node4,const PhysProp &pp);

    Element::NodesEdge getNodesEdge(const size_t &i) const;
    ID getLocalIndexNodesEdge(const size_t &i) const;
    int getEdgeNodes(const Node *,const Node *) const;

    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

    void zeroLoad(void);	
    int addLoad(ElementalLoad *theLoad, double loadFactor);

  };

//! @brief Constructor
template <class PhysProp>
  XC::QuadBase4N<PhysProp>::QuadBase4N(int classTag,const PhysProp &pp)
  : ElemPlano<4,PhysProp>(0,classTag,pp) {}

//! @brief Constructor.
template <class PhysProp>
XC::QuadBase4N<PhysProp>::QuadBase4N(int tag,int classTag,const PhysProp &pp)
  :ElemPlano<4,PhysProp>(tag,classTag,pp) {}

//! @brief Constructor.
template <class PhysProp>
XC::QuadBase4N<PhysProp>::QuadBase4N(int tag, int classTag, int node1, int node2, int node3, int node4,const PhysProp &pp)
  : ElemPlano<4,PhysProp>(tag,classTag,pp)
  {
    this->theNodes.set_id_nodos(node1,node2,node3,node4);
  }


//! @brief Put the element on the mesh being passed as parameter.
template <class PhysProp>
XC::TritrizPtrElem XC::QuadBase4N<PhysProp>::put_on_mesh(const XC::TritrizPtrNod &nodos,meshing_dir dm) const
  { return put_quad4N_on_mesh(*this,nodos,dm); }

//! @brief Returns the nodos de un lado del elemento.
template <class PhysProp>
XC::Element::NodesEdge XC::QuadBase4N<PhysProp>::getNodesEdge(const size_t &i) const
  {
    Element::NodesEdge retval(2,static_cast<Node *>(nullptr));
    const NodePtrsWithIDs &nodos= ElemPlano<4,PhysProp>::getNodePtrs();
    const size_t sz= nodos.size();
    if(i<sz)
      {
        retval[0]= nodos(i);
        if(i<(sz-1))
          retval[1]= nodos(i+1);
        else
          retval[1]= nodos(0);
      }
    return retval;    
  }

//! @brief Returns the borde del elemento
//! que tiene por extremos los nodos being passed as parameters.
template <class PhysProp>
int XC::QuadBase4N<PhysProp>::getEdgeNodes(const Node *n1,const Node *n2) const
  {
    int retval= -1;
    const NodePtrsWithIDs &nodos= ElemPlano<4,PhysProp>::getNodePtrs();
    const int i1= nodos.find(n1);
    const int i2= nodos.find(n2);
    if((i1>=0) && (i2>=0))
      {
        const int dif= i2-i1;
        if(dif==1)
          retval= i1;
        else if(dif==-1)
          retval= i2;
        else if((i1==3) && (i2==0))
          retval= 3;
        else if((i1==0) && (i2==3))
          retval= 3;
      }
    return retval;
  }

//! @brief Returns the índices locales de los nodos del elemento
//! situados sobre el borde (o arista) being passed as parameters.
template <class PhysProp>
ID XC::QuadBase4N<PhysProp>::getLocalIndexNodesEdge(const size_t &i) const
  {
    ID retval(2);
    const NodePtrsWithIDs &nodos= ElemPlano<4,PhysProp>::getNodePtrs();
    const size_t sz= nodos.size();
    if(i<sz)
      {
        retval[0]= i;
        if(i<(sz-1))
          retval[1]= i+1;
        else
          retval[1]= 0;
      }
    return retval;
  }

//! @brief Zeroes loads on element.
template <class PhysProp>
void XC::QuadBase4N<PhysProp>::zeroLoad(void)
  {
    ElemPlano<4,PhysProp>::zeroLoad();
    this->physicalProperties.getMaterialsVector().zeroInitialGeneralizedStrains(); //Removes initial deformations.
    return;
  }

//! @brief Añade al elemento the load being passed as parameter.
template <class PhysProp>
int XC::QuadBase4N<PhysProp>::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(this->isDead())
      std::cerr << this->nombre_clase() 
                << "; load over inactiva element: "
                << this->getTag() << std::endl;
    else
      {
        if(const BidimStrainLoad *strainLoad= dynamic_cast<const BidimStrainLoad *>(theLoad)) //Prescribed strains.
          {
            static std::vector<Vector> initStrains;
            initStrains= strainLoad->getStrains();
            for(std::vector<Vector>::iterator i= initStrains.begin();i!=initStrains.end();i++)
              (*i)*= loadFactor;
            this->physicalProperties.getMaterialsVector().addInitialGeneralizedStrains(initStrains);
          }
        else
          {
            std::cerr << "QuadBase4N::addLoad -- load type unknown for element with tag: " <<
            this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

//! @brief Interfaz con VTK.
template <class PhysProp>
int XC::QuadBase4N<PhysProp>::getVtkCellType(void) const
  { return VTK_QUAD; }

//! @brief Interfaz con el formato MED de Salome.
template <class PhysProp>
int XC::QuadBase4N<PhysProp>::getMEDCellType(void) const
  { return MED_QUAD4; }

} // end of XC namespace
#endif
