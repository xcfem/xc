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
// $Date: 2000/09/15 08:23:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/ShadowPetscSOE.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/petsc/ShadowPetscSOE.h
//
// Written: fmk & om
// Created: 7/98
// Revision: A
//
// Description: This file contains the class definition for ShadowPetscSOE
// ShadowPetscSOE is a subclass of LinearSOE. 

// What: "@(#) ShadowPetscSOE.h, revA"

#ifndef ShadowPetscSOE_h
#define ShadowPetscSOE_h

#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/system_of_eqn/linearSOE/petsc/PetscSOE.h>

// extern "C" {
#include <solution/system_of_eqn/linearSOE/petsc/petsc.h>
// }

namespace XC {
class PetscSolver;

//! @ingroup SOE
//
//! @brief ??.
class ShadowPetscSOE: public LinearSOE
  {
  private:
    MPI_Comm theComm; // a comm for communicating to the ActorPetscSOE's
                      // without using PETSC_COMM_WORLD
    PetscSOE theSOE;  // the local portion of the SOE
    PetscSolver *theSolver; // created by the user
    int myRank;
    int numProcessors;
    int sendData[3];
    void *sendBuffer;
    int blockSize;

    friend class SoluMethod;
    ShadowPetscSOE(SoluMethod *,PetscSolver &theSolver, int blockSize);
    SystemOfEqn *getCopy(void) const;
  public:
    ~ShadowPetscSOE(void);

    int solve(void);

    int getNumEqn(void) const;
    int setSize(Graph &theGraph);

    int addA(const Matrix &, const ID &, double fact = 1.0);
    int addB(const Vector &, const ID &,const double &fact= 1.0);
    int setB(const Vector &, const double &fact= 1.0);

    void zeroA(void);
    void zeroB(void);

    const Vector &getX(void);
    const Vector &getB(void);
    double normRHS(void);

    void setX(int loc, double value);

    int setSolver(PetscSolver &);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
inline SystemOfEqn *ShadowPetscSOE::getCopy(void) const
  { return new ShadowPetscSOE(*this); }
} // fin namespace XC


#endif






