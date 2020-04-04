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
                                                                        
// $Revision: 1.8 $
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/KrylovNewton.h,v $
                                                                        
#ifndef KrylovNewton_h
#define KrylovNewton_h

// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// KrylovNewton.  KrylovNewton is a class which uses a Krylov
// subspace accelerator on the modified Newton method.
// The accelerator is described by Carlson and Miller in
// "Design and Application of a 1D GWMFE Code"
// from SIAM Journal of Scientific Computing (Vol. 19, No. 3,
// pp. 728-765, May 1998)

#include "EquiSolnAlgo.h"
#include "utility/matrix/Vector.h"
#include "solution/analysis/integrator/IncrementalIntegrator.h"

namespace XC {

//! @ingroup EQSolAlgo
//
//! @brief KrylovNewton algorithm object which uses a Krylov
//! subspace accelerator to accelerate the convergence of the
//! modified newton method.
//
//! KrylovNewton is a class which uses a Krylov
//! subspace accelerator on the modified Newton method.
//! The accelerator is described by Carlson and Miller in
//! "Design and Application of a 1D GWMFE Code"
//! from SIAM Journal of Scientific Computing (Vol. 19, No. 3,
//! pp. 728-765, May 1998).
class KrylovNewton: public EquiSolnAlgo
  {
  private:
    int tangent;

    // Storage for update vectors
    std::vector<Vector> v;
    // Storage for subspace vectors
    std::vector<Vector> Av;

    // Array data sent to LAPACK subroutine
    Vector AvData;
    Vector rData;
    Vector work;

    // Length of work array
    int lwork;

    // Size information
    int numEqns;
    int maxDimension;

    // Private lsq routine to do Krylov updates
    // dimension is the current dimension of the subspace
    int leastSquares(int dimension);

    friend class AnalysisAggregation;
    friend class FEM_ObjectBroker;
    KrylovNewton(AnalysisAggregation *,int tangent = CURRENT_TANGENT, int maxDim = 3);    
    virtual SolutionAlgorithm *getCopy(void) const;
  public:
    int solveCurrentStep(void);    
    
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;    
  };
} // end of XC namespace

#endif


