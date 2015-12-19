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
// $Date: 2003/08/14 20:23:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/FatigueMaterial.h,v $
                                                      
// Written: Patxi
// Created: Aug 2003
//
// Description: This file contains the class definition for 
// FatigueMaterial.  FatigueMaterial wraps a UniaxialMaterial
// and imposes fatigue limits.

#ifndef FatigueMaterial_h
#define FatigueMaterial_h

#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief FatigueMaterial wraps a UniaxialMaterial
//! and imposes fatigue limits.
class FatigueMaterial : public UniaxialMaterial
  {
  private:
    UniaxialMaterial *theMaterial;

    double D;   //!< Damage index
    double X;   //!< Range in consideration
    double Y;   //!< Previous Adjacent Range
    double A;   //!< Strain at first  cycle peak/valley
    double B;   //!< Strain at second cycle peak/valley
    int R1F;    //!< % Flag for first  cycle count
    int R2F;    //!< % Flag for second cycle count
    double CS;  //!< % Current Slope
    double PS;  //!< % Previous slope
    double EP;  //!< % Previous Strain
    int FF;     //!< % Failure Flag
    int SF;     //!< % Start Flag - for initializing the very first strain
    int PF;     //!< % Peak Flag --> Did we reach a peak/valley at current strain?
    
    double Dmax; //!< = 1.0; % Maximum Damage index, normally 1.0
    double Nf;   //!<  = 0.5E6; % Number of cycles to failure at strain E0
    double E0;   //!<  = 14.5/29000;
    double FE;   //!<  = -1/2.31; % Log log slope of fracture curve
    double b;    //!<   =  log10(E0) - log10(Nf)*FE; %Theoretical Intercept
    
    bool Tfailed;
    bool Cfailed;
  public:
    FatigueMaterial(int tag, UniaxialMaterial &material, 
		    double Dmax = 1.0,
		    double Nf  = 0.5E6,
		    double E0  = 14.5/29000.0,
		    double FE  = -1.0/2.31);
    FatigueMaterial(int tag);
    FatigueMaterial(void);
    ~FatigueMaterial(void);

    int setTrialStrain(double strain, double strainRate = 0.0); 
    double getStrain(void) const;          
    double getStrainRate(void) const;
    double getStress(void) const;
    double getTangent(void) const;
    double getDampTangent(void) const;
    inline double getInitialTangent(void) const
      { return theMaterial->getInitialTangent();}

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace


#endif

