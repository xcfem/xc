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
                                                                        
// $Revision: 1.1 $
// $Date: 2004/09/01 03:53:13 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/snap/Bilinear.h,v $
//
//
// Bilinear.h: implementation of the Bilinear class from Fortran version.
// Originally from SNAP PROGRAM by Luis Ibarra and Prof H.K. Krawinkler
//
// Written: A. Altoontash & Prof. G. Deierlein 12/01
// Revised: 03/02
//
// Purpose: This file contains the implementation for the Bilinear class.
//
//////////////////////////////////////////////////////////////////////

// Bilinear.h: interface for the Bilinear class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BILINEAR_H
#define BILINEAR_H

#include <material/uniaxial/UniaxialMaterial.h>
#include <utility/matrix/Vector.h>
#include <stdio.h>
#include <material/damage/DamageModel.h>
#include <utility/recorder/response/MaterialResponse.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief ??.
class Bilinear : public UniaxialMaterial  
  {
 private:
     // Input parameters
    double elstk, fyieldPos ,fyieldNeg, alfa;            // Main properties
    double alfaCap, capDispPos, capDispNeg, Resfac;        // Cap properties
    int flagCapenv;
    DamageModel *StrDamage;
    DamageModel *StfDamage;
    DamageModel *CapDamage;
    
    // Hystory data
    double hsTrial[17], hsCommit[17], hsLastCommit[17];
    
    FILE *OutputFile;        // For debugging

    // Sensitivity related variables
    int parameterID;
    Matrix *SHVs;
  public:
    Bilinear(void);
    Bilinear(int tag);
    Bilinear(int tag, Vector inputParam ,DamageModel *strength,DamageModel *stiffness,DamageModel *capping);
    virtual ~Bilinear();
  
    int setTrialStrain(double d, double strainRate = 0.0);
    double getStrain(void) const;
    double getStress(void) const;
    double getTangent(void) const;
    double getInitialTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);  

    //virtual
    UniaxialMaterial *getCopy(void) const;
  
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  
    Response* setResponse(const std::vector<std::string> &argv, Information &matInfo);
    int getResponse(int responseID, Information &matInfo);

    void Print(std::ostream &s, int flag =0);
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);

/*
      // Reliability and sensitivity stuff
    double getInitialTangent        (void);

    double getStressSensitivity     (int gradNumber, bool conditional);
    double getStrainSensitivity     (int gradNumber);
    double getTangentSensitivity    (int gradNumber);
    double getDampTangentSensitivity(int gradNumber);
    double getRhoSensitivity        (int gradNumber);
    int    commitSensitivity        (double strainGradient, int gradNumber, int numGrads);
  */

 protected:
    void envelPosCap( double ekelstk, double fy, double ekhard, double dcap,
                           double ekcap, double fRes, double *fuPos, double d, double *f, double *ek );
    void envelNegCap( double ekelstk, double fy, double ekhard, double dcap,
                           double ekcap, double fRes, double *fuNeg, double d, double *f, double *ek );
    void recordInfo(int cond =0);
 
  };
} // fin namespace XC

#endif
