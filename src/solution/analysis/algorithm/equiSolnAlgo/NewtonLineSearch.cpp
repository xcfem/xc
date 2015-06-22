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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.5 $
// $Date: 2005/11/29 22:42:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/NewtonLineSearch.cpp,v $

// Written: fmk
// Created: 11/96
// Modified: Ed "C++" Love 10/00 to perform the line search
//
// Description: This file contains the implementation for XC::NewtonLineSearch.
//
// What: "@(#)NewtonLineSearch.h, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/NewtonLineSearch.h>
//Búsqueda lineal.
#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/LineSearch.h>
#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/BisectionLineSearch.h>
#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/InitialInterpolatedLineSearch.h>
#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/RegulaFalsiLineSearch.h>
#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/SecantLineSearch.h>

#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/analysis/StaticAnalysis.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <utility/matrix/ID.h>
#include "solution/SoluMethod.h"
#include "xc_utils/src/base/CmdStatus.h"


//! @brief Null Constructor
XC::NewtonLineSearch::NewtonLineSearch(SoluMethod *owr)
  :EquiSolnAlgo(owr,EquiALGORITHM_TAGS_NewtonLineSearch), theLineSearch(nullptr) {}

XC::NewtonLineSearch::NewtonLineSearch(const NewtonLineSearch &otro)
  : EquiSolnAlgo(otro), theLineSearch(nullptr)
  { if(otro.theLineSearch) copia(otro.theLineSearch); }

XC::NewtonLineSearch &XC::NewtonLineSearch::operator=(const NewtonLineSearch &otro)
  {
    EquiSolnAlgo::operator=(otro);
    if(otro.theLineSearch) copia(otro.theLineSearch);
    return *this;
  }

void XC::NewtonLineSearch::libera(void)
  {
    if(theLineSearch)
      {
        delete theLineSearch;
        theLineSearch= nullptr;
      }
  }

bool XC::NewtonLineSearch::alloc(const std::string &nmb)
  {
    libera();
    if(nmb=="bisection_line_search")
      theLineSearch=new BisectionLineSearch();
    else if(nmb=="initial_interpolated_line_search")
      theLineSearch=new InitialInterpolatedLineSearch();
    else if(nmb=="regula_falsi_line_search")
      theLineSearch=new RegulaFalsiLineSearch();
    else if(nmb=="secant_line_search")
      theLineSearch=new SecantLineSearch();
    if(theLineSearch)
      theLineSearch->set_owner(this);
    return (theLineSearch!=nullptr);
  }

void XC::NewtonLineSearch::copia(LineSearch *ptr)
  {
    if(ptr)
      {
        libera();
        theLineSearch= ptr->getCopy();
        theLineSearch->set_owner(this);
      }
    else
     std::cerr << "NewtonLineSearch::copia; se pasó un puntero nulo." << std::endl;
  }

//! @brief Lee un objeto LineSearch desde archivo
bool XC::NewtonLineSearch::procesa_cmd_line_search(const std::string &cmd,CmdStatus &status)
  {
    bool retval= false;
    if(cmd.find("_line_search")!=std::string::npos)
      {
        if(alloc(cmd))
          theLineSearch->LeeCmd(status);
        retval= true;
      }
    return retval;
  }

//! @brief Lee un objeto NewtonLineSearch desde archivo.
bool XC::NewtonLineSearch::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(NewtonLineSearch) Procesando comando: " << cmd << std::endl;
    //Line search.
    if(procesa_cmd_line_search(cmd,status))
      return true;
    else if(cmd=="line_search")
      {
        if(theLineSearch)
          theLineSearch->LeeCmd(status);
        else
	  std::cerr << "NewtonLineSearch::procesa_comando; " << cmd << "no está definido el objet LineSearch." << std::endl;
        return true;
      }
    else
      return EntCmd::procesa_comando(status);
  }

XC::NewtonLineSearch::~NewtonLineSearch(void)
  { libera(); }

//! @brief resuelve el paso actual.
int XC::NewtonLineSearch::solveCurrentStep(void)
  {
    // set up some pointers and check they are valid
    // NOTE this could be taken away if we set Ptrs as protecetd in superclass
    AnalysisModel *theAnaModel= getAnalysisModelPtr();
    IncrementalIntegrator *theIntegrator= getIncrementalIntegratorPtr();
    LinearSOE *theSOE= getLinearSOEPtr();
    ConvergenceTest *theTest= getConvergenceTestPtr();

    if((!theAnaModel) || (!theIntegrator) || (!theSOE) || (!theTest))
      {
        std::cerr << "WARNING XC::NewtonLineSearch::solveCurrentStep() - setLinks() has";
        std::cerr << "no se ha asignado modelo, integrador o sistema de ecuaciones.\n";
        return -5;
      }

    theLineSearch->newStep(*theSOE);

    // set itself as the XC::ConvergenceTest objects XC::EquiSolnAlgo
    theTest->set_owner(getSoluMethod());
    if(theTest->start() < 0)
      {
        std::cerr << "XC::NewtonLineSearch::solveCurrentStep() -";
        std::cerr << "the XC::ConvergenceTest object failed in start()\n";
        return -3;
      }

    if(theIntegrator->formUnbalance() < 0)
      {
        std::cerr << "WARNING XC::NewtonLineSearch::solveCurrentStep() -";
        std::cerr << "the XC::Integrator failed in formUnbalance()\n";
        return -2;
      }

    int result = -1;
    do
      {
        //residual at this iteration before next solve
        const Vector &Resid0 = theSOE->getB() ;

        //form the tangent
        if(theIntegrator->formTangent() < 0)
          {
            std::cerr << "WARNING XC::NewtonLineSearch::solveCurrentStep() -";
            std::cerr << "the XC::Integrator failed in formTangent()\n";
            return -1;
          }

        //solve
        if(theSOE->solve() < 0)
          {
            std::cerr << "WARNING XC::NewtonLineSearch::solveCurrentStep() -";
            std::cerr << "the LinearSysOfEqn failed in solve()\n";
            return -3;
          }

        //line search direction
        const Vector &dx0 = theSOE->getX() ;

        //intial value of s
        double s0= - (dx0 ^ Resid0) ;

       if(theIntegrator->update(theSOE->getX()) < 0)
          {
            std::cerr << "WARNING XC::NewtonLineSearch::solveCurrentStep() -";
            std::cerr << "the XC::Integrator failed in update()\n";
            return -4;
          }

        if(theIntegrator->formUnbalance() < 0)
          {
            std::cerr << "WARNING XC::NewtonLineSearch::solveCurrentStep() -";
            std::cerr << "the XC::Integrator failed in formUnbalance()\n";
            return -2;
          }

        //nuevo residuo
        const Vector &Resid = theSOE->getB() ;

        //nuevo valor de s
        const double s= - ( dx0 ^ Resid );

        if(theLineSearch)
          theLineSearch->search(s0, s, *theSOE, *theIntegrator);

        this->record(0); //Llama al método record(...) de todos los recorders definidos.
        result = theTest->test();
      }
    while (result == -1);

    if(result == -2)
      {
        std::cerr << "XC::NewtonLineSearch::solveCurrentStep() -";
        std::cerr << "the XC::ConvergenceTest object failed in test()\n";
        return -3;
      }
    // note - if postive result we are returning what the convergence test returned
    // which should be the number of iterations
    return result;
  }

int XC::NewtonLineSearch::sendSelf(CommParameters &cp)
  { return -1; }

int XC::NewtonLineSearch::recvSelf(const CommParameters &cp)
  { return -1; }

void XC::NewtonLineSearch::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      s << "NewtonLineSearch\n";
    if(theLineSearch != 0)
      theLineSearch->Print(s, flag);
  }









