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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:00:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/MP_Constraint.h,v $
                                                                        
                                                                        
#ifndef MP_Constraint_h
#define MP_Constraint_h

// File: ~/domain/constraints/MP_Constraint.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for MP_Constraint.
// MP_Constraint is a class which stores the information for a multi
// point constraint. A multipoint constraint relates certain dof at 
// a constrained node to be related to certain dof at a retained node: 
//                      {Uc} = [Ccr] {Ur}
//
// The MP_Constraint class assumes time invariant constraints, i.e. the
// constraint matrix does not change over time. All the methods are declared
// as pure virtual, which will allow subclasses for time varying constraints.
//
// What: "@(#) MP_Constraint, revA"

#include "MP_ConstraintBase.h"
#include <utility/matrix/Matrix.h>


namespace XC {
class Matrix;
class ID;

//! @ingroup CCont
//! @defgroup CContMP Condiciones de contorno multinodales.
//
//! @ingroup CContMP
//
//! @brief Condición de contorno multinodal.
class MP_Constraint: public MP_ConstraintBase
  {
  protected:
    int retainedNodeTag; //!< Tag del nodo retenido.

    ID retainDOF;  //!< ID of retained DOF at retained node
    
    void set_retained_dofs(const ID &);
    void set_constrained_retained_dofs(const ID &constrainedDOF,const ID &retainedDOF);
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    // constructors        
    MP_Constraint(int tag , int classTag ); // Arash
    MP_Constraint(int tag); // LCPT

    MP_Constraint(int tag,int nodeRetain, int nodeConstr, int classTag);    

    MP_Constraint(int tag,int nodeRetain, int nodeConstr, const ID &constrainedDOF, const ID &retainedDOF);    
    MP_Constraint(int tag,int nodeRetain, int nodeConstr, const ID &constrainedDOF, const ID &retainedDOF,int classTag);    

    MP_Constraint(int tag,int nodeRetain, int nodeConstr, Matrix &constrnt,ID &constrainedDOF,ID &retainedDOF);
    MP_Constraint(const MP_Constraint &otro);
    MP_Constraint &operator=(const MP_Constraint &otro);

    // method to get information about the constraint
    virtual inline const int &getNodeRetained(void) const //Tag del nodo retenido o master.
      { return retainedNodeTag; }
    virtual inline int &getNodeRetained(void) //Tag del nodo retenido o master.
      { return retainedNodeTag; }
    bool afectaANodo(int tagNodo) const;
    virtual const ID &getRetainedDOFs(void) const;            
    virtual int applyConstraint(double pseudoTime);

    // methods for output
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

    virtual void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace

#endif

