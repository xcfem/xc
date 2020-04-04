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
//BandArpackppSolver.h

#ifndef BandArpackppSolver_h
#define BandArpackppSolver_h

#include <solution/system_of_eqn/eigenSOE/EigenSolver.h>
#include "utility/matrix/Vector.h"

namespace XC {
class BandArpackppSOE;

//! @ingroup LinearSolver
//
//! @brief <a href="http://www.caam.rice.edu/software/ARPACK/" target="_new"> Arpack++</a> based band matrix eigenproblem solver.
class BandArpackppSolver: public EigenSolver
  {
  private:
    BandArpackppSOE *theSOE;
    std::vector<double> eigenvalues;
    std::vector<Vector> eigenvectors;

    void setup_autos(const size_t &nmodos,const size_t &n);

    friend class EigenSOE;
    BandArpackppSolver(void);
    BandArpackppSolver(const int &nModes);
    virtual EigenSolver *getCopy(void) const;
    bool setEigenSOE(EigenSOE *theSOE);
  public:
  
    virtual int solve(void);
    virtual int setSize(void);
    const int &getSize(void) const;
    virtual bool setEigenSOE(BandArpackppSOE &theSOE);
  
    virtual const Vector &getEigenvector(int mode) const;
    virtual const double &getEigenvalue(int mode) const;
  
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };

inline EigenSolver *BandArpackppSolver::getCopy(void) const
   { return new BandArpackppSolver(*this); }
} // end of XC namespace

#endif
