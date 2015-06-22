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
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/Lista.h"

//Ground motions.
#include "domain/load/groundMotion/GroundMotionRecord.h"
#include "domain/load/groundMotion/InterpolatedGroundMotion.h"


#include "domain/load/pattern/Combinacion.h"

#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "utility/actor/actor/MovableMap.h"

//! @brief Constructor por defecto.
XC::LoadLoader::LoadLoader(Modelador *owr)
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


//! @brief Procesa los comandos que se emplean para definir
//! las el movimiento del terreno (sismo).
//! Interpreta los siguientes comandos:
//!
//! - ground_motion_record: Define un movimiento de tipo GroundMotionRecord.
//! - interpolated_ground_motion: Define un movimiento de tipo InterpolatedGroundMotion.
bool XC::LoadLoader::procesa_ground_motion(const std::string &cmd,CmdStatus &status)
  {
    if(cmd == "ground_motion_record")
      {
        load_ground_motion<GroundMotionRecord>(cmd,status);
        return true;
      }
    else if(cmd == "interpolated_ground_motion")
      {
        load_ground_motion<InterpolatedGroundMotion>(cmd,status);
        return true;       
      }
    else
      return false;
  }


//! @brief Procesa los comandos que se emplean para definir
//! las cargas sobre el modelo de
//! elementos finitos. Interpreta los siguientes comandos:
//!
//! - combinaciones: Dispara el intérprete de comandos del contenedor
//!   de combinaciones.
//! - combinaciones[nmbComb]: Dispara el intérprete de comandos de la combinación
//!   cuyo nombre se pasa como parámetro.
//! - casos: Dispara el intérprete de comandos del contenedor
//!   de casos de carga.
//! - add_to_domain: Agrega al dominio (activa) el caso de carga (hipótesis simple) o
//!   la combinación cuyo nombre se pasa como parámetro.
//! - remove_from_domain: Elimina del dominio (desactiva) el caso de carga (hipótesis simple) o
//!   la combinación cuyo nombre se pasa como parámetro.
//! - remove_all_from_domain: Elimina del dominio (desactiva) todos los casos de carga y
//!   combinaciones.
//! - for_each_load_pattern: Solicita a cada uno de los casos de carga definidos
//!   que ejecuten el bloque de código que se pasa como argumento.
//! - for_each_combinacion: Solicita a cada una de las combinaciones definidas
//!   que ejecuten el bloque de código que se pasa como argumento.
//! - clear_combinaciones: Borra las combinaciones definidas.
//! - clear_all: Borra todas las combinaciones, casos de carga y modulaciones temporales definidas.
bool XC::LoadLoader::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(LoadLoader) Procesando comando: " << cmd << std::endl;
    LoadPattern *lp_ptr= lpatterns.buscaLoadPattern(cmd);
    if(lp_ptr)
      {
        lp_ptr->LeeCmd(status);
        return true;
      }
    Combinacion *cmb_ptr= combinaciones.busca_combinacion(cmd);
    if(cmb_ptr)
      {
        cmb_ptr->LeeCmd(status);
        return true;
      }
    else if(procesa_ground_motion(cmd,status))
      { return true; }
    else if(cmd == "combinaciones")
      {
        combinaciones.LeeCmd(status);
        return true;
      }
    else if(cmd == "combinacion")
      {
        tag_lp= combinaciones.procesa_combinacion(tag_lp,cmd,status);
        return true;
      }
    else if(cmd == "casos")
      {
        lpatterns.LeeCmd(status);
        return true;
      }
    else if(cmd == "add_to_domain")
      {
        const std::string lp_code= interpretaString(status.GetString());
        addToDomain(lp_code);
        return true;
      }
    else if(cmd == "remove_from_domain")
      {
        const std::string lp_code= interpretaString(status.GetString());
        removeFromDomain(lp_code);
        return true;
      }
    else if(cmd == "remove_all_from_domain")
      {
        status.GetString();
        removeAllFromDomain();
        return true;
      }
    else if(cmd == "for_each_load_pattern")
      {
        const std::string bloque= status.GetBloque();
        lpatterns.EjecutaBloqueForEach(status,bloque);
        return true;
      }
    else if(cmd == "for_each_combinacion")
      {
        const std::string bloque= status.GetBloque();
        combinaciones.EjecutaBloqueForEach(status,bloque);
        return true;
      }
    else if(cmd == "clear_combinaciones")
      {
        status.GetBloque(); //Ignoramos entrada.
        combinaciones.clear();
        return true;
      }
    else if(cmd == "clearAll")
      {
        status.GetBloque(); //Ignoramos entrada.
        clearAll();
        return true;
      }
    else
      return Loader::procesa_comando(status);
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

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::LoadLoader::sendData(CommParameters &cp)
  {
    int res= sendMap(ground_motions,cp,getDbTagData(),CommMetaData(0));
    res+= cp.sendMovable(lpatterns,getDbTagData(),CommMetaData(1));
    res+= cp.sendInt(tag_lp,getDbTagData(),CommMetaData(2));
    res+= cp.sendMovable(combinaciones,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::LoadLoader::recvData(const CommParameters &cp)
  {
    int res= receiveMap(ground_motions,cp,getDbTagData(),CommMetaData(0),&FEM_ObjectBroker::getNewGroundMotion);
    res+= cp.receiveMovable(lpatterns,getDbTagData(),CommMetaData(1));
    res+= cp.receiveInt(tag_lp,getDbTagData(),CommMetaData(2));
    res+= cp.receiveMovable(combinaciones,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Envía el objeto por el canal que se pasa como parámetro.
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


//! @brief Recibe el objeto por el canal que se pasa como parámetro.
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


//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
//! - numCombinaciones: Devuelve el número de combinaciones definidas en el problema.
//! - numLoadPatterns: Devuelve el número de casos de carga (hipótesis simples) definidas en el problema.
//! - listaNombresLoadPatterns: Devuelve una lista con los nombres de los casos de carga definidos en el problema.
//! - listaNombresCombinaciones: Devuelve una lista con los nombres de las combinaciones definidas en el problema.
any_const_ptr XC::LoadLoader::GetProp(const std::string &cod) const
  {
    if(cod=="numCombinaciones")
      {
        tmp_gp_szt= combinaciones.size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="numLoadPatterns")
      {
        tmp_gp_szt= lpatterns.size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod == "listaNombresLoadPatterns")
      {
        EntCmd *this_no_const= const_cast<LoadLoader *>(this);
        tmp_gp_lista.set_owner(this_no_const);
        tmp_gp_lista.clearAll();
        const std::deque<std::string> tmp= lpatterns.getListaNombres();
	for(std::deque<std::string>::const_iterator i= tmp.begin();i!= tmp.end();i++)
          tmp_gp_lista.Inserta((*i));
        return any_const_ptr(tmp_gp_lista);
      }
    else if(cod == "listaNombresCombinaciones")
      {
        EntCmd *this_no_const= const_cast<LoadLoader *>(this);
        tmp_gp_lista.set_owner(this_no_const);
        tmp_gp_lista.clearAll();
        const std::deque<std::string> tmp= combinaciones.getListaNombres();
	for(std::deque<std::string>::const_iterator i= tmp.begin();i!= tmp.end();i++)
          tmp_gp_lista.Inserta((*i));
        return any_const_ptr(tmp_gp_lista);
      }
    else
      return Loader::GetProp(cod);
  }
