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

// $Revision: 1.4 $
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/Broyden.cpp,v $


// Written: Ed C++ Love
// Created: 04/01

// Description: This file contains the class definition implementation of
// Broyden.
//
// What: "@(#)Broyden.h, revA"


#include <solution/analysis/algorithm/equiSolnAlgo/Broyden.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include "solution/AnalysisAggregation.h"

//! @brief Constructor
XC::Broyden::Broyden(AnalysisAggregation *owr,int theTangentToUse, int n)
  :BFBRoydenBase(owr,EquiALGORITHM_TAGS_Broyden,theTangentToUse,n)
  {}

//! @brief Constructor
XC::Broyden::Broyden(AnalysisAggregation *owr,ConvergenceTest &theT, int theTangentToUse, int n)
  :BFBRoydenBase(owr,EquiALGORITHM_TAGS_Broyden,theT,theTangentToUse,n)
  {}

//! @brief Computes the solution for the current step.
int XC::Broyden::solveCurrentStep(void)
  {
    AnalysisModel *theModel= getAnalysisModelPtr(); 
    LinearSOE *theSysOfEqn= getLinearSOEPtr();
    IncrementalIntegrator *theIntegrator= getIncrementalIntegratorPtr();
    ConvergenceTest *theTest= getConvergenceTestPtr();

    if((!theModel) || (!theIntegrator) || (!theSysOfEqn) || (!theTest))
      {
        std::cerr << "WARNING Broyden::solveCurrentStep() - "
                  << "no se ha asignado modelo, integrator o system of equations.\n";
        return -5;
      }

    // set itself as the ConvergenceTest objects EquiSolnAlgo
    theTest->set_owner(getAnalysisAggregation()); //LCPT. Creo que esto no hace falta.
    if(theTest->start() < 0)
      {
        std::cerr << "Broyden::solveCurrentStep() -"
                  << "the ConvergenceTest object failed in start()\n";
        return -3;
      }
    localTest->set_owner(getAnalysisAggregation());

    int result = -1;
    int count = 0;
    do
      {

        // std::cerr << "      Broyden -- Forming New_ Tangent" << std::endl;

        //form the initial tangent
        if(theIntegrator->formTangent(tangent) < 0)
          {
            std::cerr << "WARNING XC::Broyden::solveCurrentStep() -";
            std::cerr << "the Integrator failed in formTangent()\n";
            return -1;
          }

        //form the initial residual
        if(theIntegrator->formUnbalance() < 0)
          {
            std::cerr << "WARNING XC::Broyden::solveCurrentStep() -";
            std::cerr << "the Integrator failed in formUnbalance()\n";
          }

        //solve
        if(theSysOfEqn->solve() < 0)
          {
            std::cerr << "WARNING XC::Broyden::solveCurrentStep() -";
            std::cerr << "the LinearSysOfEqn failed in solve()\n";
            return -3;
          }

        //update
        if( theIntegrator->update(theSysOfEqn->getX() ) < 0)
          {
            std::cerr << "WARNING XC::Broyden::solveCurrentStep() -";
            std::cerr << "the XC::Integrator failed in update()\n";
            return -4;
          }

        //    int systemSize = ( theSysOfEqn->getB() ).Size();
        int systemSize= theSysOfEqn->getNumEqn( );

        //temporary vector
        if(temp.isEmpty())
          temp= Vector(systemSize);

        //initial displacement increment
        s[1]= theSysOfEqn->getX( );

        //initial residual

        residOld= theSysOfEqn->getB();
        residOld*= (-1.0 );

        //form the residual again
        if(theIntegrator->formUnbalance() < 0)
          {
            std::cerr << "WARNING XC::Broyden::solveCurrentStep() -";
            std::cerr << "the XC::Integrator failed in formUnbalance()\n";
          }


        localTest->start();

        int nBroyden= 1;
        do
          {
            //save residual

            residNew= theSysOfEqn->getB();
            residNew*= (-1.0 );

            //solve
            if(theSysOfEqn->solve() < 0)
              {
                std::cerr << "WARNING XC::Broyden::solveCurrentStep() -";
                std::cerr << "the LinearSysOfEqn failed in solve()\n";
                return -3;
              }

            //save displacement increment
            du= theSysOfEqn->getX( );

            //broyden modifications to du
            BroydenUpdate( theIntegrator, theSysOfEqn, du, nBroyden );

            if(theIntegrator->update(du) < 0 )
              {
                std::cerr << "WARNING Broyden::solveCurrentStep() -";
                std::cerr << "the Integrator failed in update()\n";
                return -4;
              }


            /*        std::cerr << "        Broyden Iteration " << nBroyden
                << " Residual Norm = "
                << sqrt( (*residNew) ^ (*residNew) ) << std::endl;
            */

            //increment broyden counter
            nBroyden+= 1;

            //save displacement increment
            s[nBroyden]= du;

            //swap residuals
            residOld= residNew;

            //form the residual again
            if(theIntegrator->formUnbalance() < 0)
              {
                std::cerr << "WARNING XC::Broyden::solveCurrentStep() -";
                std::cerr << "the XC::Integrator failed in formUnbalance()\n";
              }
            result = localTest->test();
          }
        while(result == -1 && nBroyden <= numberLoops);

        result = theTest->test();
        this->record(count++); //Llama al método record(...) de todos los recorders definidos.
      }
    while(result == -1);

    if(result == -2)
      {
        std::cerr << "XC::Broyden::solveCurrentStep() -"
                  << "the XC::ConvergenceTest object failed in test()\n"
                  << "convergence test message: "
		  << theTest->getStatusMsg(1) << std::endl;
        return -3;
      }
    // note - if postive result we are returning what the convergence test returned
    // which should be the number of iterations
    return result;
  }



void  XC::Broyden::BroydenUpdate( IncrementalIntegrator *theIntegrator, LinearSOE *theSOE, Vector &du, int nBroyden )
  {

    static const double eps = 1.0e-16;

    //compute z
    //  theSOE->setB( (*r[nBroyden]) - (*r[nBroyden-1]) );
    //    theSOE->setB( (*residNew) - (*residOld) );
    temp= residNew;
    temp-= residOld;
    theSOE->setB(temp);

    if(theSOE->solve() < 0)
      std::cerr << "WARNING Broyden::solveCurrentStep() -"
                << "the LinearSysOfEqn failed in solve()\n";
 
    z[nBroyden]= theSOE->getX();
    z[nBroyden]*= (-1.0);

    for(int i=1; i<=(nBroyden-1); i++ )
      {
        const double p = - ( (s[i]) ^ (z[i]) );

        if(fabs(p) < eps ) break;

        const double sdotz = (s[i]) ^ (z[nBroyden]);

        //z[nBroyden] += (1.0/p) * sdotz * ( s[i] + z[i] );
        temp= s[i];
        temp+= z[i];
        temp*= ((1.0/p)*sdotz);
        z[nBroyden]+= temp;
      } //end for i


    //broyden modifications to du
    for(int i=1; i<=nBroyden; i++ )
      {
        const double p= - ( (s[i]) ^ (z[i]) );

        if( fabs(p) < eps ) break;
        const double sdotdu = (s[i]) ^ du;

        //du += (1.0/p) * sdotdu * ( s[i] + z[i] );
        temp= s[i];
        temp+= z[i];
        temp*= ((1.0/p)*sdotdu);
        du+= temp;
      } //end for i
  }


int XC::Broyden::sendSelf(CommParameters &cp)
  { return -1; }

int XC::Broyden::recvSelf(const CommParameters &cp)
  { return -1; }


void XC::Broyden::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      {
        s << "Broyden" << std::endl;
        s << "  Number of Iterations = " << numberLoops << std::endl;
      }
  }
