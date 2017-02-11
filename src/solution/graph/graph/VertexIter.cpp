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
// $Date: 2000/09/15 08:23:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/graph/VertexIter.cpp,v $
                                                                        
                                                                        
// File: ~/OOP/graph/graph/VertexIter.C
//
// Written: fmk 
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Description: This file contains the method definitions for class 
// VertexIter. VertexIter is a class for iterating through the 
// vertices of a graph.

#include "solution/graph/graph/VertexIter.h"
#include "Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <utility/tagged/storage/TaggedObjectIter.h>
#include <utility/tagged/storage/TaggedObjectStorage.h>

//! @brief Default constructor.
XC::VertexIter::VertexIter(void)
  :myIter(nullptr) {}

// VertexIter():
// constructor that takes the model, just the basic iter
XC::VertexIter::VertexIter(TaggedObjectStorage *theStorage)
  :myIter(&theStorage->getComponents()) {}


void XC::VertexIter::reset(void)
  {
    assert(myIter);
    myIter->reset();
  }

XC::Vertex *XC::VertexIter::operator()(void)
  {
    assert(myIter);
    Vertex *retval= nullptr;
    // check if we still have elements in the model
    // if not return 0, indicating we are done
    TaggedObject *theComponent= (*myIter)();
    if(theComponent)
      retval= dynamic_cast<Vertex *>(theComponent);
    return retval;
  }




