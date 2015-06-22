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
// $Date: 2005/08/03 19:12:26 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/dof_grp/TransformationDOF_Group.h,v $
                                                                        
                                                                        
#ifndef TransformationDOF_Group_h
#define TransformationDOF_Group_h

// Written: fmk 
// Created: 05/99
// Revision: A
//
// Description: This file contains the class definition for 
// TransformationDOF_Group. A TransformationDOF_Group object is 
// instantiated by the TransformationConstraintHandler for 
// every node in the domain which is constrained by an MP_Constraint
// or an SP_Constrant.
//
// What: "@(#) TransformationDOF_Group.h, revA"

#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include "utility/matrix/Matrix.h"
#include "solution/analysis/UnbalAndTangent.h"

namespace XC {
class MP_Constraint;
class MRMP_Constraint;
class SP_Constraint;
class TransformationConstraintHandler;

//! @ingroup AnalisisDOF
//
//! @brief A TransformationDOF_Group object is 
//! instantiated by the TransformationConstraintHandler for 
//! every node in the domain which is constrained by an MP_Constraint
//! or an SP_Constrant.
class TransformationDOF_Group: public DOF_Group
  {
  private:
    MP_Constraint *theMP;
    MRMP_Constraint *theMRMP;
    
    Matrix Trans;
    ID modID;
    int modNumDOF;
    UnbalAndTangent unbalAndTangentMod;
    
    std::vector<SP_Constraint *> theSPs;
    
    // static variables - single copy for all objects of the class	    
    static UnbalAndTangentStorage unbalAndTangentArrayMod; //!< array of class wide vectors and matrices
    static int numTransDOFs;           // number of objects        
    static TransformationConstraintHandler *theHandler;

  protected:
    friend class AnalysisModel;
    TransformationDOF_Group(int tag, Node *myNode, MP_Constraint *, TransformationConstraintHandler*);
    TransformationDOF_Group(int tag, Node *myNode, MRMP_Constraint *, TransformationConstraintHandler*);
    TransformationDOF_Group(int tag, Node *myNode, TransformationConstraintHandler *);    
  public:
    ~TransformationDOF_Group();    
    
    // methods dealing with the ID and transformation matrix
    int doneID(void);    
    const ID &getID(void) const; 
    virtual void setID(int dof, int value);    
    Matrix *getT(void);
    virtual int getNumDOF(void) const;    
    virtual int getNumFreeDOF(void) const;
    virtual int getNumConstrainedDOF(void) const;
    
    // methods to form the tangent
    const Matrix &getTangent(Integrator *theIntegrator);

    // methods to form the unbalance
    const Vector &getUnbalance(Integrator *theIntegrator);
    void  addM_Force(const Vector &Udotdot, double fact = 1.0);        

    const Vector &getTangForce(const Vector &x, double fact = 1.0);
    const Vector &getC_Force(const Vector &x, double fact = 1.0);
    const Vector &getM_Force(const Vector &x, double fact = 1.0);
    
    // methods to obtain committed responses from the nodes
    const Vector & getCommittedDisp(void);
    const Vector & getCommittedVel(void);
    const Vector & getCommittedAccel(void);
    
    // methods to update the trial response at the nodes
    void setNodeDisp(const Vector &u);
    void setNodeVel(const Vector &udot);
    void setNodeAccel(const Vector &udotdot);

    void incrNodeDisp(const Vector &u);
    void incrNodeVel(const Vector &udot);
    void incrNodeAccel(const Vector &udotdot);

    virtual void setEigenvector(int mode, const Vector &eigenvalue);

    int addSP_Constraint(SP_Constraint &theSP);
    int enforceSPs(void);

// AddingSensitivity:BEGIN ////////////////////////////////////
    void addM_ForceSensitivity(const Vector &Udotdot, double fact = 1.0);        
    void addD_ForceSensitivity(const Vector &vel, double fact = 1.0);
    void addD_Force(const Vector &vel, double fact = 1.0);

    const Vector & getDispSensitivity(int gradNumber);
    const Vector & getVelSensitivity(int gradNumber);
    const Vector & getAccSensitivity(int gradNumber);
    int saveSensitivity(Vector *v,Vector *vdot,Vector *vdotdot,int gradNum,int numGrads);
// AddingSensitivity:END //////////////////////////////////////
  };
} // fin namespace XC

#endif

