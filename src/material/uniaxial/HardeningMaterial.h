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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.9 $
// $Date: 2003/03/11 03:49:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/HardeningMaterial.h,v $

#ifndef HardeningMaterial_h
#define HardeningMaterial_h

// Written: MHS
// Created: May 2000
//
// Description: This file contains the class definition for 
// HardeningMaterial.  HardeningMaterial provides the abstraction
// for a one-dimensional rate-independent plasticity model
// with combined isotropic and kinematic hardening.

#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief HardeningMaterial provides the abstraction
//! for a one-dimensional rate-independent plasticity model
//! with combined isotropic and kinematic hardening.
class HardeningMaterial : public UniaxialMaterial
  {
  private:
    // Material parameters
    double E;	// Elastic modulus
    double sigmaY;	// Yield stress
    double Hiso;	// Isotropic hardening parameter
    double Hkin;	// Kinematic hardening parameter
    double eta;
	
    // Committed history variables
    double CplasticStrain;	// Committed plastic strain
    double CbackStress;		// Committed back stress;
    double Chardening;		// Committed internal hardening variable

    // Trial history variables
    double TplasticStrain;	// Trial plastic strain
    double TbackStress;		// Trial back stress
    double Thardening;		// Trial internal hardening variable

    // Trial state variables
    double Tstrain;		// Trial strain
    double Tstress;		// Trial stress
    double Ttangent;	// Trial tangent

// AddingSensitivity:BEGIN //////////////////////////////////////////
    int parameterID;
    Matrix *SHVs;
// AddingSensitivity:END ///////////////////////////////////////////
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    HardeningMaterial(int tag, double E, double sigmaY,
		      double K, double H, double eta = 0.0);
    HardeningMaterial(int tag);
    HardeningMaterial(void);
    HardeningMaterial(const HardeningMaterial &otro);
    HardeningMaterial &operator=(const HardeningMaterial &otro);
    ~HardeningMaterial(void);

    int setTrialStrain(double strain, double strainRate = 0.0); 
    double getStrain(void) const;          
    double getStress(void) const;
    double getTangent(void) const;
    inline double getInitialTangent(void) const {return E;};

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);
    
// AddingSensitivity:BEGIN //////////////////////////////////////////
    int    setParameter(const std::vector<std::string> &argv, Parameter &param);
    int    updateParameter(int parameterID, Information &info);
    int    activateParameter        (int parameterID);
    double getStressSensitivity     (int gradNumber, bool conditional);
    double getInitialTangentSensitivity    (int gradNumber);
    int commitSensitivity        (double strainGradient, int gradNumber, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////
  };
} // fin namespace XC


#endif

