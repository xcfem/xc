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

#include <material/nD/NDMaterial.h>
#include "T2Vector.h"


namespace XC {

class MultiYieldSurface;

//! @ingroup NDMat
//
//! @brief ??.
class PressureMultiYieldBase: public NDMaterial
  {
  protected:
  // user supplied 
     static int matCount;
     static std::vector<int> ndmx;  //num of dimensions (2 or 3)
     static std::vector<int> loadStagex;  //=0 if elastic; =1 or 2 if plastic
     static std::vector<double> rhox;  //mass density
     static std::vector<double> frictionAnglex;
     static std::vector<double> peakShearStrainx;
     static std::vector<double> refPressurex;
     static std::vector<double> cohesionx;
     static std::vector<double> pressDependCoeffx;
     static std::vector<int> numOfSurfacesx;

     // internal
     static std::vector<double> residualPressx;
     static std::vector<double> stressRatioPTx;
     static Matrix theTangent;
     static T2Vector subStrainRate;

     int matN;
     mutable int e2p;
     mutable std::vector<MultiYieldSurface> theSurfaces; // NOTE: surfaces[0] is not used  
     mutable std::vector<MultiYieldSurface> committedSurfaces;  
     mutable int activeSurfaceNum;  
     mutable int committedActiveSurf;
     mutable T2Vector currentStress;
     mutable T2Vector trialStress;
     T2Vector currentStrain;
     T2Vector strainRate;

  protected:
     void setup(int nd, double r, double frictionAng,double peakShearStra, double refPress, double pressDependCoe, double cohesi,int numberOfYieldSurf);
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
     // Initialization constructor
     PressureMultiYieldBase (int tag, int classTag, int nd, double rho, double frictionAng, double peakShearStra, double refPress, double pressDependCoe, double cohesi = 0.1,int numberOfYieldSurf = 20),

     PressureMultiYieldBase(int tag, int classTag);
     PressureMultiYieldBase(const PressureMultiYieldBase &);
     PressureMultiYieldBase &operator=(const PressureMultiYieldBase &);
  };
} // end of XC namespace

#endif
