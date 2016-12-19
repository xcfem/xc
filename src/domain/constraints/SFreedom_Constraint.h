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
// $Date: 2005/11/22 19:41:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/SFreedom_Constraint.h,v $
                                                                        
                                                                        
#ifndef SFreedom_Constraint_h
#define SFreedom_Constraint_h

// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for SFreedom_Constraint.
// SFreedom_Constraint is a class which stores the information for a single
// DOF constraint. Each single point constraint specifies a particular
// degree-of-freedom response (displacement, rotation) at a node.
// The constraint may be time-varying .. time varying constarints however 
// must be implemented using subclasses.
//
// What: "@(#) SFreedom_Constraint, revA"

#include "Constraint.h"

namespace XC {
  class ID;

//! @ingroup CCont
//! @defgroup CContSP Condiciones de contorno mononodales.
//
//! @ingroup CContSP
//
//! @brief Coacción de uno o varios grados de libertad de un sólo nodo.
class SFreedom_Constraint: public Constraint
  {
  protected:
    int dofNumber; //!< identifies which of the nodes dof is constrained 
    double valueR; //!< the reference value
    double valueC; //!< if constant = the reference value, if not constant = the reference value * load factor
    bool isConstant; //!< flag indicating if constant
    int  loadPatternTag;

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    // constructors
    SFreedom_Constraint(int classTag);        
    SFreedom_Constraint(int spTag, int nodeTag);
    SFreedom_Constraint(int spTag, int nodeTag, int ndof, int classTag);
    SFreedom_Constraint(int spTag, int nodeTag, int ndof, double value, bool isConstant= true);
    virtual SFreedom_Constraint *getCopy(void) const;
    SFreedom_Constraint *getCopy(const int &) const;

    virtual int getDOF_Number(void) const;
    virtual int applyConstraint(double loadFactor);    
    virtual double getValue(void) const;
    virtual bool isHomogeneous(void) const;
    virtual void setLoadPatternTag(int loadPaternTag);
    virtual int getLoadPatternTag(void) const;
 
    int getVtkCellType(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    virtual void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace

#endif


