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
//MatrizPtrElem.cc

#include "MatrizPtrElem.h"
#include "domain/mesh/element/Element.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"
#include "boost/lexical_cast.hpp"

//! @brief Lee un objeto MatrizPtrElem desde archivo
bool XC::MatrizPtrElem::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(MatrizPtrElem) Procesando comando: " << cmd << std::endl;

    if(cmd == "elemento")
      {
        const CmdParser &parser= status.Parser();
        if(parser.TieneIndices())
          {
            interpreta(parser.GetIndices());
            if(InterpreteRPN::HayArgumentos(2,cmd))
              {
                const size_t k= convert_to_size_t(InterpreteRPN::Pila().Pop()); 
                const size_t j= convert_to_size_t(InterpreteRPN::Pila().Pop()); 
                Element *ptr= (*this)(j,k);
                if(ptr)
                  ptr->LeeCmd(status);
                else
	          std::clog << "(MatrizPtrNod) Procesando comando: '" << cmd
                            << "'; no se encontró el elemento de índices: (" << j << ',' << k << ").\n";
              }
          }
        else
	  std::cerr << "(MatrizPtrNod) Procesando comando: '" << cmd
                            << "'; faltan los índices del nodo.\n";
        return true;
      }
    else
      return MatrizPtrBase<Element>::procesa_comando(status);
  }

//! @brief Devuelve, si lo encuentra, un puntero al nodo
//! cuyo tag se pasa como parámetro.
XC::Element *XC::MatrizPtrElem::buscaElemento(const int &tag)
  {
    Element *retval= nullptr;
    Element *tmp= nullptr;
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
const XC::Element *XC::MatrizPtrElem::buscaElemento(const int &tag) const
  {
    const Element *retval= nullptr;
    const Element *tmp= nullptr;
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
XC::Element *XC::MatrizPtrElem::getNearestElement(const Pos3d &p)
  {
    Element *retval= nullptr, *ptrElem= nullptr;
    double d= DBL_MAX;
    double tmp;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    if(nfilas*ncols>500)
      std::clog << "La matriz de elementos es tiene "
                << nfilas*ncols << " componentes "
                << " es mejor buscar por coordenadas en el conjunto asociado."
                << std::endl;
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          ptrElem= operator()(j,k);
          tmp= ptrElem->getDist2(p);
          if(tmp<d)
            {
              d= tmp;
              retval= ptrElem;
            }
        }
    return retval;
  }

//! @brief Devuelve el nodo más próximo al punto que se pasa como parámetro.
const XC::Element *XC::MatrizPtrElem::getNearestElement(const Pos3d &p) const
  {
    MatrizPtrElem *this_no_const= const_cast<MatrizPtrElem *>(this);
    return this_no_const->getNearestElement(p);
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::MatrizPtrElem::GetProp(const std::string &cod) const
  {
    if(cod=="size")
      {
        tmp_gp_szt= size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="at")
      {
        size_t i= 1;
        size_t j= 1;
        static const Element *ptr= nullptr;
        if(InterpreteRPN::Pila().size()>1)
          {
            j= convert_to_size_t(InterpreteRPN::Pila().Pop());
            i= convert_to_size_t(InterpreteRPN::Pila().Pop());
            ptr= XC::MatrizPtrElem::operator()(i,j);
          }
        else if(InterpreteRPN::Pila().size()>0)
          {
            j= convert_to_size_t(InterpreteRPN::Pila().Pop());
            ptr= XC::MatrizPtrElem::operator()(i,j);
          }
        else
          {
            ptr= XC::MatrizPtrElem::operator()(i,j);
          }
        return any_const_ptr(ptr);
      }
    else if(cod=="getTagNearestElement")
      {
        const Pos3d p= popPos3d(cod);
        const Element *tmp= getNearestElement(p);
        tmp_gp_int= tmp->getTag();
        return any_const_ptr(tmp_gp_int);
      }
    else
      return EntCmd::GetProp(cod);
  }

