//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//AggregatorAdditions.cc

#include "AggregatorAdditions.h"
#include "material/section/ResponseId.h"


#include "utility/actor/actor/MovableID.h"
#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Comprueba que los pointers no sean nulos.
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
                 << " se pasó a null pointer." << std::endl;
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

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::AggregatorAdditions::sendData(CommParameters &cp)
  {
    int res= DqUniaxialMaterial::sendData(cp);
    res+= cp.sendIDPtr(matCodes,getDbTagData(),ArrayCommMetaData(2,3,4));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::AggregatorAdditions::recvData(const CommParameters &cp)
  {
    int res= DqUniaxialMaterial::recvData(cp);
    libera();
    matCodes= cp.receiveResponseIdPtr(matCodes,getDbTagData(),ArrayCommMetaData(2,3,4));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
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

//! @brief Receives object through the channel being passed as parameter.
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
