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
                                                                        
// $Revision: 1.8 $
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/KrylovNewton.h,v $
                                                                        
#ifndef KrylovNewton_h
#define KrylovNewton_h

// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// KrylovNewton.  KrylovNewton is a class which uses a Krylov
// subspace accelerator on the modified Newton method.
// The accelerator is described by Carlson and Miller in
// "Design and Application of a 1D GWMFE Code"
// from SIAM Journal of Scientific Computing (Vol. 19, No. 3,
// pp. 728-765, May 1998)

#include "EquiSolnAlgo.h"
#include "utility/matrix/Vector.h"

namespace XC {

//! @ingroup EQSolAlgo
//
//! @brief KrylovNewton is a class which uses a Krylov
//! subspace accelerator on the modified Newton method.
//! The accelerator is described by Carlson and Miller in
//! "Design and Application of a 1D GWMFE Code"
//! from SIAM Journal of Scientific Computing (Vol. 19, No. 3,
//! pp. 728-765, May 1998).
class KrylovNewton: public EquiSolnAlgo
  {
  private:
    int tangent;

    // Storage for update vectors
    std::vector<Vector> v;
    // Storage for subspace vectors
    std::vector<Vector> Av;

    // Array data sent to LAPACK subroutine
    Vector AvData;
    Vector rData;
    Vector work;

    // Length of work array
    int lwork;

    // Size information
    int numEqns;
    int maxDimension;

    // Private lsq routine to do Krylov updates
    // dimension is the current dimension of the subspace
    int leastSquares(int dimension);

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    KrylovNewton(SoluMethod *,int tangent = CURRENT_TANGENT, int maxDim = 3);    
    virtual SolutionAlgorithm *getCopy(void) const;
  public:
    int solveCurrentStep(void);    
    
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);    
  };
inline SolutionAlgorithm *KrylovNewton::getCopy(void) const
  { return new KrylovNewton(*this); }
} // fin namespace XC

#endif


