//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
// DOF_Groups of an simple analysis model. 

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

    
    
