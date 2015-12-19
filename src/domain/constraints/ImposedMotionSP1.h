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
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/ImposedMotionSP1.h,v $
                                                                        
#ifndef ImposedMotionSP1_h
#define ImposedMotionSP1_h

// File: ~/domain/constraints/ImposedMotionSP1.h
//
// Written: fmk 
// Created: 11/00
// Revision: A
//
// Purpose: This file contains the class definition for ImposedMotionSP.
// ImposedMotionSP is a class which returns as the constraint, the DISPLACEMENT 
// ground motion response for a particular time.
//
// What: "@(#) ImposedMotionSP, revA"

#include <domain/constraints/ImposedMotionBase.h>

namespace XC {
class GroundMotion;
class Node;

//! @ingroup CContSP
//
//! @brief Movimiento impuesto sobre un nodo.
class ImposedMotionSP1 : public ImposedMotionBase
  {
  private:
    int destroyMotion; //!< flag indicating if destructor to be invoked on GMotion when done    
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    // constructors    
    ImposedMotionSP1(void);        
    ImposedMotionSP1(int spTag, int nodeTag, int ndof, int patternTag, int groundMotionTag);

    int applyConstraint(double loadFactor);    
    bool isHomogeneous(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace

#endif


