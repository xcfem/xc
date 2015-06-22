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
// $Date: 2001/02/17 06:32:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSolver.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSolver.h
//
// Written: fmk 
// Created: Febuary 1997
// Revision: A
//
// Description: This file contains the class definition for 
// ProfileSPDLinDirectSolver. ProfileSPDLinDirectSolver is a subclass 
// of LinearSOESOlver. It solves a ProfileSPDLinSOE object using
// the LDL^t factorization.

// What: "@(#) ProfileSPDLinDirectSolver.h, revA"

#ifndef ProfileSPDLinDirectSolver_h
#define ProfileSPDLinDirectSolver_h

#include "ProfileSPDLinDirectBase.h"

namespace XC {
class ProfileSPDLinSOE;

//! @ingroup LinearSolver
//
//! @brief Objeto encargado de obtener la solución
//! de un sistemas de ecuaciones lineal con matriz de perfil.
class ProfileSPDLinDirectSolver : public ProfileSPDLinDirectBase
  {
  protected:
    friend class LinearSOE;
    friend class FEM_ObjectBroker;
    ProfileSPDLinDirectSolver(double tol=1.0e-12);    
    virtual LinearSOESolver *getCopy(void) const;
  public:
    virtual int solve(void);        
    virtual int setSize(void);    
    double getDeterminant(void);

    
    virtual int factor(int n);
    virtual int setProfileSOE(ProfileSPDLinSOE &theSOE);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline LinearSOESolver *ProfileSPDLinDirectSolver::getCopy(void) const
   { return new ProfileSPDLinDirectSolver(*this); }
} // fin namespace XC


#endif

