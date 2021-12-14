//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
// $Date: 2003/02/14 23:02:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/Timer.cpp,v $
                                                                        
                                                                        
// File: ~/utility/Timer.C
//
// Written: fmk 
// Created: Mar 1997
// Revision: A
//
// Description: This file contains the class definition for XC::Timer.
// Timer is a stopwatch.
//
// What: "@(#) Timer.h, revA"

#include <utility/Timer.h>


#ifdef NOW
extern "C" int getrusage(int who, struct rusage *rusage);
#endif

#ifndef CLK_TCK
#define CLK_TCK sysconf(_SC_CLK_TCK)
#endif

//! @brief Default constructor.
XC::Timer::Timer() 
  {
    r1us= &r1usage;
    r2us= &r2usage;
  }

//! @brief Sets the accounting variables to mark the start of accounting period
//! using the unix functions times() and \p getrusage.
void XC::Timer::start(void)
  {
    t1= times(&tmsstart);
    getrusage(0,r1us);
  }

//! @brief Sets the accounting variables to mark the end of accounting period
//! using the unix functions times() and \p getrusage.
void XC::Timer::pause(void)
  {
    t2= times(&tmsend);
    getrusage(0,r2us);    
  } 


//! @brief Uses the difference between the starting and ending accounting
//! variables to determine the elapsed real time between the last calls to
//! start()} and {\em pause(). Returns this value in units of seconds.
double XC::Timer::getReal(void) const
  {
    const long clktck= CLK_TCK;    
    const double Real= (t2-t1)/(double) clktck;
    return Real;
  }    

//! @brief Uses the difference between the starting and ending accounting
//! variables to determine the CPU time allocated the process between the
//! last calls to start()} and {\em pause(). Returns this value in
//! units of seconds.
double XC::Timer::getCPU(void) const
  {
    const long clktck= CLK_TCK;
    const double CPU= (tmsend.tms_utime - tmsstart.tms_utime)/(double) clktck;
    return CPU;
  }    

//! @brief Uses the difference between the starting and ending accounting
//! variables to determine the number of page faults that required reading
//! of pages from disk between the last calls to start() and {\em
//! pause()}. Returns this value.
int XC::Timer::getNumPageFaults(void) const
  {
    const int r2yes= r2us->ru_majflt;
    const int r1yes= r1us->ru_majflt;
    return r2yes-r1yes;
  }    

//! @brief Uses the difference between the starting and ending accounting
//! variables to determine the real time, CPU time, operating system time
//! allocate the process, total number of page faults, number of page
//! faults that required reading of pages from memory, and number of page
//! faults that required no reading from disk between the last calls to
//! start()} and {\em pause(). Send these values to \p s.
void XC::Timer::Print(std::ostream &s) const
  {
    const double clktck= double(CLK_TCK);
    const double Real= (t2-t1)/clktck;
    const double CPU= (tmsend.tms_utime - tmsstart.tms_utime)/clktck;
    const double System= (tmsend.tms_stime - tmsstart.tms_stime)/clktck;
    s << std::endl;
    s << "TIME(sec) Real: " << Real << "  CPU: " << CPU;
    s << "   System: " << System << std::endl;

    int r2no= r2us->ru_minflt;
    int r2yes= r2us->ru_majflt;
    int r1no= r1us->ru_minflt;
    int r1yes= r1us->ru_majflt;
    int r1page= r1no + r1yes;
    int r2page= r2no + r2yes;
    
    s << "PAGE FAULTS: " << r2page-r1page << " (NO i/o: ";
    s << r2no-r1no << " YES i/o " << r2yes-r1yes << ") ";

    r2no= r2us->ru_nivcsw;
    r2yes= r2us->ru_nvcsw;
    r1no= r1us->ru_nivcsw;
    r1yes= r1us->ru_nvcsw;
    r1page= r1no + r1yes;
    r2page= r2no + r2yes;

    s << "CONTEXT SWITCHES " << r2page-r1page << " (Invol: ";
    s << r2no-r1no << " Voluntary " << r2yes-r1yes << ") ";

    r2no= r2us->ru_nswap;
    r1no= r1us->ru_nswap;
    r2yes= r2us->ru_maxrss;
    
    s << "Swapped: " << r2no-r1no << " Max Res Set Size: "
      << r2yes << std::endl;
    s << std::endl;
  }    

//! Invokes {\em Print(s)} on the Timer object \p E. 
std::ostream &XC::operator<<(std::ostream &s, const Timer &E)
  {
    E.Print(s);
    return s;
  }


