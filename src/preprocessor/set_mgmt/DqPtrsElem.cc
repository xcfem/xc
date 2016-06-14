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

#include "DqPtrsElem.h"
#include "domain/mesh/element/Element.h"
#include "preprocessor/cad/trf/TrfGeom.h"
#include "xc_utils/src/geom/d1/Polilinea3d.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/MeshEdge.h"

void XC::DqPtrsElem::crea_arbol(void)
  {
    kdtreeElements.clear();
    for(iterator i= begin();i!=end();i++)
      {
        Element *nPtr= *i;
        assert(nPtr);
        kdtreeElements.insert(*nPtr);
      }
  }

//! @brief Constructor.
XC::DqPtrsElem::DqPtrsElem(EntCmd *owr)
  : DqPtrs<Element>(owr) {}

//! @brief Constructor de copia.
XC::DqPtrsElem::DqPtrsElem(const DqPtrsElem &otro)
  : DqPtrs<Element>(otro)
  { crea_arbol(); }

//! @brief Constructor de copia.
XC::DqPtrsElem::DqPtrsElem(const std::deque<Element *> &ts)
  : DqPtrs<Element>(ts)
  { crea_arbol(); }

//! @brief Constructor de copia.
XC::DqPtrsElem::DqPtrsElem(const std::set<const Element *> &st)
  : DqPtrs<Element>()
  {
    std::set<const Element *>::const_iterator k;
    k= st.begin();
    for(;k!=st.end();k++)
      push_back(const_cast<Element *>(*k));
  }

//! @brief Operator asignación.
XC::DqPtrsElem &XC::DqPtrsElem::operator=(const DqPtrsElem &otro)
  {
    DqPtrs<Element>::operator=(otro);
    kdtreeElements= otro.kdtreeElements;
    return *this;
  }

//! @brief Agrega a ésta lista los elementos de la que se le pasa como parámetro.
void XC::DqPtrsElem::agrega(const DqPtrsElem &otro)
  {
    for(register const_iterator i= otro.begin();i!=otro.end();i++)
      push_back(*i);
  }

// //! @brief Agrega a ésta lista los elementos de la que se le pasa como parámetro,
// //! si cumplen la condición.
// void XC::DqPtrsElem::agrega_cond(const DqPtrsElem &otro,const std::string &cond)
//   {
//     bool result= false;
//     for(register const_iterator i= otro.begin();i!=otro.end();i++)
//       {
//         result= (*i)->interpretaBool(cond);
//         if(result)
// 	  push_back(*i);
//       }
//   }

//! @brief Vacía la lista de punteros y elimina las propiedades que pudiera tener el objeto.
void XC::DqPtrsElem::clearAll(void)
  {
    DqPtrs<Element>::clear();
    kdtreeElements.clear();
  }

bool XC::DqPtrsElem::push_back(Element *e)
  {
    bool retval= DqPtrs<Element>::push_back(e);
    if(retval)
      kdtreeElements.insert(*e);
    return retval;
  }

bool XC::DqPtrsElem::push_front(Element *e)
  {
    bool retval= DqPtrs<Element>::push_front(e);
    if(retval)
      kdtreeElements.insert(*e);
    return retval;
  }

//! @brief Devuelve el elemento más próximo al punto que se pasa como parámetro.
XC::Element *XC::DqPtrsElem::getNearestElement(const Pos3d &p)
  {
    Element *retval= const_cast<Element *>(kdtreeElements.getNearestElement(p));
    return retval;
  }

//! @brief Devuelve el elemento más próximo al punto que se pasa como parámetro.
const XC::Element *XC::DqPtrsElem::getNearestElement(const Pos3d &p) const
  {
    DqPtrsElem *this_no_const= const_cast<DqPtrsElem *>(this);
    return this_no_const->getNearestElement(p);
  }

//! @brief Devuelve, si lo encuentra, un puntero al elemento
//! cuyo tag se pasa como parámetro.
XC::Element *XC::DqPtrsElem::buscaElemento(const int &tag)
  {
    Element *retval= nullptr;
    Element *tmp= nullptr;
    for(iterator i= begin();i!=end();i++)
      {
        tmp= *i;
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

//! @brief Devuelve, si lo encuentra, un puntero al elemento
//! cuyo tag se pasa como parámetro.
const XC::Element *XC::DqPtrsElem::buscaElemento(const int &tag) const
  {
    const Element *retval= nullptr;
    const Element *tmp= nullptr;
    for(const_iterator i= begin();i!=end();i++)
      {
        tmp= *i;
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

//! @brief Devuelve el número de elementos del conjunto que están activos.
size_t XC::DqPtrsElem::getNumLiveElements(void) const
  {
    size_t retval= 0;
    const Element *tmp= nullptr;
    for(const_iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          if(tmp->isAlive()) retval++;
      }
    return retval;
  }

//! @brief Devuelve el número de elementos del conjunto que están inactivos.
size_t XC::DqPtrsElem::getNumDeadElements(void) const
  {
    size_t retval= 0;
    const Element *tmp= nullptr;
    for(const_iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          if(tmp->isDead()) retval++;
      }
    return retval;
  }

//!  @brief Asigna índices a los objetos de la lista para poder emplearlos en VTK.
void XC::DqPtrsElem::numera(void)
  {
    size_t idx= 0;
    for(iterator i= begin();i!=end();i++,idx++)
      {
	Element *ptr= *i;
        ptr->set_indice(idx);
      }
  }

//! @brief Desactiva los elementos.
void XC::DqPtrsElem::kill_elements(void)
  {
    Element *tmp= nullptr;
    for(iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          tmp->kill();
      }
  }

//! @brief Reactiva los elementos.
void XC::DqPtrsElem::alive_elements(void)
  {
    Element *tmp= nullptr;
    for(iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          tmp->alive();
      }
  }

//! @brief Calcula los esfuerzos en cada uno de los elementos.
void XC::DqPtrsElem::calc_resisting_force(void)
  {
    Element *tmp= nullptr;
    for(iterator i= begin();i!=end();i++)
      {
        tmp= *i;
        if(tmp)
          tmp->getResistingForce();
      }
  }

//! @brief Devuelve los tags de los elementos.
std::set<int> XC::DqPtrsElem::getTags(void) const
  {
    std::set<int> retval;
    for(const_iterator i= begin();i!=end();i++)
      retval.insert((*i)->getTag());
    return retval;
  }

//! @brief Returns the element set contour.
Polilinea3d XC::DqPtrsElem::getContour(void) const
  {
    typedef std::set<const Element *> ElementConstPtrSet;
    Polilinea3d retval;
    const Element *elem= nullptr;
    std::deque<MeshEdge> edgesContour;
    for(const_iterator i= begin();i!=end();i++)
      {
        elem= *i;
        const int numEdges= elem->getNumEdges();
        for(int j= 0;j<numEdges;j++)
          {
	    MeshEdge meshEdge(elem->getNodesEdge(j));
            ElementConstPtrSet elementsShared= meshEdge.getConnectedElements();
            if(elementsShared.size()==1) //border element.
              if(find(edgesContour.begin(), edgesContour.end(), meshEdge) == edgesContour.end())
                { edgesContour.push_back(meshEdge); }
            //XXX Continue here 2016.06.14 
          }          
      }    
    return retval;
  }
