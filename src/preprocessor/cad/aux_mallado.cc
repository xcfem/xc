//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
