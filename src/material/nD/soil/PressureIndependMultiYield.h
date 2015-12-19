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
// $Revision: 1.15 $
// $Date: 2004/06/15 18:58:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/PressureIndependMultiYield.h,v $
                                                                        
// Written: ZHY
// Created: August 2000

// Description: This file contains the class prototype for PressureIndependMultiYield.
//
// What: "@(#) PressureIndependMultiYield.h, revA"

#ifndef PressureIndependMultiYield_h
#define PressureIndependMultiYield_h

#include "PressureMultiYieldBase.h"
#include "T2Vector.h"

namespace XC {

//! @ingroup NDMat
//
//! @brief ??.
class PressureIndependMultiYield: public PressureMultiYieldBase
  {
  private:

    // user supplied

    // internal
    mutable double refShearModulus;
    mutable double refBulkModulus;

    void elast2Plast(void) const;
    // Called by constructor
    void setUpSurfaces(double *);  

    double yieldFunc(const T2Vector & stress, const std::vector<MultiYieldSurface> &,int surface_num) const;

    void deviatorScaling(T2Vector &,const std::vector<MultiYieldSurface> &,int, int count=0) const;

    void initSurfaceUpdate(void) const;

    void paramScaling(void) const;

    // Return num_strain_subincre
    int setSubStrainRate(void) const;

    int isLoadReversal(void) const;
    void getContactStress(T2Vector &) const;
    void getSurfaceNormal(const T2Vector &,Vector &) const; 
    void setTrialStress(const T2Vector &stress) const; 
    double getLoadingFunc(const T2Vector & contact, const Vector & surfaceNormal,int crossedSurface) const;

    void stressCorrection(int crossedSurface) const;
       
    void updateActiveSurface(void) const;
        
    void updateInnerSurface(void) const;

    // Return 1 if crossing the active surface; return 0 o/w
    int  isCrossingNextSurface(void) const;  
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    // Initialization constructor
    PressureIndependMultiYield(int tag, 
                                 int nd,
                                 double rho, 
                                 double refShearModul,
                                 double refBulkModul,
                                 double cohesi,
                                 double peakShearStra,
                                 double frictionAng = 0.,
                                 double refPress = 100, 
                                 double pressDependCoe = 0.0,
                                 int   numberOfYieldSurf = 20,
                                 double * gredu = 0);

     PressureIndependMultiYield(int tag= 0);

    void setup(int nd, double r, double refShearModul, double refBulkModul, double cohesi, double peakShearStra, double frictionAng, double refPress, double pressDependCoe, int numberOfYieldSurf, double * gredu);

     double getRho(void) const
       {return rhox[matN];} ;
     // Sets the values of the trial strain tensor.
     int setTrialStrain(const Vector &strain);

     // Sets the values of the trial strain and strain rate tensors.
     int setTrialStrain(const Vector &v, const Vector &r);

     int setTrialStrainIncr(const Vector &v);
     int setTrialStrainIncr(const Vector &v, const Vector &r);

     // Calculates current tangent stiffness.
     const Matrix &getTangent(void) const;
     const Matrix &getInitialTangent(void) const;
        
     void getBackbone(Matrix &);

     // Calculates the corresponding stress increment(rate), for a given strain increment. 
     const Vector &getStress(void) const;
     const Vector &getStrain(void) const;
     const Vector &getCommittedStress(void) const;
     const Vector &getCommittedStrain(void) const;

     int setTrialStrain(const Tensor &v) {return 0;}
     int setTrialStrain(const Tensor &v, const Tensor &r) {return 0;}
     int setTrialStrainIncr(const Tensor &v) {return 0;}
     int setTrialStrainIncr(const Tensor &v, const Tensor &r) {return 0;}

     // Accepts the current trial strain values as being on the solution path, and updates 
     // all model parameters related to stress/strain states. Return 0 on success.
     int commitState(void);

     // Revert the stress/strain states to the last committed states. Return 0 on success.
     int revertToLastCommit(void);

     int revertToStart(void) {return 0;}

     // Return an exact copy of itself.
     NDMaterial *getCopy(void) const;

     // Return a copy of itself if "code"="PressureIndependMultiYield", otherwise return null.
     NDMaterial *getCopy(const std::string &) const;

     // Return the string "PressureIndependMultiYield".
     const std::string &getType(void) const ;

     // Return ndm.
     int getOrder(void) const ;

     int sendSelf(CommParameters &);  
     int recvSelf(const CommParameters &);    

     Response *setResponse(const std::vector<std::string> &argv, Information &matInfo);
     int getResponse(int responseID, Information &matInformation);
     void Print(std::ostream &s, int flag =0);

     //void setCurrentStress(const Vector stress) { currentStress=T2Vector(stress); }
    int updateParameter(int responseID, Information &eleInformation);        

    // RWB; PyLiq1 & TzLiq1 need to see the excess pore pressure and initial stresses.
    friend class PyLiq1;
    friend class TzLiq1;
  };
} // end of XC namespace

#endif



