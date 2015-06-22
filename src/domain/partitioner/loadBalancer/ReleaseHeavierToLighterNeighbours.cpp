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
// $Source: /usr/local/cvs/OpenSees/SRC/domain/loadBalancer/ReleaseHeavierToLighterNeighbours.cpp,v $
                                                                        
                                                                        
 // File: ~/domain/loadBalancer/ReleaseHeavierToLighterNeighbours.C
// 
// Written: fmk 
// Created: Fri Aug 29 17:43:25 1997
// Revision: A
//
// Description: This file contains the class definition for XC::ReleaseHeavierToLighterNeighbours.
// A XC::ReleaseHeavierToLighterNeighbours is an object used to partition a XC::PartitionedDomain.
//
// What: "@(#) ReleaseHeavierToLighterNeighbours.C, revA"

#include "ReleaseHeavierToLighterNeighbours.h"
#include "domain/partitioner/DomainPartitioner.h"
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/VertexIter.h>
#include <solution/graph/graph/Vertex.h>
 
XC::ReleaseHeavierToLighterNeighbours::ReleaseHeavierToLighterNeighbours(void)
  : LoadBalancer() {}

XC::ReleaseHeavierToLighterNeighbours::ReleaseHeavierToLighterNeighbours(double fG, int rels, bool disallowDisconnected)
  : LoadBalancer(fG,rels,disallowDisconnected){}


int XC::ReleaseHeavierToLighterNeighbours::balance(Graph &theWeightedGraph)
  {
    // check to see a domain partitioner has been set
    DomainPartitioner *thePartitioner = getDomainPartitioner();    
    if(!thePartitioner)
      {
	std::cerr << "ReleaseHeavierToLighterNeighbours::balance ";
	std::cerr << "- No domainPartitioner has been set\n";
	return -1;
      }
    else
      {
        int res = 0;
        int numPartitions = theWeightedGraph.getNumVertex();
        for(int i=1; i<=numPartitions; i++)
          {
	    res = thePartitioner->releaseBoundary(i,theWeightedGraph,true,factorGreater);  
            if(res<0)
              {
                std::cerr << "WARNING XC::ReleaseHeavierToLighterNeighbours";
	        std::cerr << "::balance - DomainPartitioner returned ";
	        std::cerr << res << std::endl;
	        return res;    
	      }
          }
        return res;
      }
  }
