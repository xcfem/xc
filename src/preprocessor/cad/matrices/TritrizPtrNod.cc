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
//TritrizPtrNod.cc

#include "TritrizPtrNod.h"
#include "domain/mesh/node/Node.h"
#include "xc_basic/src/funciones/algebra/integ_num.h"

#include <boost/any.hpp>
#include "domain/domain/Domain.h"
#include "domain/constraints/SP_Constraint.h"


#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d2/Poligono3d.h"

#include "boost/lexical_cast.hpp"

//! @brief Constructor por defecto.
XC::TritrizPtrNod::TritrizPtrNod(const size_t capas)
  : TritrizPtrBase<MatrizPtrNod>(capas) {}
//! @brief Constructor.
XC::TritrizPtrNod::TritrizPtrNod(const size_t capas,const size_t filas,const size_t cols)
  : TritrizPtrBase<MatrizPtrNod>(capas)
  {
    for(size_t i=0;i<capas;i++)
      (*this)[i]= MatrizPtrNod(filas,cols);
  }

//! @brief Devuelve, si lo encuentra, un puntero al nodo
//! cuyo tag se pasa como parámetro.
XC::Node *XC::TritrizPtrNod::buscaNodo(const int &tag)
  {
    Node *retval= nullptr;
    const size_t ncapas= GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      {
        MatrizPtrNod &capa= operator()(i);
        retval= capa.buscaNodo(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Devuelve el nodo más próximo al punto being passed as parameter.
const XC::Node *XC::TritrizPtrNod::getNearestNode(const Pos3d &p) const
  {
    TritrizPtrNod *this_no_const= const_cast<TritrizPtrNod *>(this);
    return this_no_const->getNearestNode(p);
  }

//! @brief Devuelve el nodo más próximo al punto being passed as parameter.
XC::Node *XC::TritrizPtrNod::getNearestNode(const Pos3d &p)
  {
    Node *retval= nullptr, *ptrNod= nullptr;
    const size_t ncapas= GetCapas();
    double d= DBL_MAX;
    double tmp= 0;
    if(ncapas>100)
      std::clog << "La «tritriz» de nodos es tiene "
                << ncapas << " capas "
                << " es mejor buscar por coordenadas en el conjunto asociado."
                << std::endl;
    for(size_t i=1;i<=ncapas;i++)
      {
        MatrizPtrNod &capa= operator()(i);
        ptrNod= capa.getNearestNode(p);
        tmp= ptrNod->getDist2(p);
        if(tmp<d)
          {
            d= tmp;
            retval= ptrNod;
          }
      }
    return retval;
  }

//! @brief Devuelve los índices del nodo indicado por el puntero
//! being passed as parameter.
XC::ID XC::TritrizPtrNod::getNodeIndices(const Node *n) const
  {
    ID retval(3);
    retval[0]= -1; retval[1]= -1; retval[2]= -1;
    const size_t ncapas= GetCapas();
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    Node *ptrNod= nullptr;
    for(size_t i=1;i<=ncapas;i++)
      for(size_t j=1;j<=nfilas;j++)
        for(size_t k=1;k<=ncols;k++)
          {
            ptrNod= (*this)(i,j,k);
            if(ptrNod==n)
              {
                retval[0]= i; retval[1]= j; retval[2]= k;
                break;
              }
          }
    return retval;
  }

//! @brief Devuelve, si lo encuentra, un puntero al nodo
//! cuyo tag se pasa como parámetro.
const XC::Node *XC::TritrizPtrNod::buscaNodo(const int &tag) const
  {
    const Node *retval= nullptr;
    const size_t ncapas= GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      {
        const MatrizPtrNod &capa= operator()(i);
        retval= capa.buscaNodo(tag);
        if(retval) break;
      }
    return retval;
  }



XC::Vector XC::TritrizPtrNod::IntegSimpsonFilaI(const size_t &f,const size_t &c,const ExprAlgebra &e,const size_t &n) const
  {
    const_ref_fila_i fila= GetConstRefFilaI(f,c);
    const std::deque<double> dq_retval= IntegSimpsonFila(fila,"z",2,e,n);
    const size_t szr= dq_retval.size();
    Vector retval(szr);
    for(size_t i= 0;i<szr;i++)
      retval[i]= dq_retval[i];
    return retval;
  }

XC::Vector XC::TritrizPtrNod::IntegSimpsonFilaJ(const size_t &capa, const size_t &c,const ExprAlgebra &e,const size_t &n) const
  {
    const_ref_fila_j fila= GetConstRefFilaJ(capa,c);
    const std::deque<double> dq_retval= IntegSimpsonFila(fila,"x",0,e,n);
    const size_t szr= dq_retval.size();
    Vector retval(szr);
    for(size_t i= 0;i<szr;i++)
      retval[i]= dq_retval[i];
    return retval;
  }

XC::Vector XC::TritrizPtrNod::IntegSimpsonFilaK(const size_t &capa,const size_t &f,const ExprAlgebra &e,const size_t &n) const
  {
    const_ref_fila_k fila= GetConstRefFilaK(capa,f);
    const std::deque<double> dq_retval= IntegSimpsonFila(fila,"y",1,e,n);
    const size_t szr= dq_retval.size();
    Vector retval(szr);
    for(size_t i= 0;i<szr;i++)
      retval[i]= dq_retval[i];
    return retval;
  }

//! @brief Impone desplazamiento nulo en los nodos de este conjunto.
void XC::TritrizPtrNod::fix(const SP_Constraint &spc) const
  {
    if(Null()) return;
    const size_t ncapas= GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      {
        const MatrizPtrNod &capa= operator()(i);
        capa.fix(spc);
      }
  }

std::vector<int> XC::TritrizPtrNod::getTags(void) const
  {
    const size_t ncapas= GetCapas();
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    std::vector<int> retval(ncapas*nfilas*ncols,-1);
    size_t conta= 0;
    for(size_t i=1;i<=ncapas;i++)
      for(size_t j=1;j<=nfilas;j++)
        for(size_t k=1;k<=ncols;k++)
          retval[conta++]= (*this)(i,j,k)->getTag();
    return retval;
  }

void XC::TritrizPtrNod::Print(std::ostream &os) const
  {
    const size_t ncapas= GetCapas();
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t i=1;i<=ncapas;i++)
      {
        for(size_t j=1;j<=nfilas;j++)
          {
            for(size_t k=1;k<=ncols;k++)
	      os << (*this)(i,j,k)->getTag() << " ";
	    os << std::endl;
          }
        os << std::endl;
      }
  }

std::ostream &XC::operator<<(std::ostream &os, const TritrizPtrNod &t)
  {
    t.Print(os);
    return os;
  }

void XC::fix(const XC::TritrizPtrNod::var_ref_caja &ref_caja,const XC::SP_Constraint &spc)
  {
    if(ref_caja.Empty()) return;
    const size_t ncapas= ref_caja.GetCapas();
    const size_t nfilas= ref_caja.getNumFilas();
    const size_t ncols= ref_caja.getNumCols();

    // Obtenemos el dominio.
    const Node *n= ref_caja(1,1,1);
    Domain *dom= nullptr;
    dom= n->getDomain();
    if(dom)
      {
        for(size_t i=1;i<=ncapas;i++)
          for(size_t j=1;j<=nfilas;j++)
            for(size_t k=1;k<=ncols;k++)
              {
                const Node *nod= ref_caja(i,j,k);
                if(nod)
                  {
                    const int tag_nod= nod->getTag();
                    SP_Constraint *sp= spc.getCopy();
                    sp->setNodeTag(tag_nod); 
                    dom->addSP_Constraint(sp);
                  }
              }
      } 
  }

//! @brief Devuelve los índices de los nodos (j,k),(j+1,k),(j+1,k+1),(j,k+1). 
std::vector<int> XC::getIdNodosQuad4N(const XC::TritrizPtrNod::const_ref_capa_i_cte &nodos,const size_t &j,const size_t &k)
  {
    std::vector<int> retval(4,-1);
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();
    if(j>=nfilas)
      {
        std::cerr << "getIdNodosQuad; índice de fila j= " << j << " fuera de rango.\n";
        return retval;
      }
    if(k>=ncols)
      {
        std::cerr << "getIdNodosQuad; índice de columna k= " << k << " fuera de rango.\n";
        return retval;
      }


    Pos3d p1;
    const Node *ptr= nodos(j,k);
    if(ptr)
      {
        retval[0]= ptr->getTag();
        if(retval[0]<0)
          std::cerr << "getIdNodosQuad; error al obtener el identificador de nodo (" << j << ',' << k << ").\n";
        p1= ptr->getPosInicial3d();
      }

    Pos3d p2;
    ptr= nodos(j,k+1);
    if(ptr)
      {
        retval[1]= ptr->getTag();
        if(retval[1]<0)
          std::cerr << "getIdNodosQuad; error al obtener el identificador de nodo (" << j << ',' << k+1 << ").\n";
        p2= ptr->getPosInicial3d();
      }

    Pos3d p3;
    ptr= nodos(j+1,k+1);
    if(ptr)
      {
        retval[2]= ptr->getTag();
        if(retval[2]<0)
          std::cerr << "getIdNodosQuad; error al obtener el identificador de nodo (" << j+1 << ',' << k+1 << ").\n";
        p3= ptr->getPosInicial3d();
      }

    Pos3d p4;
    ptr= nodos(j+1,k);
    if(ptr)
      {
        retval[3]=ptr->getTag();
        if(retval[3]<0)
          std::cerr << "getIdNodosQuad; error al obtener el identificador de nodo (" << j+1 << ',' << k << ").\n";
        p4= ptr->getPosInicial3d();
      }

//     const Vector3d v2= p2-p1;
//     const Vector3d v3= p3-p2;
//     const Vector3d v4= p4-p3;
//     const Vector3d v1= p1-p4;
//     const double d1= dot(v1,v3);
//     const double d2= dot(v2,v4);
//     if((d1>0))
//       {
//         std::swap(p3,p2);
//         std::swap(retval[2],retval[1]);
//       }
//     if((d2>0))
//       {
// 	std::swap(p3,p4);
//         std::swap(retval[2],retval[3]);
//       }

    std::list<Pos3d> posiciones;
    posiciones.push_back(p1);
    posiciones.push_back(p2);
    posiciones.push_back(p3);
    posiciones.push_back(p4);
    Poligono3d tmp(posiciones.begin(),posiciones.end());
    const double area= tmp.Area();
    if(area<1e-3)
      {
        std::cerr << "Al obtener la celda de índices (" << j << ',' << k
                  << ") se obtuvo un área muy pequeña (" << area << ").\n";
        std::cerr << " posición del nodo (j,k) " << p1 << std::endl;
	std::cerr << " posición del nodo (j+1,k) " << p2 << std::endl;
	std::cerr << " posición del nodo (j+1,k+1) " << p3 << std::endl;
	std::cerr << " posición del nodo (1,k+1) " << p4 << std::endl;
      }
    return retval;
  }

//! @brief Devuelve los índices de los nodos (j,k),(j+1,k),(j+1,k+1),(j,k+1). 
std::vector<int> XC::getIdNodosQuad9N(const XC::TritrizPtrNod::const_ref_capa_i_cte &nodos,const size_t &j,const size_t &k)
  {
    std::vector<int> retval(9,-1);
    std::cerr << "getIdNodosQuad9N no implementada." << std::endl;
    return retval;
  }
