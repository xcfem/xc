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

#ifndef SteelBase0103_h
#define SteelBase0103_h

#include "material/uniaxial/steel/SteelBase.h"


namespace XC {

// Default values for isotropic hardening parameters a1, a2, a3, and a4
const double STEEL_0103_DEFAULT_A1= 0.0;
const double STEEL_0103_DEFAULT_A2= 55.0;
const double STEEL_0103_DEFAULT_A3= 0.0;
const double STEEL_0103_DEFAULT_A4= 55.0;

//! @ingroup MatUnx
//
//! @brief Base class for Steel01 and Steel03.
class SteelBase0103: public SteelBase
  {
  protected:
    /*** CONVERGED History Variables ***/
    double CminStrain;  //!< Minimum strain in compression
    double CmaxStrain;  //!< Maximum strain in tension
    double CshiftP;     //!< Shift in hysteresis loop for positive loading
    double CshiftN;     //!< Shift in hysteresis loop for negative loading
    int Cloading;       //!< Flag for loading/unloading
                        // 1 = loading (positive strain increment)
                        // -1 = unloading (negative strain increment)
                        // 0 initially

    /*** CONVERGED State Variables ***/
    double Cstrain;
    double Cstress;
    double Ctangent;

    /*** TRIAL State Variables ***/
    double Tstrain;
    double Tstress;
    double Ttangent; // Not really a state variable, but declared here
                     // for convenience

    /*** TRIAL History Variables ***/
    double TminStrain;
    double TmaxStrain;
    double TshiftP;
    double TshiftN;
    int Tloading;

    virtual void determineTrialState(double dStrain)= 0;

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    virtual int setup_parameters(void);
  public:
    SteelBase0103(int tag, int classTag, double fy, double E0, double b,
       double a1 = STEEL_0103_DEFAULT_A1, double a2 = STEEL_0103_DEFAULT_A2,
       double a3 = STEEL_0103_DEFAULT_A3, double a4 = STEEL_0103_DEFAULT_A4);
    SteelBase0103(int tag, int classTag);
    SteelBase0103(int classTag);

    int setTrialStrain(double strain, double strainRate = 0.0);
    int setTrial(double strain, double &stress, double &tangent, double strainRate = 0.0);
    double getStrain(void) const;
    double getStress(void) const;
    double getTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif
