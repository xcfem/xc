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
//IntegratorVectors.h

#ifndef IntegratorVectors_h
#define IntegratorVectors_h

#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"

namespace XC {

//! @ingroup Integrator
//
//! @brief Vectors used by many integrators.
class IntegratorVectors: public MovableObject
  {
  protected:
    Vector deltaUhat, deltaUbar, deltaU, deltaUstep;
    Vector phat; // the reference load vector
    double deltaLambdaStep, currentLambda;

    void resize(const int &size);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    IntegratorVectors(void);

    inline const Vector &getPhat(void) const
      { return phat; }
    inline Vector &getPhat(void)
      { return phat; }

    inline const Vector &getDeltaUhat(void) const
      { return deltaUhat; }
    inline void setDeltaUhat(const Vector &v)
      { deltaUhat= v; }

    inline const Vector &getDeltaUbar(void) const
      { return deltaUbar; }
    inline void setDeltaUbar(const Vector &v)
      { deltaUbar= v; }

    inline const Vector &getDeltaUstep(void) const
      { return deltaUstep; }
    inline void setDeltaUstep(const Vector &v)
      { deltaUstep= v; }

    inline const Vector &getDeltaU(void) const
      { return deltaU; }
    inline void setDeltaU(const Vector &v)
      { deltaU= v; }

    inline const double &getDeltaLambdaStep(void) const
      { return deltaLambdaStep; }
    inline void setDeltaLambdaStep(const double &d)
      { deltaLambdaStep= d; }

    inline const double &getCurrentLambda(void) const
      { return currentLambda; }
    inline double &getCurrentLambda(void)
      { return currentLambda; }
    inline void setCurrentLambda(const double &d)
      { currentLambda= d; }

    void newStep(const double &dLambda,const Vector &);
    void determineUhat(LinearSOE &);
    void distribDetermineUhat(const int &,LinearSOE &);
    void solve(const Vector &,LinearSOE &);
    void update(const double &);
    void domainChanged(const size_t &sz,IncrementalIntegrator &,LinearSOE &);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif

