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

#include "MeshEdge.h"
#include "domain/mesh/node/Node.h"
#include <iostream>

//! @brief Constructor.
XC::MeshEdge::MeshEdge(void)
  :nodes() {}

//! @brief Constructor.
XC::MeshEdge::MeshEdge(const NodesEdge &ns)
  :nodes(ns) {}

//! @brief Returns the edge nodes. 
XC::MeshEdge::NodesEdge XC::MeshEdge::getNodes(void) const
  { return nodes; }

//! @brief Returns the first node from the edge:
//      x-----+-------+--------x
//      ^
//      |
const XC::Node *XC::MeshEdge::getFirstNode(void) const
  { return nodes.front(); }

//! @brief Returns the last node from the edge:
//      x-----+-------+--------x
//                             ^
//                             |
const XC::Node *XC::MeshEdge::getLastNode(void) const
  { return nodes.back(); }

//! @brief Returns true if the edge is connected to the node.
bool XC::MeshEdge::isConnected(const Node &n) const
  {
    bool retval= false;
    if((getFirstNode()== &n) || (getLastNode()== &n))
      retval= true;
    return retval;
  }

//! @brief Returns true if the edge is connected to the edge being passed as parameter.
bool XC::MeshEdge::isConnected(const MeshEdge &edge) const
  {
    bool retval= false;
    if(isConnected(*edge.getFirstNode()) || isConnected(*edge.getLastNode()))
      retval= true;
    return retval;
  }

//! @brief Returns a deque<Element *> with the elements that share the edge.
XC::MeshEdge::ElementConstPtrSet XC::MeshEdge::getConnectedElements(void) const
  {
    ElementConstPtrSet retval;
    ElementConstPtrSet elementsNode0= getFirstNode()->getConnectedElements();
    ElementConstPtrSet elementsNode1= getLastNode()->getConnectedElements();
    set_intersection(elementsNode0.begin(),elementsNode0.end(),elementsNode1.begin(),elementsNode1.end(),std::inserter(retval,retval.begin()));
    return retval;
  }

//! @brief Prints edge information.
void XC::MeshEdge::print(std::ostream &os) const
  {
    os << "mesh edge; from node: " << getFirstNode()->getTag() 
       << " to node: " << getLastNode()->getTag() << std::endl;
  }

//! @brief Equal operator
bool XC::MeshEdge::operator==(const MeshEdge &otro) const
  {
    bool retval= true;
    const size_t sz= size();
    if(sz!= otro.size())
      retval= false;
    else
      for(size_t i= 0;i<sz;i++)
        if(nodes[i]!=otro.nodes[i])
          {
            retval= false;
            break;
          }
    return retval; 
  }
