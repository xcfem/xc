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
//MatrizPtrNod.cc

#include "MatrizPtrNod.h"
#include "domain/mesh/node/Node.h"
#include "domain/domain/Domain.h"
#include "domain/constraints/SP_Constraint.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"
#include "boost/lexical_cast.hpp"

//! @brief Lee un objeto MatrizPtrNod desde archivo
bool XC::MatrizPtrNod::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(MatrizPtrNod) Procesando comando: " << cmd << std::endl;

    if(cmd == "nodo")
      {
        const CmdParser &parser= status.Parser();
        if(parser.TieneIndices())
          {
            interpreta(parser.GetIndices());
            if(InterpreteRPN::HayArgumentos(2,cmd))
              {
                const size_t k= convert_to_size_t(InterpreteRPN::Pila().Pop()); 
                const size_t j= convert_to_size_t(InterpreteRPN::Pila().Pop()); 
                Node *ptr= (*this)(j,k);
                if(ptr)
                  ptr->LeeCmd(status);
                else
	          std::clog << "(MatrizPtrNod) Procesando comando: '" << cmd
                            << "'; no se encontró el nodo de índices: (" << j << ',' << k << ").\n";
              }
          }
        else
	  std::cerr << "(MatrizPtrNod) Procesando comando: '" << cmd
                            << "'; faltan los índices del nodo.\n";
        return true;
      }
    else
      return MatrizPtrBase<Node>::procesa_comando(status);
  }

//! @brief Devuelve, si lo encuentra, un puntero al nodo
//! cuyo tag se pasa como parámetro.
XC::Node *XC::MatrizPtrNod::buscaNodo(const int &tag)
  {
    Node *retval= nullptr;
    Node *tmp= nullptr;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          tmp= operator()(j,k);
          if(tmp)
            {
              if(tag == tmp->getTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Devuelve, si lo encuentra, un puntero al nodo
//! cuyo tag se pasa como parámetro.
const XC::Node *XC::MatrizPtrNod::buscaNodo(const int &tag) const
  {
    const Node *retval= nullptr;
    const Node *tmp= nullptr;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          tmp= operator()(j,k);
          if(tmp)
            {
              if(tag == tmp->getTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Devuelve el nodo más próximo al punto que se pasa como parámetro.
XC::Node *XC::MatrizPtrNod::getNearestNode(const Pos3d &p)
  {
    Node *retval= nullptr, *ptrNod= nullptr;
    double d= DBL_MAX;
    double tmp;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    if(nfilas*ncols>500)
      std::clog << "La matriz de nodos es tiene "
                << nfilas*ncols << " elementos "
                << " es mejor buscar por coordenadas en el conjunto asociado."
                << std::endl;
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          ptrNod= operator()(j,k);
          tmp= ptrNod->getDist2(p);
          if(tmp<d)
            {
              d= tmp;
              retval= ptrNod;
            }
        }
    return retval;
  }

//! @brief Devuelve el nodo más próximo al punto que se pasa como parámetro.
const XC::Node *XC::MatrizPtrNod::getNearestNode(const Pos3d &p) const
  {
    MatrizPtrNod *this_no_const= const_cast<MatrizPtrNod *>(this);
    return this_no_const->getNearestNode(p);
  }

//! @brief Impone desplazamiento nulo en los nodos de este conjunto.
void XC::MatrizPtrNod::fix(const SP_Constraint &semilla) const
  {
    if(Null()) return;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        operator()(j,k)->fix(semilla);
  }

//! @brief Devuelve una matriz con los identificadores de los nodos.
m_int XC::MatrizPtrNod::getTags(void) const
  {
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    m_int retval(nfilas,ncols,-1);
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          const Node *ptr= operator()(j,k);
          if(ptr)
            retval(j,k)= ptr->getTag();
        }
    return retval;
  }


//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::MatrizPtrNod::GetProp(const std::string &cod) const
  {
    if(cod=="size")
      {
        tmp_gp_szt= size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="numFilas")
      {
        tmp_gp_szt= getNumFilas();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="numCols")
      {
        tmp_gp_szt= getNumCols();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod == "nodo")
      {
        if(InterpreteRPN::Pila().size()>1)
          {
            const size_t &k= convert_to_size_t(InterpreteRPN::Pila().Pop());
            const size_t &j= convert_to_size_t(InterpreteRPN::Pila().Pop());
            const Node *ptr= (*this)(j,k);
            return any_const_ptr(ptr);
          }
        else
          {
            err_num_argumentos(std::cerr,2,"GetProp",cod);
            return any_const_ptr();
          }
      }
    else if(cod=="at")
      {
        size_t i= 1;
        size_t j= 1;
        static const Node *ptr= nullptr;
        if(InterpreteRPN::Pila().size()>1)
          {
            j= convert_to_size_t(InterpreteRPN::Pila().Pop());
            i= convert_to_size_t(InterpreteRPN::Pila().Pop());
            ptr= XC::MatrizPtrNod::operator()(i,j);
          }
        else if(InterpreteRPN::Pila().size()>0)
          {
            j= convert_to_size_t(InterpreteRPN::Pila().Pop());
            ptr= XC::MatrizPtrNod::operator()(i,j);
          }
        else
          {
            ptr= XC::MatrizPtrNod::operator()(i,j);
          }
        return any_const_ptr(ptr);
      }
    else if(cod=="getTagNearestNode")
      {
        const Pos3d p= popPos3d(cod);
        const Node *tmp= getNearestNode(p);
        tmp_gp_int= tmp->getTag();
        return any_const_ptr(tmp_gp_int);
      }
    else if(cod=="getTags")
      {
        tmp_gp_mint= getTags();
        return any_const_ptr(tmp_gp_mint);
      }
    else
      return EntCmd::GetProp(cod);
  }
