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
                                                                        
// $Revision: 1.6 $
// $Date: 2005/10/20 23:52:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/beam2d/beam2d03.h,v $
                                                                        
                                                                        
// File: ~/element/beam2d03.h
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for beam2d03.
// beam2d03 is a plane frame member.

#ifndef beam2d03_h
#define beam2d03_h

#include "beam2d.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {
class Channel;

//! \ingroup ElemBarra
//
//! @brief Elemento barra 03 para problemas bidimensionales.
class beam2d03 : public beam2d
  {
  private:
    Matrix k; // the stiffness matrix
	
    mutable Vector rForce;
    Matrix trans;  // hold part of transformation matrix

    const Matrix &getStiff(void);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    beam2d03(int tag= 0);
    beam2d03(int tag, double A, double E, double I, int Nd1, int Nd2);
    Element *getCopy(void) const;

    void setDomain(Domain *theDomain);
    int revertToLastCommit(void);        
    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);    
    
  };
} // fin namespace XC

#endif


