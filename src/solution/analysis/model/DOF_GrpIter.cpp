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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/11/28 22:07:24 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/DOF_GrpIter.cpp,v $
                                                                        
// Written: fmk 
// Created: 10/05
// Revision: A
//

#include "solution/analysis/model/DOF_GrpIter.h"

#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <utility/tagged/storage/TaggedObjectIter.h>
#include <utility/tagged/storage/TaggedObjectStorage.h>


// DOF_GrpIter(SingleDomain &theDomain):
//	constructor that takes the model, just the basic iter

XC::DOF_GrpIter::DOF_GrpIter(TaggedObjectStorage *theStorage)
  :myIter(&(theStorage->getComponents())) {}


void XC::DOF_GrpIter::reset(void)
  {
    myIter->reset();
  }    


XC::DOF_Group *XC::DOF_GrpIter::operator()(void)
  {
    // check if we still have elements in the model
    // if not return 0, indicating we are done
    TaggedObject *theComponent = (*myIter)();
    if(theComponent == 0)
      return 0;
    else
      {
	DOF_Group *result = (DOF_Group *)theComponent;
	return result;
      }
  }

    
    
