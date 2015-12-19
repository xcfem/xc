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
**                                                                    **
** Additions and changes by:                                          **
**   Boris Jeremic (@ucdavis.edu)                                     **
**                                                                    **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.16 $
// $Date: 2004/07/20 22:39:02 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/NDMaterial.h,v $


#ifndef NDMaterial_h
#define NDMaterial_h

// File: ~/material/NDMaterial.h
//
// Written: MHS
// Created: Feb 2000
// Revision: A
//
// Description: This file contains the class definition for NDMaterial.
// NDMaterial is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.
//
// What: "@(#) NDMaterial.h, revA"

#include <material/Material.h>
#include <utility/matrix/nDarray/stresst.h>

namespace XC {
class Matrix;
class ID;
class Vector;
class Tensor;
class Information;
class Response;

//! @ingroup Mat
//
//! @defgroup NDMat Materiales n dimensionales.
//
//! @ingroup NDMat
//
//! @brief Clase base para los materiales en dos y tres dimensiones.
class NDMaterial: public Material
  {
  private:
    static Matrix errMatrix;
    static Vector errVector;
    static Tensor errTensor;
    static stresstensor errstresstensor;
    static straintensor errstraintensor;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    NDMaterial();
    NDMaterial(int tag, int classTag);

    // methods to set state and retrieve state using Matrix and Vector classes
    virtual double getRho(void) const;
// BJ added 19June2002
    virtual double getE(void);
    virtual double getnu(void);
    virtual double getpsi(void);


    virtual int setTrialStrain(const Vector &v);
    virtual int setTrialStrain(const Vector &v, const Vector &r);
    virtual int setTrialStrainIncr(const Vector &v);
    virtual int setTrialStrainIncr(const Vector &v, const Vector &r);
    virtual const Matrix &getTangent(void) const;
    inline virtual const Matrix &getInitialTangent(void) const
      {return this->getTangent();};

    virtual const Vector &getStress(void) const;
    virtual const Vector &getStrain(void) const;
    inline const Vector &getGeneralizedStress(void) const
      { return getStress(); }
    inline const Vector &getGeneralizedStrain(void) const
      { return getStrain(); }

    virtual void setInitialGeneralizedStrain(const Vector &);
    const Vector &getInitialGeneralizedStrain(void) const;

    virtual const Vector &getCommittedStress(void);
    virtual const Vector &getCommittedStrain(void);

    // methods to set and retrieve state using the Tensor class
    virtual int setTrialStrain(const Tensor &v);
    virtual int setTrialStrain(const Tensor &v, const Tensor &r);
    virtual int setTrialStrainIncr(const Tensor &v);
    virtual int setTrialStrainIncr(const Tensor &v, const Tensor &r);
    virtual const Tensor &getTangentTensor(void) const;
    virtual const stresstensor &getStressTensor(void) const;
    virtual const straintensor &getStrainTensor(void) const;
    virtual const straintensor &getPlasticStrainTensor(void) const; //Added Joey Aug. 13, 2001

//Zhao (zcheng@ucdavis.edu)
// added Sept 22 2003 for Large Deformation, F is the Deformation Gradient
    virtual int setTrialF(const straintensor &f);
    virtual int setTrialFIncr(const straintensor &df);
    virtual int setTrialC(const straintensor &c);
    virtual int setTrialCIncr(const straintensor &dc);
    virtual const stresstensor getPK1StressTensor(void);
    virtual const stresstensor getCauchyStressTensor(void);
    virtual const straintensor &getF(void) const;
    virtual const straintensor &getC(void) const;
    virtual const straintensor getFp(void);
// Only For Large Deformation, END////////////////////////////////////////

    virtual NDMaterial *getCopy(void) const= 0;
    virtual NDMaterial *getCopy(const std::string &) const= 0;

    virtual const std::string &getType(void) const = 0;
    virtual int getOrder(void) const = 0;

    virtual Response *setResponse(const std::vector<std::string> &argv, Information &matInformation);
    virtual int getResponse (int responseID, Information &matInformation);

// AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual int            setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int            updateParameter          (int parameterID, Information &info);
    virtual int            activateParameter        (int parameterID);
    virtual const Vector & getStressSensitivity     (int gradNumber, bool conditional);
    virtual const Vector & getStrainSensitivity     (int gradNumber);
    virtual const Matrix & getTangentSensitivity    (int gradNumber);
    virtual const Matrix & getDampTangentSensitivity(int gradNumber);
    virtual double         getRhoSensitivity        (int gradNumber);
    virtual int            commitSensitivity        (Vector & strainGradient, int gradNumber, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////

  };

NDMaterial *receiveNDMaterialPtr(NDMaterial *,DbTagData &,const CommParameters &,const BrokedPtrCommMetaData &);

} // end of XC namespace


#endif
