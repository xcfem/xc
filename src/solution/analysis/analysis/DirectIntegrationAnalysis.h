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
// $Date: 2005/11/29 23:36:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/DirectIntegrationAnalysis.h,v $
                                                                        
                                                                        
#ifndef DirectIntegrationAnalysis_h
#define DirectIntegrationAnalysis_h

// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for 
// DirectIntegrationAnalysis. DirectIntegrationAnalysis is a 
// subclass of TransientAnalysis. It is used to perform a 
// dynamic analysis on the FE\_Model using a direct integration scheme.
//
// What: "@(#) DirectIntegrationAnalysis.h, revA"

#include <solution/analysis/analysis/TransientAnalysis.h>

namespace XC {
class ConvergenceTest;

// AddingSensitivity:BEGIN ///////////////////////////////
#ifdef _RELIABILITY
class SensitivityAlgorithm;
#endif
// AddingSensitivity:END ///////////////////////////////

//! @ingroup TipoAnalisis
//
//! @brief Direct integration dynamic analysis.
class DirectIntegrationAnalysis: public TransientAnalysis
  {
  private:
    int domainStamp;
    // AddingSensitivity:BEGIN ///////////////////////////////
#ifdef _RELIABILITY
    SensitivityAlgorithm *theSensitivityAlgorithm;
#endif
    // AddingSensitivity:END ///////////////////////////////
  protected:
    friend class ProcSolu;
    DirectIntegrationAnalysis(SoluMethod *metodo);
    Analysis *getCopy(void) const;
  public:
    virtual ~DirectIntegrationAnalysis(void);

    void clearAll(void);

    int analyze(int numSteps, double dT);
    int initialize(void);

    int domainChanged(void);

    int setNumberer(DOF_Numberer &theNumberer);
    int setAlgorithm(EquiSolnAlgo &theAlgorithm);
    int setIntegrator(TransientIntegrator &theIntegrator);
    int setLinearSOE(LinearSOE &theSOE); 
    int setConvergenceTest(ConvergenceTest &theTest);
    
    int checkDomainChange(void);

    // AddingSensitivity:BEGIN ///////////////////////////////
#ifdef _RELIABILITY
    int setSensitivityAlgorithm(SensitivityAlgorithm *theSensitivityAlgorithm);
#endif
    // AddingSensitivity:END /////////////////////////////////
  };
inline Analysis *DirectIntegrationAnalysis::getCopy(void) const
  { return new DirectIntegrationAnalysis(*this); }
} // end of XC namespace

#endif

