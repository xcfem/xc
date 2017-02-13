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
//MovableContainer
                                                                        
                                                                        
#ifndef MovableContainer_h
#define MovableContainer_h

#include "utility/actor/actor/MovableObject.h"
#include <deque>

namespace XC {

template <class C>
class MovableContainer: public MovableObject
  {
  public:
    typedef typename C::iterator iterator;
    typedef typename C::const_iterator const_iterator;
    typedef typename C::reference reference;
    typedef typename C::const_reference const_reference;
  protected:
    C &contenedor;
    DbTagData &getDbTagData(void) const;
    virtual int sendItem(const_reference s,CommParameters &,DbTagData &, const CommMetaData &)= 0;
    virtual int receiveItem(reference s,const CommParameters &,DbTagData &, const CommMetaData &)= 0;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    explicit MovableContainer(C &);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };

//! @brief Constructor.
template <class C>
XC::MovableContainer<C>::MovableContainer(C &v)
  : MovableObject(0), contenedor(v) {}


//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
template <class C>
XC::DbTagData &XC::MovableContainer<C>::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Envía los datos through the channel being passed as parameter.
template <class C>
int XC::MovableContainer<C>::sendData(CommParameters &cp)
  {
    const size_t sz= contenedor.size();
    DbTagData &dt= getDbTagData();

    dt.setDbTagDataPos(0, sz);

    DbTagData dbTags(sz);

    int res= 0;
    int loc= 0;
    for(const_iterator i= contenedor.begin();i!=contenedor.end();i++)
      res+= this->sendItem(*i,cp,dbTags,CommMetaData(loc++));
    res+= dbTags.send(dt,cp,CommMetaData(1));
    if(res<0)
      std::cerr << "MovableContainer::sendSelf() - failed to send ID.\n";
    return res;
  }

//! @brief Recibe los datos through the channel being passed as parameter.
template <class C>
int XC::MovableContainer<C>::recvData(const CommParameters &cp)
  {
    DbTagData &dt= getDbTagData();
    const int sz= dt.getDbTagDataPos(0);
    contenedor.resize(sz);
    DbTagData dbTags(sz);


    int res= dbTags.receive(dt,cp,CommMetaData(1));
    int loc= 0;
    for(iterator i= contenedor.begin();i!=contenedor.end();i++)
      res+= this->receiveItem(*i,cp,dbTags,CommMetaData(loc++));
    return res;
  }

//! @brief Envia el deque through the channel being passed as parameter.
template <class C>
int XC::MovableContainer<C>::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    this->inicComm(2);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "MovableContainer::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Recibe el deque through the channel being passed as parameter.
template <class C>
int XC::MovableContainer<C>::recvSelf(const CommParameters &cp)
  {
    this->inicComm(2);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "MovableContainer::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(cp);
        if(res<0)
          std::cerr << "MovableContainer::recvSelf - failed to receive data.\n";
      }
    return res;
  }


} // end of XC namespace

#endif

