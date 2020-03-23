// -*-c++-*-
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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/10/30 22:46:56 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/DiscretizedRandomProcessSeries.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu), February 2002
// Revised: 
//

#ifndef DiscretizedRandomProcessSeries_h
#define DiscretizedRandomProcessSeries_h

#include <domain/load/pattern/TimeSeries.h>
#include "utility/matrix/Vector.h"


namespace XC {
  class ModulatingFunction;
  class Vector;

//! @ingroup TSeries
//
//! @brief ??.
class DiscretizedRandomProcessSeries: public TimeSeries
  {
  private:
    int numModFuncs;
    double c;
    double mean;
    double maxStdv;
    double maxStdvTime;
    std::vector<ModulatingFunction *> theModulatingFunctions;
    Vector randomVariables;
    Vector kickInTimes;
    int parameterID;

  protected:

  public:
    DiscretizedRandomProcessSeries(int num, const std::vector<ModulatingFunction *> &theModFuncs,double p_mean,double targetStdv);
    TimeSeries *getCopy(void) const;

    // method to get load factor
    double getFactor(double pseudoTime) const;

    // None of the following functions should be invoked on this type
    // of object
    double getDuration(void) const {return 0.0;} // dummy function
    double getPeakFactor(void) const {return 0.0;} // dummy function
    double getTimeIncr(double pseudoTime) const {return 1.0;} // dummy function
    
    // methods for output    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;
    
// AddingSensitivity:BEGIN //////////////////////////////////////////
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter  (int parameterID, Information &info);
    int activateParameter(int parameterID);
    double getFactorSensitivity(double time);
// AddingSensitivity:END ////////////////////////////////////////////

  };
} // end of XC namespace

#endif
