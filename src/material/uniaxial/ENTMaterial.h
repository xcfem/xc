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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/25 23:33:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ENTMaterial.h,v $
                                                                        
                                                                        
#ifndef ENTMaterial_h
#define ENTMaterial_h

// File: ~/material/ENTMaterial.h
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for 
// ENTMaterial. ENTMaterial provides the abstraction
// of an viscoelastic uniaxial material,
// i.e. stress = E*strain + eta*strainrate
//
//
// What: "@(#) ENTMaterial.h, revA"

#include <material/uniaxial/ElasticBaseMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Material elástico bajo compresión y que
//! bajo tracción exhibe el siguiente comportamiento:
//! stress = a*(tanh(strain*b))
//! tangent = a*(1-tanh(strain*b)*tanh(strain*b));
class ENTMaterial : public ElasticBaseMaterial
  {
    double a;
    double b;
    int parameterID;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    bool procesa_comando(CmdStatus &status);
  public:
    ENTMaterial(int tag= 0, const double &E=0.0,const double &a=0.0,const double &b=1.0);    

    int setTrialStrain(double strain, double strainRate = 0.0); 
    double getStress(void) const;
    double getTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
        
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int activateParameter(int parameterID);
    double getStressSensitivity(int gradIndex, bool conditional);
    double getInitialTangentSensitivity(int gradIndex);
    int commitSensitivity(double strainGradient, int gradIndex, int numGrads);
    // AddingSensitivity:END ///////////////////////////////////////////

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    virtual any_const_ptr GetProp(const std::string &cod) const;
    void Print(std::ostream &s, int flag =0);
  };
} // fin namespace XC


#endif

