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
//LoadLoader.cc

#include "LoadLoader.h"
#include "domain/domain/Domain.h"

//Ground motions.
#include "domain/load/groundMotion/GroundMotionRecord.h"
#include "domain/load/groundMotion/InterpolatedGroundMotion.h"


#include "domain/load/pattern/Combinacion.h"



#include "utility/actor/actor/MovableMap.h"

//! @brief Constructor por defecto.
XC::LoadLoader::LoadLoader(Preprocessor *owr)
  : Loader(owr), lpatterns(this), tag_lp(0), combinaciones(this) {}

//! @brief Agrega el caso de carga al dominio.
void XC::LoadLoader::addToDomain(const std::string &lp_code)
  {
    LoadPattern *lp= lpatterns.buscaLoadPattern(lp_code);
    if(lp)
      {
        bool result= getDomain()->addLoadPattern(lp);
        if((!result) && (verborrea>3))
          std::cerr << "LoadLoader::add_to_domain; no se pudo agregar la acción: '"
                    << lp_code << "'\n";
      }
    else
      combinaciones.addToDomain(lp_code);
  }

//! @brief Elimina el caso de carga del dominio.
void XC::LoadLoader::removeFromDomain(const std::string &lp_code)
  {
    LoadPattern *lp= lpatterns.buscaLoadPattern(lp_code);
    if(lp)
      getDomain()->removeLoadPattern(lp);
    else
      combinaciones.removeFromDomain(lp_code);
  }

void XC::LoadLoader::removeAllFromDomain(void)
  {
    combinaciones.removeAllFromDomain();
    lpatterns.removeAllFromDomain();
  }

//! @brief Borra todos los objetos.
void XC::LoadLoader::clearAll(void)
  {
    combinaciones.clear();
    lpatterns.clear();
  }

//! @brief Destructor.
XC::LoadLoader::~LoadLoader(void)
  { clearAll(); }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::LoadLoader::getDbTagData(void) const
  {
    static DbTagData retval(4);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::LoadLoader::sendData(CommParameters &cp)
  {
    int res= sendMap(ground_motions,cp,getDbTagData(),CommMetaData(0));
    res+= cp.sendMovable(lpatterns,getDbTagData(),CommMetaData(1));
    res+= cp.sendInt(tag_lp,getDbTagData(),CommMetaData(2));
    res+= cp.sendMovable(combinaciones,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::LoadLoader::recvData(const CommParameters &cp)
  {
    int res= receiveMap(ground_motions,cp,getDbTagData(),CommMetaData(0),&FEM_ObjectBroker::getNewGroundMotion);
    res+= cp.receiveMovable(lpatterns,getDbTagData(),CommMetaData(1));
    res+= cp.receiveInt(tag_lp,getDbTagData(),CommMetaData(2));
    res+= cp.receiveMovable(combinaciones,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::LoadLoader::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "::sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::LoadLoader::recvSelf(const CommParameters &cp)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }


