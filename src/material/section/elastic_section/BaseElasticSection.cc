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
//BaseElasticSection.cpp

#include <material/section/elastic_section/BaseElasticSection.h>
#include "material/section/repres/geom_section/GeomSection.h"
#include "modelador/loaders/MaterialLoader.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/utils_any.h"

XC::BaseElasticSection::BaseElasticSection(int tag, int classTag, const size_t &dim, MaterialLoader *mat_ldr)
  : SeccionBarraPrismatica(tag, classTag,mat_ldr), eTrial(dim), eInic(dim), eCommit(dim) {}

//! @brief Lee un objeto XC::BaseElasticSection desde archivo
bool XC::BaseElasticSection::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(BaseElasticSection) Procesando comando: " << cmd << std::endl;

    return SeccionBarraPrismatica::procesa_comando(status);
  }

int XC::BaseElasticSection::commitState(void)
  {
    eCommit = eTrial;
    return 0;
  }

int XC::BaseElasticSection::revertToLastCommit(void)
  {
    eTrial= eCommit;
    return 0;
  }

int XC::BaseElasticSection::revertToStart(void)
  {
    eCommit.Zero();
    return 0;
  }

int XC::BaseElasticSection::setInitialSectionDeformation(const XC::Vector &def)
  {
    eInic= def;
    return 0;
  }

int XC::BaseElasticSection::setTrialSectionDeformation(const XC::Vector &def)
  {
    eTrial= def;
    return 0;
  }

const XC::Vector &XC::BaseElasticSection::getSectionDeformation(void) const
  {
    static Vector retval;
    retval= eTrial-eInic;
    return retval;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::BaseElasticSection::sendData(CommParameters &cp)
  {
    int res= SeccionBarraPrismatica::sendData(cp);
    res+= cp.sendVector(eTrial,getDbTagData(),CommMetaData(5));
    res+= cp.sendVector(eInic,getDbTagData(),CommMetaData(6));
    res+= cp.sendVector(eCommit,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::BaseElasticSection::recvData(const CommParameters &cp)
  {
    int res= SeccionBarraPrismatica::recvData(cp);
    res+= cp.receiveVector(eTrial,getDbTagData(),CommMetaData(5));
    res+= cp.receiveVector(eInic,getDbTagData(),CommMetaData(6));
    res+= cp.receiveVector(eCommit,getDbTagData(),CommMetaData(7));
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::BaseElasticSection::GetProp(const std::string &cod) const
  {
    return SeccionBarraPrismatica::GetProp(cod);
  }
 
void XC::BaseElasticSection::Print(std::ostream &s, int flag) const
  {
    s << "BaseElasticSection, tag: " << this->getTag() << std::endl;
  }

