// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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

#ifndef DamperBase_h
#define DamperBase_h

#include "material/uniaxial/UniaxialMaterial.h"

namespace XC {

class DamperBase: public UniaxialMaterial
  {
  protected:
    // Fixed Input Material Variables
    double K; //!< Elastic stiffness of linear spring to model the axial flexibility of a viscous damper (e.g. combined stiffness of the supporting brace and internal damper portion) 
    double C; //!< Damping coefficient.
    double LGap; //!< Gap length to simulate the gap length due to the pin tolerance.
    double NM; //!< Employed adaptive numerical algorithm (default value NM = 1; 1 = Dormand-Prince54, 2=6th order Adams-Bashforth-Moulton, 3=modified Rosenbrock Triple)
    double RelTol; //!< Tolerance for absolute relative error control of the adaptive iterative algorithm (default value 10^-6) 
    double AbsTol; //!< Tolerance for absolute error control of adaptive iterative algorithm (default value 10^-10) 
    double MaxHalf; //!< Maximum number of sub-step iterations within an integration step (default value 15).

    // Trial State Variables
    double Tstrain; // Trial Strain
    double Tstress; // Trial Stress
    double Ttangent; // Trial Tangent
    double TVel;   // Trial Velocity
    double Tpugr;   // Trial gap initiation displacement
    double Tnugr;   // Trial gap initiation displacement
    
    // Committeed State Variables
    double Cstrain;  // Committed Strain
    double Cstress;  // Committed Stress
    double Ctangent; // Committed Tangent
    double CVel;    // Committed velocity
    double Cpugr;	// Committed gap initiation displacement
    double Cnugr;   // Trial gap initiation displacement

    static double sgn(double dVariable);
    virtual double f(double v, double fd) const= 0;
    int DormandPrince(double vel0, double vel1, double y0, double h, double& yt, double& eps, double& error) const;
    int sendData(Communicator &);  
    int recvData(const Communicator &);
  public:
    DamperBase(int tag, int classTag); 
    DamperBase(int tag, int classTag, double K, double C, double LGap, double NM= 1, double RelTol= 1e-6, double AbsTol= 1e-10, double MaxHalf= 15);    

    inline void setElasticStiffness(const double &d)
      { this->K= d; }
    inline double getElasticStiffness() const
      { return this->K; }
    inline void setDampingCoeff(const double &d)
      { this->C= d; }
    inline double getDampingCoeff() const
      { return this->C; }
    inline void setGapLength(const double &d)
      { this->LGap= d; }
    inline double getGapLength() const
      { return this->LGap; }
    inline void setNumericalAlgorithm(const double &d)
      { this->NM= d; }
    inline double getNumericalAlgorithm() const
      { return this->NM; }
    inline void setRelativeTolerance(const double &d)
      { this->RelTol= d; }
    inline double getRelativeTolerance() const
      { return this->RelTol; }
    inline void setAbsoluteTolerance(const double &d)
      { this->AbsTol= d; }
    inline double getAbsoluteTolerance() const
      { return this->AbsTol; }
    inline void setMaxIter(const double &d)
      { this->MaxHalf= d; }
    inline double getMaxIter() const
      { return this->MaxHalf; }
    void setup(void);
    
    double getStrain(void) const; 
    double getStrainRate(void) const;
    double getStress(void) const;

    double getTangent(void) const;
    double getInitialTangent(void) const;
    double getDampTangent(void) const;


    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        
        
    
    void Print(std::ostream &s, int flag =0) const;
    
    // //////////////////////////////////////////
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    // AddingSensitivity:END
    // ///////////////////////////////////////////
    
  };

} // end of XC namespace

#endif
