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


#include "BidimStrainLoad.h"
#include "utility/matrix/Vector.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "utility/matrix/ID.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "utility/actor/actor/MovableVectors.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/Matrix.h"


XC::BidimStrainLoad::BidimStrainLoad(int tag, const std::vector<Vector> &t,const ID &theElementTags)
  :BidimLoad(tag, LOAD_TAG_BidimStrainLoad, theElementTags), deformaciones(t) {}
XC::BidimStrainLoad::BidimStrainLoad(int tag,const size_t &sz,const Vector &t,const ID &theElementTags)
  :BidimLoad(tag, LOAD_TAG_BidimStrainLoad, theElementTags), deformaciones(sz,t) {}

XC::BidimStrainLoad::BidimStrainLoad(int tag,const size_t &sz, const ID &theElementTags)
  :BidimLoad(tag, LOAD_TAG_BidimStrainLoad, theElementTags), deformaciones(sz) {}

XC::BidimStrainLoad::BidimStrainLoad(int tag,const size_t &sz, const Vector &t)
  :BidimLoad(tag, LOAD_TAG_BidimStrainLoad), deformaciones(sz,t) {}

XC::BidimStrainLoad::BidimStrainLoad(int tag,const size_t &sz)
  :BidimLoad(tag, LOAD_TAG_BidimStrainLoad), deformaciones(sz) {}

XC::BidimStrainLoad::BidimStrainLoad(const size_t &sz)
  :BidimLoad(0,LOAD_TAG_BidimStrainLoad), deformaciones(sz) {}

//! @brief Asigna el valor de la deformación de uno de los puntos de Gauss.
void XC::BidimStrainLoad::set_comp(const std::string &cmd,CmdStatus &status)
  {
    const std::vector<size_t> &indices= GetSubscriptsNumericos(status,1);
    if(!indices.empty())
      {
        if(indices[0]<deformaciones.size())
          deformaciones.at(indices[0])= Vector(convert_to_vector_double(interpretaVectorAny(status.GetBloque())));
        else
          std::cerr << nombre_clase() << "procesando comando " << cmd
                    << " no existe el punto de Gauss de índice: "  << indices[0]
                    << "." << std::endl;
      }
  }

//! @brief Asigna el valor de la deformación de uno de los puntos de Gauss.
//! el primer índice es el del punto de gauss y el segundo la dirección de
//! la deformación.
void XC::BidimStrainLoad::set_strain(const std::string &cmd,CmdStatus &status)
  {
    const std::vector<size_t> &indices= GetSubscriptsNumericos(status,2);
    if(!indices.empty())
      {
        if(indices[0]<deformaciones.size())
          {
            Vector &def= deformaciones.at(indices[0]);
            if(indices[1]<size_t(def.Size()))
              def(indices[1])= interpretaDouble(status.GetString());
            else
              std::cerr << nombre_clase() << "procesando comando " << cmd
                        << " no existe la componente del vector deformación de índice: "
                        << indices[1] << "." << std::endl;
          }
        else
          std::cerr << nombre_clase() << "procesando comando " << cmd
                    << " no existe el punto de Gauss de índice: "  << indices[0]
                    << "." << std::endl;
      }
  }

//! @brief Asigna el valor de la deformación de uno de los puntos de Gauss.
//! el primer índice es el del punto de gauss y el segundo la dirección de
//! la deformación.
void XC::BidimStrainLoad::setStrainComp(const size_t &i,const size_t &j,const double &strain)
  {
    if(i<deformaciones.size())
      {
        Vector &def= deformaciones.at(i);
        if(j<size_t(def.Size()))
          def(j)= strain;
        else
          std::cerr << nombre_clase() << "::setStrainComp "
                    << " no existe la componente del vector deformación de índice: "
                    << j << "." << std::endl;
      }
    else
      std::cerr << nombre_clase() << "::setStrainComp "
                << " no existe el punto de Gauss de índice: "  << i
                << std::endl;
  }


//! @brief Lee un objeto BidimStrainLoad desde archivo
bool XC::BidimStrainLoad::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(BidimStrainLoad) Procesando comando: " << cmd << std::endl;
    if(cmd == "comp") //Asigna el valor de la deformación
      {               //en uno de los puntos de Gauss.
        set_comp(cmd,status);
        return true;
      }
    else if(cmd == "strain")
      {
        set_strain(cmd,status);
        return true;
      }
    else
      return BidimLoad::procesa_comando(status);
  }

//! @brief Asigna las deformaciones.
void XC::BidimStrainLoad::setDeformaciones(const Matrix &def)
  {
    const int nRows= def.noRows();
    const int nCols= def.noCols();
    std::vector<Vector> tmp(nRows);
    Vector ri(nCols);
    for(int i= 0;i<nRows;i++)
      {
        for(int j= 0;j<nCols;j++)
          ri[j]= def(i,j);
        tmp[i]= ri;
      }
    deformaciones= tmp;
  }


//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::BidimStrainLoad::getDbTagData(void) const
  {
    static DbTagData retval(6);
    return retval;
  }

//! @brief Envía los datos a través del canal que se pasa como parámetro.
int XC::BidimStrainLoad::sendData(CommParameters &cp)
  {
    int res= BidimLoad::sendData(cp);
    res+= cp.sendVectors(deformaciones,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
int XC::BidimStrainLoad::recvData(const CommParameters &cp)
  {
    int res= BidimLoad::recvData(cp);
    res+= cp.receiveVectors(deformaciones,getDbTagData(),CommMetaData(5));
    return res;
  }

int XC::BidimStrainLoad::sendSelf(CommParameters &cp)
  {
    inicComm(6);
    int res= sendData(cp);

    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "BidimStrainLoad::sendSelf() - failed to send data\n";    
    return res;
  }

int XC::BidimStrainLoad::recvSelf(const CommParameters &cp)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "TrussStrainLoad::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }

void XC::BidimStrainLoad::Print(std::ostream &s, int flag) const
  {
    s << "BidimStrainLoad" << std::endl;
    if(!deformaciones.empty())
      {
        std::vector<Vector>::const_iterator i= deformaciones.begin();
        s << *i;
        for(;i!=deformaciones.end();i++)
          s << ", " << *i;
      }
    BidimLoad::Print(s,flag);
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::BidimStrainLoad::GetProp(const std::string &cod) const
  {
    if(cod=="getSize")
      {
        tmp_gp_szt= deformaciones.size();
        return any_const_ptr(tmp_gp_szt);
      }
    if(cod=="getStrainVectorSize")
      {
           
        tmp_gp_szt= 0;
        if(!deformaciones.empty())
          tmp_gp_szt= deformaciones[0].Size();
        return any_const_ptr(tmp_gp_szt);
      }
    else
      return BidimLoad::GetProp(cod);
  }
