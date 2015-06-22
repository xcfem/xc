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
                                                                        
// $Revision: 1.2 $
// $Date: 2002/06/26 23:00:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasConcr2Material.h,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasConcr2Material. FedeasConcr2Material wraps the FEDEAS
// 1d material subroutine Concr_2.

#ifndef FedeasConcr2Material_h
#define FedeasConcr2Material_h

#include "FedeasConcrMaterial.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief FedeasConcr2Material wraps the FEDEAS
//! 1d material subroutine Concr_2.
class FedeasConcr2Material: public FedeasConcrMaterial
  {
  public:
    FedeasConcr2Material(int tag, double fc, double ec, double fu, double eu, double ratio, double ft, double Ets);
    FedeasConcr2Material(int tag, const Vector &data);
    FedeasConcr2Material(int tag= 0);

    UniaxialMaterial *getCopy(void) const;
  };
} // fin namespace XC


#endif

