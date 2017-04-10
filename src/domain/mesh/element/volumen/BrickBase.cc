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
//BrickBase.cc

#include "BrickBase.h"
#include "preprocessor/cad/matrices/TritrizPtrNod.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "domain/mesh/node/Node.h"

//! @brief Constructor
XC::BrickBase::BrickBase(int classTag)
  : ElemWithMaterial<8,NDMaterialPhysicalProperties>(0,classTag,NDMaterialPhysicalProperties(8,nullptr))
  {}


//! @brief Constructor.
XC::BrickBase::BrickBase(int tag,int classTag,const NDMaterialPhysicalProperties &pp)
  :ElemWithMaterial<8,NDMaterialPhysicalProperties>(tag,classTag,pp)
  {}

//! @brief Constructor.
XC::BrickBase::BrickBase(int tag, int classTag, int node1, int node2, int node3, int node4,int node5,int node6,int node7,int node8, const NDMaterialPhysicalProperties &pp) 
  : ElemWithMaterial<8,NDMaterialPhysicalProperties>(tag,classTag,pp)
  {
    theNodes.set_id_nodes(node1,node2,node3,node4,node5,node6,node7,node8);
  }

//! @brief Return the element dimension (0, 1, 2 o3 3).
size_t XC::BrickBase::getDimension(void) const
  { return 3; }

//Put the element on the mesh being passed as parameter.
XC::TritrizPtrElem XC::BrickBase::put_on_mesh(const XC::TritrizPtrNod &nodos,meshing_dir dm) const
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();
    const size_t mesh_dim= nodos.GetDim();
    TritrizPtrElem retval(ncapas-1,nfilas-1,ncols-1);
    if(mesh_dim<3)
      std::cerr << "BrickBase::put_on_mesh; three-dimensional mesh needed, can't create elements." << std::endl;
    else
      {
        for(size_t i=1;i<ncapas;i++)
          for(size_t j=1;j<nfilas;j++)
            for(size_t k=1;k<ncols;k++)
              {
	        XC::Element *tmp= this->getCopy();
                const int &Nd1= nodos(i,j,k)->getTag();
                const int &Nd2= nodos(i,j,k+1)->getTag();
                const int &Nd3= nodos(i,j+1,k+1)->getTag();
                const int &Nd4= nodos(i,j+1,k)->getTag();
                const int &Nd5= nodos(i+1,j,k)->getTag();
                const int &Nd6= nodos(i+1,j,k+1)->getTag();
                const int &Nd7= nodos(i+1,j+1,k+1)->getTag();
                const int &Nd8= nodos(i+1,j+1,k)->getTag();
                tmp->getNodePtrs().set_id_nodes(Nd1,Nd2,Nd3,Nd4,Nd5,Nd6,Nd7,Nd8);
                retval(i,j,k)= tmp;
              }
      }
    return retval;
  }

