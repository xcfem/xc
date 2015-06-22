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


//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
template <class C>
XC::DbTagData &XC::MovableContainer<C>::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }

//! @brief Envía los datos a través del canal que se pasa como parámetro.
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

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
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

//! @brief Envia el deque por el canal que se pasa como parámetro.
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

//! @brief Recibe el deque a través del canal que se pasa como parámetro.
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


} // fin namespace XC

#endif

