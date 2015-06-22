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
// $Date: 2005/05/18 19:24:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/petsc/PetscSOE.h,v $
                                                                        
                                                                        
// Written: fmk & om
// Created: 7/98
// Revision: A
//
// Description: This file contains the class definition for PetscSOE
// PetscSOE is a subclass of LinearSOE. It uses the LAPACK storage
// scheme to store the components of the A matrix, which is a full matrix.


// What: "@(#) PetscSOE.h, revA"

#ifndef PetscSOE_h
#define PetscSOE_h

#include "solution/system_of_eqn/linearSOE/FactoredSOEBase.h"
#include "solution/system_of_eqn/linearSOE/DistributedLinSOE.h"

// extern "C" {
#include "petsc/petscksp.h"

// }

#if (PETSC_VERSION_MAJOR == 3) && (PETSC_VERSION_MINOR < 2)
typedef PetscTruth PetscBool;
#endif

namespace XC {
class PetscSolver;

//! @ingroup SOE
//
//! @brief Sistema de ecuaciones que hace
//! uso de la biblioteca «Portable, Extensible Toolkit for
//! Scientific Computation»: PetSC (http://www.mcs.anl.gov/petsc/petsc-as).
class PetscSOE: public FactoredSOEBase, public DistributedLinSOE
  {
  private:
    int numProcesses;

    Mat A;
    Vec x, b;
    int blockSize;
    PetscBool flg;

    int startRow, endRow;
    void InvokePetscDestructors(void);
  protected:
    inline ChannelQueue &getChannels(void)
      { return theChannels; }
    virtual bool setSolver(PetscSolver *);

    friend class SoluMethod;
    PetscSOE(SoluMethod *,int blockSize=1);    
    SystemOfEqn *getCopy(void) const;    
  public:
    ~PetscSOE(void);

    int setSize(Graph &theGraph);
    
    int addA(const Matrix &, const ID &, double fact = 1.0);

    void zeroA(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    friend class PetscSolver;
    friend class ActorPetscSOE;
    friend class ShadowPetscSOE;
    
  };
inline SystemOfEqn *PetscSOE::getCopy(void) const
  { return new PetscSOE(*this); }
} // fin namespace XC


#endif

