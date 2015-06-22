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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/14 23:01:39 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/HystereticMaterial.h,v $

// Written: MHS
// Created: July 2000
//
// Description: This file contains the class definition for 
// HystereticMaterial.  HystereticMaterial provides the implementation
// of a one-dimensional hysteretic model with pinching of both
// force and deformation, damage due to deformation and energy, and
// degraded unloading stiffness based on maximum ductility.  This
// is a modified implementation of Hyster2.f90 by Filippou.

#ifndef HystereticMaterial_h
#define HystereticMaterial_h

#include "UniaxialMaterial.h"
#include "UniaxialStateVars.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief HystereticMaterial provides the implementation
//! of a one-dimensional hysteretic model with pinching of both
//! force and deformation, damage due to deformation and energy, and
//! degraded unloading stiffness based on maximum ductility.  This
//! is a modified implementation of Hyster2.f90 by Filippou.
class HystereticMaterial: public UniaxialMaterial
  {
  private:
    // Pinching parameters
    double pinchX; // Deformation pinching
    double pinchY; // Force pinching

    // Damage parameters
    double damfc1; // Deformation
    double damfc2; // Energy

    // Unloading parameter
    double beta;

    // Trial history variables
    double TrotMax;
    double TrotMin;
    double TrotPu;
    double TrotNu;
    double TenergyD;
    int TloadIndicator;

    // Trial state variables
    UniaxialStateVars trial;

    // Converged history variables
    double CrotMax;
    double CrotMin;
    double CrotPu;
    double CrotNu;
    double CenergyD;
    int CloadIndicator;

    // Converged state variables
    UniaxialStateVars converged;

    // Backbone parameters
    double mom1p, rot1p;
    double mom2p, rot2p;
    double mom3p, rot3p;
    double mom1n, rot1n;
    double mom2n, rot2n;
    double mom3n, rot3n;

    double E1p, E1n;
    double E2p, E2n;
    double E3p, E3n;

    double energyA;

    void setEnvelope(void);

    double posEnvlpStress(double strain);
    double negEnvlpStress(double strain);

    double posEnvlpTangent(double strain);
    double negEnvlpTangent(double strain);

    double posEnvlpRotlim(double strain);
    double negEnvlpRotlim(double strain);

    void positiveIncrement(double dStrain);
    void negativeIncrement(double dStrain);
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    HystereticMaterial(int tag,
    	double mom1p, double rot1p, double mom2p, double rot2p,
    	double mom3p, double rot3p,
    	double mom1n, double rot1n, double mom2n, double rot2n,
    	double mom3n, double rot3n,
    	double pinchX, double pinchY,
    	double damfc1 = 0.0, double damfc2 = 0.0,
    	double beta = 0.0);
    HystereticMaterial(int tag,
    	double mom1p, double rot1p, double mom2p, double rot2p,
    	double mom1n, double rot1n, double mom2n, double rot2n,
    	double pinchX, double pinchY,
    	double damfc1 = 0.0, double damfc2 = 0.0,
    	double beta = 0.0);
    HystereticMaterial(int tag);
    HystereticMaterial();

    int setTrialStrain(double strain, double strainRate = 0.0);
    double getStrain(void) const;
    double getStress(void) const;
    double getTangent(void) const;
    inline double getInitialTangent(void) const {return E1p;};

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);
  };
} // fin namespace XC

#endif
