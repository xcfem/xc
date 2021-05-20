// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
#ifndef ROOT_H
#define ROOT_H


/*
            Copyright (c) 1995 Namir C. Shammas

      Version 1.0                                Date 6/3/94

      Module: C root-finding routines which support the following
        general methods for a single real function:

            + Bisection method
            + Tangent method
            + Newton's method
            + Richmond's method
            + Brent's method
            + Combined method (Bisection and Newton)
            + Lin-Bairston method for polynomials
            + Newton's method for two equations
            + Newton's method for multiple equations

*/

#include "utility/matrices/m_double.h"
#include "utility/matrices/m_func.h"
#include "utility/matrices/m_int.h"
#include "utility/matrices/solvers/solver_lu.h"
#include "Metodo2Func.h"
#include "bisection.h"

#define ROOT_EPS 1.0e-30

typedef struct polyRootTag 
  {
    double real;
    double imag;
    bool isComplex;
  } polyRoot;


int NewtonApprox(double *root, double tolerance,
                 int maxIter, double* params,
                 double (*f)(double, double*));

int NewtonExact(double *root, double tolerance,
                int maxIter, double* params,
                double (*f)(double, double*),
                double (*fderiv)(double, double*));

int RichmondApprox(double *root, double tolerance,
                   int maxIter, double* params,
                   double (*f)(double, double*));

int RichmondExact(double *root, double tolerance,
                  int maxIter, double* params,
                  double (*f)(double, double*),
                  double (*f1stDeriv)(double, double*),
                  double (*f2ndDeriv)(double, double*));

int Combined(double low, double high, double tolerance,
             double* root, int maxIter, double* params,
             double (*f)(double, double*));

int Brent(double low, double high, double tolerance,
          double* root, int maxIter, double* params,
          double (*f)(double, double*));

int NewtonMultiRoots(double *roots, int* numRoots, int maxRoots,
                     double tolerance, int maxIter, double* params,
                     double (*f)(double, double*));

int DeflatePolyRoots(double* coeff, double initGuess,
                     double* roots, int* numRoots,
                     int polyOrder, int maxIter,
                     double tolerance);

int LBPolyRoots(double* coeff, polyRoot* roots,
                int polyOrder, double tolerance);

int Newton2Functions(double *rootX, double* rootY,
                     double tolerance, int maxIter,
                     double* paramsX, double* paramsY,
                     double (*fx)(double, double, double*),
                     double (*fy)(double, double, double*));

template<class MF>
int NewtonSimNLE(m_double &X, double tolerance,int maxIter, const MF &f)
  {
    int moreIter, iter= 0;
    double h;
    size_t numEqns= X.getNumberOfRows();

    m_double J(numEqns,numEqns);
    m_double Xdash(X);
    m_double Fvector(numEqns,1);
    m_int index(numEqns,1);

    do
      {
        iter++;
        // copy the values of array X into array Xdash
        Xdash= X;
        // calculate the array of function values
        Fvector= f.Eval(X);
	std::cout <<"F( " << X << ")= " << Fvector << std::endl;
        // calculate the J matrix
        for(size_t i= 1;i<=numEqns; i++)
          for(size_t j= 1;j<=numEqns;j++)
            {
              // calculate increment in variable number j
              //h = (fabs(X(j)) > 1) ? 0.01 * X(j) : 0.01; //xxx no se porqué hacía esto
              h= 0.01*X(j);
              Xdash(j)+= h;
	      std::cout << "h= " << h << std::endl;
/*               cout << "f= " << (f(i))(Xdash) << endl; */
/*               cout << "F= " << Fvector(i) << endl; */
              J(i,j) = ((f(i))(Xdash) - Fvector(i)) / h;
              // restore incremented value
              Xdash(j)= X(j);
            }
	//        cout << "J= " << J << endl;
        // solve for the guess refinement vector
        SolverLU<m_double,m_double> solver;
        solver.putMatrix(J);
        solver.Decomp();
        Fvector= solver.BackSub(Fvector);

        // clear the more-iteration flag
        moreIter = 0;
        // update guess and test for convergence
        for(size_t i= 1;i<= numEqns;i++)
          {
            X(i)-= Fvector(i);
            if(fabs(Fvector(i)) > tolerance) moreIter = 1;
          }
	std::cout << "iteración: " << iter << std::endl;
        if(moreIter)
          moreIter = (iter > maxIter) ? 0 : 1;
      }
    while (moreIter);

    return 1 - moreIter;
  }

#endif
