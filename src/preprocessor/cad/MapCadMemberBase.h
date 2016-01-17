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
//MapCadMemberBase.h

#ifndef MAPCADMEMBERBASE_H
#define MAPCADMEMBERBASE_H

#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {

class Preprocessor;
class Cad;

//! @ingroup Cad
//
//! @brief Base para los contenedores de entidades del modelo.
class MapCadMemberBase: public EntCmd
  {
  public:
    typedef size_t Indice;

  protected:
    Indice tag; //! Identificador por defecto para el próximo punto.



  public:
    MapCadMemberBase(Cad *cad= NULL);

    const Cad *getCad(void) const;
    Cad *getCad(void);
    const Preprocessor *getPreprocessor(void) const;
    Preprocessor *getPreprocessor(void);

    inline size_t getTag(void) const
      { return tag; }
    void setTag(const size_t &t)
      { tag= t; }


  };

} //end of XC namespace
#endif
