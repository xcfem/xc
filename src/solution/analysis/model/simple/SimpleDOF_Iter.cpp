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
// $Date: 2000/09/15 08:23:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/simple/SimpleDOF_Iter.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/model/simple/SimpleDOF_Iter.C
//
// Written: fmk 
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Description: This file contains the method definitions for class 
// SimpleDOF_Iter. SimpleDOF_Iter is a class for iterating through the 
// DOF_Groups of an simple analysis mdoel. 

#include "solution/analysis/model/simple/SimpleDOF_Iter.h"
#include "solution/analysis/model/AnalysisModel.h"


// SimpleDOF_Iter(AnalysisModel *theModel):
//	constructor that takes the model, just the basic iter

XC::SimpleDOF_Iter::SimpleDOF_Iter(AnalysisModel &theModel)
  :myModel(theModel), currIndex(0), numDone(0) {}


void XC::SimpleDOF_Iter::reset(void)
  {
    currIndex = 0;
    numDone = 0;
  }


XC::DOF_Group *XC::SimpleDOF_Iter::operator()(void)
  {
    // check if we still have elements in the model
    // if not return 0, indicating we are done

    if(numDone >= myModel.getNumDOF_Groups())
      return 0;

    // search through domains ele list till we find the next element
    while((currIndex < myModel.sizeDOF) && (myModel.getDOF_GroupPtr(currIndex) == 0)) //&& (myModel.theDOFs[currIndex] == 0)) LCPT
      currIndex++;

    // if not at the end of the list return the element
    if(currIndex < myModel.sizeDOF)
      {
        DOF_Group *result = myModel.getDOF_GroupPtr(currIndex); //theDOFs[currIndex]; LCPT
        numDone++; currIndex++;
        return(result);
      }
    return (0);
  }

    
    
