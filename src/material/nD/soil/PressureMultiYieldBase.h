//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
     // Initialization constructor
     PressureMultiYieldBase (int tag, int classTag, int nd, double rho, double frictionAng, double peakShearStra, double refPress, double pressDependCoe, double cohesi = 0.1,int numberOfYieldSurf = 20),

     PressureMultiYieldBase(int tag, int classTag);
     PressureMultiYieldBase(const PressureMultiYieldBase &);
     PressureMultiYieldBase &operator=(const PressureMultiYieldBase &);
  };
} // fin namespace XC

#endif
