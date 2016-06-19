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
//MeshEdges.cpp

#include "MeshEdges.h"
#include "xc_utils/src/geom/d1/Polilinea3d.h"
#include "domain/mesh/node/Node.h"
#include <set>

//! @brief Constructor.
XC::MeshEdges::MeshEdges(void)
  : EntCmd() {}

std::deque<const XC::MeshEdge *> XC::MeshEdges::getLoop(const MeshEdge *first) const
  {
    const Node *firstNode= first->getFirstNode();
    const Node *lastNode= first->getLastNode();
    std::set<const MeshEdge *> visited;
    visited.insert(first);
    std::deque<const MeshEdge *> retval;
    retval.push_back(first);
    const size_t maxSizeLoop= 2*size()-1; //Max. loop size.
    size_t count= 1;
    do
      {
        const MeshEdge *next= lastNode->next(*this,visited);
        if(next)
          {
            retval.push_back(next);
            visited.insert(next);
            first= next;
            lastNode= first->getLastNode();
          }
        else
          {
	    std::cerr << "MeshEdges::getLoop error; next edge not found." 
                      << std::endl;
            break;
          }
        count++;
        if(count>maxSizeLoop) //Something goes wrong.
          {
	    std::cerr << "MeshEdges::getLoop error; error in loop finding." 
                      << std::endl;
            break;
          }
      }
    while(firstNode!=lastNode);
    return retval;
  }

Polilinea3d getPolylineFromLoop(const std::deque<const XC::MeshEdge *> &loop,bool undeformedGeometry)
  {
    Polilinea3d retval;
    Pos3d pt;
    for(std::deque<const XC::MeshEdge *>::const_iterator i= loop.begin();i!=loop.end();i++)
      {
        if(undeformedGeometry)
          pt= (*i)->getFirstNode()->getPosInicial3d();
        else
          pt= (*i)->getFirstNode()->getPosFinal3d();
        retval.AgregaVertice(pt);
      }
    pt= retval.front(); //First point.
    retval.AgregaVertice(pt); //Polyline closed.
    return retval;
  } 

std::deque<Polilinea3d> XC::MeshEdges::getContours(bool undeformedGeometry) const
  {
    std::deque<Polilinea3d> retval;
    if(!empty())
      {
        const_iterator i= begin();
        const MeshEdge *first= &(*i);
        std::deque<const MeshEdge *> loop= getLoop(first);
        if(loop.size()!=size())
	  std::cerr << "MeshEdges::getContours error; contour with more than a loop not implemented."
                    << std::endl;
        retval.push_back(getPolylineFromLoop(loop,undeformedGeometry));
      }
    return retval;
  }

//! @brief Prints edges information.
void XC::MeshEdges::print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      os << *i << ' '; 
  }
