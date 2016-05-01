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
//ElementEdges.cpp

#include "ElementEdges.h"
#include "domain/mesh/element/Element.h"
#include "domain/domain/Domain.h"


#include "xc_basic/src/matrices/m_int.h"
#include "utility/matrix/ID.h"


//! @brief Constructor.
XC::ElementEdges::ElementEdges(void)
  : theEdges(0) {}

//! @brief Asigna los punteros a partir de los identificadores de elemento.
void XC::ElementEdges::setPtrs(Domain *theDomain, const ID &theElementTags, const ID &theEdgesIndex)
  {
    size_t sz= theElementTags.Size();
    theEdges.clear();
    if(theDomain)
      {
        theEdges.resize(sz);
        for(size_t i=0; i<sz; i++)
          {
            Element *tmp= theDomain->getElement(theElementTags(i));
            
            if(tmp)
              theEdges[i]= ElementEdge(tmp,theEdgesIndex(i));
            else
              {
                std::cerr << "WARNING - XC::ElementEdges::setDomain - ele with tag "
	                  << theElementTags(i) << " does not exist in the domain\n";
              }
          }
      }
  }

//! @brief Devuelve un iterador al elemento cuyo tag se pasa como parámetro.
XC::ElementEdges::iterator XC::ElementEdges::find(const int &tag)
  {
    iterator retval= end();
    for(iterator i= begin();i!=end();i++)
      if((*i).getElementPtr()->getTag() == tag)
        { retval= i; break; }
    return retval;
  }

//! @brief Devuelve un iterador al elemento cuyo tag se pasa como parámetro.
XC::ElementEdges::const_iterator XC::ElementEdges::find(const int &tag) const
  {
    const_iterator retval= end();
    for(const_iterator i= begin();i!=end();i++)
      if((*i).getElementPtr()->getTag() == tag)
        { retval= i; break; }
    return retval;
  }


//! @brief Devuelve el índice el elemento en el vector,
//! si no lo encuentra devuelve -1.
//XXX: Funciona mal si el elemento aparece más de una vez en el vector.
int XC::ElementEdges::getLocElement(const Element *ptr) const
  {
    int retval= -1;
    const size_t sz= theEdges.size();
    for(size_t i= 0;i<sz;i++)
      if(theEdges[i].getElementPtr() == ptr)
        { retval= i; break; }
    return retval;
  }

//! @brief Elimina el elemento cuyo tag se pasa como parámetro.
size_t XC::ElementEdges::removeElement(const int &tag) 
  {
    iterator i= find(tag);
    if(i!=end())
      theEdges.erase(i,i);
    return size();
  }



XC::ElementEdges::const_reference XC::ElementEdges::operator()(const size_t &i) const
  { return theEdges[i]; }
XC::ElementEdges::reference XC::ElementEdges::operator()(const size_t &i)
  { return theEdges[i]; }
XC::ElementEdges::const_reference XC::ElementEdges::operator[](const size_t &i) const
  { return theEdges[i]; }
XC::ElementEdges::reference XC::ElementEdges::operator[](const size_t &i)
  { return theEdges[i]; }
