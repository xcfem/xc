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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/loadBalancer/LoadBalancer.h,v $
                                                                        
                                                                        
// File: ~/domain/loadBalancer/LoadBalancer.h
// 
// Written: fmk 
// Created: Fri Aug 29 17:43:25 1997
// Revision: A
//
// Description: This file contains the class definition for LoadBalancer.
// A LoadBalancer is an object used to partition a PartitionedDomain.
//
// What: "@(#) LoadBalancer.h, revA"

#ifndef LoadBalancer_h
#define LoadBalancer_h

namespace XC {
class Vector;
class DomainPartitioner;
class Graph;

class LoadBalancer
  {
  private:
    DomainPartitioner *theDomainPartitioner;    
  protected:
    int numReleases;
    double factorGreater;
    bool disallowDisconnectedGraphs;

    DomainPartitioner *getDomainPartitioner(void);
  public:
    LoadBalancer(void);
    LoadBalancer(double factGreater, int releases, bool disallowDisconnected= true);

    virtual void setLinks(DomainPartitioner &thePartitioner);
    virtual int balance(Graph &theWeightedGraph) =0;
  };
} // fin namespace XC

#endif


