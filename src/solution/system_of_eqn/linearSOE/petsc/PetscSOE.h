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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/05/18 19:24:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/PetscSOE.h,v $
                                                                        
                                                                        
// Written: fmk & om
// Created: 7/98
// Revision: A
//
// Description: This file contains the class definition for PetscSOE
// PetscSOE is a subclass of LinearSOE. It uses the LAPACK storage
// scheme to store the components of the A matrix, which is a full matrix.


// What: "@(#) PetscSOE.h, revA"

#ifndef PetscSOE_h
#define PetscSOE_h

#include "solution/system_of_eqn/linearSOE/FactoredSOEBase.h"
#include "solution/system_of_eqn/linearSOE/DistributedLinSOE.h"

// extern "C" {
#include "petsc/petscksp.h"

// }

#if (PETSC_VERSION_MAJOR == 3) && (PETSC_VERSION_MINOR < 2)
typedef PetscTruth PetscBool;
#endif

namespace XC {
class PetscSolver;

//! @ingroup SOE
//
//! @brief <a href="http://www.mcs.anl.gov/petsc/petsc-as" target="_new"> PETSC</a> based system of equations.
class PetscSOE: public FactoredSOEBase, public DistributedLinSOE
  {
  private:
    int numProcesses;

    Mat A;
    Vec x, b;
    int blockSize;
    PetscBool flg;

    int startRow, endRow;
    void InvokePetscDestructors(void);
  protected:
    inline ChannelQueue &getChannels(void)
      { return theChannels; }
    virtual bool setSolver(PetscSolver *);

    friend class SoluMethod;
    PetscSOE(SoluMethod *,int blockSize=1);    
    SystemOfEqn *getCopy(void) const;    
  public:
    ~PetscSOE(void);

    int setSize(Graph &theGraph);
    
    int addA(const Matrix &, const ID &, double fact = 1.0);

    void zeroA(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    friend class PetscSolver;
    friend class ActorPetscSOE;
    friend class ShadowPetscSOE;
    
  };
inline SystemOfEqn *PetscSOE::getCopy(void) const
  { return new PetscSOE(*this); }
} // end of XC namespace


#endif

