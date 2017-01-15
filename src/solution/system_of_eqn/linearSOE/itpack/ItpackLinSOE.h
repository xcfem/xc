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
                                                                        
// $Revision: 1.2 $
// $Date: 2002/06/08 16:17:26 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/itpack/ItpackLinSOE.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/umfGEN/ItpackGenLinSOE.h
//
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for 
// ItpackGenLinSOE. It stores the sparse matrix A in a fashion
// required by the ItpackLinSolver object.
//
// What: "@(#) ItpackGenLinSOE.h, revA"


#ifndef ItpackGenLinSOE_h
#define ItpackGenLinSOE_h

#include <solution/system_of_eqn/linearSOE/LinearSOEData.h>
#include "utility/matrix/Vector.h"

namespace XC {
class ItpackLinSolver;

//! @ingroup SOE
//
//! @brief Sistemas de ecuaciones que
//! hacen uso de la biblioteca ITPACK
//! (http://rene.ma.utexas.edu/CNA/ITPACK) .
class ItpackLinSOE : public LinearSOEData
  {
  private:
    int nnz; //!< number of non-zeros in A
    Vector A; //!< 1d arrays containing coefficients of A, B and X
    ID colA;
    ID rowStartA; // int arrays containing info about coeff's in A
    
    bool Aformed;        // false when zeroA() is called
  protected:
    virtual bool setSolver(LinearSOESolver *);

    friend class SoluMethod;
    ItpackLinSOE(SoluMethod *);
    SystemOfEqn *getCopy(void) const;
  public:
    int setSize(Graph &theGraph);
    int addA(const Matrix &, const ID &, double fact = 1.0);
    
    void zeroA(void);
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    friend class ItpackLinSolver;
  };
inline SystemOfEqn *ItpackLinSOE::getCopy(void) const
  { return new ItpackLinSOE(*this); }
} // end of XC namespace

#endif
