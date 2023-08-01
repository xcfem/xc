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
//SteelBase.h
#ifndef SteelBase_h
#define SteelBase_h

#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Base class for steel uniaxial materials.
class SteelBase: public UniaxialMaterial
  {
  protected:
    /*** Material Properties ***/
    double fy;  //!< Yield stress
    double E0;  //!< Initial stiffness
    double b;   //!< Hardening ratio (b = Esh/E0)
    double a1;  //!< increase of compression yield envelope as proportion of yield strength after a plastic strain of a2∗(Fy/E0) (optional)
    double a2;  //!< coefficient for isotropic hardening in compression (see a1).
    double a3;  //!< isotropic hardening parameter, increase of tension yield envelope as proportion of yield strength after a plastic strain of a4∗(Fy/E0). (optional)
    double a4;  //!< coefficient for isotropic hardening in tension (see a3)
    double ezero; //!< Initial strain.

    int sendData(Communicator &);
    int recvData(const Communicator &);

    virtual int setup_parameters(void)= 0;    
  public:
    SteelBase(int tag,int classTag,const double &fy,const double &e0,const double &b,const double &a1,const double &a2,const double &a3,const double &a4, const double &initialStrain= 0.0);
    SteelBase(int tag,int classTag);

    int setInitialStrain(const double &);
    int incrementInitialStrain(const double &);
    void zeroInitialStrain(void);
    inline double getInitialStrain(void) const
      { return ezero; }
    void setInitialTangent(const double &);
    double getInitialTangent(void) const;
    void setFy(const double &);
    double getFy(void) const;

    double getA1(void) const;
    void setA1(const double &);
    double getA2(void) const;
    void setA2(const double &);
    double getA3(void) const;
    void setA3(const double &);
    double getA4(void) const;
    void setA4(const double &);

    inline void setHardeningRatio(const double &d)
      { b= d; }
    inline double getHardeningRatio(void) const
      { return b; }
    inline double getEsh(void) const
      { return b*E0; }
    inline double getEpsy(void) const
      { return fy/E0; }
    
    int revertToStart(void);
    
// AddingSensitivity:BEGIN //////////////////////////////////////////
    int    setParameter(const std::vector<std::string> &argv, Parameter &param);
    int    updateParameter(int parameterID, Information &info);
// AddingSensitivity:END ///////////////////////////////////////////    
  };
} // end of XC namespace

#endif
