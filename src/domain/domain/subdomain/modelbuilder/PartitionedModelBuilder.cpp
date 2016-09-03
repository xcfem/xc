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
// $Date: 2003/02/14 23:01:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/modelbuilder/PartitionedModelBuilder.cpp,v $
                                                                        
                                                                        
// File: ~/model/PartitionedModelBuilder.C
//
// Written: fmk 
// Created: 03/98
// Revision: A
//

#include "PartitionedModelBuilder.h"
#include "domain/domain/partitioned/PartitionedDomain.h"
#include "domain/domain/subdomain/Subdomain.h"
#include "domain/domain/SubdomainIter.h"

//  PartitionedModelBuilderModel(Domain &theDomain, int theClassTag);
//	constructor
XC::PartitionedModelBuilder::PartitionedModelBuilder(PartitionedDomain &aPartitionedDomain,
						 int theClassTag)
:ModelBuilder(aPartitionedDomain), MovableObject(theClassTag),
thePartitionedDomain(&aPartitionedDomain)
{

}

XC::PartitionedModelBuilder::PartitionedModelBuilder(Subdomain &aSubdomain,
						 int theClassTag)
:ModelBuilder(aSubdomain), MovableObject(theClassTag),
thePartitionedDomain(0)
{

}

int XC::PartitionedModelBuilder::buildFE_Model(void)
{
  int result;

  if (thePartitionedDomain == 0) {
    std::cerr << "XC::PartitionedModelBuilder::buildFE_Model(void) -";
    std::cerr << "No XC::PartitionedDomain associated with this object\n";
    return -1;
  }

  // we build the interface, i.e. nodes on boundaries and any constraints and loads
  int numSubdomains = thePartitionedDomain->getNumSubdomains();
  result = this->buildInterface(numSubdomains);
  if (result != 0) {
    std::cerr << "XC::PartitionedModelBuilder::buildFE_Model(void) -";
    std::cerr << "buildInterface failed\n";
    return result;
  }

  // now build the subdomains, stopping if an error in building any subdomain
  SubdomainIter &theSubs = thePartitionedDomain->getSubdomains();
  Subdomain *theSubdomain;
  while ((theSubdomain = theSubs()) != 0) {
    result = theSubdomain->buildSubdomain(numSubdomains, *this);
    if (result != 0) {
	std::cerr << "XC::PartitionedModelBuilder::buildFE_Model(void) -";
	std::cerr << "buildSubdomain failed for XC::Subdomain " << theSubdomain->getTag();
	std::cerr << std::endl;
	return result;
    }

  }

  // if got here a PartitiondDomain has been populated
  return 0;
}

 XC::PartitionedDomain *
XC::PartitionedModelBuilder::getPartitionedDomainPtr(void) const 
{
    return thePartitionedDomain;
}
    

