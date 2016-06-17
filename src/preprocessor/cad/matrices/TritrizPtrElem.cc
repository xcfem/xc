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


#include "xc_utils/src/geom/pos_vec/Pos3d.h"

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

//! @brief Devuelve el nodo más próximo al punto being passed as parameter.
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

//! @brief Devuelve el nodo más próximo al punto being passed as parameter.
const XC::Element *XC::TritrizPtrElem::getNearestElement(const Pos3d &p) const
  {
    TritrizPtrElem *this_no_const= const_cast<TritrizPtrElem *>(this);
    return this_no_const->getNearestElement(p);
  }
