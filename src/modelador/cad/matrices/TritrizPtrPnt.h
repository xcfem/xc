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
//TritrizPtrPnt.h
//Vector de matrices de puntos

#ifndef TRITRIZPTRPNT_H
#define TRITRIZPTRPNT_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "MatrizPtrPnt.h"
#include "TritrizPtrBase.h"
#include "utility/matrix/Vector.h"
#include "xc_utils/src/base/Lista.h"
#include "xc_basic/src/matrices/TritrizIndices.h"

class ExprAlgebra;
class Intervalo1D;
class Lista;
class RangoIndice;
class RangoTritriz;

namespace XC{

//! \ingroup CadMR
//! 
//! @brief "Tritriz" de punteros a elementos.
class TritrizPtrPnt: public TritrizPtrBase<MatrizPtrPnt>
  {
  protected:
    virtual bool procesa_comando(CmdStatus &status);

    friend class MapEsquemas3d;
    TritrizPtrPnt(const size_t capas= 0);
    TritrizPtrPnt(const size_t ,const size_t ,const size_t );
  public:
    inline virtual ~TritrizPtrPnt(void) {}
    Pnt *buscaPunto(const int &tag);
    const Pnt *buscaPunto(const int &tag) const;
    Pnt *getNearestPnt(const Pos3d &p);
    const Pnt *getNearestPnt(const Pos3d &p) const;

    const Cad *getCad(void) const;
    Cad *getCad(void);

    std::deque<size_t> CopiaPuntos(const RangoTritriz &,const std::vector<size_t> &,const Vector3d &);
    TritrizPtrPnt getRangoPuntos(const RangoTritriz &);
    Pnt *getPunto(const VIndices &i);
    TritrizPtrPnt getPuntos(const TritrizIndices &);
    MatrizPtrPnt getPuntos(const MatrizIndices &);
    TritrizPtrPnt getCeldaPuntos(const size_t &,const size_t &,const size_t &,const TritrizIndices &);
    MatrizPtrPnt getCeldaPuntos(const size_t &,const size_t &,const MatrizIndices &);

    Pos3d getCentroide(void) const;

    void setPnt(const size_t &,const size_t &,const size_t &,const int &);
    Pnt *getPnt(const size_t &,const size_t &,const size_t &);

    virtual any_const_ptr GetProp(const std::string &cod) const;
    void Print(std::ostream &os) const;
  };


std::ostream &operator<<(std::ostream &os,const TritrizPtrPnt &);

std::vector<size_t> getIdPuntosQuad(const TritrizPtrPnt::const_ref_capa_i_cte &,const size_t &j,const size_t &k);

} //fin namespace XC.

#endif
