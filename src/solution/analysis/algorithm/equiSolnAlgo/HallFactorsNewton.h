// -*-c++-*-
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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/29 22:42:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/HallFactorsNewton.h,v $
                                                                        
                                                                        
#ifndef HallFactorsNewton_h
#define HallFactorsNewton_h

#include <solution/analysis/algorithm/equiSolnAlgo/NewtonBased.h>

namespace XC {

//! @brief Base class for algorithms based on Newton method that have
//! Hall factors (cFactor and iFactor).
//!
//! @ingroup EQSolAlgo
class HallFactorsNewton: public NewtonBased
  {
  protected:
    int numIterations;
    int factorOnce;

    double iFactor;
    double cFactor;
    int sendData(Communicator &);
    int recvData(const Communicator &);
    
    HallFactorsNewton(SolutionStrategy *, int classTag, int tangent= CURRENT_TANGENT, double iFactor= 0.0, double cFactor= 1.0, int factOnce= 0);
  public:

    void useInitialTangent(void);
    void useSecantTangent(int factOnce= 0);
    void useHallTangent(double iFactor= 0.1, double cFactor= 0.9, int factOnce= 0);
    void useCurrentTangent(int factOnce= 0);
    
    inline int getNumIterations(void) const
      { return numIterations; }
    inline void setNumIterations(const int &ni)
      { this->numIterations= ni; }
    
    inline int getFactorOnce(void) const
      { return factorOnce; }
    inline void setFactorOnce(const int &fo)
      { this->factorOnce= fo; }
    
    inline int getIFactor(void) const
      { return this->iFactor; }
    inline void setIFactor(const double &i_factor)
      { this->iFactor= i_factor; }
    
    inline int getCFactor(void) const
      { return this->cFactor; }
    inline void setCFactor(const double &c_factor)
      { this->cFactor= c_factor; }
    
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
    
    void Print(std::ostream &, int) const;
  };

} // end of XC namespace

#endif


