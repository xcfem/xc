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
                                                                        
// $Revision: 1.8 $
// $Date: 2003/03/04 00:48:18 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/UniaxialMaterial.h,v $
                                                                        
                                                                        
// File: ~/material/UniaxialMaterial.h
//
// Written: fmk 
// Created: 05/98
// Revision: A
//
// Description: This file contains the class definition for 
// UniaxialMaterial. UniaxialMaterial is a base class and 
// thus no objects of it's type can be instantiated. It has pure virtual 
// functions which must be implemented in it's derived classes. 
//
// What: "@(#) UniaxialMaterial.h, revA"

#ifndef UniaxialMaterial_h
#define UniaxialMaterial_h

#define POS_INF_STRAIN        1.0e16
#define NEG_INF_STRAIN       -1.0e16

#include <material/Material.h>
namespace XC {
class ID;
class Vector;
class Matrix;
class Information;
class Response;

class SectionForceDeformation;

//! @ingroup Mat
//
//! @defgroup MatUnx Modelos unidimensionales de un material.
//
//! @ingroup MatUnx
//
//! @brief Clase base para los materiales unidimensionales.
class UniaxialMaterial : public Material
  {
  private:
    double rho; //!< Masa por unidad de longitud.
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    UniaxialMaterial(int tag, int classTag);
        
    virtual int setInitialStrain(double strain);
    virtual int setTrialStrain(double strain, double strainRate = 0.0)= 0;
    virtual int setTrial(double strain, double &stress, double &tangent, double strainRate = 0.0);

    virtual double getInitialStrain(void) const;
    virtual double getStrain(void) const= 0;
    virtual double getStrainRate(void) const;
    virtual double getStress(void) const= 0;
    const Vector &getGeneralizedStress(void) const;
    const Vector &getGeneralizedStrain(void) const;

    virtual void setInitialGeneralizedStrain(const Vector &);
    const Vector &getInitialGeneralizedStrain(void) const;

    virtual double getTangent(void) const= 0;
    virtual double getInitialTangent(void) const= 0;
    virtual double getDampTangent(void) const;
    virtual double getSecant(void) const;
    virtual double getFlexibility(void) const;
    virtual double getInitialFlexibility(void) const;
    virtual double getRho(void) const;
    void setRho(const double &);

    virtual UniaxialMaterial *getCopy(void) const=0;
    virtual UniaxialMaterial *getCopy(SectionForceDeformation *s) const;
	
    virtual Response *setResponse(const std::vector<std::string> &argv, Information &matInformation);
    virtual int getResponse(int responseID, Information &matInformation);    

// AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual int    setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int    updateParameter(int parameterID, Information &info);
    virtual int    activateParameter(int parameterID);
    virtual double getStressSensitivity(int gradNumber, bool conditional);
    virtual double getStrainSensitivity(int gradNumber);
    virtual double getInitialTangentSensitivity(int gradNumber);
    virtual double getDampTangentSensitivity(int gradNumber);
    virtual double getRhoSensitivity(int gradNumber);
    virtual int    commitSensitivity(double strainGradient, int gradNumber, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////
  };
UniaxialMaterial *receiveUniaxialMaterialPtr(UniaxialMaterial *,DbTagData &,const CommParameters &,const BrokedPtrCommMetaData &);

} // fin namespace XC


#endif

