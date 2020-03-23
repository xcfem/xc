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
// $Date: 2005/02/17 01:15:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/EarthquakePattern.h,v $
                                                                        
                                                                        
#ifndef EarthquakePattern_h
#define EarthquakePattern_h

// File: ~/domain/load/pattern/EarthquakePattern.h
//
// Written: fmk 11/98
// Revised:
//
// Purpose: This file contains the class definition for EarthquakePattern.
// EarthquakePattern is an abstract class.

#include "EQBasePattern.h"
#include "utility/matrix/Vector.h"

namespace XC {
class GroundMotion;

//! @ingroup LPatterns
//
//! @brief Earthquake load pattern.
//!
//! The EarthquakePattern class is an abstract class. An
//! EarthquakePattern is an object which adds earthquake loads to
//! models. This abstract class keeps track of the GroundMotion objects
//! and implements the applyLoad() method. It is up to the concrete
//! subclasses to set the appropriate values of \p R at each node in
//! the model.
class EarthquakePattern: public EQBasePattern
  {
  private:
    Vector uDotG; //!< Velocities.
    Vector uDotDotG; //!< Accelerations.
    double currentTime;

// AddingSensitivity:BEGIN //////////////////////////////////////////
    int parameterID;
// AddingSensitivity:END ///////////////////////////////////////////
    EarthquakePattern(const EarthquakePattern &);
    EarthquakePattern &operator=(const EarthquakePattern &);
  protected:
    void copy(const Vector *,const Vector *);
    int addMotion(GroundMotion &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    EarthquakePattern(int tag, int classTag);

    virtual void applyLoad(double time);
    virtual bool addSFreedom_Constraint(SFreedom_Constraint *);
    virtual bool addNodalLoad(NodalLoad *);
    virtual bool addElementalLoad(ElementalLoad *);

    // methods for o/p
    virtual int sendSelf(CommParameters &) =0;
    virtual int recvSelf(const CommParameters &) =0;

    virtual void Print(std::ostream &s, int flag =0) const;        

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual void applyLoadSensitivity(double pseudoTime = 0.0);
    virtual int  setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int  updateParameter(int parameterID, Information &info);
    virtual int  activateParameter(int parameterID);
    // AddingSensitivity:END ///////////////////////////////////////////

  };
} // end of XC namespace

#endif
