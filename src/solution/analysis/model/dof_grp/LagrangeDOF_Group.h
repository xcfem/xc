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
// $Date: 2004/10/12 21:52:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/dof_grp/LagrangeDOF_Group.h,v $
                                                                        
                                                                        
#ifndef LagrangeDOF_Group_h
#define LagrangeDOF_Group_h

// File: ~/analysis/model/dof_grp/LagrangeDOF_Group.h
// 
// Written: fmk 
// Created: 02/99
// Revision: A
//
// Description: This file contains the class definition for LagrangeDOF_Group.
// A LagrangeDOF_Group object is instantiated by a LagrangeConstraintHandler for 
// every constrained node in the domain. 
//
// What: "@(#) LagrangeDOF_Group.h, revA"

#include <solution/analysis/model/dof_grp/DOF_Group.h>
namespace XC {
class SP_Constraint;
class MP_Constraint;
class MRMP_Constraint;

//! @ingroup AnalisisDOF
//
//! @brief A LagrangeDOF_Group object is instantiated 
//! by a LagrangeConstraintHandler for 
//! every constrained node in the domain. 
class LagrangeDOF_Group: public DOF_Group
  {
  protected:
    friend class AnalysisModel;
    LagrangeDOF_Group(int tag, SP_Constraint &);    
    LagrangeDOF_Group(int tag, MP_Constraint &);        
    LagrangeDOF_Group(int tag, MRMP_Constraint &);        
  public:
    // methods to form the tangent
    virtual const Matrix &getTangent(Integrator *theIntegrator);

    // methods to form the unbalance
    virtual const Vector &getUnbalance(Integrator *theIntegrator);

    // methods to obtain committed responses .. always 0
    virtual const Vector &getCommittedDisp(void);
    virtual const Vector &getCommittedVel(void);
    virtual const Vector &getCommittedAccel(void);
    
    // methods to update the trial response at the nodes
    virtual void setNodeDisp(const Vector &u);
    virtual void setNodeVel(const Vector &udot);
    virtual void setNodeAccel(const Vector &udotdot);

    virtual void incrNodeDisp(const Vector &u);
    virtual void incrNodeVel(const Vector &udot);
    virtual void incrNodeAccel(const Vector &udotdot);

    virtual void  zeroTangent(void);
    virtual void  addMtoTang(double fact = 1.0);    
    virtual void  zeroUnbalance(void);
    virtual void  addPtoUnbalance(double fact = 1.0);
    virtual void  addPIncInertiaToUnbalance(double fact = 1.0);    
    virtual void  addM_Force(const Vector &Udotdot, double fact = 1.0);        

    virtual const Vector &getTangForce(const Vector &x, double fact = 1.0);
    virtual const Vector &getC_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getM_Force(const Vector &x, double fact = 1.0);
  };
} // fin namespace XC

#endif

