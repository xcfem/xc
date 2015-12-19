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
//MatrizPtrElem.h
//Matriz de puntos

#ifndef MATRIZPTRELEM_H
#define MATRIZPTRELEM_H

#include "MatrizPtrBase.h"
#include <vector>

class Pos3d;

namespace XC{

  class Element;

//! \ingroup Cad
//! 
//! \defgroup CadMR Referencias a nodos y elementos.
//
//! @ingroup CadMR
//
//! @brief Matriz de punteros a elementos.
class MatrizPtrElem: public MatrizPtrBase<Element>
  {
  protected:
    virtual bool procesa_comando(CmdStatus &status);
  public:
    //! @brief Constructor.
    MatrizPtrElem(const size_t &f=0,const size_t &c=0)
      : MatrizPtrBase<Element>(f,c) {}

    Element *buscaElemento(const int &tag);
    const Element *buscaElemento(const int &tag) const;
    Element *getNearestElement(const Pos3d &p);
    const Element *getNearestElement(const Pos3d &p) const;

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

} //end of XC namespace

#endif
