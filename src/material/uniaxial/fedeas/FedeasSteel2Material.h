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
// $Date: 2002/06/26 23:00:13 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasSteel2Material.h,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasSteel2Material. FedeasSteel2Material wraps the FEDEAS
// 1d material subroutine Steel_2.

#ifndef FedeasSteel2Material_h
#define FedeasSteel2Material_h

#include <material/uniaxial/FedeasMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief FedeasSteel2Material wraps the FEDEAS
//! 1d material subroutine Steel_2.
class FedeasSteel2Material : public FedeasMaterial
  {
  public:
    FedeasSteel2Material(int tag,
		double fy, double E0, double b,
		double R0, double cR1, double cR2,
		double a1, double a2, double a3, double a4);

	// Constructor for no isotropic hardening
    FedeasSteel2Material(int tag,
		double fy, double E0, double b,
		double R0, double cR1, double cR2);

     // Constructor for no isotropic hardening
     // Also provides default values for R0, cR1, and cR2
     FedeasSteel2Material(int tag,double fy, double E0, double b);

     FedeasSteel2Material(int tag, const Vector &d);
     FedeasSteel2Material(int tag= 0);

     double getInitialTangent(void) const;
     UniaxialMaterial *getCopy(void) const;
  };
} // fin namespace XC


#endif

