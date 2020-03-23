// -*-c++-*-
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
// $Date: 2000/09/15 08:23:23 $
// $Source: /usr/local/cvs/OpenSees/SRC/modelbuilder/PartitionedModelBuilder.h,v $
                                                                        
                                                                        
// File: ~/modelbuilder/PartitionedModelBuilder.h
// 
// Written: fmk 
// Created: 03/98
// Revision: A
//
// Description: This file contains the class definition for 
// PartitionedModelBuilder. PartitionedModelBuilder is an abstract class,
// A PartitionedModelBuilder creates the discritization of the
// structure, placing the components into the appropriate subdomains
// within the PartitionedDomain.
//
// What: "@(#) PartitionedModelBuilder.h, revA"

#ifndef PartitionedModelBuilder_h
#define PartitionedModelBuilder_h

#include "ModelBuilder.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {
class PartitionedDomain;
class Subdomain;


//! The PartitionedModelBuilder class is an abstract class. A subclass
//! of PartitionedModelBuilder is a class which creates a partitioned finite
//! element discretization of a structure: that is it discretizes the structure
//! to be modeled into Elements, Nodes, Constraints, etc. and adds these
//! components to the Subdomains of a PartitionedDomain.
//! PartitionedModelBuilders can be used for creating models for analysis
//! involving domain decomposition methods where there exist natural
//! partitions or where a model has previously been partitioned and this
//! information has been saved.
class PartitionedModelBuilder: public ModelBuilder, public MovableObject
  {
  public:
    PartitionedModelBuilder(PartitionedDomain &aPartitionedDomain, int classTag);
    PartitionedModelBuilder(Subdomain &aSubdomain, int classTag);

    virtual int buildFE_Model(void);
    //! This method must be provided by the subclasses. It is used to add any
    //! boundary nodes, nodal loads and constraints to the PartitionedDomain
    //! object. The integer \p numSubdomains is passed to provide information
    //! about the number of subdomains in the PartitionedDomain. To return
    //! \f$0\f$ if successful, a negative number if not.
    virtual int buildInterface(int numSubdomains) =0;
    //! This method must be provided by the subclasses. It is used to add
    //! nodes, elements, loads and constraints to the subdomain {\em
    //! theSubdomain}. The integers \p partition and \p numPartitions
    //! are used to identify which partition is being built and the total
    //! number of partitions. To return \f$0\f$ if successful, a negative number
    //! if not.
    virtual int buildSubdomain(int partition, int numPartitions, Subdomain &) =0;
    
  protected:
    PartitionedDomain *getPartitionedDomainPtr(void) const;
    
  private:
    PartitionedDomain *thePartitionedDomain;
  };
} // end of XC namespace

#endif

