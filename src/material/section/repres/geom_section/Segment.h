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
//Segment.h

#ifndef SEGMENT_H
#define SEGMENT_H

#include "Eje.h"


namespace XC {

//! @ingroup MATSCCEntGeom
//
//! @brief Segment de recta entre dos puntos.
class Segment: public Eje
  {
    Spot *p1; //!< Extremo dorsal del segment.
    Spot *p2; //!< Extremo frontal del segment.
  protected:

  public:
    Segment(GeomSection *m,const size_t &ndiv= 4);
    Segment(const std::string &nombre= "",GeomSection *m= NULL,const size_t &ndiv= 4);
    Spot *P1(void);
    const Spot *P1(void) const;
    Spot *P2(void);
    const Spot *P2(void) const;
    void setEndPoints(size_t, size_t);
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;
    //! @brief Devuelve el número de vértices.
    inline virtual size_t NumVertices(void) const
      { return 2; }
    virtual const Spot *GetVertice(const size_t &i) const;
    virtual Spot *GetVertice(const size_t &i);
    virtual void SetVertice(const size_t &,Spot *);
    virtual BND2d Bnd(void) const;
    double getLong(void) const;

  };

} //end of XC namespace
#endif
