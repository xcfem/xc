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
                                                                        
#ifndef MRMFreedom_Constraint_h
#define MRMFreedom_Constraint_h

// File: ~/domain/constraints/MRMFreedom_Constraint.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for MRMFreedom_Constraint.
// MRMFreedom_Constraint is a class which stores the information for a multiple
// retained nodes constraint. A multipoint constraint relates certain dofs at 
// a constrained node to be related to certains dofs at multiple retained nodes:

// The MRMFreedom_Constraint class assumes time invariant constraints, i.e. the
// constraint matrix does not change over time. All the methods are declared
// as pure virtual, which will allow subclasses for time varying constraints.
//

#include "MFreedom_ConstraintBase.h"
#include <utility/matrix/Matrix.h>


namespace XC {
class Matrix;
class ID;

//! @ingroup CCont
//! @defgroup CContMP Condiciones de contorno multinodales.
//
//! @ingroup CContMP
//
//! @brief Multiple retained nodes constraint.
class MRMFreedom_Constraint: public MFreedom_ConstraintBase
  {
  protected:
    ID retainedNodeTags; //!< Retained nodes tags.

  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    // constructors        
    MRMFreedom_Constraint(int tag , int classTag ); // Arash
    MRMFreedom_Constraint(int tag); // LCPT

    MRMFreedom_Constraint(int tag,const ID &, int nodeConstr, int classTag);    

    MRMFreedom_Constraint(int tag,const ID &, int nodeConstr, const ID &constrainedDOF);    
    MRMFreedom_Constraint(int tag,const ID &, int nodeConstr, const ID &constrainedDOF,int classTag);    

    MRMFreedom_Constraint(int tag,const ID &, int nodeConstr, const Matrix &, const ID &constrainedDOF);
    MRMFreedom_Constraint(int tag,const Element &, const Node &, const ID &);


    // method to get information about the constraint
    virtual inline const ID &getRetainedNodeTags(void) const
      { return retainedNodeTags; }
    virtual inline ID &getRetainedNodeTags(void)
      { return retainedNodeTags; }
    inline virtual const ID &getRetainedDOFs(void) const
      { return getConstrainedDOFs(); } //Same as constrained.
    int getNumDofGroups(void) const;         
    int getNumDofs(void) const;           
    int getNumRetainedDofs(void) const;           
    int getNumConstrainedDofs(void) const;           
    int getNumLagrangeDofs(void) const;           
    bool afectaANodo(int tagNodo) const;
    virtual int applyConstraint(double pseudoTime);

    int addResistingForceToNodalReaction(bool inclInertia);

    // methods for output
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

    virtual void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace

#endif

