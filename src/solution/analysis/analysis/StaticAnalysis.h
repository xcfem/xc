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
                                                                        
// $Revision: 1.6 $
// $Date: 2005/11/29 23:36:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/StaticAnalysis.h,v $
                                                                        
                                                                        
#ifndef StaticAnalysis_h
#define StaticAnalysis_h

// Written: fmk 
// Revision: A
//
// Description: This file contains the interface for the StaticAnalysis
// class. StaticAnalysis is a subclass of Analysis, it is used to perform 
// a static analysis on the FE\_Model.
//
// What: "@(#) StaticAnalysis.h, revA"

#include <solution/analysis/analysis/Analysis.h>

namespace XC {
class ConvergenceTest;

// AddingSensitivity:BEGIN ///////////////////////////////
#ifdef _RELIABILITY
class SensitivityAlgorithm;
#endif
// AddingSensitivity:END ///////////////////////////////

//! @ingroup TipoAnalisis
//
//! @brief Análisis estático.
class StaticAnalysis: public Analysis
  {
  protected:
    int domainStamp;

// AddingSensitivity:BEGIN ///////////////////////////////
#ifdef _RELIABILITY
    SensitivityAlgorithm *theSensitivityAlgorithm;
#endif
// AddingSensitivity:END ///////////////////////////////

    int new_domain_step(int num_step);
    int check_domain_change(int num_step,int numSteps);
    int new_integrator_step(int num_step);
    int solve_current_step(int num_step);
    int compute_sensitivities_step(int num_step);
    int commit_step(int num_step);
    int run_analysis_step(int num_step,int numSteps);

    friend class ProcSolu;
    StaticAnalysis(SoluMethod *metodo);
    Analysis *getCopy(void) const;
  public:

    ~StaticAnalysis(void);

    void clearAll(void);	    
    
    virtual int analyze(int numSteps);
    int initialize(void);
    int domainChanged(void);

    int setNumberer(DOF_Numberer &theNumberer);
    int setAlgorithm(EquiSolnAlgo &theAlgorithm);
    int setIntegrator(StaticIntegrator &theIntegrator);
    int setLinearSOE(LinearSOE &theSOE);
    int setConvergenceTest(ConvergenceTest &theTest);

    ConvergenceTest *getConvergenceTest(void);

    // AddingSensitivity:BEGIN ///////////////////////////////
#ifdef _RELIABILITY
    int setSensitivityAlgorithm(SensitivityAlgorithm *theSensitivityAlgorithm);
#endif
    // AddingSensitivity:END /////////////////////////////////
  };
inline Analysis *StaticAnalysis::getCopy(void) const
  { return new StaticAnalysis(*this); }
} // end of XC namespace

#endif
