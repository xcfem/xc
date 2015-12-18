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
// $Date: 2000/09/15 08:23:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/numberer/PlainNumberer.h,v $
                                                                        
                                                                        
// File: ~/analysis/numberer/PlainNumberer.h
// 
// Written: fmk 
// Created: 9/96
// Revision: A
//
// Description: This file contains the class definition for PlainNumberer.
// PlainNumberer is a subclass of DOF_Numberer. The PlainNumberer assigns
// equation numbers to the DOFs on a first come first serve basis; that is 
// it gets the DOF_GrpIter and assigns the equation numbers to the DOFs
// as it iterates through the iter.
//
// What: "@(#) PlainNumberer.h, revA"

#ifndef PlainNumberer_h
#define PlainNumberer_h

#define START_EQN_NUMBER 0

#include <solution/analysis/numberer/DOF_Numberer.h>

namespace XC {

//! @ingroup Numerador
//
//! @brief Numerador de grados de libertad trivial.
class PlainNumberer: public DOF_Numberer
  {
  protected:
    friend class ModelWrapper;
    friend class FEM_ObjectBroker;
    PlainNumberer(ModelWrapper *);
  public:
    virtual DOF_Numberer *getCopy(void) const;

    int numberDOF(int lastDOF = -1);
    int numberDOF(ID &lastDOFs);    

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif

