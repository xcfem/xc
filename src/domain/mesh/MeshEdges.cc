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

std::deque<Polilinea3d> XC::MeshEdges::getContours(bool undeformedGeometry) const
  {
    std::deque<Polilinea3d> retval;
    if(!empty())
      {
        const size_t sz= size();
        const_iterator i= begin();
        const MeshEdge *first= &(*i); 
        const Pos3d pt;
        if(undeformedGeometry)
          first->getFirstNode()->getPosInicial3d();
        else
          first->getFirstNode()->getPosFinal3d();
        Polilinea3d pol;
        pol.AgregaVertice(pt);

        std::set<const MeshEdge *> visitados;
        visitados.insert(first);
        i++;
        do
          {
            const MeshEdge *next= first->next(*this);
	  }
        while(visitados.size()<sz);
      }
    return retval;
  }
