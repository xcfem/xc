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
                                                                        
// $Revision: 1.1 $
// $Date: 2002/10/29 20:26:23 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/feap/FeapMaterial02.cpp,v $
                                                                      
// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// FeapMaterial02.

#include <material/nD/feap/FeapMaterial02.h>
#include <utility/matrix/Vector.h>

XC::FeapMaterial02::FeapMaterial02(int tag,
			       double K, double G, double muK, double muG,
			       double lamK, double lamG, double theta):
  // 14 history variables and 7 material parameters
    FeapMaterial(tag, ND_TAG_FeapMaterial02, 14, 7)
{
  ud[0] = K;
  ud[1] = G;
  ud[2] = muK;
  ud[3] = muG;
  ud[4] = lamK;
  ud[5] = lamG;
  ud[6] = theta;
}

XC::FeapMaterial02::FeapMaterial02(void):
  FeapMaterial(0, ND_TAG_FeapMaterial02, 14, 7)
{
  // Does nothing
}

int XC::FeapMaterial02::fillDArray(void)
{
  // Look in FEAP programmer's manual
  
  return 0;
}
