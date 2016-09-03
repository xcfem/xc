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
** file 'COPYRIGHT'  in main directory for information on usage and   **
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
// $Source: /usr/local/cvs/OpenSees/SRC/domain/loadBalancer/ShedHeaviest.cpp,v $
                                                                        
                                                                        
 // File: ~/domain/loadBalancer/ShedHeaviest.C
// 
// Written: fmk 
// Created: Fri Aug 29 17:43:25 1997
// Revision: A
//
// Description: This file contains the class definition for ShedHeaviest.
// A ShedHeaviest is an object used to partition a PartitionedDomain.
//
// What: "@(#) ShedHeaviest.C, revA"

#include "ShedHeaviest.h"
#include "domain/partitioner/DomainPartitioner.h"
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/VertexIter.h>
#include <solution/graph/graph/Vertex.h>
 
XC::ShedHeaviest::ShedHeaviest(void)
 : LoadBalancer() {}

XC::ShedHeaviest::ShedHeaviest(double fG, int rels, bool disallowDisconnected)
 :LoadBalancer(fG,rels,disallowDisconnected){}

int XC::ShedHeaviest::balance(Graph &theWeightedGraph)
  {
    // check to see a domain partitioner has been set
    DomainPartitioner *thePartitioner = this->getDomainPartitioner();
    if(!thePartitioner)
      {
	std::cerr << "ShedHeaviest::balance - No XC::DomainPartitioner has been set\n";
	return -1;
      }

    // determine the max loaded partition
    VertexIter &theVertices = theWeightedGraph.getVertices();
    Vertex *vertexPtr = theVertices();
    int maxPartition = vertexPtr->getTag();
    double maxLoad = vertexPtr->getWeight();
    while((vertexPtr = theVertices()) != 0)
      if(vertexPtr->getWeight() > maxLoad)
        {
	  maxLoad = vertexPtr->getWeight();
	  maxPartition = vertexPtr->getTag();
	}
    
    // release the boundary numReleases times
    int res = 0;
    for(int j=0; j<numReleases; j++)
      {
	res= thePartitioner->releaseBoundary(maxPartition,theWeightedGraph,true,factorGreater);  
	if(res < 0)
          {
	    std::cerr << "WARNING XC::ShedHeaviest::balance() ";
	    std::cerr << " - XC::DomainPartitioner::releaseBoundary returned ";
	    std::cerr << res << std::endl;
	    j = numReleases;
	  }
      }
    return res;
  }
