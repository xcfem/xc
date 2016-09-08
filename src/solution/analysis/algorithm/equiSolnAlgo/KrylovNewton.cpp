//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

// $Revision: 1.11 $
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/KrylovNewton.cpp,v $

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

#include <solution/analysis/algorithm/equiSolnAlgo/KrylovNewton.h>
#include <solution/analysis/analysis/StaticAnalysis.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include "solution/SoluMethod.h"

// Constructor
XC::KrylovNewton::KrylovNewton(SoluMethod *owr,int theTangentToUse, int maxDim)
  :EquiSolnAlgo(owr,EquiALGORITHM_TAGS_KrylovNewton),
   tangent(theTangentToUse), v(0), Av(0), AvData(0), rData(0), work(0), lwork(0),
   numEqns(0), maxDimension(maxDim)
  { if(maxDimension < 0) maxDimension = 0; }

//! @brief resuelve el paso actual.
int XC::KrylovNewton::solveCurrentStep(void)
  {
    // set up some pointers and check they are valid
    // NOTE this could be taken away if we set Ptrs as protecetd in superclass
    AnalysisModel *theAnaModel= getAnalysisModelPtr();
    IncrementalIntegrator *theIntegrator= getIncrementalIntegratorPtr();
    LinearSOE *theSOE= getLinearSOEPtr();
    ConvergenceTest *theTest= getConvergenceTestPtr();

    if((theAnaModel == 0) || (theIntegrator == 0) || (theSOE == 0) || (theTest == 0))
      {
        std::cerr << "WARNING KrylovNewton::solveCurrentStep() - "
                  << "no se ha asignado modelo, integrador o sistema de ecuaciones.\n";
        return -5;
      }

    // Get size information from SOE
    numEqns  = theSOE->getNumEqn();
    if(maxDimension > numEqns)
      maxDimension = numEqns;

    if(v.empty())
      v= std::vector<Vector>(maxDimension+1,Vector(numEqns));

    if(Av.empty())
      Av= std::vector<Vector>(maxDimension+1,Vector(numEqns));

    AvData.resize(maxDimension*numEqns);

    // The LAPACK least squares subroutine overwrites the RHS vector
    // with the solution vector ... these vectors are not the same
    // size, so we need to use the max size
    rData.resize((numEqns > maxDimension) ? numEqns : maxDimension);

    // Length of work vector should be >= 2*min(numEqns,maxDimension)
    // See dgels subroutine documentation
    lwork= 2 * ((numEqns < maxDimension) ? numEqns : maxDimension);

    work.resize(lwork);

    // Evaluate system residual R(y_0)
    if(theIntegrator->formUnbalance() < 0)
      {
        std::cerr << "WARNING XC::KrylovNewton::solveCurrentStep() -"
                  << "the XC::Integrator failed in formUnbalance()\n";
        return -2;
      }


    // set itself as the XC::ConvergenceTest objects XC::EquiSolnAlgo
    theTest->set_owner(getSoluMethod());
    if(theTest->start() < 0)
      {
        std::cerr << "KrylovNewton::solveCurrentStep() -"
                  << "the XC::ConvergenceTest object failed in start()\n";
        return -3;
      }


    // Evaluate system Jacobian J = R'(y)|y_0
    if(theIntegrator->formTangent(tangent) < 0)
      {
        std::cerr << "WARNING XC::KrylovNewton::solveCurrentStep() -"
                  << "the XC::Integrator failed in formTangent()\n";
        return -1;
      }

    // Loop counter
    int k = 1;

    // Current dimension of Krylov subspace
    int dim = 0;

    int result = -1;

    do
      {
        // Clear the subspace if its dimension has exceeded max
        if(dim > maxDimension)
          {
            dim = 0;
            if(theIntegrator->formTangent(tangent) < 0)
              {
                std::cerr << "WARNING XC::KrylovNewton::solveCurrentStep() -";
                std::cerr << "the XC::Integrator failed to produce new_ formTangent()\n";
                return -1;
              }
          }

        // Solve for residual f(y_k) = J^{-1} R(y_k)
        if(theSOE->solve() < 0)
          {
            std::cerr << "WARNING XC::KrylovNewton::solveCurrentStep() -";
            std::cerr << "the LinearSysOfEqn failed in solve()\n";
            return -3;
          }

        // Solve least squares A w_{k+1} = r_k
        if(this->leastSquares(dim) < 0)
          {
            std::cerr << "WARNING XC::KrylovNewton::solveCurrentStep() -";
            std::cerr << "the XC::Integrator failed in leastSquares()\n";
            return -1;
          }

        // Update system with v_k
        if(theIntegrator->update(v[dim]) < 0)
          {
            std::cerr << "WARNING XC::KrylovNewton::solveCurrentStep() -";
            std::cerr << "the XC::Integrator failed in update()\n";
            return -4;
          }

        // Evaluate system residual R(y_k)
        if(theIntegrator->formUnbalance() < 0)
          {
            std::cerr << "WARNING XC::KrylovNewton::solveCurrentStep() -"
                      << "the Integrator failed in formUnbalance()\n";
            return -2;
          }

        // Increase current dimension of Krylov subspace
        dim++;

        result = theTest->test();
        this->record(k++); //Llama al método record(...) de todos los recorders definidos.
      }
    while(result == -1);

    if(result == -2)
      {
        std::cerr << "XC::KrylovNewton::solveCurrentStep() -"
                  << "the XC::ConvergenceTest object failed in test()\n"
                  << "convergence test message: "
		  << theTest->getStatusMsg(1) << std::endl;
        return -3;
      }

    // note - if postive result we are returning what the convergence
    // test returned which should be the number of iterations
    return result;
  }

int XC::KrylovNewton::sendSelf(CommParameters &cp)
  { return -1; }

int XC::KrylovNewton::recvSelf(const CommParameters &cp)
  { return -1; }

void XC::KrylovNewton::Print(std::ostream &s, int flag)
  {
    s << "KrylovNewton";
    s << "\n\tMax subspace dimension: " << maxDimension;
    s << "\n\tNumber of equations: " << numEqns << std::endl;
  }

extern "C" int dgels_(char *T, int *M, int *N, int *NRHS,
                      double *A, int *LDA, double *B, int *LDB,
                      double *WORK, int *LWORK, int *INFO);

int XC::KrylovNewton::leastSquares(int k)
{
  LinearSOE *theSOE = this->getLinearSOEPtr();
  const XC::Vector &r = theSOE->getX();

  // v_{k+1} = w_{k+1} + q_{k+1}
  v[k]= r;
  Av[k]= r;

  // Subspace is empty
  if(k == 0)
    return 0;

  // Compute Av_k = f(y_{k-1}) - f(y_k) = r_{k-1} - r_k
  Av[k-1].addVector(1.0, r, -1.0);

  int i,j;

  // Put subspace vectors into AvData
  Matrix A(AvData.getDataPtr(), numEqns, k);
  for(i = 0; i < k; i++) {
    Vector &Ai = Av[i];
    for(j = 0; j < numEqns; j++)
      A(j,i) = Ai(j);
  }

  // Put residual vector into rData (need to save r for later!)
  Vector B(rData.getDataPtr(), numEqns);
  B = r;

  // No transpose
  char trans[]= "N";

  // The number of right hand side vectors
  int nrhs = 1;

  // Leading dimension of the right hand side vector
  int ldb = (numEqns > k) ? numEqns : k;

  // Subroutine error flag
  int info = 0;

  // Call the LAPACK least squares subroutine
  dgels_(trans, &numEqns, &k, &nrhs, AvData.getDataPtr(), &numEqns, rData.getDataPtr(), &ldb, work.getDataPtr(), &lwork, &info);

  // Check for error returned by subroutine
  if(info < 0)
    {
      std::cerr << "WARNING XC::KrylovNewton::leastSquares() - \n";
      std::cerr << "error code " << info << " returned by LAPACK dgels\n";
      return info;
    }

  // Compute the correction vector
  double cj;
  for(j = 0; j < k; j++) {

    // Solution to least squares is written to rData
    cj = rData[j];

    // Compute w_{k+1} = c_1 v_1 + ... + c_k v_k
    v[k].addVector(1.0, v[j], cj);

    // Compute least squares residual q_{k+1} = r_k - (c_1 Av_1 + ... + c_k Av_k)
    v[k].addVector(1.0, Av[j], -cj);
  }

  return 0;
}

