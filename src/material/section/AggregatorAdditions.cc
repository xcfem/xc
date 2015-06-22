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
//AggregatorAdditions.cc

#include "AggregatorAdditions.h"
#include "material/section/ResponseId.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Comprueba que los punteros no sean nulos.
bool XC::AggregatorAdditions::check_ptrs(void) const
  {
    if(!matCodes)
      {
        std::cerr << "AggregatorAdditions::AggregatorAdditions -- out of memory\n";
        return false;
      }
    return true;
  }

//! @brief Reserva memoria.
void XC::AggregatorAdditions::alloc_ptrs(const ResponseId &addCodes)
  {
    libera();
    matCodes = new ResponseId(addCodes);
    check_ptrs();
  }

//! @brief Reserva memoria.
void XC::AggregatorAdditions::alloc_ptrs(const ResponseId *addCodes)
  {
     if(addCodes)
       alloc_ptrs(*addCodes);
     else
       std::cerr << "AggregatorAdditions::alloc_ptrs;"
                 << " se pasó un puntero nulo." << std::endl;
  }

//! @brief Libera memoria.
void XC::AggregatorAdditions::libera(void)
  {
    if(matCodes)
      {
        delete matCodes;
        matCodes= nullptr;
      }
  }

//! @brief Constructor.
XC::AggregatorAdditions::AggregatorAdditions(EntCmd *owner)
  : DqUniaxialMaterial(owner), matCodes(nullptr) {}

//! @brief Constructor.
XC::AggregatorAdditions::AggregatorAdditions(EntCmd *owner,const UniaxialMaterial &um, int c)
  : DqUniaxialMaterial(owner,um), matCodes(nullptr)
  {
    alloc_ptrs(XC::ResponseId(1));
    (*matCodes)(0) = c;
  }

//! @brief Constructor de copia.
XC::AggregatorAdditions::AggregatorAdditions(const AggregatorAdditions &otro)
  : DqUniaxialMaterial(otro), matCodes(nullptr)
  {
    if(otro.matCodes)
      alloc_ptrs(otro.matCodes);
  }

//! @brief Constructor de copia.
XC::AggregatorAdditions::AggregatorAdditions(const AggregatorAdditions &otro,SectionForceDeformation *s)
  : DqUniaxialMaterial(otro,s)
  { 
    if(otro.matCodes)
      alloc_ptrs(otro.matCodes);
  }

//! @brief Operator asignación.
XC::AggregatorAdditions &XC::AggregatorAdditions::operator=(const AggregatorAdditions &otro)
  {
    DqUniaxialMaterial::operator=(otro);
    if(otro.matCodes)
      alloc_ptrs(otro.matCodes);
    return *this;
  }


//!  @brief Lee un objeto AggregatorAdditions desde el archivo de entrada.
//! 
//!  Soporta los comandos:
//! 
bool XC::AggregatorAdditions::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(AggregatorAdditions) Procesando comando: " << cmd << std::endl;
    if(cmd == "for_each_addition")
      { 
        const std::string bloque= status.GetBloque();
	DqUniaxialMaterial::ejecuta_bloque_for_each(status,bloque);
        return true;
      }
    else
      return DqUniaxialMaterial::procesa_comando(status);
  }
//! @brief Destructor.
XC::AggregatorAdditions::~AggregatorAdditions(void)
  { libera(); }


void XC::AggregatorAdditions::putMatCodes(const ResponseId &codes)
  { alloc_ptrs(codes); }

//! @brief Devuelve los códigos de los esfuerzos para los que la
//! sección aporta rigidez.
void XC::AggregatorAdditions::getType(ResponseId &retval,const size_t &offset) const
  {
    const size_t n= size();
    for(register size_t i=0;i<n;i++)
      retval(offset+i)= (*matCodes)(i);
  }

//! @brief Imprime el objeto.
void XC::AggregatorAdditions::Print(std::ostream &s, int flag) const
  {
    DqUniaxialMaterial::Print(s,flag);
    s << "\tUniaxial codes " << *matCodes << std::endl;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::AggregatorAdditions::sendData(CommParameters &cp)
  {
    int res= DqUniaxialMaterial::sendData(cp);
    res+= cp.sendIDPtr(matCodes,getDbTagData(),ArrayCommMetaData(2,3,4));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::AggregatorAdditions::recvData(const CommParameters &cp)
  {
    int res= DqUniaxialMaterial::recvData(cp);
    libera();
    matCodes= cp.receiveResponseIdPtr(matCodes,getDbTagData(),ArrayCommMetaData(2,3,4));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::AggregatorAdditions::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::AggregatorAdditions::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
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

//! @brief Devuelve la propiedad del objeto cuyo nombre (de la propiedad) se pasa como parámetro.
any_const_ptr XC::AggregatorAdditions::GetProp(const std::string &cod) const
  {
    if(cod=="strResponseId")
      {
        const size_t i= popSize_t(cod);
        tmp_gp_str= "NIL";
        if(matCodes)
          {
            const size_t sz= matCodes->Size();
            if(i<sz)
              tmp_gp_str= ResponseId::RespIdToString((*matCodes)[i]);
            else
	      std::cerr << "AggregatorAdditions::GetProp(strResponseId); índice fuera de rango." << std::endl;
	  }
        else
	  std::cerr << "AggregatorAdditions::GetProp, no se han definido respuestas." << std::endl;
        return any_const_ptr(tmp_gp_str);
      }
    else
      return DqUniaxialMaterial::GetProp(cod);
  }

