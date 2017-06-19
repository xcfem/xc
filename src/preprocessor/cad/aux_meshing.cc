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
void meshing_quad4N_on_jk(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_i_cte &nodos,XC::TritrizPtrElem::var_ref_capa_i_cte &elements)
  {
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();

    for(size_t j=1;j<nfilas;j++)
      for(size_t k=1;k<ncols;k++)
        {
	  XC::Element *tmp= e.getCopy();
          const std::vector<int> indices= getIdNodosQuad4N(nodos,j,k);
          if((indices[0]>=0) && (indices[1]>=0) && (indices[2]>=0)  && (indices[3]>=0))
            tmp->setIdNodos(indices);
          else
            {
              delete tmp;
              tmp= nullptr;
            }
          elements(j,k)= tmp;
        }
  }

//! @ brief Mesh one row (j= constant) of 4-nodes quadrangles.
void meshing_quad4N_on_ik(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_j_cte &nodos,XC::TritrizPtrElem::var_ref_capa_j_cte &elements)
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t ncols= nodos.getNumCols();
    std::vector<int> in(4);
    for(size_t i=1;i<ncapas;i++)
      for(size_t k=1;k<ncols;k++)
        {
	  XC::Element *tmp= e.getCopy();
          in[0]= nodos(i,k)->getTag();
          in[1]= nodos(i,k+1)->getTag();
          in[2]= nodos(i+1,k+1)->getTag();
          in[3]= nodos(i+1,k)->getTag();
          tmp->setIdNodos(in);
          elements(i,k)= tmp;
        }
  }

//! @ brief Mesh one column (k= constant)  of 4-nodes quadrangles.
void meshing_quad4N_on_ij(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_k_cte &nodos,XC::TritrizPtrElem::var_ref_capa_k_cte &elements)
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t nfilas= nodos.getNumFilas();
    std::vector<int> in(4);
    for(size_t i=1;i<ncapas;i++)
      for(size_t j=1;j<nfilas;j++)
        {
	  XC::Element *tmp= e.getCopy();
          in[0]= nodos(i,j)->getTag();
          in[1]= nodos(i,j+1)->getTag();
          in[2]= nodos(i+1,j+1)->getTag();
          in[3]= nodos(i+1,j)->getTag();
          tmp->setIdNodos(in);
          elements(i,j)= tmp;
        }
  }

//! @ brief Mesh a quadrangle with 4-node elements
void meshing_quad4N_bidimensional(const XC::Element &e,const XC::TritrizPtrNod &nodos,XC::TritrizPtrElem &elements)
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();
    if(!nodos.HasNull()) //No hay pointers nulos.
      {
        if(nodos.EsCapaICte())
          {
            elements= XC::TritrizPtrElem(ncapas,XC::MatrizPtrElem(nfilas-1,ncols-1));
            XC::TritrizPtrElem::var_ref_capa_i_cte capa_elem= elements.GetVarRefCapaICte(1);
            meshing_quad4N_on_jk(e,nodos.GetConstRefCapaICte(1),capa_elem);
          }
        else if(nodos.EsCapaJCte())
          {
            elements= XC::TritrizPtrElem(ncapas-1,XC::MatrizPtrElem(nfilas,ncols-1));
            XC::TritrizPtrElem::var_ref_capa_j_cte capa_elem= elements.GetVarRefCapaJCte(1);
            meshing_quad4N_on_ik(e,nodos.GetConstRefCapaJCte(1),capa_elem);
          }
        else if(nodos.EsCapaKCte())
          {
            elements= XC::TritrizPtrElem(ncapas-1,XC::MatrizPtrElem(nfilas-1,ncols));
            XC::TritrizPtrElem::var_ref_capa_k_cte capa_elem= elements.GetVarRefCapaKCte(1);
            meshing_quad4N_on_ij(e,nodos.GetConstRefCapaKCte(1),capa_elem);
          }
      }
    else  //There are null pointers.
      std::cerr << "meshing_quad4N_bidimensional; one or more pointes to node are null." << std::endl;
  }

//! @brief Place the elements on the mesh passed as parameter.
XC::TritrizPtrElem XC::put_quad4N_on_mesh(const Element &e,const TritrizPtrNod &nodos,meshing_dir dm)
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();
    const size_t mesh_dim= nodos.GetDim();


    TritrizPtrElem retval;
    if(mesh_dim<2 && (e.getVerbosityLevel() > 4))
      std::cerr << __FUNCTION__
	        << "; bidimensional mesh needed, can't create elements."
		<< std::endl;
    else
      {
        if(mesh_dim<3) //Bidimensional mesh
          meshing_quad4N_bidimensional(e,nodos,retval);
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
                        meshing_quad4N_on_jk(e,nodos.GetConstRefCapaICte(i),capa_elem);
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
                        meshing_quad4N_on_ik(e,nodos.GetConstRefCapaJCte(j),capa_elem);
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
                        meshing_quad4N_on_ij(e,nodos.GetConstRefCapaKCte(k),capa_elem);
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
void meshing_quad9N_on_jk(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_i_cte &nodos,XC::TritrizPtrElem::var_ref_capa_i_cte &elements)
  {
    std::cerr << "meshing_quad9N_on_jk not implemented." << std::endl;
  }

//! @ brief Mesh one row (j= constant)  with 9-nodes elements.
void meshing_quad9N_on_ik(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_j_cte &nodos,XC::TritrizPtrElem::var_ref_capa_j_cte &elements)
  {
    std::cerr << "meshing_quad9N_on_ik not implemented." << std::endl;
  }

//! @ brief Mesh one column (k= constant) with 9-nodes elements.
void meshing_quad9N_on_ij(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_k_cte &nodos,XC::TritrizPtrElem::var_ref_capa_k_cte &elements)
  {
    std::cerr << "meshing_quad9N_on_ij not implemented." << std::endl;
  }

//! @ brief Mesh one quadrangle  with 9-nodes elements.
void meshing_quad9N_bidimensional(const XC::Element &e,const XC::TritrizPtrNod &nodos,XC::TritrizPtrElem &elements)
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();
    if(!nodos.HasNull()) //No hay pointers nulos.
      {
        if(nodos.EsCapaICte())
          {
            elements= XC::TritrizPtrElem(ncapas,XC::MatrizPtrElem(nfilas-1,ncols-1));
            XC::TritrizPtrElem::var_ref_capa_i_cte capa_elem= elements.GetVarRefCapaICte(1);
            meshing_quad9N_on_jk(e,nodos.GetConstRefCapaICte(1),capa_elem);
          }
        else if(nodos.EsCapaJCte())
          {
            elements= XC::TritrizPtrElem(ncapas-1,XC::MatrizPtrElem(nfilas,ncols-1));
            XC::TritrizPtrElem::var_ref_capa_j_cte capa_elem= elements.GetVarRefCapaJCte(1);
            meshing_quad9N_on_ik(e,nodos.GetConstRefCapaJCte(1),capa_elem);
          }
        else if(nodos.EsCapaKCte())
          {
            elements= XC::TritrizPtrElem(ncapas-1,XC::MatrizPtrElem(nfilas-1,ncols));
            XC::TritrizPtrElem::var_ref_capa_k_cte capa_elem= elements.GetVarRefCapaKCte(1);
            meshing_quad9N_on_ij(e,nodos.GetConstRefCapaKCte(1),capa_elem);
          }
      }
    else  //There are null pointers.
      std::cerr <<  __FUNCTION__
		<< "; there were null pointers to nodes." << std::endl;
  }

//! @brief Places the elements on the mesh passed as parameter.
XC::TritrizPtrElem XC::put_quad9N_on_mesh(const Element &e,const TritrizPtrNod &nodos,meshing_dir dm)
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();
    const size_t mesh_dim= nodos.GetDim();


    TritrizPtrElem retval;
    if(mesh_dim<2 && (e.getVerbosityLevel() > 4))
      std::cerr <<  __FUNCTION__
	        << "; bidimensional mesh needed, can't create elements."
		<< std::endl;
    else
      {
        if(mesh_dim<3) //Bidimensional mesh
          meshing_quad9N_bidimensional(e,nodos,retval);
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
                        meshing_quad9N_on_jk(e,nodos.GetConstRefCapaICte(i),capa_elem);
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
                        meshing_quad9N_on_ik(e,nodos.GetConstRefCapaJCte(j),capa_elem);
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
                        meshing_quad9N_on_ij(e,nodos.GetConstRefCapaKCte(k),capa_elem);
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
