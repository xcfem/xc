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

#include "NodeTopology.h"
#include "Node.h"
#include "domain/mesh/element/ElementEdges.h"
#include "domain/mesh/element/Element.h"

//! @brief Devuelve los nodos conectados simultáneamente a ambos nodos.
std::set<XC::Element *> XC::getElementosEntreNodos(Node &n1, Node &n2)
  {
    const std::set<Element *> set1= n1.getElementosConectados();
    const std::set<Element *> set2= n2.getElementosConectados();
    std::set<Element *> retval;
    //std::set<const Element *>::const_iterator iter= set_intersection(set1.begin(),set1.end(),set2.begin(),set2.end(),retval.begin());
    for(std::set<Element *>::const_iterator i= set1.begin();i!=set1.end();i++)
      {
        std::set<Element *>::const_iterator j= set2.find(*i);
        if(*i == *j)
          retval.insert(*i);
      }
    return retval;
  }

//! @brief Devuelve los bordes de elemento que tienen por extremos
//! a ambos nodos.
XC::ElementEdges XC::getElementEdgesEntreNodos(Node *n1, Node *n2)
  {
    ElementEdges retval;
    if(n1 && n2)
      {
        const std::set<Element *> elems= getElementosEntreNodos(*n1,*n2);
        for(std::set<Element *>::const_iterator i= elems.begin();i!=elems.end();i++)
          {
            const int edge= (*i)->getEdgeNodes(n1,n2);
            if(edge>=0)
              retval.push_back(ElementEdge(*i,edge));
          }
      }
    return retval;
  }

//! @brief Devuelve los elementos conectados a alguno de los nodos del conjunto.
std::set<XC::Element *> XC::getElementosConectados(const NodePtrSet &nodos)
  {
    std::set<Element *> retval;
    for(NodePtrSet::const_iterator i= nodos.begin();i!=nodos.end();i++)
      {
        std::set<Element *> tmp= (*i)->getElementosConectados();
        retval.insert(tmp.begin(),tmp.end());
      }
    return retval;
  }
//! @brief Devuelve los bordes de elemento que tienen ambos extremos
//! en el conjunto de nodos que se pasa como parámetro.
XC::ElementEdges XC::getElementEdgesEntreNodos(const NodePtrSet &nodos)
  {
    ElementEdges retval;
    std::set<Element *> elems= getElementosConectados(nodos);
    for(std::set<Element *>::const_iterator i= elems.begin();i!=elems.end();i++)
      {
        Element *elem_ptr= *i;
        const std::set<int> idEdges= elem_ptr->getEdgesNodes(nodos);
        for(std::set<int>::const_iterator j= idEdges.begin();j!=idEdges.end();j++)
          retval.push_back(ElementEdge(elem_ptr,*j));
      }
    return retval;
  }

//! @brief Devuelve verdadero si los punteros a nodo del vector
//! están contenidos en el conjunto que se pasa como parámetro.
bool XC::in(const NodePtrSet &nodos,const std::vector<const Node *> &nodos_edge)
  {
    bool retval= true;
    for(std::vector<const Node *>::const_iterator i= nodos_edge.begin();i!=nodos_edge.end();i++)
      {
        if(find(nodos.begin(),nodos.end(),*i)==nodos.end())
          {
            retval= false;
            break;
          }
      }
    return retval;
  }
