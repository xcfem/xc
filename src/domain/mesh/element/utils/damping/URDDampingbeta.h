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

// $Revision: 1.0 $
// $Date: 2021-07-02 14:29:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/damping/URDDampingbeta.h,v $

// Revised by: Y Tian
// Created: 02/2020
// Revision: A
//
// Description: This file contains the definition for the URDDampingbeta class.
// URDDampingbeta provides the abstraction of an elemental damping imposition
// providing user-defined damping over a frequency range
//
// Reference:
// 

// What: "@(#) URDDampingbeta.h, revA"

#ifndef URDDampingbeta_h
#define URDDampingbeta_h

#include "UDampingBase.h"

namespace XC {

class URDDampingbeta: public UDampingBase
  {
  private:
    Vector beta;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    URDDampingbeta(int tag, int nFilter, const Vector &omegac, const Vector &beta, double ta, double td, TimeSeries *fac);
  
    URDDampingbeta();
  
    int Initialize(void);
  
    int update(const Vector &q);
  
    double getStiffnessMultiplier(void) const;
  
    Damping *getCopy(void) const;
  
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    void Print(std::ostream &s, int flag = 0);
  };
} // end of XC namespace

#endif
