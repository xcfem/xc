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
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.2 $
// $Date: 2003/03/04 00:46:02 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/FEsensitivity/StaticSensitivityIntegrator.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#ifndef StaticSensitivityIntegrator_h
#define StaticSensitivityIntegrator_h

#include <reliability/FEsensitivity/SensitivityIntegrator.h>
#include <solution/analysis/integrator/StaticIntegrator.h>
namespace XC {
class AnalysisModel;

class StaticSensitivityIntegrator : public SensitivityIntegrator, public StaticIntegrator
  {
  private:
    int gradNumber;
  public:
    StaticSensitivityIntegrator(SoluMethod *owr);
    

	// Methods promised by the ordinary integrator
    int newStep(void);    
    int update(const Vector &deltaU);
    int setDeltaLambda(double newDeltaLambda);
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);    

    // Sensitivity related methods
    int formEleResidual(FE_Element *theEle);
    int formSensitivityRHS(int gradNum);
    int formIndependentSensitivityRHS();
    int saveSensitivity(const Vector &v, int gradNum, int numGrads);
    int commitSensitivity(int gradNum, int numGrads);
  };
} // end of XC namespace

#endif

