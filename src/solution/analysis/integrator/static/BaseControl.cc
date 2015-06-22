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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/10/19 21:53:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/BaseControl.cpp,v $
                                                                        
                                                                        
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for XC::BaseControl.
// BaseControl is an algorithmic class for perfroming a static analysis
// using a load control integration scheme.
//
// What: "@(#) BaseControl.h, revA"



#include <solution/analysis/integrator/static/BaseControl.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>
#include "xc_utils/src/base/CmdStatus.h"

//! @brief Constructor.
XC::BaseControl::BaseControl(SoluMethod *owr,int classTag,int numIncr)
:StaticIntegrator(owr,classTag), specNumIncrStep(numIncr), numIncrLastStep(numIncr)
  { setup_numIncr(numIncr); }

void XC::BaseControl::setup_numIncr(const int &numIncr)
  {
    // to avoid divide-by-zero error on first update() ensure numIncr != 0
    if(numIncr == 0)
      {
        std::cerr << "WARNING XC::BaseControl::BaseControl() - numIncr set to 0, 1 assumed\n";
        specNumIncrStep= 1.0;
        numIncrLastStep= 1.0;
      }
    else
      {
        specNumIncrStep= static_cast<double>(numIncr);
        numIncrLastStep= static_cast<double>(numIncr);
      }
  }

//! @brief Lee un objeto XC::BaseControl desde archivo
bool XC::BaseControl::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(BaseControl) Procesando comando: " << cmd << std::endl;
    if(cmd == "Jd")
      {
        setup_numIncr(interpretaInt(status.GetString()));
        return true;
      }
    else
      return StaticIntegrator::procesa_comando(status);
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::BaseControl::sendData(CommParameters &cp)
  {
    int res= StaticIntegrator::sendData(cp);
    res+= cp.sendDoubles(specNumIncrStep,numIncrLastStep,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::BaseControl::recvData(const CommParameters &cp)
  {
    int res= StaticIntegrator::recvData(cp);
    res+= cp.receiveDoubles(specNumIncrStep,numIncrLastStep,getDbTagData(),CommMetaData(1));
    return res;
  }
