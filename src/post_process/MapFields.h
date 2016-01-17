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
//MapFields.h
//Contenedor de conjuntos.

#ifndef MAPFIELDS_H
#define MAPFIELDS_H

#include "FieldInfo.h"
#include "utility/actor/actor/MovableObject.h"
#include <deque>


namespace XC {
class Preprocessor;

//!  \ingroup POST_PROCESS
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
    MapFields(Preprocessor *preprocessor= NULL);
 
    FieldInfo &newField(const std::string &);

    const_iterator buscaField(const std::string &nmb) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif
