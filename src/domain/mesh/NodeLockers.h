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
//NodeLockers.h

#ifndef NODELOCKERS_H
#define NODELOCKERS_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "domain/load/pattern/NodeLocker.h"
#include <map>

namespace XC {

class Mesh;

//! @brief Contenedor de grupos de condiciones de contorno mononodales.
class NodeLockers: public EntCmd, public MovableObject
  {
    typedef std::map<std::string,NodeLocker *> map_node_lockers; //!< Bloqueo de nodos.
    map_node_lockers node_lockers;
    int tag; //!< Identificador por defecto.
    std::string code; //!< Identificador del pattern para las nuevas coacciones.
  protected:
    friend class Mesh;
    NodeLocker *busca_node_locker(const std::string &);
    const NodeLocker *busca_node_locker(const std::string &) const;
    NodeLocker *nuevoNodeLocker(const std::string &nmb);
    void borraNodeLocker(const std::string &nmb);

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

    NodeLockers(Mesh *owr);
    ~NodeLockers(void);
  public:
    const std::string getCurrentNodeLockerId(void) const;
    NodeLocker *getCurrentNodeLockerPtr(void);
    const NodeLocker *getCurrentNodeLockerPtr(void) const;
    void clearAll(void);
    Domain *getDomain(void);
    const Domain *getDomain(void) const;

    void removeFromDomain(const std::string &);
    void removeAllFromDomain(void);

    NodeLocker *buscaNodeLocker(const int &tag);
    const NodeLocker *buscaNodeLocker(const int &tag) const;

    const int &getTag(void) const
      { return tag; }

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };

} // end of XC namespace

#endif
