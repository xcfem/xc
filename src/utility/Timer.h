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

