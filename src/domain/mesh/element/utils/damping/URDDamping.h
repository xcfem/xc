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
// $Source: /usr/local/cvs/OpenSees/SRC/damping/URDDamping.h,v $

// Revised by: Y Tian
// Created: 02/2020
// Revision: A
//
// Description: This file contains the definition for the URDDamping class.
// URDDamping provides the abstraction of an elemental damping imposition
// providing user-defined damping over a frequency range
//
// Reference:
// 

// What: "@(#) URDDamping.h, revA"

#ifndef URDDamping_h
#define URDDamping_h

#include "UDampingBase.h"
#include "utility/matrix/Matrix.h"
#include <tuple>

namespace XC {

class URDDamping: public UDampingBase
  {
  private:
    // internal data
    int numfreq, prttag, maxiter;
    double dptol;
    Vector alpha, omegaetaf;
    Vector Freqlog, Fredif, Freqk, Freqb; 
    Matrix etaFreq;
    
    std::tuple<int, int> compute_count_freq(int numfreq, const double &df, Vector &);
    bool computeErrorA(const int &, const double &, const double &, const Vector &, const double &, const double &) const;
    bool computeErrorB(const double &) const;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);  
  public:
    URDDamping(int tag, int numfreq, const Matrix &etaFreq, double dptol, double ta, double td, TimeSeries *fac, int prttag, int maxiter);
    URDDamping(int tag, int numfreq, const Matrix &etaFreq, double dptol, double ta, double td, TimeSeries *fac, int nFilter, const Vector &alpha, const Vector &omegac, const Vector &omegaetaf, int prttag, int maxiter);

    URDDamping();

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
