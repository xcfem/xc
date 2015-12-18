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
//Linea.h

#ifndef LINEA_H
#define LINEA_H

#include "LineaBase.h"


namespace XC {

//! \ingroup CadEnt
//!
//! @brief Segmento de recta entre dos puntos.
class Linea: public LineaBase
  {
  protected:
    virtual bool procesa_comando(CmdStatus &status);
    MatrizPos3d get_posiciones(void) const;
    Edge *split_at(Pnt *,const double &,const double &);
  public:
    Linea(Preprocessor *m,const size_t &ndiv= 4);
    Linea(const std::string &nombre= "",Preprocessor *m= NULL,const size_t &ndiv= 4);
    virtual SetEstruct *getCopy(void) const;

    double getLongitud(void) const;
    Edge *splitAtPoint(Pnt *p);
    Edge *splitAtLambda(const double &);
    Edge *splitAtCooNatural(const double &);
    
    //! @brief Devuelve el número de vértices.
    inline virtual size_t NumVertices(void) const
      { return 2; }
    virtual BND3d Bnd(void) const;
    double DistanciaA2(const Pos3d &pt) const;

    const Vector &getVector(void) const;
    Segmento3d getSegmento(void) const;
    virtual const Vector &getTang(const double &) const;
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

    any_const_ptr GetProp(const std::string &cod) const;
  };

} //end of XC namespace
#endif
