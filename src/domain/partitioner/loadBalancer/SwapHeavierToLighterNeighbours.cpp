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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:58 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/loadBalancer/SwapHeavierToLighterNeighbours.cpp,v $
                                                                        
                                                                        
 // File: ~/domain/loadBalancer/HeavierToSmallerNeighbours.C
// 
// Written: fmk 
// Created: Fri Aug 29 17:43:25 1997
// Revision: A
//
// Description: This file contains the class definition for 
// HeavierToSmallerNeighbours. A HeavierToSmallerNeighbours is an object 
// is used to partition a XC::PartitionedDomain.
//
// What: "@(#) HeavierToSmallerNeighbours.C, revA"

#include "SwapHeavierToLighterNeighbours.h"
#include "domain/partitioner/DomainPartitioner.h"
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/VertexIter.h>
#include <solution/graph/graph/Vertex.h>
#include <utility/matrix/ID.h> 


XC::SwapHeavierToLighterNeighbours::SwapHeavierToLighterNeighbours()
 :LoadBalancer() {}

XC::SwapHeavierToLighterNeighbours::SwapHeavierToLighterNeighbours(double fG, int rels)
 :LoadBalancer(fG,rels){}

int XC::SwapHeavierToLighterNeighbours::balance(Graph &theWeightedGraph)
  {
    // check to see a domain partitioner has been set
    DomainPartitioner *thePartitioner = this->getDomainPartitioner();
    if(!thePartitioner)
      {
        std::cerr << "SwapHeavierToLighterNeighbours::balance";
        std::cerr << "- No XC::DomainPartitioner has been set\n"; 
        return -1;
      }

    int res = 0;

    for(int ii=0; ii<numReleases; ii++)
      {
        VertexIter &theVertices = theWeightedGraph.getVertices();
        Vertex *vertexPtr;
        while((vertexPtr = theVertices()) != 0)
          {
            int vertexTag = vertexPtr->getTag();
            double vertexLoad = vertexPtr->getWeight();
            const std::set<int> &adjacency= vertexPtr->getAdjacency();
            for(std::set<int>::const_iterator j= adjacency.begin(); j!=adjacency.end(); j++)
              {
                const int otherVertexTag= *j;
                Vertex *otherVertexPtr= theWeightedGraph.getVertexPtr(otherVertexTag);
                double otherVertexLoad= otherVertexPtr->getWeight();
                
                if(vertexLoad > otherVertexLoad && otherVertexLoad != 0) 
                  if(vertexLoad/otherVertexLoad > factorGreater)
                    {
                      res = thePartitioner->swapBoundary(vertexTag,otherVertexTag);
                        if(res < 0)
                          {
                            std::cerr << "WARNING XC::SwapHeavierToLighterNeighbours";
                            std::cerr << "::balance - DomainPartitioner returned ";
                            std::cerr << res << std::endl;
                            return res;
                          }
                    }
                
                if(vertexLoad != 0 && otherVertexLoad == 0)
                  {
                    res= thePartitioner->swapBoundary(vertexTag,otherVertexTag); 
                    if(res < 0)
                      {
                        std::cerr << "WARNING XC::SwapHeavierToLighterNeighbours";
                        std::cerr << "::balance - DomainPartitioner returned ";
                        std::cerr << res << std::endl;
                        return res;
                      }
                  }
              }                
          }
      }
    return res;
  }
