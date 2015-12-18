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
                                                                        
// $Revision: 1.10 $
// $Date: 2003/02/14 23:01:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/ElasticIsotropicMaterial.h,v $
                                                                        
                                                                        
#ifndef ElasticIsotropicMaterial_h
#define ElasticIsotropicMaterial_h

// File: ~/material/ElasticIsotropicMaterial.h
//
// Written: MHS
// Created: Feb 2000
// Revision: A
//
// Description: This file contains the class definition for ElasticIsotropicMaterialModel.
// ElasticIsotropicMaterialModel is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes. 
//
// What: "@(#) ElasticIsotropicMaterial.h, revA"

#include <material/nD/NDMaterial.h>
#include <utility/matrix/Vector.h>

namespace XC {
//! @ingroup NDMat
//
//! @defgroup EINDMat Material n dimensional elástico e isótropo.
//
//! @ingroup EINDMat
//
//! @brief Clase base para los materiales elásticos e isótropos.
class ElasticIsotropicMaterial : public NDMaterial
  {
  protected:
    double E; //!< Elastic modulus
    double v; //!< Poisson ratio
    double rho ; //!< mass per unit 3D volume
    Vector epsilon; //!< Strain vector

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    virtual bool procesa_comando(CmdStatus &);
  public:
    // Only called by subclasses to pass their tags to NDMaterialModel
    ElasticIsotropicMaterial(int tag, int classTag, int eps_size, double E, double nu, double rho = 0.0);

    // Called by clients
    ElasticIsotropicMaterial(int tag, int eps_size, double E, double nu, double rho = 0.0);
    ElasticIsotropicMaterial(int tag, int eps_size);

    // For parallel processing
    ElasticIsotropicMaterial(void);

    virtual double getRho(void) const;
    inline void setRho(const double &r)
      { rho= r; }
// BJ added 19June2002
    double getE(void);
    inline void setE(const double &e)
      { E= e; }
    double getnu(void);
    inline void setnu(const double &nu)
      { v= nu; }

    virtual int setTrialStrain(const Vector &v);
    virtual int setTrialStrain(const Vector &v, const Vector &r);
    virtual int setTrialStrainIncr(const Vector &v);
    virtual int setTrialStrainIncr(const Vector &v, const Vector &r);
    virtual const Matrix &getTangent(void) const;
    virtual const Matrix &getInitialTangent(void) const;
    virtual const Vector &getStress(void) const;
    virtual const Vector &getStrain(void) const;

    int setTrialStrain(const Tensor &v);
    int setTrialStrain(const Tensor &v, const Tensor &r);
    int setTrialStrainIncr(const Tensor &v);
    int setTrialStrainIncr(const Tensor &v, const Tensor &r);
    const Tensor &getTangentTensor(void) const;
    const stresstensor &getStressTensor(void) const;
    const straintensor &getStrainTensor(void) const;
    const straintensor &getPlasticStrainTensor(void) const;
    
    
    virtual int commitState(void);
    virtual int revertToLastCommit(void);
    virtual int revertToStart(void);
    
    // Create a copy of material parameters AND state variables
    // Called by GenericSectionXD
    virtual NDMaterial *getCopy(void) const;

    // Create a copy of just the material parameters
    // Called by the continuum elements
    virtual NDMaterial *getCopy(const std::string &) const;

    // Return a string indicating the type of material model
    virtual const std::string &getType(void) const;

    virtual int getOrder(void) const;
    
    virtual int sendSelf(CommParameters &);  
    virtual int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0);

    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);

    any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace


#endif
