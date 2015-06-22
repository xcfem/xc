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
//DbTagData.cc

#include "DbTagData.h"
#include "CommParameters.h"
#include "xc_basic/src/util/print_trace.h"

//! @brief Constructor.
XC::DbTagData::DbTagData(const int &sz)
  : data(sz) {}

//! @brief Constructor.
XC::DbTagData::DbTagData(const ID &id)
  : data(id) {}

//! @brief Inicializa los parámetros para comenzar la comunicación
//! de sz objetos.
void XC::DbTagData::inicComm(const int &sz)
  {
    const int szOld= data.Size();
    if(sz!=szOld)
      {
        data.resize(sz);
	std::cerr << "DbTagData::inicComm; se cambió el tamaño del vector: ("
                  << szOld << "->" << sz << ")" << std::endl;
        print_trace();
      }
    data.Zero();
    visitados.clear();
  }

//! @brief Devuelve el entero en la posición que se pasa como
//! parámetro.
const int &XC::DbTagData::getDbTagDataPos(const size_t &i) const
  {
    static int retval= 0;
    if(i<size_t(data.Size()))
      {
        if(visitados.find(i)!=visitados.end())
	  std::cerr << "DbTagData::getDbTagDataPos: el índice "
                << i << " ya se ha utilizado." << std::endl;
        retval= data(i);
        visitados.insert(i);
      }
    else
      std::cerr << "DbTagData::getDbTagDataPos: índice "
                << i << " fuera de rango; [0," << data.Size()-1
                << "]." << std::endl;
    return retval;
  }

//! @brief Asigna el entero en la posición que se pasa como
//! parámetro.
void XC::DbTagData::setDbTagDataPos(const size_t &i,const int &v)
  {
    if(i<size_t(data.Size()))
      {
        if(visitados.find(i)!=visitados.end())
	  std::cerr << "DbTagData::setDbTagDataPos: el índice "
                << i << " ya se ha utilizado." << std::endl;
        data[i]= v;
        visitados.insert(i);
      }
    else
      std::cerr << "DbTagData::setDbTagDataPos: índice "
                << i << " fuera de rango; [0," << data.Size()-1
                << "]." << std::endl;
  }

//! @brief Función de conveniencia.
int XC::DbTagData::sendIdData(CommParameters &cp,const int &dataTag) const
  { return cp.sendID(data,dataTag); }

//! @brief Función de conveniencia.
int XC::DbTagData::receiveIdData(const CommParameters &cp,const int &dataTag)
  { return cp.receiveID(data,dataTag); }

//! @brief Envía el objeto.
int XC::DbTagData::send(DbTagData &dt,CommParameters &cp, const CommMetaData &meta) const
  { return cp.sendID(data,dt,meta); }

//! @brief Recibe el objeto.
int XC::DbTagData::receive(DbTagData &dt,const CommParameters &cp, const CommMetaData &meta)
  { return cp.receiveID(data,dt,meta); }

//! @brief Imprime el objeto.
void XC::DbTagData::Print(std::ostream &os) const
  {
    os << "dbTags: [" << data << "] visitados: [";
    if(!visitados.empty())
      {
        std::set<size_t>::const_iterator i= visitados.begin();
        os << *i;i++;
        for(;i!=visitados.end();i++)
          os << " " << *i;
      }
    os << "]";
  }

std::ostream &XC::operator<<(std::ostream &os,const XC::DbTagData &dt)
  {
    dt.Print(os);
    return os;
  }
