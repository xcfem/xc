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

// File: ~/analysis/model/fe_ele/lagrange/LagrangeMRMFreedom_FE.h
// 
// Written: lcpt 
// Created: 03/2015
// Revision: A
//
// Description: This file contains the class definition for LagrangeMRMFreedom_FE.
// LagrangeMRMFreedom_FE is a subclass of FE_Element which handles MRMFreedom_Constraints
// using the Lagrange method.
//
// What: "@(#) LagrangeMRMFreedom_FE.h, revA"


#ifndef LagrangeMRMFreedom_FE_h
#define LagrangeMRMFreedom_FE_h

#include "solution/analysis/model/fe_ele/MRMFreedom_FE.h"
#include "Lagrange_FE.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"

namespace XC {
class Element;
class Integrator;
class AnalysisModel;
class Domain;
class DOF_Group;

//! @ingroup AnalisisFE
//
//! @brief LagrangeMRMFreedom_FE is a subclass of FE_Element
//! which handles MRMFreedom_Constraints using the Lagrange method.
class LagrangeMRMFreedom_FE: public MRMFreedom_FE, public Lagrange_FE
  {
  private:
    void determineTangent(void);
    
    friend class AnalysisModel;
    LagrangeMRMFreedom_FE(int tag, Domain &theDomain, MRMFreedom_Constraint &,DOF_Group &, double alpha = 1.0);
  public:

    // public methods
    virtual int setID(void);
    virtual const Matrix &getTangent(Integrator *theIntegrator);    
    virtual const Vector &getResidual(Integrator *theIntegrator);    
    virtual const Vector &getTangForce(const Vector &x, double fact = 1.0);

    virtual const Vector &getK_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getC_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getM_Force(const Vector &x, double fact = 1.0);
  };
} // end of XC namespace

#endif


