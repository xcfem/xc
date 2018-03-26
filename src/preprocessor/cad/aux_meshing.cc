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
#include "preprocessor/cad/matrices/TritrizPtrNod.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"

//! @ brief Mesh one layer (i= constant) of 4-nodes quadrangles.
void meshing_quad4N_on_jk(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_i_cte &nodes,XC::TritrizPtrElem::var_ref_capa_i_cte &elements)
  {
    const size_t nfilas= nodes.getNumFilas();
    const size_t ncols= nodes.getNumCols();

    for(size_t j=1;j<nfilas;j++)
      for(size_t k=1;k<ncols;k++)
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
void meshing_quad4N_on_ik(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_j_cte &nodes,XC::TritrizPtrElem::var_ref_capa_j_cte &elements)
  {
    const size_t ncapas= nodes.GetCapas();
    const size_t ncols= nodes.getNumCols();
    std::vector<int> in(4);
    for(size_t i=1;i<ncapas;i++)
      for(size_t k=1;k<ncols;k++)
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
void meshing_quad4N_on_ij(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_k_cte &nodes,XC::TritrizPtrElem::var_ref_capa_k_cte &elements)
  {
    const size_t ncapas= nodes.GetCapas();
    const size_t nfilas= nodes.getNumFilas();
    std::vector<int> in(4);
    for(size_t i=1;i<ncapas;i++)
      for(size_t j=1;j<nfilas;j++)
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
    const size_t ncapas= nodes.GetCapas();
    const size_t nfilas= nodes.getNumFilas();
    const size_t ncols= nodes.getNumCols();
    if(!nodes.HasNull()) //No hay pointers nulos.
      {
        if(nodes.EsCapaICte())
          {
            elements= XC::TritrizPtrElem(ncapas,XC::MatrizPtrElem(nfilas-1,ncols-1));
            XC::TritrizPtrElem::var_ref_capa_i_cte capa_elem= elements.GetVarRefCapaICte(1);
            meshing_quad4N_on_jk(e,nodes.GetConstRefCapaICte(1),capa_elem);
          }
        else if(nodes.EsCapaJCte())
          {
            elements= XC::TritrizPtrElem(ncapas-1,XC::MatrizPtrElem(nfilas,ncols-1));
            XC::TritrizPtrElem::var_ref_capa_j_cte capa_elem= elements.GetVarRefCapaJCte(1);
            meshing_quad4N_on_ik(e,nodes.GetConstRefCapaJCte(1),capa_elem);
          }
        else if(nodes.EsCapaKCte())
          {
            elements= XC::TritrizPtrElem(ncapas-1,XC::MatrizPtrElem(nfilas-1,ncols));
            XC::TritrizPtrElem::var_ref_capa_k_cte capa_elem= elements.GetVarRefCapaKCte(1);
            meshing_quad4N_on_ij(e,nodes.GetConstRefCapaKCte(1),capa_elem);
          }
      }
    else  //There are null pointers.
      std::cerr << "meshing_quad4N_bidimensional; one or more pointes to node are null." << std::endl;
  }

//! @brief Place the elements on the mesh passed as parameter.
XC::TritrizPtrElem XC::put_quad4N_on_mesh(const Element &e,const TritrizPtrNod &nodes,meshing_dir dm)
  {
    const size_t ncapas= nodes.GetCapas();
    const size_t nfilas= nodes.getNumFilas();
    const size_t ncols= nodes.getNumCols();
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
                if(ncapas<2)
		  std::cerr << __FUNCTION__
			    << "Not enough nodes in 'i' direction."
		            << " Elements were not created." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas,MatrizPtrElem(nfilas-1,ncols-1));
                    for(size_t i=1;i<=ncapas;i++)
                      {
                        TritrizPtrElem::var_ref_capa_i_cte capa_elem= retval.GetVarRefCapaICte(i);
                        meshing_quad4N_on_jk(e,nodes.GetConstRefCapaICte(i),capa_elem);
                      }
                  }
                break;
              case dirm_j:
                if(nfilas<2)
		  std::cerr <<  __FUNCTION__
			    << "Not enough nodes in 'j' direction."
		            << " Elements were not created." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas-1,MatrizPtrElem(nfilas,ncols-1));
                    for(size_t j=1;j<=nfilas;j++)
                      {
                        TritrizPtrElem::var_ref_capa_j_cte capa_elem= retval.GetVarRefCapaJCte(j);
                        meshing_quad4N_on_ik(e,nodes.GetConstRefCapaJCte(j),capa_elem);
                      }
                  }
                break;
              case dirm_k:
                if(ncols<2)
		  std::cerr <<  __FUNCTION__
			    << "Not enough nodes in 'k' direction."
		            << " Elements were not created." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas-1,MatrizPtrElem(nfilas-1,ncols));
                    for(size_t k=1;k<=ncols;k++)
                      {
                        TritrizPtrElem::var_ref_capa_k_cte capa_elem= retval.GetVarRefCapaKCte(k);
                        meshing_quad4N_on_ij(e,nodes.GetConstRefCapaKCte(k),capa_elem);
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
void meshing_quad9N_on_jk(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_i_cte &nodes,XC::TritrizPtrElem::var_ref_capa_i_cte &elements)
  {
    std::cerr << "meshing_quad9N_on_jk not implemented." << std::endl;
  }

//! @ brief Mesh one row (j= constant)  with 9-nodes elements.
void meshing_quad9N_on_ik(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_j_cte &nodes,XC::TritrizPtrElem::var_ref_capa_j_cte &elements)
  {
    std::cerr << "meshing_quad9N_on_ik not implemented." << std::endl;
  }

//! @ brief Mesh one column (k= constant) with 9-nodes elements.
void meshing_quad9N_on_ij(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_k_cte &nodes,XC::TritrizPtrElem::var_ref_capa_k_cte &elements)
  {
    std::cerr << "meshing_quad9N_on_ij not implemented." << std::endl;
  }

//! @ brief Mesh one quadrangle  with 9-nodes elements.
void meshing_quad9N_bidimensional(const XC::Element &e,const XC::TritrizPtrNod &nodes,XC::TritrizPtrElem &elements)
  {
    const size_t ncapas= nodes.GetCapas();
    const size_t nfilas= nodes.getNumFilas();
    const size_t ncols= nodes.getNumCols();
    if(!nodes.HasNull()) //No hay pointers nulos.
      {
        if(nodes.EsCapaICte())
          {
            elements= XC::TritrizPtrElem(ncapas,XC::MatrizPtrElem(nfilas-1,ncols-1));
            XC::TritrizPtrElem::var_ref_capa_i_cte capa_elem= elements.GetVarRefCapaICte(1);
            meshing_quad9N_on_jk(e,nodes.GetConstRefCapaICte(1),capa_elem);
          }
        else if(nodes.EsCapaJCte())
          {
            elements= XC::TritrizPtrElem(ncapas-1,XC::MatrizPtrElem(nfilas,ncols-1));
            XC::TritrizPtrElem::var_ref_capa_j_cte capa_elem= elements.GetVarRefCapaJCte(1);
            meshing_quad9N_on_ik(e,nodes.GetConstRefCapaJCte(1),capa_elem);
          }
        else if(nodes.EsCapaKCte())
          {
            elements= XC::TritrizPtrElem(ncapas-1,XC::MatrizPtrElem(nfilas-1,ncols));
            XC::TritrizPtrElem::var_ref_capa_k_cte capa_elem= elements.GetVarRefCapaKCte(1);
            meshing_quad9N_on_ij(e,nodes.GetConstRefCapaKCte(1),capa_elem);
          }
      }
    else  //There are null pointers.
      std::cerr <<  __FUNCTION__
		<< "; there were null pointers to nodes." << std::endl;
  }

//! @brief Places the elements on the mesh passed as parameter.
XC::TritrizPtrElem XC::put_quad9N_on_mesh(const Element &e,const TritrizPtrNod &nodes,meshing_dir dm)
  {
    const size_t ncapas= nodes.GetCapas();
    const size_t nfilas= nodes.getNumFilas();
    const size_t ncols= nodes.getNumCols();
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
                if(ncapas<2)
		  std::cerr <<  __FUNCTION__
			    << "Not enough nodes in 'i' direction."
		            << " Elements were not created." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas,MatrizPtrElem(nfilas-1,ncols-1));
                    for(size_t i=1;i<=ncapas;i++)
                      {
                        TritrizPtrElem::var_ref_capa_i_cte capa_elem= retval.GetVarRefCapaICte(i);
                        meshing_quad9N_on_jk(e,nodes.GetConstRefCapaICte(i),capa_elem);
                      }
                  }
                break;
              case dirm_j:
                if(nfilas<2)
		  std::cerr <<  __FUNCTION__
			    << "Not enough nodes in 'j' direction."
		            << " Elements were not created." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas-1,MatrizPtrElem(nfilas,ncols-1));
                    for(size_t j=1;j<=nfilas;j++)
                      {
                        TritrizPtrElem::var_ref_capa_j_cte capa_elem= retval.GetVarRefCapaJCte(j);
                        meshing_quad9N_on_ik(e,nodes.GetConstRefCapaJCte(j),capa_elem);
                      }
                  }
                break;
              case dirm_k:
                if(ncols<2)
		  std::cerr <<  __FUNCTION__
			    << "Not enough nodes in 'k' direction."
		            << " Elements were not created." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas-1,MatrizPtrElem(nfilas-1,ncols));
                    for(size_t k=1;k<=ncols;k++)
                      {
                        TritrizPtrElem::var_ref_capa_k_cte capa_elem= retval.GetVarRefCapaKCte(k);
                        meshing_quad9N_on_ij(e,nodes.GetConstRefCapaKCte(k),capa_elem);
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
