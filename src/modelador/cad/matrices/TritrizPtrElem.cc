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
//TritrizPtrElem.cc

#include "TritrizPtrElem.h"
#include "domain/mesh/element/Element.h"
#include <boost/any.hpp>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"
#include "boost/lexical_cast.hpp"

//! @brief Constructor por defecto.
XC::TritrizPtrElem::TritrizPtrElem(const size_t capas,const MatrizPtrElem &m)
  : TritrizPtrBase<MatrizPtrElem>(capas,m) {}
//! @brief Constructor.
XC::TritrizPtrElem::TritrizPtrElem(const size_t capas,const size_t filas,const size_t cols)
  : TritrizPtrBase<MatrizPtrElem>(capas)
  {
    for(size_t i=0;i<capas;i++)
      (*this)[i]= MatrizPtrElem(filas,cols);
  }

//! @brief Lee un objeto TritrizPtrElem desde archivo
//! Soporta los comandos:
bool XC::TritrizPtrElem::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(TritrizPtrElem) Procesando comando: " << cmd << std::endl;

    if(cmd == "element")
      {
        const CmdParser &parser= status.Parser();
        if(parser.TieneIndices())
          {
            interpreta(parser.GetIndices());
            if(InterpreteRPN::HayArgumentos(3,cmd))
              {
                const size_t k= convert_to_size_t(InterpreteRPN::Pila().Pop()); 
                const size_t j= convert_to_size_t(InterpreteRPN::Pila().Pop()); 
                const size_t i= convert_to_size_t(InterpreteRPN::Pila().Pop()); 
                Element *ptr= (*this)(i,j,k);
                if(ptr)
                  ptr->LeeCmd(status);
                else
	          std::clog << "(TritrizPtrElem) Procesando comando: '" << cmd
                            << "' no se encontró el elemento de índices: (" << i << ',' << j << ',' << k << ").\n";
              }
          }
        return true;
      }
    else if(cmd=="elemento_con_tag")
      {
        std::deque<boost::any> fnc_args= status.Parser().SeparaArgs(this);
        if(fnc_args.size()<1)
          std::cerr << "puntos - uso: puntos(numPuntos) " << std::endl;
        const int tag= convert_to_int(fnc_args[0]); //Tag del nodo.
        Element *tmp= buscaElemento(tag);
        if(tmp)
          tmp->LeeCmd(status);
        else
          {
	    std::cerr << "(TritrizPtrElem) Procesando comando: " << cmd
                      << " no se encotró el elemento: " << tag
                      << " en este conjunto. Se ignora la entrada." << std::endl;
            status.GetBloque();
          }
        return true;
      }
    else
      return TritrizPtrBase<MatrizPtrElem>::procesa_comando(status);
  }



//! @brief Devuelve, si lo encuentra, un puntero al nodo
//! cuyo tag se pasa como parámetro.
XC::Element *XC::TritrizPtrElem::buscaElemento(const int &tag)
  {
    Element *retval= nullptr;
    const size_t ncapas= GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      {
        MatrizPtrElem &capa= operator()(i);
        retval= capa.buscaElemento(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Devuelve, si lo encuentra, un puntero al nodo
//! cuyo tag se pasa como parámetro.
const XC::Element *XC::TritrizPtrElem::buscaElemento(const int &tag) const
  {
    const Element *retval= nullptr;
    const size_t ncapas= GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      {
        const MatrizPtrElem &capa= operator()(i);
        retval= capa.buscaElemento(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Devuelve el nodo más próximo al punto que se pasa como parámetro.
XC::Element *XC::TritrizPtrElem::getNearestElement(const Pos3d &p)
  {
    Element *retval= nullptr, *ptrElem= nullptr;
    const size_t ncapas= GetCapas();
    double d= DBL_MAX;
    double tmp;
    if(ncapas>100)
      std::clog << "La «tritriz» de elementos es tiene "
                << ncapas << " capas "
                << " es mejor buscar por coordenadas en el conjunto asociado."
                << std::endl;
    for(size_t i=1;i<=ncapas;i++)
      {
        MatrizPtrElem &capa= operator()(i);
        ptrElem= capa.getNearestElement(p);
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
const XC::Element *XC::TritrizPtrElem::getNearestElement(const Pos3d &p) const
  {
    TritrizPtrElem *this_no_const= const_cast<TritrizPtrElem *>(this);
    return this_no_const->getNearestElement(p);
  }


//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::TritrizPtrElem::GetProp(const std::string &cod) const
  {
    if(cod=="size")
      {
        tmp_gp_szt= NumPtrs();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod == "elemento")
      {
        if(InterpreteRPN::Pila().size()>2)
          {
            const size_t &k= convert_to_size_t(InterpreteRPN::Pila().Pop());
            const size_t &j= convert_to_size_t(InterpreteRPN::Pila().Pop());
            const size_t &i= convert_to_size_t(InterpreteRPN::Pila().Pop());
            const Element *ptr= (*this)(i,j,k);
            return any_const_ptr(ptr);
          }
        else
          {
            err_num_argumentos(std::cerr,3,"GetProp",cod);
            return any_const_ptr();
          }
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
