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
//GrupoCombinaciones.h

#ifndef GRUPOCOMBINACIONES_H
#define GRUPOCOMBINACIONES_H

#include "preprocessor/loaders/LoadLoaderMember.h"
#include <map>
#include "boost/python/list.hpp"

namespace XC {
class Combinacion;
class LoadLoader;
class Domain;

typedef std::map<std::string,Combinacion *> map_combinaciones; //!< Combinaciones.

//! @ingroup LPatterns
//
//! @brief Contenedor de combinaciones.
 class GrupoCombinaciones: public LoadLoaderMember, public map_combinaciones
  {
  protected:
    Combinacion *busca_combinacion(const std::string &);
    friend class LoadLoader;

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    int procesa_combinacion(const int &,const std::string &,CmdStatus &);
    bool procesa_comando(CmdStatus &status);
  public:
    GrupoCombinaciones(LoadLoader *owr);
    ~GrupoCombinaciones(void);

    boost::python::list getKeys(void) const;
    void remove(const std::string &);
    void clear(void);

    XC::Combinacion *newLoadCombination(const std::string &,const std::string &);

    void addToDomain(const std::string &);
    void removeFromDomain(const std::string &);
    void removeAllFromDomain(void);

    std::deque<std::string> getListaNombres(void) const;
    const std::string &getNombreCombinacion(const Combinacion *) const;

    const Combinacion *buscaCombinacion(const std::string &) const;
    const_iterator buscaCombPrevia(const Combinacion &) const;
    const Combinacion *getPtrCombPrevia(const Combinacion &) const;
    const_iterator buscaCombPrevia(const std::string &) const;
    const std::string getNombreCombPrevia(const std::string &) const;
    int getTagCombPrevia(const std::string &) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void EjecutaBloqueForEach(CmdStatus &,const std::string &);

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

} // fin namespace XC

#endif
