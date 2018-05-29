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
#ifndef CyclicModel_H
#define CyclicModel_H

#include <utility/tagged/TaggedObject.h>
#include <utility/actor/actor/MovableObject.h>

namespace XC {
//! @ingroup OneDimensionalElem
//
//! @brief ??.
class CyclicModel : public TaggedObject, public MovableObject
  {
  private:
    int initNewTask();
  protected:
    double resFactor;
    double cycFactor, cycFactor_hist;
    double f_hist, d_hist;
    double f_curr, d_curr;
    double delT_curr, delT_hist;
    double f_bgn, d_bgn;
    double f_end, d_end;

    bool initYieldPos, initYieldNeg;
    bool   initCyc;
    bool   yielding, yielding_hist;
    double initFpos,initDpos;
    double initFneg,initDneg;
    double initFMag,initDMag;
    double k_init;
    double k_hist, k_curr;
    double fpeakPos, fpeakNeg;
    double dpeakPos, dpeakNeg;
    int state_hist, state_curr;

    const static int Loading,Unloading, Crossover;
    const static double Tol, delK;
  public:
    CyclicModel(int tag, int classTag);
	
    int commitState(double newResidual);
    void update(double f, double d, bool yield);
    virtual CyclicModel *getCopy()=0;
	
    virtual double getFactor(void);
    // for now ignore
    virtual int sendSelf(CommParameters &)
      {return -1;}
    virtual int recvSelf(const CommParameters &)
      {return -1;}
    virtual void Print (std::ostream &s, int flag=0);
  protected:
// virtual methods
    virtual int createFullCycleTask();
    virtual int createHalfCycleTask();
    virtual double getTaskFactor()=0;

// protected:
    int setCurrent(double f, double d);
    int dir(double x);

    int taskStatus(void);
    double rationalize(double x1, double y1, double x2, double y2);
    bool contains(double x1, double x2, double x);
  };
} // end of XC namespace

#endif

