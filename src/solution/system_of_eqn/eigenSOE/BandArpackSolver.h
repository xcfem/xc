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
// File: ~/system_of_eqn/eigenSOE/BandArpackSolver.h
//
// Written: Jun Peng
// Created: Feb. 11, 1999
// Revision: A
//
// Description: This file contains the class definition for 
// BandArpackSolver. It solves the BandArpackSOE object by calling
// Arpack routines.


#ifndef BandArpackSolver_h
#define BandArpackSolver_h

#include <solution/system_of_eqn/eigenSOE/EigenSolver.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackSOE.h>

namespace XC {

//! @brief Auxiliary variables for calling
//! Arpack functions.
struct ArpackAuxVars
  {
    int ldv;
    int lworkl; //!< at least NCV**2+8*NCV
    std::vector<double> v; //!< Lanczos basis vectors.
    std::vector<double> workl; //!< Private (replicated) array of each PE or array allocated on the front end.
    std::vector<double> workd; //!< Reverse communication.
    Vector d;
    Vector z;
    std::vector<double> resid; //!< residual vector.
    int iparam[11]; //!< Method for selecting the implicit shifts.
    int ipntr[11]; //!< Pointer to mark the starter location in the workd and workl arrays for matrices/vectors used by the Lanczos iteration.
    std::vector<long int> select;
    const char bmat= 'G'; //!< 'G': generalized eigenvalue problem A*x= lambda*B*x
    const char howmy= 'A';
    const std::string which= "LM"; //! LM: compute the largest (in magnitude) eigenvalues.
	
    ArpackAuxVars(int n, int ncv, int nev, int maxitr, int mode);
    int dsaupd(int &ido, const int &n, const int &nev, const double &tol,
	       const int &ncv, int &info);
  };
//! @ingroup LinearSolver
//
//! @brief Arpack solver for banded matrices.
//! The ARnoldi PACKage, is a numerical software
//! library written in FORTRAN 77 for solving large scale eigenvalue problems.
class BandArpackSolver : public EigenSolver
  {
  private:
    BandArpackSOE *theSOE;
    Vector value;
    Vector eigenvector;
    double tol; //!< Tolerancia para los eigenvalues obtenidos.
    int maxitr; //!< Maximum number of iterations.
    ID iPiv;
    mutable Vector eigenV;
    
    void myMv(int n, double *v, double *result);
    void myCopy(int n, double *v, double *result);
    int dsaupd_loop(const int &ncv, const int &nev,ArpackAuxVars &);
    int getNCV(int n, int nev);

    void print_err_info(int);
    void dseupd_print_err_info(int);
  protected:


    friend class EigenSOE;
    BandArpackSolver(int numE = 0);   
    virtual EigenSolver *getCopy(void) const;
    bool setEigenSOE(EigenSOE *theSOE);
  public:

    virtual int solve(void);
    virtual int solve(int nModes);
    virtual int setSize(void);
    const int &getSize(void) const;
    virtual bool setEigenSOE(BandArpackSOE &theSOE);
    
    virtual const Vector &getEigenvector(int mode) const;
    virtual const double &getEigenvalue(int mode) const;
    
    double getRCond(const char &);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline EigenSolver *BandArpackSolver::getCopy(void) const
   { return new BandArpackSolver(*this); }
} // end of XC namespace

#endif


