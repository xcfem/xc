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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/LoadPath.h,v $
                                                                        
                                                                        
#ifndef LoadPath_h
#define LoadPath_h

// File: ~/analysis/integrator/LoadPath.h
// 
// Written: fmk 
// Created: 05/99
// Revision: A
//
// Description: This file contains the class definition for LoadPath.
// LoadPath is an algorithmic class for perfroming a static analysis
// using a user defined load path (a user specified lambda path)
//
// What: "@(#) LoadPath.h, revA"

#include "solution/analysis/integrator/StaticIntegrator.h"
#include "utility/matrix/Vector.h"

namespace XC {
class LinearSOE;
class AnalysisModel;

//! @ingroup StaticIntegrator
//
//! @brief LoadPath is an algorithmic class for performing a static analysis
//! using a user defined load path (a user specified lambda path).
class LoadPath: public StaticIntegrator
  {
  private:
    Vector loadPath;
    int currentStep;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class SoluMethod;
    LoadPath(SoluMethod *);
    LoadPath(SoluMethod *,const Vector &theLoadPath);
    Integrator *getCopy(void) const;
  public:

    int newStep(void);    
    int update(const Vector &deltaU);

    // Public methods for Output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
inline Integrator *LoadPath::getCopy(void) const
  { return new LoadPath(*this); }
} // end of XC namespace

#endif

