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
//  YS_Evolution2D.h
//
//////////////////////////////////////////////////////////////////////

#ifndef YS_EVOLUTION2D_H
#define YS_EVOLUTION2D_H

#include "YS_Evolution.h"

namespace XC {
//! @ingroup YSEvolution
//!
//! @brief 2D yield surface evolution law.
class YS_Evolution2D: public YS_Evolution
  {
  protected:
//  double sumPlasticDeformX, sumPlasticDeformX_hist;
//  double sumPlasticDeformY, sumPlasticDeformY_hist;
    bool   softening;
    static Vector v2;
    double minIsoFactor;
    YieldSurface_BC *tmpYSPtr;
  public:
    YS_Evolution2D(int tag, int classTag, double min_iso_factor,
                        double iso_ratio, double kin_ratio);

//  Methods inherited
    virtual void Print(std::ostream &s, int flag =0) const=0;
    virtual YS_Evolution *getCopy(void) = 0;
    virtual Response *setResponse(const std::vector<std::string> &argv, Information &info);
    virtual int getResponse(int responseID, Information &info);
    
    virtual int update(int flag);

    virtual int  sendSelf(CommParameters &){return -1;}
    virtual int  recvSelf(const CommParameters &){return -1;}

    virtual int commitState();
    virtual int revertToLastCommit(void);
    virtual const Vector &getEquiPlasticStiffness(void)=0;

    // no checks are performed
    virtual int evolveSurface(YieldSurface_BC *ys, double magPlasticDefo, Vector &G, Vector &F_Surface, int flag=0);

protected:
    virtual void setTrialPlasticStrains(double ep, const Vector &f, const Vector &g)=0;
    virtual double getIsoPlasticStiffness(int dir)=0;
    virtual double getKinPlasticStiffness(int dir)=0;
    virtual Vector& getEvolDirection(Vector &f_new)=0;

  };
} // end of XC namespace

#endif

