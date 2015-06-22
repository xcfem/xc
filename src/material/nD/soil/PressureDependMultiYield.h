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
//<<<<<<< PressureDependMultiYield.h
// $Revision: 1.19 $
// $Date: 2004/06/15 18:58:01 $
//=======
// $Revision: 1.19 $
// $Date: 2004/06/15 18:58:01 $
//>>>>>>> 1.7
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/PressureDependMultiYield.h,v $
                                                                        
// Written: ZHY
// Created: August 2000


// Description: This file contains the class prototype for PressureDependMultiYield.
//
// What: "@(#) PressureDependMultiYield.h, revA"

#ifndef PressureDependMultiYield_h
#define PressureDependMultiYield_h

#include "material/nD/soil/PressureDependMultiYieldBase.h"

namespace XC {

class MultiYieldSurface;

//! @ingroup NDMat
//
//! @brief ??.
class PressureDependMultiYield: public PressureDependMultiYieldBase
  {
  private:
  // user supplied 
     static double* liquefyParam4x;

     mutable T2Vector reversalStress;
     mutable T2Vector lockStress;

     T2Vector reversalStressCommitted;
     T2Vector lockStressCommitted;
     
     // Called by constructor
     void setUpSurfaces(double *);  
     void initStrainUpdate(void);

     // Return num_strain_subincre
     int isCriticalState(const T2Vector & stress) const;
     void updatePPZ(const T2Vector & stress) const;
     void PPZTranslation(const T2Vector & contactStress) const;
     double getPPZLimits(int which, const T2Vector & contactStress) const;
     double getPlasticPotential(const T2Vector & stress, const T2Vector & surfaceNormal) const;
     //return 1 if stress locked; o/w return 0.
     int stressCorrection(int crossedSurface) const;

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
     // Initialization constructor
     PressureDependMultiYield(int tag, 
			       int nd,
						 double rho,
			       double refShearModul,
			       double refBulkModul,
			       double frictionAng,
			       double peakShearStra,
			       double refPress,
			       double pressDependCoe,
			       double phaseTransformAngle, 
			       double contractionParam1,
			       double dilationParam1,
			       double dilationParam2,
			       double liquefactionParam1,
			       double liquefactionParam2,
			       double liquefactionParam4,
			       int   numberOfYieldSurf = 20,
				   double * gredu = 0,
		           double e = 0.6,
			       double volLimit1 = 0.9,
			       double volLimit2 = 0.02,
			       double volLimit3 = 0.7,
			       double atm = 101.,
				   double cohesi = 0.1,
				   double hv = 0.,
				   double pv = 1.);

     PressureDependMultiYield(int tag);

     // Default constructor
     PressureDependMultiYield();

     // Copy constructor
     PressureDependMultiYield(const PressureDependMultiYield &);

     // Calculates current tangent stiffness.
     const Matrix &getTangent(void) const;

     // Calculates the corresponding stress increment(rate), for a given strain increment. 
     const Vector &getStress(void) const;

     // Return an exact copy of itself.
     NDMaterial *getCopy(void) const;

     // Return a copy of itself if "code"="PressureDependMultiYield", otherwise return null.
     NDMaterial *getCopy(const std::string &) const;

     // Return the string "PressureDependMultiYield".
     const std::string &getType(void) const ;

     int sendSelf(CommParameters &);  
     int recvSelf(const CommParameters &);
     void Print(std::ostream &s, int flag =0);
    // RWB; PyLiq1 & TzLiq1 need to see the excess pore pressure and initial stresses.
    friend class PyLiq1;
    friend class TzLiq1;
  };
} // fin namespace XC

#endif
