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
// $Date: 2005/11/28 21:38:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/transformation/TransformationFE.h,v $
                                                                        
                                                                        
#ifndef TransformationFE_h
#define TransformationFE_h

// Written: fmk 
// Created: 05/99
// Revision: A
//
// Description: This file contains the class definition for TransformationFE.
// TransformationFE objects handle MP_Constraints using the transformation
// method T^t K T. SP_Constraints are handled by the TransformationConstraintHandler.
//
// What: "@(#) TransformationFE.h, revA"

#include <solution/analysis/model/fe_ele/FE_Element.h>
#include "solution/analysis/UnbalAndTangent.h"

namespace XC {
class SP_Constraint;
class DOF_Group;
class TransformationConstraintHandler;

//! @ingroup AnalisisFE
//
//! @brief TransformationFE objects handle MP_Constraints using
//! the transformation method T^t K T. SP_Constraints are
//! handled by the TransformationConstraintHandler.
class TransformationFE: public FE_Element
  {
  private:
    // private variables - a copy for each object of the class        
    std::vector<DOF_Group *> theDOFs;
    //int numSPs;
    //std::vector<SP_Constraint *> theSPs;
    ID modID;
    int numGroups;
    int numTransformedDOF;
    int numOriginalDOF;
    UnbalAndTangent unbalAndTangentMod;
    
    // static variables - single copy for all objects of the class	
    static UnbalAndTangentStorage unbalAndTangentArrayMod; //!< array of class wide vectors and matrices
    static std::vector<Matrix *> theTransformations; // for holding pointers to the T matrices
    static int numTransFE;     // number of objects    
    static int transCounter;   // a counter used to indicate when to do something
    static int sizeTransformations; // size of theTransformations array
    static Vector dataBuffer;
    static Vector localKbuffer;
    static ID dofData;
    static int sizeBuffer;
  protected:
    int transformResponse(const Vector &modResponse, Vector &unmodResponse);
 
    friend class AnalysisModel;
    TransformationFE(int tag, Element *theElement);
  public:
    ~TransformationFE(void);    

    // public methods for setting/obtaining mapping information
    virtual const ID &getDOFtags(void) const;
    virtual const ID &getID(void) const;
    void setAnalysisModel(AnalysisModel &theModel);
    virtual int setID(void);
    
    // methods to form and obtain the tangent and residual
    virtual const Matrix &getTangent(Integrator *theIntegrator);
    virtual const Vector &getResidual(Integrator *theIntegrator);
    
    // methods for ele-by-ele strategies
    virtual const Vector &getTangForce(const Vector &x, double fact = 1.0);
    virtual const Vector &getK_Force(const Vector &accel, double fcat = 1.0);
    virtual const Vector &getM_Force(const Vector &accel, double fcat = 1.0);
    virtual const Vector &getC_Force(const Vector &vel, double fcat = 1.0);
    virtual void  addD_Force(const Vector &vel,   double fact = 1.0);
    virtual void  addM_Force(const Vector &accel, double fact = 1.0);    
    
    const Vector &getLastResponse(void);
    int addSP(SP_Constraint &theSP);

    // AddingSensitivity:BEGIN ////////////////////////////////////
    virtual void addM_ForceSensitivity(int gradNumber, const Vector &vect, double fact = 1.0);
    virtual void addD_ForceSensitivity(int gradNumber, const Vector &vect, double fact = 1.0);
    // AddingSensitivity:END //////////////////////////////////////

  };
} // fin namespace XC

#endif




