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

// $Revision: 1.7 $
// $Date: 2005/12/15 00:30:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/PDeltaCrdTransf2d.h,v $


// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Modified: May 2001 
//
// Description: This file contains the class definition for
// PDeltaCrdTransf2d.h. PDeltaCrdTransf2d provides the
// abstraction of a linear transformation for a spatial frame
// between the global and basic coordinate systems

// What: "@(#) PDeltaCrdTransf2d.h, revA"

#ifndef PDeltaCrdTransf2d_h
#define PDeltaCrdTransf2d_h

#include "SmallDispCrdTransf2d.h"

namespace XC {
//! \ingroup ElemCT
//
//! @brief PDeltaCrdTransf2d provides the
//! abstraction of a linear transformation for a spatial frame
//! between the global and basic coordinate systems
class PDeltaCrdTransf2d: public SmallDispCrdTransf2d
  {
  private: 
    double ul14; //!< Transverse local displacement offset of P-Delta

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    PDeltaCrdTransf2d(int tag);
    PDeltaCrdTransf2d(int tag, const Vector &rigJntOffsetI, const Vector &rigJntOffsetJ);
    
    PDeltaCrdTransf2d(void);

    int update(void);
    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);
        
    const Vector &getGlobalResistingForce(const Vector &basicForce, const Vector &p0) const;
    const Matrix &getGlobalStiffMatrix(const Matrix &basicStiff, const Vector &basicForce) const;
    const Matrix &getInitialGlobalStiffMatrix(const Matrix &basicStiff) const;
    
    CrdTransf2d *getCopy(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0);
  };
} // fin namespace XC

#endif

