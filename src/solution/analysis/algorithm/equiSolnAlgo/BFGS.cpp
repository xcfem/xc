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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/BFGS.cpp,v $
                                                                        
// Written: Ed Love
// Created: 06/01

// What: "@(#)BFGS.cpp, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/BFGS.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h> 
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include "solution/SoluMethod.h"

//! @brief Constructor
XC::BFGS::BFGS(SoluMethod *owr,int theTangentToUse, int n)
  :BFBRoydenBase(owr,EquiALGORITHM_TAGS_BFGS,theTangentToUse,n) {}

//! @brief Constructor.
XC::BFGS::BFGS(SoluMethod *owr,ConvergenceTest &theT, int theTangentToUse, int n)
  :BFBRoydenBase(owr,EquiALGORITHM_TAGS_BFGS,theT,theTangentToUse,n) {}

//! @brief resuelve el paso actual.
int XC::BFGS::solveCurrentStep(void)
  {
    AnalysisModel *theModel = getAnalysisModelPtr(); 
    LinearSOE *theSysOfEqn = getLinearSOEPtr();
    IncrementalIntegrator *theIntegrator= getIncrementalIntegratorPtr();
    ConvergenceTest *theTest= getConvergenceTestPtr();
 
    if((theModel == 0) || (theIntegrator == 0) || (theSysOfEqn == 0) || (theTest == 0))
      {
	std::cerr << "WARNING XC::BFGS::solveCurrentStep() - setLinks() has";
	std::cerr << "no se ha asignado modelo, integrador o sistema de ecuaciones.\n";
	return -5;
      }	

    // set itself as the ConvergenceTest objects EquiSolnConvAlgo
    theTest->set_owner(getSoluMethod());
    if(theTest->start() < 0)
      {
        std::cerr << "XC::BFGS::solveCurrentStep() -";
        std::cerr << "the ConvergenceTest object failed in start()\n";
        return -3;
      }

    localTest->set_owner(getSoluMethod());

    rdotz.resize(numberLoops+3);
    sdotr.resize(numberLoops+3);

    int result = -1;
    int count = 0;
    do
      {
        // std::cerr << "      BFGS -- Forming New_ Tangent" << std::endl;
        //form the initial tangent
        if(theIntegrator->formTangent(tangent) < 0)
          {
            std::cerr << "WARNING BFGS::solveCurrentStep() -";
            std::cerr << "the Integrator failed in formTangent()\n";
            return -1; 
          }

        //form the initial residual 
        if(theIntegrator->formUnbalance() < 0)
          {
            std::cerr << "WARNING BFGS::solveCurrentStep() -";
            std::cerr << "the Integrator failed in formUnbalance()\n";	
          }	    

        //solve
        if(theSysOfEqn->solve() < 0)
          {
	    std::cerr << "WARNING BFGS::solveCurrentStep() -";
	    std::cerr << "the LinearSysOfEqn failed in solve()\n";	
	    return -3;
	  }	    

        //update
        if(theIntegrator->update(theSysOfEqn->getX() ) < 0)
          {
	    std::cerr << "WARNING BFGS::solveCurrentStep() -";
	    std::cerr << "the Integrator failed in update()\n";	
	    return -4;
          }	        


        //    int systemSize = ( theSysOfEqn->getB() ).Size();
        int systemSize = theSysOfEqn->getNumEqn( );

        //temporary vector
        if(temp.Nulo())
	  temp= Vector(systemSize);

        //initial displacement increment
        s[1]= theSysOfEqn->getX( );


        residOld= theSysOfEqn->getB( ) ;
        residOld*= (-1.0 );

        //form the residual again
        if(theIntegrator->formUnbalance() < 0)
          {
            std::cerr << "WARNING XC::BFGS::solveCurrentStep() -";
            std::cerr << "the XC::Integrator failed in formUnbalance()\n";	
          }	    

        if(residNew.Nulo()) 
	  residNew= Vector(systemSize);
 
        if(du.Nulo()) 
	  du= Vector(systemSize);

        if(b.Nulo())
	  b= Vector(systemSize);

        localTest->start();

        int nBFGS = 1;
        do
          {

            //save residual
            residNew=  theSysOfEqn->getB( ); 
            residNew*= (-1.0 );

      
            //solve
            if(theSysOfEqn->solve() < 0)
              {
	        std::cerr << "WARNING BFGS::solveCurrentStep() -";
	        std::cerr << "the LinearSysOfEqn failed in solve()\n";	
	        return -3;
              }	    

	    //save right hand side
            b= theSysOfEqn->getB( );

            //save displacement increment
            du= theSysOfEqn->getX( );

            //BFGS modifications to du
            BFGSUpdate(theIntegrator, theSysOfEqn, du, b, nBFGS );

            if(theIntegrator->update(du) < 0)
              {
	        std::cerr << "WARNING XC::BFGS::solveCurrentStep() -";
	        std::cerr << "the XC::Integrator failed in update()\n";	
	        return -4;
              }	        

	    /* std::cerr << "        BFGS Iteration " << nBFGS 
                << " Residual Norm = " 
                << sqrt( (*residNew) ^ (*residNew) ) << std::endl;
	    */
        
            //increment broyden counter
            nBFGS += 1;

            //save displacement increment
            s[nBFGS]= du;

            //swap residuals
	    residOld= residNew;

            //form the residual again
            if(theIntegrator->formUnbalance() < 0)
              {
                std::cerr << "WARNING BFGS::solveCurrentStep() -";
                std::cerr << "the XC::Integrator failed in formUnbalance()\n";	
              }	    
            result = localTest->test();
          
          }
        while(result == -1 && nBFGS <= numberLoops );

        result = theTest->test();
        this->record(count++); //Llama al método record(...) de todos los recorders definidos.
      }
    while(result == -1);

    if(result == -2)
      {
        std::cerr << "XC::BFGS::solveCurrentStep() -";
        std::cerr << "the XC::ConvergenceTest object failed in test()\n";
        return -3;
      }
    // note - if postive result we are returning what the convergence test returned
    // which should be the number of iterations
    return result;
  }

//! @brief Actualiza.
void  XC::BFGS::BFGSUpdate(IncrementalIntegrator *theIntegrator, LinearSOE *theSOE, Vector &du, Vector &b, int nBFGS)
  {
    static const double eps = 1.0e-16;


    //compute z
    //  theSOE->setB( (*r[nBFGS]) - (*r[nBFGS-1]) );
    //    theSOE->setB( (residNew) - (residOld) );
    temp= residNew;
    temp-= residOld;
    theSOE->setB(temp);


    if(theSOE->solve() < 0)
      {
        std::cerr << "WARNING XC::BFGS::solveCurrentStep() -";
        std::cerr << "the LinearSysOfEqn failed in solve()\n";	
      }	    
  
    z[nBFGS]= theSOE->getX(); 
    //  z[nBFGS] *= (-1.0);

    int i;
    for(i=1; i<=(nBFGS-1); i++ )
      {

        if(sdotr[i] < eps ) 
          break; 

        double fact1 = 1.0 + ( rdotz[i] / sdotr[i] );
        fact1 /= sdotr[i];
        const double pdotb = (s[i]) ^ ( theSOE->getB() );
        fact1*= pdotb;

        //    z[nBFGS] +=  fact1 * ( s[i] );
        temp= s[i];
        temp*= fact1;
        z[nBFGS]+= temp;


        const double bdotz = (z[i]) ^ ( theSOE->getB() );  

        //    z[nBFGS] -= (1.0/sdotr[i]) * 
        //             ( bdotz * (s[i])   +  pdotb * (z[i]) );   
        temp= s[i];
        temp*= bdotz;
        temp/= sdotr[i];
        z[nBFGS]-= temp;

        temp= z[i];
        temp*= pdotb;
        temp/= sdotr[i];
        z[nBFGS]-= temp;
      } //end for i


    //sdotr[nBFGS] = s[nBFGS] ^ ( residNew - residOld );

    //rdotz[nBFGS] = z[nBFGS] ^ ( residNew - residOld );   

      temp= residNew;
      temp-= residOld;

      sdotr[nBFGS]= s[nBFGS]^temp;
      rdotz[nBFGS]= z[nBFGS]^temp;


    //BFGS modifications to du
    for(i=1;i<=nBFGS;i++ )
      {
        if(sdotr[i] < eps )
          break;

        double fact1 = 1.0 + ( rdotz[i] / sdotr[i] );
        fact1/= sdotr[i];

        const double sdotb = (s[i]) ^ b;
        fact1*= sdotb;

        //du +=  fact1 * ( s[i] );
        temp= s[i];
        temp*= fact1;
        du+= temp;


        const double bdotz = (z[i]) ^ b;  

        //du -= (1.0/sdotr[i]) * 
        //             ( bdotz * (s[i])   +  sdotb * (z[i]) );   
        temp= s[i];
        temp*= bdotz;
        temp/= sdotr[i];
        du-= temp;

        temp= z[i];
        temp*= sdotb;
        temp/= sdotr[i];
        du-= temp;
      } //end for i
  }


int XC::BFGS::sendSelf(CommParameters &cp)
  { return -1; }

int XC::BFGS::recvSelf(const CommParameters &cp)
  { return -1; }


void XC::BFGS::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      {
        s << "BFGS" << std::endl;
        s << "  Number of Iterations = " << numberLoops << std::endl;
      }
  }
