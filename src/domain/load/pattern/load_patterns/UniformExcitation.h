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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/03/04 00:48:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/UniformExcitation.h,v $
                                                                        
                                                                        
// File: ~/domain/load/UniformExcitation.h
//
// Written: fmk 11/98
// Revised:
//
// Purpose: This file contains the class definition for UniformExcitation.
// UniformExcitation is a concrete class. It sets the R for a single
// ground motion acting on a structure.

#ifndef UniformExcitation_h
#define UniformExcitation_h

#include "EarthquakePattern.h"

namespace XC {
//! @ingroup LPatterns
//
//! @brief Load pattern for a earthquake with a similar
//! excitation for all supports.
//!
//! A UniformExcitation is an object which adds the loads imposed
//! by a single ground excitation to the model. For a UniformExcitation
//! this means that the \p R matrix at each node will have \f$1\f$ column
//! and all entries but those corresponding to the degree of freedom
//! direction will be set to \f$0\f$, the value for the degree of freedom
//! direction will be set to \f$1\f$.
class UniformExcitation: public EarthquakePattern
  {
  private:
    GroundMotion *theMotion; //!< the ground motion
    int theDof; //!< the dof corresponding to the ground motion
    double vel0; //!< the initial velocity, should be neg of ug dot(0)

    UniformExcitation(const UniformExcitation &otro);
    UniformExcitation &operator=(const UniformExcitation &otro);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    UniformExcitation(int tag= 0);
    UniformExcitation(GroundMotion &theMotion, int dof, int tag, double vel0 = 0.0);

    GroundMotion &getGroundMotionRecord(void);
    
    void setDomain(Domain *theDomain);
    void applyLoad(double time);
    void Print(std::ostream &s, int flag =0);

    //! @brief set the affected dof.
    inline void setDof(const int &dof)
      { theDof= dof; }
    //! @brief return the affected dof.
    inline int getDof(void) const
      { return theDof; }
    //! @brief set initial velocity.
    inline void setInitialVelocity(const int &v0)
      { vel0= v0; }
    //! @brief return initial velocity.
    inline int getInitialVelocity(void) const
      { return vel0; }
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    // AddingSensitivity:BEGIN /////////////////////////////////
    void applyLoadSensitivity(double time);
    // AddingSensitivity:END ///////////////////////////////////

  };
} // end of XC namespace

#endif
