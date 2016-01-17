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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/25 23:33:37 $: 2001/07/16 08:23:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/CableMaterial.h,v $
                                                                        
                                                                        
#ifndef CableMaterial_h
#define CableMaterial_h

// Written: Charles Chadwell
// Created: 07/01
//
// Description: This file contains the class definition for 
// CableMaterial. CableMaterial provides the abstraction
// of an elastic uniaxial material,
//
// The input parameters are the Prestress, E, Effective Self Weight (gravity component of 
// Weight per volume transverse to the cable), and Length of the cable.
//
// The cable Force Displacement is converted to Stress Strain material for use 
// with the truss element.  The stress strain ranges from slack (large strain at zero 
// stress) to taught (linear with modulus E).  The material has no history and is not
// path dependent.
//
//
// What: "@(#) CableMaterial.h, revA"


#include <material/uniaxial/ElasticBaseMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief CableMaterial provides the abstraction of an elastic
//! uniaxial material, the input parameters are the Prestress, E,
//! Effective Self Weight (gravity component of 
//! Weight per volume transverse to the cable), and Length of the cable.
//!
//! The cable Force Displacement is converted to Stress Strain material
//! for use with the truss element. The stress strain ranges from
//! slack (large strain at zero stress) to taught (linear with modulus E).
//! The material has no history and is not path dependent.
class CableMaterial : public ElasticBaseMaterial
  {
  private:
    double Ps;
    double Mue;
    double L;
    double trialStress;      // current trial stress
    double trialTangent;     // current trial tangent
    
    double evalStress(double stress);
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    CableMaterial(int tag, double Prestress, double E, double unitWeightEff, double L_Element);    
    CableMaterial(int tag= 0);    

    void setLength(const double &);
    inline double getLength(void) const
      { return L; }
    int setTrialStrain(double strain, double strainRate = 0.0); 
    int setTrial(double strain, double &stress, double &tangent, double strainRate = 0.0);

    inline double getStrain(void) const
      {return trialStrain;};
    double getStress(void) const;
    double getTangent(void) const;
    inline double getInitialTangent(void) const
      {return 1.0e-8;}; 

    inline void setPrestress(const double &d)
      { Ps= d; }
    double getPrestress(void) const
      { return Ps; }

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);

    
    //int setParameter(const std::vector<std::string> &argv, Parameter &param);
    //int updateParameter(int parameterID, Information &info);
  };
} // end of XC namespace


#endif

