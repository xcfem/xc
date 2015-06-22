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
//DqGroundMotions.cc

#include "DqGroundMotions.h"
#include "GroundMotion.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "boost/any.hpp"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::DqGroundMotions::DqGroundMotions(const size_t &num)
  : EntCmd(), MovableObject(0), gMotions(num,static_cast<GroundMotion *>(nullptr)) {}

//! @brief Constructor de copia.
XC::DqGroundMotions::DqGroundMotions(const DqGroundMotions &otro)
  : EntCmd(otro), MovableObject(0), gMotions(otro.gMotions) {}

//! @brief Operador asignación.
XC::DqGroundMotions &XC::DqGroundMotions::operator=(const DqGroundMotions &otro)
  {
    EntCmd::operator=(otro);
    gMotions= otro.gMotions;
    return *this;
  }

void XC::DqGroundMotions::addMotion(GroundMotion &gm)
  {
    const_iterator i= std::find(begin(),end(),&gm);
    if(i == end())
      gMotions.push_back(&gm);
  }


//! @brief Lee un objeto XC::DqGroundMotions desde archivo
bool XC::DqGroundMotions::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(DqGroundMotions) Procesando comando: " << cmd << std::endl;
    const CmdParser &parser= status.Parser();

    if(cmd == "for_each")
      { 
        const std::string bloque= status.GetBloque();
        ejecuta_bloque_for_each(status,bloque);
        return true;
      }
    else if(cmd == "groundMotion")
      {
        std::deque<boost::any> indices;
        if(parser.TieneIndices())
          {
            indices= parser.SeparaIndices(this);
            if(!indices.empty())
              {
                const int idx= convert_to_int(indices[0]);
                GroundMotion *gm= gMotions[idx];
                if(gm)
                  gm->LeeCmd(status);
                else
                  std::cerr << "XC::DqGroundMotions::procesa_comando: '" << cmd
			    << "'; no se encontró la el movimiento de índice: " << idx
                            << std::endl;
              }
          }
        else
	  std::cerr << "XC::DqGroundMotions::procesa_comando: '" << cmd
                    << "'; se necesita el índice del movimiento." << std::endl;
        return true;
      }
    else
      return EntCmd::procesa_comando(status);
  }

void XC::DqGroundMotions::clear(void)
  {
    iterator i= gMotions.begin();
    for(;i!= gMotions.end();i++)
      if(*i)
        delete (*i);
    gMotions.erase(gMotions.begin(),gMotions.end());
  }

//! @brief Destructor:
XC::DqGroundMotions::~DqGroundMotions(void)
  { clear(); }

//! @brief Hace que cada una de los objetos ejecute el bloque que se pasa como parámetro.
void XC::DqGroundMotions::ejecuta_bloque_for_each(CmdStatus &status,const std::string &bloque)
  {
    const std::string nmbBlq= nombre_clase()+":for_each";
    iterator i= gMotions.begin();
    for(;i!= gMotions.end();i++)
      if(*i)
        (*i)->EjecutaBloque(status,bloque,nmbBlq);
      else
	std::cerr << "DqGroundMotions::ejecuta_bloque_for_each; Puntero nulo." << std::endl;
  }

//! como parámetro.
any_const_ptr XC::DqGroundMotions::GetProp(const std::string &cod) const
  {
    if(verborrea>4)
      std::clog << "DqGroundMotions::GetProp (" << nombre_clase() << "::GetProp) Buscando propiedad: " << cod << std::endl;

    if(cod=="size")
      {
        tmp_gp_szt= getNumGroundMotions();
        return any_const_ptr(tmp_gp_szt);
      }
    else
      return EntCmd::GetProp(cod);
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::DqGroundMotions::sendData(CommParameters &cp)
  {
    const size_t sz= getNumGroundMotions();
    setDbTagDataPos(0,sz);
    DbTagData cpGM(sz*2);
    int res= 0;
    for(size_t i= 0;i<sz;i++)
      res+= cp.sendBrokedPtr((*this)[i],cpGM,BrokedPtrCommMetaData(i,0,i+sz));
    res+= cpGM.send(getDbTagData(),cp,CommMetaData(1));
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::DqGroundMotions::recvData(const CommParameters &cp)
  {
    const size_t sz= getDbTagDataPos(0);
    DbTagData cpGM(sz*2);
    int res= cpGM.receive(getDbTagData(),cp,CommMetaData(1));

    for(size_t i= 0;i<sz;i++)
      {
        // Receive the ground motion
        (*this)[i]= cp.getBrokedGroundMotion((*this)[i],cpGM,BrokedPtrCommMetaData(i,0,i+sz));
        if(!(*this)[i])
          std::cerr << "DqGroundMotions::recvSelf() - ground motion "
                    << i << "failed to recv itself\n";
      }
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::DqGroundMotions::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "WARNING DqGroundMotions::sendSelf() - " 
                << dataTag << " failed to send.";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::DqGroundMotions::recvSelf(const CommParameters &cp)
  {
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "WARNING DqGroundMotions::recvSelf() - "
                << dataTag << " failed to receive ID\n";
    else
      res+= recvData(cp);
    return res;
  }

void XC::DqGroundMotions::Print(std::ostream &s,const int &flag)
  {
    s << "\tNumber of GroundMotions: " << getNumGroundMotions() << std::endl;
  }
