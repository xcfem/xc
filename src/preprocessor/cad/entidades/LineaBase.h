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
//LineaBase.h

#ifndef LINEABASE_H
#define LINEABASE_H

#include "Edge.h"


namespace XC {

//! \ingroup CadEnt
//!
//! @brief Segmento de recta entre dos puntos.
class LineaBase: public Edge
  {
  protected:
    Pnt *p1; //!< Extremo dorsal.
    Pnt *p2; //!< Extremo frontal.

    inline Pnt *P1(void)
      { return p1; }
    virtual Pnt *P2(void)
      { return p2; }
  public:
    LineaBase(Preprocessor *m,const size_t &ndiv= 4);
    LineaBase(const std::string &nombre,Preprocessor *m,const size_t &ndiv= 4);
    const Pnt *P1(void) const;
    const Pnt *P2(void) const;

    virtual const Pnt *GetVertice(const size_t &i) const;
    virtual void SetVertice(const size_t &,Pnt *);
    void SetVertice(const size_t &,const size_t &);
    void SetVertices(Pnt *,Pnt *);


  };

} //end of XC namespace
#endif
