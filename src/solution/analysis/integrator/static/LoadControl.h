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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/LoadControl.h,v $
                                                                        
                                                                        
#ifndef LoadControl_h
#define LoadControl_h

// File: ~/analysis/integrator/LoadControl.h
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for LoadControl.
// LoadControl is an algorithmic class for perfroming a static analysis
// using a load control integration scheme.
//
// What: "@(#) LoadControl.h, revA"

#include "solution/analysis/integrator/static/BaseControl.h"

namespace XC {

//! @ingroup StaticIntegrator
//
//! @brief Especifica el factor incremental a aplicar a las cargas para cada
//! paso of the analysis.
class LoadControl : public BaseControl
  {
  private:
    double deltaLambda;  //!< Valor de dLambda en el paso (i-1).
    double dLambdaMin, dLambdaMax; //!< Valores máximo y mínimo para dlambda en el paso (i).
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);


    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    LoadControl(SoluMethod *,double deltaLambda= 1.0, int numIncr= 1, double minLambda=1.0, double maxlambda=1.0);
    Integrator *getCopy(void) const;
  public:

    int newStep(void);    
    int update(const Vector &deltaU);
    void setDeltaLambda(const double &);
    inline double getDeltaLambda(void) const
      { return deltaLambda; }
    void setDeltaLambdaMin(const double &);
    inline double getDeltaLambdaMin(void) const
      { return dLambdaMin; }
    void setDeltaLambdaMax(const double &);
    inline double getDeltaLambdaMax(void) const
      { return dLambdaMax; }

    // Public methods for Output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0);    
  };
inline Integrator *LoadControl::getCopy(void) const
  { return new LoadControl(*this); }
} // end of XC namespace

#endif

