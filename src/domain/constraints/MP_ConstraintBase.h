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
                                                                        
#ifndef MP_ConstraintBase_h
#define MP_ConstraintBase_h

// File: ~/domain/constraints/MP_ConstraintBase.h
//
// Written: lcpt 
// Created: 03/2015
// Revision: A
//
// Purpose: This file contains the class definition for MP_ConstraintBase.
// MP_ConstraintBase is a base clas for multi node constraints.
// A multipoint constraint relates certain dofs at 
// a constrained node to be related to certain dosf at one or more
// retained nodes

#include "Constraint.h"
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
class MP_ConstraintBase: public Constraint
  {
  protected:
    Matrix constraintMatrix;  //!< Constraint matrix.
    ID constrDOF;  //!< ID of constrained DOF at constrained node
    
    void set_constraint(const Matrix &c);
    void set_constrained_dofs(const ID &);
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    // constructors        
    MP_ConstraintBase(int tag , int classTag ); // Arash

    MP_ConstraintBase(int tag, int nodeConstr, int classTag);    

    MP_ConstraintBase(int tag, int nodeConstr, const ID &,int classTag);    

    MP_ConstraintBase(int tag, int nodeConstr, const Matrix &, const ID &, int classTag);

    // method to get information about the constraint
    virtual inline const int &getNodeConstrained(void) const
      { return constrNodeTag; }
    virtual inline int &getNodeConstrained(void)
      { return constrNodeTag; }
    bool afectaANodo(int tagNodo) const;
    virtual const ID &getConstrainedDOFs(void) const;        
    virtual const Matrix &getConstraint(void) const;

    bool isTimeVarying(void) const;

    int addResistingForceToNodalReaction(bool inclInertia);

    virtual void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace

#endif

