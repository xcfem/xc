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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:18 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/domain/single/SingleDomAllSP_Iter.cpp,v $
                                                                        
                                                                        
// File: ~/OOP/domain/domain/SingleDomAllSP_Iter.C
//
// Written: fmk 
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Description: This file contains the method definitions for class 
// SingleDomAllSP_Iter. SingleDomAllSP_Iter is a class for iterating through the 
// elements of a domain. 

#include "SingleDomAllSP_Iter.h"

#include <domain/domain/Domain.h>
#include <domain/load/pattern/LoadPattern.h>
#include <domain/load/pattern/LoadPatternIter.h>
#include <domain/load/pattern/NodeLocker.h>
#include <domain/load/pattern/NodeLockerIter.h>
#include <domain/constraints/SP_Constraint.h>
#include <utility/tagged/storage/TaggedObjectIter.h>
#include <utility/tagged/storage/TaggedObjectStorage.h>

// SingleDomAllSP_Iter(SingleDomAllain &theDomain):
//	constructor that takes the model, just the basic iter

XC::SingleDomAllSP_Iter::SingleDomAllSP_Iter(Domain &domain)
  :theDomain(&domain), doneDomainSPs(false) {}


void XC::SingleDomAllSP_Iter::reset(void)
  {
    theDomainSPs = &(theDomain->getCondsContorno().getSPs());

    theLoadPatterns= &(theDomain->getCondsContorno().getLoadPatterns());
    currentLoadPattern= theLoadPatterns->begin();
    if(currentLoadPattern!=theLoadPatterns->end())
      { theLoadPatternSPs= &(currentLoadPattern->second->getSPs()); }

    theNodeLockers= &(theDomain->getCondsContorno().getNodeLockers());
    currentNodeLocker= theNodeLockers->begin();
    if(currentNodeLocker!=theNodeLockers->end())
      { theNodeLockerSPs = &(currentNodeLocker->second->getSPs()); }
    doneDomainSPs = false;
  }


XC::SP_Constraint *XC::SingleDomAllSP_Iter::operator()(void)
  {
    SP_Constraint *theRes = 0;

    if(doneDomainSPs == false)
      {
        theRes= (*theDomainSPs)();
        if(theRes)
          return theRes;
        else
          doneDomainSPs = true;
      }

    while(currentLoadPattern!=theLoadPatterns->end())
      {
        theRes= (*theLoadPatternSPs)();
        if(!theRes)
          {
            currentLoadPattern++;
            if(currentLoadPattern!=theLoadPatterns->end())
              theLoadPatternSPs= &(currentLoadPattern->second->getSPs());
          }
        else
	  return theRes;
      }

    while(currentNodeLocker!=theNodeLockers->end())
      {
        theRes= (*theNodeLockerSPs)();
        if(!theRes)
          {
            currentNodeLocker++;
            if(currentNodeLocker!=theNodeLockers->end())
              theNodeLockerSPs = &(currentNodeLocker->second->getSPs());
          }
        else
          return theRes;
      }
    return 0;
  }
    
    


    
    
