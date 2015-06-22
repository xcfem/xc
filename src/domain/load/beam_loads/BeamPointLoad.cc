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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/14 23:00:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/beam_loads/BeamPointLoad.cpp,v $
                                                                        
// Written: fmk 

// Purpose: This file contains the class implementation XC::BeamPointLoad.

#include <domain/load/beam_loads/BeamPointLoad.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/MovableVector.h"

XC::BeamPointLoad::BeamPointLoad(int tag, int classTag, double Pt, double dist, const XC::ID &theElementTags, double Pa)
  :BeamMecLoad(tag, classTag,Pt,Pa,theElementTags),x(dist) {}

XC::BeamPointLoad::BeamPointLoad(int tag, int classTag)
  :BeamMecLoad(tag, classTag), x(0.0) {}

//! @brief Lee un objeto BeamPointLoad desde archivo
bool XC::BeamPointLoad::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(BeamPointLoad) Procesando comando: " << cmd << std::endl;
    if(cmd == "x")
      {
        x= interpretaDouble(status.GetString());
        if(x<0.0 || x>1.0)
	  std::cerr << "Error; " << cmd << " la distancia relativa debe estar"
                    << "comprendida entre 0 y 1." << std::endl;
        return true;
      }
    else
      return BeamMecLoad::procesa_comando(status);
  }


std::string XC::BeamPointLoad::Categoria(void) const
  { return "puntual"; }

//! @brief Envía los datos a través del canal que se pasa como parámetro.
int XC::BeamPointLoad::sendData(CommParameters &cp)
  {
    int res= BeamMecLoad::sendData(cp);
    res+= cp.sendDouble(x,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
int XC::BeamPointLoad::recvData(const CommParameters &cp)
  {
    int res= BeamMecLoad::recvData(cp);
    res+= cp.receiveDouble(x,getDbTagData(),CommMetaData(6));
    return res;
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::BeamPointLoad::GetProp(const std::string &cod) const
  {
    if(cod == "x")
      return any_const_ptr(x);
    else
      return BeamMecLoad::GetProp(cod);
  }
