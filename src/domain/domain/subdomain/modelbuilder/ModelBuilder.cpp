//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
// $Date: 2000/09/15 08:23:23 $
// $Source: /usr/local/cvs/OpenSees/SRC/modelbuilder/ModelBuilder.cpp,v $
                                                                        
                                                                        
// File: ~/model/ModelBuilder.C
//
// Written: fmk 
// Created: Mon Sept 15 14:47:47: 1996
// Revision: A
//
// Description: This file contains the class definition for XC::ModelBuilder
// ModelBuilder is a class used to model a structure with a plane frame. 
// The object creates the components of the model and adds these to the
// Domain with which it is associated.



#include "ModelBuilder.h"
#include "domain/domain/Domain.h"

//! @brief Constructor.
//!
//! All models are associated with a single domain, this constructor
//! sets up the link between the model and the domain, setting its link
//! to the Domain object \p theDomain.
XC::ModelBuilder::ModelBuilder(Domain &theDomain)
  :myDomain(&theDomain) {}

//! @brief Return a pointer to the domain.
//!
//! Returns a pointer to the Domain object passed in the constructor. This
//! method can be used in the subclasses to get a pointer the Domain object
//! to which to add the domain components.
XC::Domain *XC::ModelBuilder::getDomainPtr(void) const 
  { return myDomain; }
    

