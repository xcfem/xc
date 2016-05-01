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
//MatrizPtrPnt.h
//Matriz de puntos

#ifndef MATRIZPTRPNT_H
#define MATRIZPTRPNT_H

#include "MatrizPtrBase.h"
#include "xc_basic/src/matrices/m_int.h"
#include <vector>

class Pos3d;
class RangoMatriz;
class Vector3d;

namespace XC{
class Pnt;
class SP_Constraint;
class Cad;

//! \ingroup CadMR
//! 
//! @brief Matriz de punteros a puntos.
class MatrizPtrPnt: public MatrizPtrBase<Pnt>
  {
  protected:


    friend class Cad;
  public:
    //! @brief Constructor.
    MatrizPtrPnt(const size_t &f=0,const size_t &c=0)
      : MatrizPtrBase<Pnt>(f,c) {}
    m_int getTags(void) const;

    const Cad *getCad(void) const;
    Cad *getCad(void);

    void setPnt(const size_t &,const size_t &,const int &);
    Pnt *getPnt(const size_t &,const size_t &);

    Pnt *buscaPunto(const size_t &);
    const Pnt *buscaPunto(const size_t &) const;
    Pnt *getNearestPnt(const Pos3d &p);
    const Pnt *getNearestPnt(const Pos3d &p) const;
    std::deque<size_t> CopiaPuntos(const RangoMatriz &,const std::vector<size_t> &,const Vector3d &);

    Pos3d getCentroide(void) const;


  };

} //end of XC namespace

#endif
