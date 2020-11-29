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
//<<<<<<< T2Vector.h
//<<<<<<< T2Vector.h
// $Revision: 1.7 $
// $Date: 2002/05/16 00:07:47 $
//=======
// $Revision: 1.7 $
// $Date: 2002/05/16 00:07:47 $
//>>>>>>> 1.4
//=======
// $Revision: 1.7 $
// $Date: 2002/05/16 00:07:47 $
//>>>>>>> 1.6
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/T2Vector.h,v $
                                                                        
// Written: ZHY
// Created: August 2000

//
// T2Vector.h
// --------
//

#ifndef _T2Vector_H_
#define _T2Vector_H_

#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"
#include <cfloat>

#define UP_LIMIT    1.0e+30
#define LOW_LIMIT   20.*DBL_EPSILON

namespace XC {
double operator &&(const Vector &, const Vector &);

//! @brief Second order tensor vector class.
//! @ingroup NDMat
class T2Vector: public MovableObject
  {
  private:
    Vector theT2Vector;
    Vector theDeviator;
    double theVolume;
    static Vector engrgStrain;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    // constructors
    T2Vector();
    T2Vector(const Vector & T2Vector_init, int isEngrgStrain=0);
    T2Vector(const Vector & deviat_init, double volume_init);
    
    void setData(const Vector &init, int isEngrgStrain =0);
    void setData(const Vector &deviat, double volume);

    const Vector & t2Vector(int isEngrgStrain=0) const; 
    const Vector & deviator(int isEngrgStrain=0) const;
    double volume() const {return theVolume; }
    const Vector &unitT2Vector() const;
    const Vector &unitDeviator() const;
    double t2VectorLength() const;
    double deviatorLength() const;
    double octahedralShear(int isEngrgStrain=0) const;

    // = -sqrt(3/2*(S:S))/(p+residualPress)
    double deviatorRatio(double residualPress=0.) const; 

    //next function return the angle between two T2Vectors in radians (-PI to PI)
    double angleBetweenT2Vector(const T2Vector &) const; 

    //next function return the angle between deviatoric components of
    //two vectors in radians (-PI to PI)
    double angleBetweenDeviator(const T2Vector &) const; 

    int operator == (const T2Vector & a) const;
    int isZero(void) const;
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
  };
} // end of XC namespace


#endif
