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
                                                                        
// $Revision: 1.1 $
// $Date: 2001/11/19 22:44:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/eigenSOE/SymBandEigenSolver.h,v $

// Written: MHS
// Created: Oct 2001
//
// Description: This file contains the class definition for
// SymBandEigenSolver, which computes the eigenvalues and eigenvectors
// of a symmetric banded matrix using the LAPACK subroutine dsbevx.

#ifndef SymBandEigenSolver_h
#define SymBandEigenSolver_h

#include <solution/system_of_eqn/eigenSOE/EigenSolver.h>
#include "utility/matrix/Vector.h"

namespace XC {
class Vector;
class SymBandEigenSOE;

//! @ingroup EigenSolver
//
//! @brief Objetos que se encargan de la solución
//! de problemas de autovalores l sistema de ecuaciones.
class SymBandEigenSolver : public EigenSolver
  {
  private:
    SymBandEigenSOE *theSOE;

    Vector eigenvalue;
    Vector eigenvector;
    mutable Vector eigenV;
    Vector work;

    friend class EigenSOE;
    SymBandEigenSolver(void);    
    virtual EigenSolver *getCopy(void) const;
    bool setEigenSOE(EigenSOE *theSOE);
  public:  
    virtual int solve(void);
    virtual int solve(int nModes);
    virtual int setSize(void);
    const int &getSize(void) const;
    virtual bool setEigenSOE(SymBandEigenSOE &theSOE);
  
    virtual const Vector &getEigenvector(int mode) const;
    virtual const double &getEigenvalue(int mode) const;
  
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline EigenSolver *SymBandEigenSolver::getCopy(void) const
   { return new SymBandEigenSolver(*this); }
} // fin namespace XC

#endif
