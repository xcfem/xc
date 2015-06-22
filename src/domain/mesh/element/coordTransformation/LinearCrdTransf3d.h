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

// $Revision: 1.9 $
// $Date: 2005/12/15 00:30:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/LinearCrdTransf3d.h,v $

// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the class definition for
// LinearCrdTransf3d.h. LinearCrdTransf3d provides the
// abstraction of a linear transformation for a spatial frame
// between the global and basic coordinate systems

// What: "@(#) LinearCrdTransf3d.h, revA"

#ifndef LinearCrdTransf3d_h
#define LinearCrdTransf3d_h

#include "SmallDispCrdTransf3d.h"

namespace XC {

//! \ingroup ElemCT
//
//! @brief Transformación de coordenadas lineal.
//! este objeto realiza una transformación geométrica lineal de la
//! rigidez y de la respuesta en fuerzas de un elemento desde el sistema local
//! al sistema global.
class LinearCrdTransf3d: public SmallDispCrdTransf3d
  {
  public:
    LinearCrdTransf3d(int tag);
    LinearCrdTransf3d(int tag, const Vector &vecInLocXZPlane);
    LinearCrdTransf3d(int tag, const Vector &vecInLocXZPlane, const Vector &rigJntOffsetI, const Vector &rigJntOffsetJ);
    
    LinearCrdTransf3d(void);
    
    int update(void);
    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);
    
    const Vector &getGlobalResistingForce(const Vector &basicForce, const Vector &p0) const;
    const Matrix &getGlobalStiffMatrix(const Matrix &basicStiff, const Vector &basicForce) const;
    
    CrdTransf3d *getCopy(void) const;
    
    void Print(std::ostream &s, int flag = 0);
    
    // functions used in post-processing only    
    const Vector &getPointGlobalCoordFromLocal(const Vector &) const;
    const Vector &getPointGlobalDisplFromBasic(double xi, const Vector &) const;
  };
} // fin namespace XC

#endif

