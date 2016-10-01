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

//! @brief returns the loop that starts in vertex passed as parameter.
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


//! @brief returns closed contours from de edge set.
std::deque<Polilinea3d> XC::MeshEdges::getContours(bool undeformedGeometry) const
  { return XC::getContours(*this,undeformedGeometry); }

//! @brief returns closed contours from de edge set.
std::deque<Polilinea3d> XC::getContours(MeshEdges edges, bool undeformedGeometry)
  {
    const size_t max_iter= 100;
    size_t iter= 0;
    std::deque<Polilinea3d> retval;
    do
      {
	MeshEdges::const_iterator i= edges.begin();
        const MeshEdge *first= &(*i);
        std::deque<const MeshEdge *> loop= edges.getLoop(first);
        retval.push_back(getPolylineFromLoop(loop,undeformedGeometry));
	edges= edges.getEdgesNotInLoop(loop);
	iter++;
	if(iter>max_iter)
	  {
	    std::cerr << "getContours error; " << edges.size()
	              << " left, after " << iter << " iterations." << std::endl;
	    break;
	  }
      }
    while(!edges.empty());
    return retval;
  }

//! @brief Returns the edges that result from removing the edges from the loop passed as parameter.
XC::MeshEdges XC::MeshEdges::getEdgesNotInLoop(const std::deque<const MeshEdge *> &loop) const
  {
    MeshEdges retval;
    for(MeshEdges::const_iterator i= begin();i!=end();i++)
      {
	const MeshEdge *edgePtr= &(*i);
        std::deque<const MeshEdge *>::const_iterator j= find(loop.begin(),loop.end(),edgePtr);
	if(j==loop.end()) //Not found.
	  retval.push_back(*edgePtr);
      }
    return retval;
  }

//! @brief Prints edges information.
void XC::MeshEdges::print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      os << *i << ' '; 
  }
