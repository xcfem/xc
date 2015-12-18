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
// $Source: /usr/local/cvs/OpenSees/SRC/PDeltaCrdTransf3d.h,v $

// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the class definition for
// PDeltaCrdTransf3d.h. PDeltaCrdTransf3d provides the
// abstraction of a linear transformation for a spatial frame
// between the global and basic coordinate systems

// What: "@(#) PDeltaCrdTransf3d.h, revA"

#ifndef PDeltaCrdTransf3d_h
#define PDeltaCrdTransf3d_h

#include "SmallDispCrdTransf3d.h"

namespace XC {
//! \ingroup ElemCT
//
//! @brief PDeltaCrdTransf3d provides the
//! abstraction of a linear transformation for a spatial frame
//! between the global and basic coordinate systems
class PDeltaCrdTransf3d: public SmallDispCrdTransf3d
  {
  private:
    double ul17; //!< Transverse local displacement offsets of P-Delta
    double ul28;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    PDeltaCrdTransf3d(int tag);
    PDeltaCrdTransf3d(int tag, const Vector &vecInLocXZPlane);
    PDeltaCrdTransf3d(int tag, const Vector &vecInLocXZPlane, const Vector &rigJntOffsetI, const Vector &rigJntOffsetJ);
    
    PDeltaCrdTransf3d(void);
    
    int update(void);
    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);
    
    const Vector &getGlobalResistingForce(const Vector &basicForce, const Vector &p0) const;
    const Matrix &getGlobalStiffMatrix(const Matrix &basicStiff, const Vector &basicForce) const;
    
    CrdTransf3d *getCopy(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0);
    
    // functions used in post-processing only    
    const Vector &getPointGlobalCoordFromLocal(const Vector &localCoords) const;
    const Vector &getPointGlobalDisplFromBasic(double xi, const Vector &basicDisps) const;
  };
} // end of XC namespace

#endif

