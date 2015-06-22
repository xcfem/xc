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
// $Date: 2004/09/01 03:54:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/damage/DamageResponse.h,v $
                                                                        
// Written: Arash Altoontash, Gregory Deierlein
// Created: Aug 2002
//
// Description: This file contains the DamageResponse class interface

#ifndef DamageResponse_h
#define DamageResponse_h

#include <domain/mesh/element/Information.h>
#include <utility/recorder/response/Response.h>

namespace XC {
class ID;
class DamageModel;

class DamageResponse : public Response
  {
  private:
    DamageModel *theDamage;
    int responseID;
  public:
    DamageResponse(DamageModel *dmg, int id);
    DamageResponse(DamageModel *dmg, int id, int val);
    DamageResponse(DamageModel *dmg, int id, double val);
    DamageResponse(DamageModel *dmg, int id, const ID &val);
    DamageResponse(DamageModel *dmg, int id, const Vector &val);
    DamageResponse(DamageModel *dmg, int id, const Matrix &val);
    DamageResponse(DamageModel *dmg, int id, const BJtensor &val);
    int getResponse(void);
  };
} // fin namespace XC

#endif
