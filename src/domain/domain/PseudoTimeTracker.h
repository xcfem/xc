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
//PseudoTimeTracker.h

// Written: lcpt

#ifndef PseudoTimeTracker_h
#define PseudoTimeTracker_h

#include "utility/actor/actor/MovableObject.h"
#include <iostream>

namespace XC {

//! @brief Registro del tiempo.
class PseudoTimeTracker: public MovableObject
  {
  private:
    double currentTime; //!< current pseudo time
    double committedTime; //!< the committed pseudo time
    double dT; //!< difference between committed and current time
    double eigenvalueTimeSet;
  protected:
    friend class Domain;
    PseudoTimeTracker(void);
    void setCurrentTime(const double &newTime);
    void setCommittedTime(const double &newTime);
    void setEigenvalueTime(void);    
    void Inic(const double &newTime);
    void Zero(void);

    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    const double &getCurrentTime(void) const;
    const double &getCommittedTime(void) const;
    const double &getDt(void) const;
    const double &getEigenValueTimeSet(void) const;

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);

    void Print(std::ostream &s, int flag =0) const;
  };

std::ostream &operator<<(std::ostream &s,const PseudoTimeTracker &tt);
} // end of XC namespace
#endif
