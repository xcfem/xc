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
//PressureMultiYieldBase.h

#ifndef PressureMultiYieldBase_h
#define PressureMultiYieldBase_h

#include <material/nD/soil/SoilMaterialBase.h>
#include "T2Vector.h"


namespace XC {

class MultiYieldSurface;

//! @ingroup NDMat
//
//! @brief ??.
class PressureMultiYieldBase: public SoilMaterialBase
  {
  protected:
    // user supplied 
    static std::vector<double> rhox; //!< mass density
    static std::vector<double> frictionAnglex; //!< internal friction angle.
    static std::vector<double> peakShearStrainx; //!< peak shear strain.
    static std::vector<double> refPressurex; //!< reference pressure.
    static std::vector<double> cohesionx; //!< apparent cohesion.
    static std::vector<double> pressDependCoeffx; //!< pressure dependency coefficient
    static std::vector<int> numOfSurfacesx; //!< number of yield surfaces.

    // internal
    static std::vector<double> residualPressx;
    static std::vector<double> stressRatioPTx;
    static Matrix theTangent;
    static T2Vector subStrainRate;

    mutable std::vector<MultiYieldSurface> theSurfaces; // NOTE: surfaces[0] is not used  
    mutable std::vector<MultiYieldSurface> committedSurfaces;  
    mutable int activeSurfaceNum;  
    mutable int committedActiveSurf;
    mutable T2Vector currentStress;
    mutable T2Vector trialStress;
    T2Vector currentStrain;
    T2Vector strainRate;

  protected:
    void resizeIfNeeded(void);
    void setup(int nd, double r, double frictionAng,double peakShearStra, double refPress, double pressDependCoe, double cohesi,int numberOfYieldSurf);
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
     // Initialization constructor
    PressureMultiYieldBase (int tag, int classTag, int nd, double rho, double frictionAng, double peakShearStra, double refPress, double pressDependCoe, double cohesi = 0.1,int numberOfYieldSurf = 20),

    PressureMultiYieldBase(int tag, int classTag);
    PressureMultiYieldBase(const PressureMultiYieldBase &);
    PressureMultiYieldBase &operator=(const PressureMultiYieldBase &);
    
    inline double getRho(void) const
      {return rhox[matN];}
    void setRho(const double &);
    
    inline double getFrictionAngle(void) const
      { return frictionAnglex[matN];}
    void setFrictionAngle(const double &); 
    inline double getRefPressure(void) const
      { return refPressurex[matN]; }
    void setRefPressure(const double &);
    inline double getPeakShearStrain(void) const
      { return peakShearStrainx[matN]; }
    void setPeakShearStrain(const double &);
    inline double getCohesion(void) const
      { return cohesionx[matN]; }
    void setCohesion(const double &);
    inline double getPressDependCoeff(void) const
      { return pressDependCoeffx[matN]; }
    void setPressDependCoeff(const double &);
    inline int getNumberOfSurfaces(void) const
      { return numOfSurfacesx[matN]; }
    void setNumberOfSurfaces(const int &); 
  };
} // end of XC namespace

#endif
