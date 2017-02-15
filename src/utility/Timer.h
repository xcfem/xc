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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:02:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/Timer.h,v $
                                                                        
                                                                        
// File: ~/utility/Timer.h
//
// Written: fmk 
// Created: Mar 1997
// Revision: A
//
// Description: This file contains the class definition for Timer.
// Timer is a stopwatch.
//
// What: "@(#) Timer.h, revA"

#ifndef Timer_h
#define Timer_h

#ifdef _WIN32

#else
#ifdef _MAC
#include <sys/time.h>
#else

#include <time.h>
#include <unistd.h>

#endif
#include <sys/times.h>
#include <sys/resource.h>
#endif

#include <iostream>

namespace XC {
//!  @defgroup Utils Utilities.

//! @ingroup Utils
//
class Timer
  {
  private:
    clock_t t1, t2;
    struct tms tmsstart, tmsend;
    struct rusage r1usage, r2usage;
    struct rusage *r1us, *r2us;
  public:
    Timer(void);    

    void start(void);
    void pause(void);
    double getReal(void) const;
    double getCPU(void) const;
    int getNumPageFaults(void) const;
    
    virtual void Print(std::ostream &s) const;   
    friend std::ostream &operator<<(std::ostream &s, const Timer &E);    
  };
std::ostream &operator<<(std::ostream &s, const Timer &E);    
} // end of XC namespace


#endif

