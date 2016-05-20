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
//SetBase.cc

#include "SetBase.h"

#include <boost/any.hpp>
#include "domain/constraints/SP_Constraint.h"


#include "utility/matrix/ID.h"
#include "preprocessor/Preprocessor.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

//! @brief Constructor.
XC::SetBase::SetBase(const std::string &nmb,Preprocessor *md)
  : EntMdlrBase(nmb,md) {}

//! @brief Genera la malla a partir de los objetos del conjunto.
void XC::SetBase::Malla(dir_mallado dm)
  {
    std::cerr << "Método Malla() no implementado." << std::endl;
  }

//! @brief Impone desplazamiento nulo en los nodos de este conjunto.
void XC::SetBase::fix(const SP_Constraint &)
  {
    std::cerr << "Método fix() no implementado." << std::endl;
  }

//! @brief Devuelve los tags de los nodos en un vector de enteros.
const XC::ID &XC::SetBase::getIdNodeTags(void) const
  {
    static ID retval;
    const std::set<int> tmp= getNodeTags();
    if(!tmp.empty())
      {
        const size_t sz= tmp.size();
	retval.resize(sz);
        size_t conta= 0;
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++,conta++)
          retval[conta]= *i;
      }
    return retval;
  }

//! @brief Devuelve los tags de los elementos en un vector de enteros.
const XC::ID &XC::SetBase::getIdElementTags(void) const
  {
    static ID retval;
    const std::set<int> tmp= getElementTags();
    if(!tmp.empty())
      {
        const size_t sz= tmp.size();
	retval.resize(sz);
        size_t conta= 0;
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++,conta++)
          retval[conta]= *i;
      }
    return retval;
  }

//! @brief Devuelve verdadero si el punto pertenece al conjunto.
bool XC::SetBase::In(const Pnt *) const
  { return false; }

//! @brief Devuelve verdadero si el «edge» pertenece al conjunto.
bool XC::SetBase::In(const Edge *) const
  { return false; }

//! @brief Devuelve verdadero si la superficie pertenece al conjunto.
bool XC::SetBase::In(const Face *) const
  { return false; }

//! @brief Devuelve verdadero si el cuerpo pertenece al conjunto.
bool XC::SetBase::In(const Body *) const
  { return false; }

//! @brief Devuelve verdadero si la «uniform grid» pertenece al conjunto.
bool XC::SetBase::In(const UniformGrid *) const
  { return false; }

//! @brief Anula las magnitudes tributarias de los nodos conectados
//! a los elementos del conjunto.
void XC::SetBase::resetTributarias(void) const
  {
    const std::set<int> tmp= getElementTags();
    const Domain *dom= GetPreprocessor()->GetDominio();
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_elem= *i;
            const Element *elem= dom->getElement(tag_elem);
            if(elem)
              elem->resetTributarias();
            else
	      std::cerr << "SetBase::resetTributarias; "
                        << " no se encontró el elemento: "
                        << tag_elem << std::endl;
          }
      }
    else
      std::cerr << "No se ha definido el dominio." << std::endl;    
  }
 
//! @brief Calcula las longitudes tributarias correspondientes a cada
//! nodo de los elementos del conjunto.
void XC::SetBase::calculaLongsTributarias(bool initialGeometry) const
  {
    const std::set<int> tmp= getElementTags();
    const Domain *dom= GetPreprocessor()->GetDominio();
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_elem= *i;
            const Element *elem= dom->getElement(tag_elem);
            if(elem)
              elem->calculaLongsTributarias(initialGeometry);
            else
	      std::cerr << "SetBase::calculaLongsTributarias; "
                        << " no se encontró el elemento: "
                        << tag_elem << std::endl;
          }
      }
    else
      std::cerr << "No se ha definido el dominio." << std::endl;
  }

//! @brief Calcula las áreas tributarias correspondientes a cada
//! nodo de los elementos del conjunto.
void XC::SetBase::calculaAreasTributarias(bool initialGeometry) const
  {
    const std::set<int> tmp= getElementTags();
    const Domain *dom= GetPreprocessor()->GetDominio();
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_elem= *i;
            const Element *elem= dom->getElement(tag_elem);
            if(elem)
              elem->calculaAreasTributarias(initialGeometry);
            else
	      std::cerr << "SetBase::calculaLongsTributarias; "
                        << " no se encontró el elemento: "
                        << tag_elem << std::endl;
          }
      }
    else
      std::cerr << "No se ha definido el dominio." << std::endl;
  }

//! @brief Calcula los volúmenes tributarios correspondientes a cada
//! nodo de los elementos del conjunto.
void XC::SetBase::calculaVolsTributarios(bool initialGeometry) const
  {
    const std::set<int> tmp= getElementTags();
    const Domain *dom= GetPreprocessor()->GetDominio();
    if(dom)
      {
        for(std::set<int>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          {
            const int &tag_elem= *i;
            const Element *elem= dom->getElement(tag_elem);
            if(elem)
              elem->calculaVolsTributarios(initialGeometry);
            else
	      std::cerr << "SetBase::calculaLongsTributarias; "
                        << " no se encontró el elemento: "
                        << tag_elem << std::endl;
          }
      }
    else
      std::cerr << "No se ha definido el dominio." << std::endl;
  }


