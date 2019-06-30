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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/11/29 21:55:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/numberer/ParallelNumberer.h,v $
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for ParallelNumberer.
// ParallelNumberer is a subclass of DOF_Numberer. The ParallelNumberer numbers
// the dof of a partitioned domain, where the partitions are on different processors
// and each processor has a ParallelNumberer. The ParallelNumberer sitting on P0, 
// collects each partition graph from P1 through Pn-1, merges them into 1 large graph, 
// & then numbers this graph. The ParallelNumberers sitting on P1 through Pn-1 then 
// receive the mapping info for the dof tag and dof numbering from P0.
//
// What: "@(#) ParallelNumberer.h, revA"

#ifndef ParallelNumberer_h
#define ParallelNumberer_h

#include "utility/actor/actor/DistributedObj.h"
#include "solution/analysis/numberer/DOF_Numberer.h"

namespace XC {
class Graph;

//! @ingroup Numerador
//
//! @brief The ParallelNumberer numbers the dof of a
//! partitioned domain, where the partitions are
//! on different processors and each processor has a
//! ParallelNumberer. The ParallelNumberer sitting on P0, 
//! collects each partition graph from P1 through Pn-1, merges
//! them into 1 large graph,  & then numbers this graph.
//! The ParallelNumberers sitting on P1 through Pn-1 then 
//! receive the mapping info for the dof tag and dof numbering from P0.
class ParallelNumberer: public DOF_Numberer, public DistributedObj
  {
  private:
    GraphNumberer *theNumberer;
  protected:
    int mergeSubGraph(Graph &theGraph, Graph &theSubGraph, ID &vertexTags, ID &vertexRefs, ID &theSubdomainMap);

    friend class ModelWrapper;
    friend class FEM_ObjectBroker;
    ParallelNumberer(ModelWrapper *);
    ParallelNumberer(ModelWrapper *,int domainTag, int numSubdomains,const ChannelQueue &);
    virtual DOF_Numberer *getCopy(void) const;
  public:
    ~ParallelNumberer(void);

    int numberDOF(int lastDOF = -1);
    int numberDOF(ID &lastDOFs);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

  };
} // end of XC namespace

#endif

