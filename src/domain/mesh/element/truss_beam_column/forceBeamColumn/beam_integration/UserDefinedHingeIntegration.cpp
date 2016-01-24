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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.1 $
// $Date: 2006/01/18 21:58:24 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/UserDefinedHingeIntegration.cpp,v $

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedHingeIntegration.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

XC::UserDefinedHingeIntegration::UserDefinedHingeIntegration(int npL,
							 const XC::Vector &ptL,
							 const XC::Vector &wtL,
							 int npR,
							 const XC::Vector &ptR,
							 const XC::Vector &wtR):
  UserDefinedHingeIntegrationBase(BEAM_INTEGRATION_TAG_UserHinge,ptL,wtL,ptR,wtR)
  {}

XC::UserDefinedHingeIntegration::UserDefinedHingeIntegration():
  UserDefinedHingeIntegrationBase(BEAM_INTEGRATION_TAG_UserHinge)
  {}

XC::BeamIntegration* XC::UserDefinedHingeIntegration::getCopy(void) const
  { return new UserDefinedHingeIntegration(*this); }
