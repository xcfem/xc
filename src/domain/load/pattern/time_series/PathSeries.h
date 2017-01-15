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
// $Date: 2003/02/25 23:32:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/PathSeries.h,v $
                                                                        
                                                                        
#ifndef PathSeries_h
#define PathSeries_h

// File: ~/analysis/integrator/PathSeries.h
// 
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Description: This file contains the class definition for PathSeries.
// PathSeries is a PathSeriesBase class which linear interpolates the
// load factor using user specified control points provided in a vector object.
// the points in the vector are given at regular time increments pathTimeIncr
// apart. (could be provided in another vector if different)
//
// What: "@(#) PathSeries.h, revA"

#include "PathSeriesBase.h"
#include <string>

namespace XC {
class Vector;

//! @ingroup TSeries
//
//! @brief Función del tiempo expresada mediante una seríe de puntos (ti,fi) en
//! los que la diferencia entre abcisas consecutivas (incremento de tiempo)
//! ti - t(i-1) es una cantidad constante.
class PathSeries : public PathSeriesBase
  {
  private:
    double pathTimeIncr; //!< Incremento de tiempo.
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    // constructors
    PathSeries(const Vector &thePath,double pathTimeIncr = 1.0, double cfactor = 1.0);
    PathSeries(const std::string &fileName, double pathTimeIncr = 1.0, double cfactor = 1.0);
    PathSeries(void);
    TimeSeries *getCopy(void) const
      { return new PathSeries(*this); }

    // method to get factor
    double getFactor(double pseudoTime) const;
    double getDuration(void) const;
    double getTimeIncr(double pseudoTime) const {return pathTimeIncr;}

    inline double getPathTimeIncr(void) const
      { return pathTimeIncr; }
    inline void setPathTimeIncr(const double &d)
      { pathTimeIncr= d; }

    void readFromFile(const std::string &fileName);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);


    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif

