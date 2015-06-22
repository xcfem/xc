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
//DomainRecorderBase.cc

#include <utility/recorder/DomainRecorderBase.h>
#include "xc_utils/src/base/CmdStatus.h"

XC::DomainRecorderBase::DomainRecorderBase(int classTag,Domain *ptr_dom)
  :Recorder(classTag),theDomain(0) {}


//! @brief Lee un objeto XC::DomainRecorderBase desde archivo
bool XC::DomainRecorderBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(DomainRecorderBase) Procesando comando: " << cmd << std::endl;
    return Recorder::procesa_comando(status);
  }

int XC::DomainRecorderBase::setDomain(Domain &theDom)
  {
    theDomain = &theDom;
    return 0;
  }
