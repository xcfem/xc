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
// $Date: 2000/09/15 08:23:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/simple/SimpleFE_Iter.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/model/simple/SimpleFE_Iter.C
//
// Written: fmk 
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Description: This file contains the method definitions for class 
// SimpleFE_Iter. SimpleFE_Iter is a class for iterating through the 
// elements of an analysis mdoel. 


#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/simple/SimpleFE_Iter.h>


// SimpleFE_Iter(AnalysisModel &theModel):
//	constructor that takes the model, just the basic iter

XC::SimpleFE_Iter::SimpleFE_Iter(AnalysisModel &theModel)
  :myModel(theModel), currIndex(0), numDone(0)
{
}


void XC::SimpleFE_Iter::reset(void)
{
    currIndex = 0;
    numDone = 0;
}

 XC::FE_Element *
XC::SimpleFE_Iter::operator()(void)
{
  // check if we still have elements in the model
  // if not return 0, indicating we are done
  if (numDone >= myModel.numFE_Ele)
    return 0;

  // search through domains ele list till we find the next element
  while ((currIndex < myModel.sizeEle) 
	 && (myModel.theFEs[currIndex] == 0))
      currIndex++;

  // if not at the end of the list return the element
  if (currIndex < myModel.sizeEle) {
      FE_Element *result = myModel.theFEs[currIndex];
      numDone++; currIndex++;
      return(result);
  }
  return (0);
}

    
    
