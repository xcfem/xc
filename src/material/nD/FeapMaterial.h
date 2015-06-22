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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:01:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/FeapMaterial.h,v $
                                                                      
// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// FeapMaterial. FeapMaterial wraps a Feap material subroutine.

#ifndef FeapMaterial_h
#define FeapMaterial_h

#include <material/nD/NDMaterial.h>

namespace XC {
//! @ingroup NDMat
//
//! @defgroup FeapNDMat Materiales n dimensionales que llaman a rutinas de FEAP.
//
//! @ingroup FeapNDMat
//
//! @brief Clase base para los materiales FEAP en dos y tres dimensiones.
class FeapMaterial : public NDMaterial
  {
  public:
   FeapMaterial(int tag, int classTag, int numHV, int numData,double rho = 0.0);
  FeapMaterial(int classTag);
  virtual ~FeapMaterial();
  
  virtual int setTrialStrain(const Vector &strain);
  virtual const Vector &getStrain(void) const;
  virtual const Vector &getStress(void) const;
  virtual const Matrix &getTangent(void) const;
  virtual double getRho(void) const;
  
  virtual int commitState(void);
  virtual int revertToLastCommit(void);    
  virtual int revertToStart(void);        
  
  virtual NDMaterial *getCopy(void) const;
  virtual NDMaterial *getCopy(const std::string &) const;
  virtual const std::string &getType(void) const;
  virtual int getOrder(void) const;
  
  virtual int sendSelf(CommParameters &);  
  virtual int recvSelf(const CommParameters &);    
  
  virtual void Print(std::ostream &s, int flag = 0);
  
 protected:
  // Invokes the Feap subroutine
  virtual int invokeSubroutine(int isw);
  virtual int fillDArray(void);
  
  double *ud;	// Material parameters array
  double *hstv;	// History array: first half is committed, second half is trial
  
  static double d[];    // Feap material parameter array
  
  double rho;           // Material mass density
  
 private:
  int numHV;		// Number of history variables
  int numData;		// Number of material parameters
  
  double eps[6];        // Strain vector
  static double sig[6]; // Stress vector
  static double dd[36]; // Tangent matrix
  
  static Vector strain3;
  static Vector strain4;
  static Vector strain6;
  
  static Vector sigma3;
  static Vector sigma4;
  static Vector sigma6;
  
  static Matrix tangent3;
  static Matrix tangent4;
  static Matrix tangent6;
  
  enum Formulation{Unknown, ThreeDimensional, PlaneStrain, AxiSymmetric};
  int myFormulation;
  void setType(Formulation form);
};
} // fin namespace XC

#endif
