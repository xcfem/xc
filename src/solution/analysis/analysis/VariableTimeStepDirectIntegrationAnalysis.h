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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/11/29 23:36:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.h,v $
                                                                        
                                                                        
#ifndef VariableTimeStepDirectIntegrationAnalysis_h
#define VariableTimeStepDirectIntegrationAnalysis_h

// File: ~/analysis/analysis/VariableTimeStepDirectIntegrationAnalysis.h
// 
// Written: fmk 
// Created: 10/00
// Revision: A
//
// Description: This file contains the class definition for 
// VariableTimeStepDirectIntegrationAnalysis. VariableTimeStepDirectIntegrationAnalysis 
// is a subclass of DirectIntegrationAnalysis. It is used to perform a 
// dynamic analysis on the FE\_Model using a direct integration scheme.  
//
// What: "@(#) VariableTimeStepDirectIntegrationAnalysis.h, revA"

#include <solution/analysis/analysis/DirectIntegrationAnalysis.h>

namespace XC {
class TransientIntegrator;
class ConvergenceTest;

//! @ingroup TipoAnalisis
//
//! @brief Análisis con paso variable en el tiempo.
class VariableTimeStepDirectIntegrationAnalysis: public DirectIntegrationAnalysis
  {
  protected:
    virtual double determineDt(double dT, double dtMin, double dtMax, int Jd,ConvergenceTest *theTest);

    friend class ProcSolu;
    VariableTimeStepDirectIntegrationAnalysis(SoluMethod *metodo);
    Analysis *getCopy(void) const;
  public:

    int analyze(int numSteps, double dT, double dtMin, double dtMax, int Jd);
  };
inline Analysis *VariableTimeStepDirectIntegrationAnalysis::getCopy(void) const
  { return new VariableTimeStepDirectIntegrationAnalysis(*this); }
} // end of XC namespace

#endif

