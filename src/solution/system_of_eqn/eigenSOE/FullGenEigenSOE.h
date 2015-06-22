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
// $Date: 2007/11/29 19:31:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/eigenSOE/FullGenEigenSOE.h,v $


#ifndef FullGenEigenSOE_h
#define FullGenEigenSOE_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 11/07
// Revision: A
//
// Description: This file contains the class definition for
// FullGenEigenSOE, which stores full nonsymmetric matrices,
// A and M, for generalized eigenvalue computations.

#include "EigenSOE.h"
#include "utility/matrix/Vector.h"

namespace XC {
class FullGenEigenSolver;

//! @ingroup EigenSOE
//
//! @brief Sistema de ecuaciones correspondiente
//! a un problema de autovalores con matriz densa no simétrica.
class FullGenEigenSOE : public EigenSOE
  {
  private:
    Vector A;
    Vector M;
  protected:
    bool setSolver(EigenSolver *);

    friend class SoluMethod;
    friend class FullGenEigenSolver;
    FullGenEigenSOE(SoluMethod *);
    SystemOfEqn *getCopy(void) const;
  public:
    virtual int setSize(Graph &theGraph);

    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    virtual int addM(const Matrix &, const ID &, double fact = 1.0);    

    virtual void zeroA(void);
    virtual void zeroM(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
inline SystemOfEqn *FullGenEigenSOE::getCopy(void) const
  { return new FullGenEigenSOE(*this); }
} // fin namespace XC

#endif
