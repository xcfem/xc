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
// $Revision: 1.11 $
// $Date: 2003/10/07 21:20:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/FluidSolidPorousMaterial.h,v $
                                                                        
// Written: ZHY
// Created: August 2000
// Revision: A
//
// Description: This file contains the class prototype for FluidSolidPorousMaterial.
//
// What: "@(#) FluidSolidPorousMaterial.h, revA"

#ifndef FluidSolidPorousMaterial_h
#define FluidSolidPorousMaterial_h

#include <material/nD/NDMaterial.h>

namespace XC {
class Response;

//! @ingroup NDMat
//
//! @defgroup SoilNDMat Materiales para modelizar suelos.
//
//! @ingroup SoilNDMat
//
//! @brief ??.
class FluidSolidPorousMaterial: public NDMaterial
  {
  private:
    static int matCount;
    static int* ndmx;
    static int* loadStagex;
    static double *combinedBulkModulusx;
    static double pAtm;
    int matN;
    NDMaterial *theSoilMaterial;
    mutable double trialExcessPressure;
    double currentExcessPressure;
    double trialVolumeStrain;
    double currentVolumeStrain;
    mutable double initMaxPress;
    mutable int e2p;

    static Vector workV3;
    static Vector workV6;
    static Matrix workM3;
    static Matrix workM6;

    void libera(void);
    void alloc(const NDMaterial *);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
     // Initialization constructor
     FluidSolidPorousMaterial(int tag= 0);
     FluidSolidPorousMaterial(int tag, int nd,const NDMaterial &soilMat, double combinedBulkModul, double atm=101.);
     FluidSolidPorousMaterial(const FluidSolidPorousMaterial &);
     FluidSolidPorousMaterial &operator=(const FluidSolidPorousMaterial &tro);

     // Destructor: clean up memory storage space.
     virtual ~FluidSolidPorousMaterial();

     // Sets the values of the trial strain tensor.
     int setTrialStrain(const Vector &strain);

     // Sets the values of the trial strain and strain rate tensors.
     int setTrialStrain(const Vector &v, const Vector &r);

     int setTrialStrainIncr(const Vector &v);
     int setTrialStrainIncr(const Vector &v, const Vector &r);

     // Calculates current tangent stiffness.
     const Matrix &getTangent(void) const;
     const Matrix &getInitialTangent(void) const;

     double getRho(void) const;

     // Calculates the corresponding stress increment(rate), for a given strain increment. 
     const Vector &getStress(void) const;
     const Vector &getStrain(void) const;
     const Vector &getCommittedStress(void);
     const Vector &getCommittedStrain(void);
     const Vector &getCommittedPressure(void);

     // Accepts the current trial strain values as being on the solution path, and updates 
     // all model parameters related to stress/strain states. Return 0 on success.
     int commitState(void);

     // Revert the stress/strain states to the last committed states. Return 0 on success.
     int revertToLastCommit(void);

     int revertToStart(void);

     // Return an exact copy of itself.
     NDMaterial *getCopy(void) const;

     // Return a copy of itself if "code"="PlainStrain" or "ThreeDimensional", otherwise return null.
     NDMaterial *getCopy(const std::string &) const;

     // Return the string "PlaneStrain" or "ThreeDimensional".
     const std::string &getType(void) const ;

     // Return 3 or 6.
     int getOrder(void) const ;

     int sendSelf(CommParameters &);  
     int recvSelf(const CommParameters &);     
     Response *setResponse(const std::vector<std::string> &argv, Information &matInfo);
     int getResponse(int responseID, Information &matInformation);
     void Print(std::ostream &s, int flag =0);

     int updateParameter(int responseID, Information &eleInformation);

     // RWB; PyLiq1 & TzLiq1 need to see the excess pore pressure and initial stresses.
     friend class PyLiq1;
     friend class TzLiq1;
  };
} // fin namespace XC

#endif
