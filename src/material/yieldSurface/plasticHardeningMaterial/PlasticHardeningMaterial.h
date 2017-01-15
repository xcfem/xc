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
#ifndef PlasticHardeningMaterial_h
#define PlasticHardeningMaterial_h

#include "material/Material.h"

namespace XC {
class Information;
class Response;

//! @ingroup Mat
//
//! @brief Material plástico con endurecimiento por deformación.
class PlasticHardeningMaterial : public Material
  {
  protected:
    double val_hist, val_trial;
    double residual, sFactor;
  public:
    PlasticHardeningMaterial (int tag, int classTag);

	// could be delta_plastic, |back-stress|, 
	// distance between force-point on YS and conjugate
	// point on Bounding-Surface
    virtual int setTrialValue(double xVal, double factor);
    virtual int setTrialIncrValue(double dxVal);
    void setResidual(double res=1);

    const Vector &getGeneralizedStress(void) const;
    const Vector &getGeneralizedStrain(void) const;

    virtual void setInitialGeneralizedStrain(const Vector &);
    const Vector &getInitialGeneralizedStrain(void) const;

    virtual int commitState(void);
    virtual int revertToLastCommit(void);
    virtual int revertToStart(void);

    virtual double getTrialPlasticStiffness()=0;
    double getTrialValue(void);
    virtual PlasticHardeningMaterial *getCopy(void)= 0;

    virtual Response *setResponse(const std::vector<std::string> &argv, Information &matInformation);
    virtual int getResponse (int responseID, Information &matInformation);
    virtual void Print(std::ostream &s, int flag =0);

    virtual int sendSelf(CommParameters &)
      {return -1;}
    virtual int recvSelf(const CommParameters &)
      {return -1;}
  };
} // end of XC namespace


#endif

