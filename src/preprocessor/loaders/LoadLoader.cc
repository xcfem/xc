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
//LoadLoader.cc

#include "LoadLoader.h"
#include "domain/domain/Domain.h"

//Ground motions.
#include "domain/load/groundMotion/GroundMotionRecord.h"
#include "domain/load/groundMotion/InterpolatedGroundMotion.h"


#include "domain/load/pattern/LoadCombination.h"



#include "utility/actor/actor/MovableMap.h"

//! @brief Default constructor.
XC::LoadLoader::LoadLoader(Preprocessor *owr)
  : Loader(owr), lpatterns(this), tag_lp(0), combinations(this) {}

//! @brief Agrega el load pattern al domain.
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
      combinations.addToDomain(lp_code);
  }

//! @brief Elimina el load pattern del domain.
void XC::LoadLoader::removeFromDomain(const std::string &lp_code)
  {
    LoadPattern *lp= lpatterns.buscaLoadPattern(lp_code);
    if(lp)
      getDomain()->removeLoadPattern(lp);
    else
      combinations.removeFromDomain(lp_code);
  }

void XC::LoadLoader::removeAllFromDomain(void)
  {
    combinations.removeAllFromDomain();
    lpatterns.removeAllFromDomain();
  }

//! @brief Clears all the objects.
void XC::LoadLoader::clearAll(void)
  {
    combinations.clear();
    lpatterns.clear();
  }

//! @brief Destructor.
XC::LoadLoader::~LoadLoader(void)
  { clearAll(); }

//! @brief Returns a vector para almacenar los dbTags
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
    res+= cp.sendMovable(combinations,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::LoadLoader::recvData(const CommParameters &cp)
  {
    int res= receiveMap(ground_motions,cp,getDbTagData(),CommMetaData(0),&FEM_ObjectBroker::getNewGroundMotion);
    res+= cp.receiveMovable(lpatterns,getDbTagData(),CommMetaData(1));
    res+= cp.receiveInt(tag_lp,getDbTagData(),CommMetaData(2));
    res+= cp.receiveMovable(combinations,getDbTagData(),CommMetaData(3));
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


