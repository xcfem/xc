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

#include "aux_mallado.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/NodePtrsWithIDs.h"
#include <domain/mesh/node/Node.h>
#include "preprocessor/cad/matrices/TritrizPtrNod.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"

//! @ brief Malla una capa (i= cte).
void malla_quad4N_en_jk(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_i_cte &nodos,XC::TritrizPtrElem::var_ref_capa_i_cte &elementos)
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
          elementos(j,k)= tmp;
        }
  }

//! @ brief Malla una fila (j= cte).
void malla_quad4N_en_ik(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_j_cte &nodos,XC::TritrizPtrElem::var_ref_capa_j_cte &elementos)
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
          elementos(i,k)= tmp;
        }
  }

//! @ brief Malla una columna (k= cte).
void malla_quad4N_en_ij(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_k_cte &nodos,XC::TritrizPtrElem::var_ref_capa_k_cte &elementos)
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
          elementos(i,j)= tmp;
        }
  }

void malla_quad4N_bidimensional(const XC::Element &e,const XC::TritrizPtrNod &nodos,XC::TritrizPtrElem &elementos)
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();
    if(!nodos.HasNull()) //No hay punteros nulos.
      {
        if(nodos.EsCapaICte())
          {
            elementos= XC::TritrizPtrElem(ncapas,XC::MatrizPtrElem(nfilas-1,ncols-1));
            XC::TritrizPtrElem::var_ref_capa_i_cte capa_elem= elementos.GetVarRefCapaICte(1);
            malla_quad4N_en_jk(e,nodos.GetConstRefCapaICte(1),capa_elem);
          }
        else if(nodos.EsCapaJCte())
          {
            elementos= XC::TritrizPtrElem(ncapas-1,XC::MatrizPtrElem(nfilas,ncols-1));
            XC::TritrizPtrElem::var_ref_capa_j_cte capa_elem= elementos.GetVarRefCapaJCte(1);
            malla_quad4N_en_ik(e,nodos.GetConstRefCapaJCte(1),capa_elem);
          }
        else if(nodos.EsCapaKCte())
          {
            elementos= XC::TritrizPtrElem(ncapas-1,XC::MatrizPtrElem(nfilas-1,ncols));
            XC::TritrizPtrElem::var_ref_capa_k_cte capa_elem= elementos.GetVarRefCapaKCte(1);
            malla_quad4N_en_ij(e,nodos.GetConstRefCapaKCte(1),capa_elem);
          }
      }
    else  //Hay punteros nulos.
      std::cerr << "malla_quad4N_bidimensional; existen punteros a nodo nulos." << std::endl;
  }

//Coloca el elemento en la malla being passed as parameter.
XC::TritrizPtrElem XC::coloca_quad4N_en_malla(const Element &e,const TritrizPtrNod &nodos,dir_mallado dm)
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();
    const size_t dim_malla= nodos.GetDim();


    TritrizPtrElem retval;
    if(dim_malla<2 && (e.GetNivelVerborrea() > 4))
      std::cerr << "coloca_quad4N_en_malla; se necesita una malla bidimensional, no se pudieron crear elementos." << std::endl;
    else
      {
        if(dim_malla<3) //Malla bidimensional
          malla_quad4N_bidimensional(e,nodos,retval);
        else
          {
            switch(dm)
              {
              case dirm_i:
                if(ncapas<2)
		  std::cerr << "Número insuficiente de nodos en dirección i. No se crean elementos." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas,MatrizPtrElem(nfilas-1,ncols-1));
                    for(size_t i=1;i<=ncapas;i++)
                      {
                        TritrizPtrElem::var_ref_capa_i_cte capa_elem= retval.GetVarRefCapaICte(i);
                        malla_quad4N_en_jk(e,nodos.GetConstRefCapaICte(i),capa_elem);
                      }
                  }
                break;
              case dirm_j:
                if(nfilas<2)
		  std::cerr << "Número insuficiente de nodos en dirección j. No se crean elementos." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas-1,MatrizPtrElem(nfilas,ncols-1));
                    for(size_t j=1;j<=nfilas;j++)
                      {
                        TritrizPtrElem::var_ref_capa_j_cte capa_elem= retval.GetVarRefCapaJCte(j);
                        malla_quad4N_en_ik(e,nodos.GetConstRefCapaJCte(j),capa_elem);
                      }
                  }
                break;
              case dirm_k:
                if(ncols<2)
		  std::cerr << "Número insuficiente de nodos en dirección k. No se crean elementos." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas-1,MatrizPtrElem(nfilas-1,ncols));
                    for(size_t k=1;k<=ncols;k++)
                      {
                        TritrizPtrElem::var_ref_capa_k_cte capa_elem= retval.GetVarRefCapaKCte(k);
                        malla_quad4N_en_ij(e,nodos.GetConstRefCapaKCte(k),capa_elem);
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

//! @ brief Malla una capa (i= cte).
void malla_quad9N_en_jk(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_i_cte &nodos,XC::TritrizPtrElem::var_ref_capa_i_cte &elementos)
  {
    std::cerr << "malla_quad9N_en_jk no implementada." << std::endl;
  }

//! @ brief Malla una fila (j= cte).
void malla_quad9N_en_ik(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_j_cte &nodos,XC::TritrizPtrElem::var_ref_capa_j_cte &elementos)
  {
    std::cerr << "malla_quad9N_en_ik no implementada." << std::endl;
  }

//! @ brief Malla una columna (k= cte).
void malla_quad9N_en_ij(const XC::Element &e,const XC::TritrizPtrNod::const_ref_capa_k_cte &nodos,XC::TritrizPtrElem::var_ref_capa_k_cte &elementos)
  {
    std::cerr << "malla_quad9N_en_ij no implementada." << std::endl;
  }

void malla_quad9N_bidimensional(const XC::Element &e,const XC::TritrizPtrNod &nodos,XC::TritrizPtrElem &elementos)
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();
    if(!nodos.HasNull()) //No hay punteros nulos.
      {
        if(nodos.EsCapaICte())
          {
            elementos= XC::TritrizPtrElem(ncapas,XC::MatrizPtrElem(nfilas-1,ncols-1));
            XC::TritrizPtrElem::var_ref_capa_i_cte capa_elem= elementos.GetVarRefCapaICte(1);
            malla_quad9N_en_jk(e,nodos.GetConstRefCapaICte(1),capa_elem);
          }
        else if(nodos.EsCapaJCte())
          {
            elementos= XC::TritrizPtrElem(ncapas-1,XC::MatrizPtrElem(nfilas,ncols-1));
            XC::TritrizPtrElem::var_ref_capa_j_cte capa_elem= elementos.GetVarRefCapaJCte(1);
            malla_quad9N_en_ik(e,nodos.GetConstRefCapaJCte(1),capa_elem);
          }
        else if(nodos.EsCapaKCte())
          {
            elementos= XC::TritrizPtrElem(ncapas-1,XC::MatrizPtrElem(nfilas-1,ncols));
            XC::TritrizPtrElem::var_ref_capa_k_cte capa_elem= elementos.GetVarRefCapaKCte(1);
            malla_quad9N_en_ij(e,nodos.GetConstRefCapaKCte(1),capa_elem);
          }
      }
    else  //Hay punteros nulos.
      std::cerr << "malla_quad9N_bidimensional; existen punteros a nodo nulos." << std::endl;
  }

//Coloca el elemento en la malla being passed as parameter.
XC::TritrizPtrElem XC::coloca_quad9N_en_malla(const Element &e,const TritrizPtrNod &nodos,dir_mallado dm)
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();
    const size_t dim_malla= nodos.GetDim();


    TritrizPtrElem retval;
    if(dim_malla<2 && (e.GetNivelVerborrea() > 4))
      std::cerr << "coloca_quad9N_en_malla; se necesita una malla bidimensional, no se pudieron crear elementos." << std::endl;
    else
      {
        if(dim_malla<3) //Malla bidimensional
          malla_quad9N_bidimensional(e,nodos,retval);
        else
          {
            switch(dm)
              {
              case dirm_i:
                if(ncapas<2)
		  std::cerr << "Número insuficiente de nodos en dirección i. No se crean elementos." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas,MatrizPtrElem(nfilas-1,ncols-1));
                    for(size_t i=1;i<=ncapas;i++)
                      {
                        TritrizPtrElem::var_ref_capa_i_cte capa_elem= retval.GetVarRefCapaICte(i);
                        malla_quad9N_en_jk(e,nodos.GetConstRefCapaICte(i),capa_elem);
                      }
                  }
                break;
              case dirm_j:
                if(nfilas<2)
		  std::cerr << "Número insuficiente de nodos en dirección j. No se crean elementos." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas-1,MatrizPtrElem(nfilas,ncols-1));
                    for(size_t j=1;j<=nfilas;j++)
                      {
                        TritrizPtrElem::var_ref_capa_j_cte capa_elem= retval.GetVarRefCapaJCte(j);
                        malla_quad9N_en_ik(e,nodos.GetConstRefCapaJCte(j),capa_elem);
                      }
                  }
                break;
              case dirm_k:
                if(ncols<2)
		  std::cerr << "Número insuficiente de nodos en dirección k. No se crean elementos." << std::endl;
                else
                  {
                    retval= TritrizPtrElem(ncapas-1,MatrizPtrElem(nfilas-1,ncols));
                    for(size_t k=1;k<=ncols;k++)
                      {
                        TritrizPtrElem::var_ref_capa_k_cte capa_elem= retval.GetVarRefCapaKCte(k);
                        malla_quad9N_en_ij(e,nodos.GetConstRefCapaKCte(k),capa_elem);
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
