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
#include "solution/SoluMethod.h"

//! @brief Constructor
XC::Broyden::Broyden(SoluMethod *owr,int theTangentToUse, int n)
  :BFBRoydenBase(owr,EquiALGORITHM_TAGS_Broyden,theTangentToUse,n)
  {}

//! @brief Constructor
XC::Broyden::Broyden(SoluMethod *owr,ConvergenceTest &theT, int theTangentToUse, int n)
  :BFBRoydenBase(owr,EquiALGORITHM_TAGS_Broyden,theT,theTangentToUse,n)
  {}

//! @brief Obtiene la solución para el paso actual.
int XC::Broyden::solveCurrentStep(void)
  {
    AnalysisModel *theModel= getAnalysisModelPtr(); 
    LinearSOE *theSysOfEqn= getLinearSOEPtr();
    IncrementalIntegrator *theIntegrator= getIncrementalIntegratorPtr();
    ConvergenceTest *theTest= getConvergenceTestPtr();

    if((!theModel) || (!theIntegrator) || (!theSysOfEqn) || (!theTest))
      {
        std::cerr << "WARNING Broyden::solveCurrentStep() - "
                  << "no se ha asignado modelo, integrador o sistema de ecuaciones.\n";
        return -5;
      }

    // set itself as the ConvergenceTest objects EquiSolnAlgo
    theTest->set_owner(getSoluMethod()); //LCPT. Creo que esto no hace falta.
    if(theTest->start() < 0)
      {
        std::cerr << "Broyden::solveCurrentStep() -"
                  << "the ConvergenceTest object failed in start()\n";
        return -3;
      }
    localTest->set_owner(getSoluMethod());

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
        if(temp.Nulo())
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
                  << "the XC::ConvergenceTest object failed in test()\n";
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
