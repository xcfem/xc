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

#include "aux_meshing.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include <domain/mesh/node/Node.h>
#include "preprocessor/multi_block_topology/matrices/TritrizPtrNod.h"
#include "preprocessor/multi_block_topology/matrices/TritrizPtrElem.h"

//! @ brief Mesh one layer (i= constant) of 4-nodes quadrangles.
void meshing_quad4N_on_jk(const XC::Element &e,const XC::TritrizPtrNod::constant_i_layer_const_ref &nodes,XC::TritrizPtrElem::constant_i_layer_variable_ref &elements)
  {
    const size_t numberOfRows= nodes.getNumberOfRows();
    const size_t numberOfColumns= nodes.getNumberOfColumns();

    for(size_t j=1;j<numberOfRows;j++)
      for(size_t k=1;k<numberOfColumns;k++)
        {
	  XC::Element *tmp= e.getCopy();
          const std::vector<int> indices= getNodeIdsQuad4N(nodes,j,k);
          if((indices[0]>=0) && (indices[1]>=0) && (indices[2]>=0)  && (indices[3]>=0))
            tmp->setIdNodes(indices);
          else
            {
              delete tmp;
              tmp= nullptr;
            }
          elements(j,k)= tmp;
        }
  }

//! @ brief Mesh one row (j= constant) of 4-nodes quadrangles.
void meshing_quad4N_on_ik(const XC::Element &e,const XC::TritrizPtrNod::constant_j_layer_const_ref &nodes,XC::TritrizPtrElem::constant_j_layer_variable_ref &elements)
  {
    const size_t numberOfLayers= nodes.getNumberOfLayers();
    const size_t numberOfColumns= nodes.getNumberOfColumns();
    std::vector<int> in(4);
    for(size_t i=1;i<numberOfLayers;i++)
      for(size_t k=1;k<numberOfColumns;k++)
        {
	  XC::Element *tmp= e.getCopy();
          in[0]= nodes(i,k)->getTag();
          in[1]= nodes(i,k+1)->getTag();
          in[2]= nodes(i+1,k+1)->getTag();
          in[3]= nodes(i+1,k)->getTag();
          tmp->setIdNodes(in);
          elements(i,k)= tmp;
        }
  }

//! @ brief Mesh one column (k= constant)  of 4-nodes quadrangles.
void meshing_quad4N_on_ij(const XC::Element &e,const XC::TritrizPtrNod::constant_k_layer_const_ref &nodes,XC::TritrizPtrElem::constant_k_layer_variable_ref &elements)
  {
    const size_t numberOfLayers= nodes.getNumberOfLayers();
    const size_t numberOfRows= nodes.getNumberOfRows();
    std::vector<int> in(4);
    for(size_t i=1;i<numberOfLayers;i++)
      for(size_t j=1;j<numberOfRows;j++)
        {
	  XC::Element *tmp= e.getCopy();
          in[0]= nodes(i,j)->getTag();
          in[1]= nodes(i,j+1)->getTag();
          in[2]= nodes(i+1,j+1)->getTag();
          in[3]= nodes(i+1,j)->getTag();
          tmp->setIdNodes(in);
          elements(i,j)= tmp;
        }
  }

//! @ brief Mesh a quadrangle with 4-node elements
void meshing_quad4N_bidimensional(const XC::Element &e,const XC::TritrizPtrNod &nodes,XC::TritrizPtrElem &elements)
  {
    const size_t numberOfLayers= nodes.getNumberOfLayers();
    const size_t numberOfRows= nodes.getNumberOfRows();
    const size_t numberOfColumns= nodes.getNumberOfColumns();
    if(!nodes.HasNull()) //No hay pointers nulos.
      {
        if(nodes.isIConstantLayer())
          {
            elements= XC::TritrizPtrElem(numberOfLayers,XC::MatrizPtrElem(numberOfRows-1,numberOfColumns-1));
            XC::TritrizPtrElem::constant_i_layer_variable_ref elem_layer= elements.getConstantILayerVarRef(1);
            meshing_quad4N_on_jk(e,nodes.getConstantILayerConstRef(1),elem_layer);
          }
        else if(nodes.isJConstantLayer())
          {
            elements= XC::TritrizPtrElem(numberOfLayers-1,XC::MatrizPtrElem(numberOfRows,numberOfColumns-1));
            XC::TritrizPtrElem::constant_j_layer_variable_ref elem_layer= elements.getConstantJLayerVarRef(1);
            meshing_quad4N_on_ik(e,nodes.getConstantJLayerConstRef(1),elem_layer);
          }
        else if(nodes.isKConstantLayer())
          {
            elements= XC::TritrizPtrElem(numberOfLayers-1,XC::MatrizPtrElem(numberOfRows-1,numberOfColumns));
            XC::TritrizPtrElem::constant_k_layer_variable_ref elem_layer= elements.getConstantKLayerVarRef(1);
            meshing_quad4N_on_ij(e,nodes.getConstantKLayerConstRef(1),elem_layer);
          }
      }
    else  //There are null pointers.
      std::cerr << "meshing_quad4N_bidimensional; one or more pointes to node are null." << std::endl;
  }

//! @brief Place the elements on the mesh passed as parameter.
XC::TritrizPtrElem XC::put_quad4N_on_mesh(const Element &e,const TritrizPtrNod &nodes,meshing_dir dm)
  {
    const size_t numberOfLayers= nodes.getNumberOfLayers();
    const size_t numberOfRows= nodes.getNumberOfRows();
    const size_t numberOfColumns= nodes.getNumberOfColumns();
    const size_t mesh_dim= nodes.GetDim();


    TritrizPtrElem retval;
    if(mesh_dim<2 && (e.getVerbosityLevel() > 4))
      std::cerr << __FUNCTION__
	        << "; bidimensional mesh needed, can't create elements."
		<< std::endl;
    else
      {
        if(mesh_dim<3) //Bidimensional mesh
          meshing_quad4N_bidimensional(e,nodes,retval);
        else
          {
            switch(dm)
              {
              case dirm_i:
                if(numberOfLayers<2)
		  std::cerr << __FUNCTION__
			    << "Not enough nodes in 'i' direction."
		            << " Elements were not created." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(numberOfLayers,MatrizPtrElem(numberOfRows-1,numberOfColumns-1));
                    for(size_t i=1;i<=numberOfLayers;i++)
                      {
                        TritrizPtrElem::constant_i_layer_variable_ref elem_layer= retval.getConstantILayerVarRef(i);
                        meshing_quad4N_on_jk(e,nodes.getConstantILayerConstRef(i),elem_layer);
                      }
                  }
                break;
              case dirm_j:
                if(numberOfRows<2)
		  std::cerr <<  __FUNCTION__
			    << "Not enough nodes in 'j' direction."
		            << " Elements were not created." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(numberOfLayers-1,MatrizPtrElem(numberOfRows,numberOfColumns-1));
                    for(size_t j=1;j<=numberOfRows;j++)
                      {
                        TritrizPtrElem::constant_j_layer_variable_ref elem_layer= retval.getConstantJLayerVarRef(j);
                        meshing_quad4N_on_ik(e,nodes.getConstantJLayerConstRef(j),elem_layer);
                      }
                  }
                break;
              case dirm_k:
                if(numberOfColumns<2)
		  std::cerr <<  __FUNCTION__
			    << "Not enough nodes in 'k' direction."
		            << " Elements were not created." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(numberOfLayers-1,MatrizPtrElem(numberOfRows-1,numberOfColumns));
                    for(size_t k=1;k<=numberOfColumns;k++)
                      {
                        TritrizPtrElem::constant_k_layer_variable_ref elem_layer= retval.getConstantKLayerVarRef(k);
                        meshing_quad4N_on_ij(e,nodes.getConstantKLayerConstRef(k),elem_layer);
                      }
                  }
                break;
              default:
                break;
              }
          }  
      }
    return retval;
  }

//! @ brief Mesh one layer (i= constant) with 9-nodes elements.
void meshing_quad9N_on_jk(const XC::Element &e,const XC::TritrizPtrNod::constant_i_layer_const_ref &nodes,XC::TritrizPtrElem::constant_i_layer_variable_ref &elements)
  {
    std::cerr << "meshing_quad9N_on_jk not implemented." << std::endl;
  }

//! @ brief Mesh one row (j= constant)  with 9-nodes elements.
void meshing_quad9N_on_ik(const XC::Element &e,const XC::TritrizPtrNod::constant_j_layer_const_ref &nodes,XC::TritrizPtrElem::constant_j_layer_variable_ref &elements)
  {
    std::cerr << "meshing_quad9N_on_ik not implemented." << std::endl;
  }

//! @ brief Mesh one column (k= constant) with 9-nodes elements.
void meshing_quad9N_on_ij(const XC::Element &e,const XC::TritrizPtrNod::constant_k_layer_const_ref &nodes,XC::TritrizPtrElem::constant_k_layer_variable_ref &elements)
  {
    std::cerr << "meshing_quad9N_on_ij not implemented." << std::endl;
  }

//! @ brief Mesh one quadrangle  with 9-nodes elements.
void meshing_quad9N_bidimensional(const XC::Element &e,const XC::TritrizPtrNod &nodes,XC::TritrizPtrElem &elements)
  {
    const size_t numberOfLayers= nodes.getNumberOfLayers();
    const size_t numberOfRows= nodes.getNumberOfRows();
    const size_t numberOfColumns= nodes.getNumberOfColumns();
    if(!nodes.HasNull()) //No hay pointers nulos.
      {
        if(nodes.isIConstantLayer())
          {
            elements= XC::TritrizPtrElem(numberOfLayers,XC::MatrizPtrElem(numberOfRows-1,numberOfColumns-1));
            XC::TritrizPtrElem::constant_i_layer_variable_ref elem_layer= elements.getConstantILayerVarRef(1);
            meshing_quad9N_on_jk(e,nodes.getConstantILayerConstRef(1),elem_layer);
          }
        else if(nodes.isJConstantLayer())
          {
            elements= XC::TritrizPtrElem(numberOfLayers-1,XC::MatrizPtrElem(numberOfRows,numberOfColumns-1));
            XC::TritrizPtrElem::constant_j_layer_variable_ref elem_layer= elements.getConstantJLayerVarRef(1);
            meshing_quad9N_on_ik(e,nodes.getConstantJLayerConstRef(1),elem_layer);
          }
        else if(nodes.isKConstantLayer())
          {
            elements= XC::TritrizPtrElem(numberOfLayers-1,XC::MatrizPtrElem(numberOfRows-1,numberOfColumns));
            XC::TritrizPtrElem::constant_k_layer_variable_ref elem_layer= elements.getConstantKLayerVarRef(1);
            meshing_quad9N_on_ij(e,nodes.getConstantKLayerConstRef(1),elem_layer);
          }
      }
    else  //There are null pointers.
      std::cerr <<  __FUNCTION__
		<< "; there were null pointers to nodes." << std::endl;
  }

//! @brief Places the elements on the mesh passed as parameter.
XC::TritrizPtrElem XC::put_quad9N_on_mesh(const Element &e,const TritrizPtrNod &nodes,meshing_dir dm)
  {
    const size_t numberOfLayers= nodes.getNumberOfLayers();
    const size_t numberOfRows= nodes.getNumberOfRows();
    const size_t numberOfColumns= nodes.getNumberOfColumns();
    const size_t mesh_dim= nodes.GetDim();


    TritrizPtrElem retval;
    if(mesh_dim<2 && (e.getVerbosityLevel() > 4))
      std::cerr <<  __FUNCTION__
	        << "; bidimensional mesh needed, can't create elements."
		<< std::endl;
    else
      {
        if(mesh_dim<3) //Bidimensional mesh
          meshing_quad9N_bidimensional(e,nodes,retval);
        else
          {
            switch(dm)
              {
              case dirm_i:
                if(numberOfLayers<2)
		  std::cerr <<  __FUNCTION__
			    << "Not enough nodes in 'i' direction."
		            << " Elements were not created." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(numberOfLayers,MatrizPtrElem(numberOfRows-1,numberOfColumns-1));
                    for(size_t i=1;i<=numberOfLayers;i++)
                      {
                        TritrizPtrElem::constant_i_layer_variable_ref elem_layer= retval.getConstantILayerVarRef(i);
                        meshing_quad9N_on_jk(e,nodes.getConstantILayerConstRef(i),elem_layer);
                      }
                  }
                break;
              case dirm_j:
                if(numberOfRows<2)
		  std::cerr <<  __FUNCTION__
			    << "Not enough nodes in 'j' direction."
		            << " Elements were not created." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(numberOfLayers-1,MatrizPtrElem(numberOfRows,numberOfColumns-1));
                    for(size_t j=1;j<=numberOfRows;j++)
                      {
                        TritrizPtrElem::constant_j_layer_variable_ref elem_layer= retval.getConstantJLayerVarRef(j);
                        meshing_quad9N_on_ik(e,nodes.getConstantJLayerConstRef(j),elem_layer);
                      }
                  }
                break;
              case dirm_k:
                if(numberOfColumns<2)
		  std::cerr <<  __FUNCTION__
			    << "Not enough nodes in 'k' direction."
		            << " Elements were not created." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(numberOfLayers-1,MatrizPtrElem(numberOfRows-1,numberOfColumns));
                    for(size_t k=1;k<=numberOfColumns;k++)
                      {
                        TritrizPtrElem::constant_k_layer_variable_ref elem_layer= retval.getConstantKLayerVarRef(k);
                        meshing_quad9N_on_ij(e,nodes.getConstantKLayerConstRef(k),elem_layer);
                      }
                  }
                break;
              default:
                break;
              }
          }  
      }
    return retval;
  }
