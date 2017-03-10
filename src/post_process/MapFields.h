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
//MapFields.h
//Sets container.

#ifndef MAPFIELDS_H
#define MAPFIELDS_H

#include "FieldInfo.h"
#include "utility/actor/actor/MovableObject.h"
#include <deque>


namespace XC {
class Preprocessor;

//!  @ingroup POST_PROCESS
// 
//!  @brief Contenedor de definiciones de campo.
class MapFields: public EntCmd, public MovableObject, public std::deque<FieldInfo>
  {
  public:
    typedef std::deque<FieldInfo> dq_fields;
    typedef dq_fields::iterator iterator;
    typedef dq_fields::const_iterator const_iterator;
  private:
    Preprocessor *preprocessor; //!< Puntero al preprocesador.

    bool existe(const std::string &nmb) const;


    friend class ProblemaEF;
    iterator buscaField(const std::string &nmb);
    void clearAll(void);

  protected:

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    friend class Preprocessor;
  public:
    MapFields(Preprocessor *preprocessor= nullptr);
 
    FieldInfo &newField(const std::string &);

    const_iterator buscaField(const std::string &nmb) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif
