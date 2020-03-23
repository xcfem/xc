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
//ResponseQuantities.h                                        
                                                                        
#ifndef ResponseQuantities_h
#define ResponseQuantities_h


#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"

namespace XC {

//! @ingroup TransientIntegrator
//
//! @brief Response quantities.
class ResponseQuantities: public MovableObject
  {
  private:
    Vector R, Rdot, Rdotdot; //!< Response quantities
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    ResponseQuantities(const int &size= 0);

    inline const Vector &get(void) const
      { return R; }
    inline const Vector &getDot(void) const
      { return Rdot; }
    inline const Vector &getDotDot(void) const
      { return Rdotdot; }
    inline Vector &get(void)
      { return R; }
    inline Vector &getDot(void)
      { return Rdot; }
    inline Vector &getDotDot(void)
      { return Rdotdot; }
    inline void set(const Vector &v)
      { R= v; }
    inline void setDot(const Vector &v)
      { Rdot= v; }
    inline void setDotDot(const Vector &v)
      { Rdotdot= v; }

    void setDisp(const ID &,const Vector &);
    void setVel(const ID &,const Vector &);
    void setAccel(const ID &,const Vector &);
    
    inline const Vector &getDisp(void) const
      { return R; }
    inline const Vector &getVel(void) const
      { return Rdot; }
    inline const Vector &getAccel(void) const
      { return Rdotdot; }

    void resize(const int &size);
    void Zero(void);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    void Print(std::ostream &s) const;        
  };
} // end of XC namespace

#endif

