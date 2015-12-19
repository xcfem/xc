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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSubstrSolver.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSubstrSolver.h
//
// Written: fmk 
// Created: Febuary 1997
// Revision: A
//
// Description: This file contains the class definition for 
// ProfileSPDLinSubstrSolver. ProfileSPDLinSubstrSolver is a 
// subclass of DomainSolver and ProfileSPDlinDirectSolver.
// It perform the operations needed of a substr domain decomp method
// on a ProfileSPDlinSOE object. 

// What: "@(#) ProfileSPDLinSubstrSolver.h, revA"

#ifndef ProfileSPDLinSubstrSolver_h
#define ProfileSPDLinSubstrSolver_h

#include <solution/system_of_eqn/linearSOE/DomainSolver.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSolver.h>
#include "utility/matrix/Matrix.h"

namespace XC {
class ProfileSPDLinSOE;

//! @ingroup LinearSolver
//
//! @brief Objetos que se encargan de obtener la solución
//! de sistemas de ecuaciones lineales con matriz de perfil.
class ProfileSPDLinSubstrSolver : public ProfileSPDLinDirectSolver,
                                  public DomainSolver
  {
  private:
    int dSize;
    Vector DU;
    Matrix Aext;
    Vector Yext;

  protected:
    friend class FEM_ObjectBroker;
    ProfileSPDLinSubstrSolver(double tol=1.0e-12);    
    virtual LinearSOESolver *getCopy(void) const;
  public:

    int solve(void);
    int condenseA(int numInt);
    int condenseRHS(int numInt, Vector *v =0);
    int computeCondensedMatVect(int numInt, const Vector &u);    
    const Matrix &getCondensedA(void);
    const Vector &getCondensedRHS(void);
    const Vector &getCondensedMatVect(void);
    
    int setComputedXext(const Vector &);
    int solveXint(void);

    int setSize(void);
    int getClassTag() const;
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline LinearSOESolver *ProfileSPDLinSubstrSolver::getCopy(void) const
   { return new ProfileSPDLinSubstrSolver(*this); }
} // end of XC namespace


#endif

