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
//LinearSOEData.h
                                                                        
                                                                        
#ifndef LinearSOEData_h
#define LinearSOEData_h

#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include "utility/matrix/Vector.h"

namespace XC {

//! @ingroup SOE
//
//! @brief Data (size, arrays,...) for linear system of equations.
class LinearSOEData : public LinearSOE
  {
  protected:
    int size; //! order of A
    Vector B, X;   //! 1d arrays containing coefficients of B and X  

    void inic(const size_t &);
    inline const double &getB(const size_t &i) const
      { return B[i]; }
    inline double &getB(const size_t &i)
      { return B(i); }
    inline const double &getX(const size_t &i) const
      { return X(i); }
    inline double &getX(const size_t &i)
      { return X(i); }
    inline double *getPtrB(void)
      { return B.getDataPtr(); }
    inline const double *getPtrB(void) const
      { return B.getDataPtr(); }
    inline double *getPtrX(void)
      { return X.getDataPtr(); }
    inline const double *getPtrX(void) const
      { return X.getDataPtr(); }

    LinearSOEData(SoluMethod *,int classTag,int N= 0);
  public:
    virtual int getNumEqn(void) const;
    virtual void zeroB(void);
    virtual void zeroX(void);
    virtual void zero(void);

    virtual int addB(const Vector &, const double &fact= 1.0);    
    virtual int addB(const Vector &, const ID &,const double &fact= 1.0);    
    virtual int setB(const XC::Vector &v,const double &fact= 1.0);
    virtual void setX(int loc, double value);
    virtual void setX(const Vector &x);
    virtual const Vector &getX(void) const;
    virtual Vector &getX(void);
    virtual const Vector &getB(void) const;
    virtual Vector &getB(void);
    virtual double normRHS(void) const;

    void receiveB(const CommParameters &);
    void receiveX(const CommParameters &);
    void receiveBX(const CommParameters &);
    void sendB(CommParameters &) const;
    void sendBX(CommParameters &) const;
  };
} // end of XC namespace


#endif

