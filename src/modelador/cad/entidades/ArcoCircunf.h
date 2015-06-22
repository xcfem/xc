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
//ArcoCircunf.h

#ifndef ARCOCIRCUNF_H
#define ARCOCIRCUNF_H

#include "LineaBase.h"


class SectorCircular3d;

namespace XC {

//! \ingroup CadEnt
//!
//! @brief Arco de circunferencia.
class ArcoCircunf: public LineaBase
  {
    Pnt *p3; //!< Punto intermedio del arco.
  protected:
    virtual bool procesa_comando(CmdStatus &status);
    const SectorCircular3d get_sector_circular3d(void) const;
    MatrizPos3d get_posiciones(void) const;
  public:
    ArcoCircunf(Modelador *m);
    ArcoCircunf(const std::string &nombre= "",Modelador *m= NULL);
    virtual SetEstruct *getCopy(void) const;
    const Pnt *P3(void) const;

    void actualiza_topologia(void);

    double getLongitud(void) const;
    double getAnguloComprendido(void) const;
    double getTheta1(void) const;
    double getTheta2(void) const;
    Pos3d getCentro(void) const;
    Pos3d getPInic(void) const;
    Pos3d getPFin(void) const;
    Pos3d getPMed(void) const;
    double getRadio(void) const;

    //! @brief Devuelve el número de vértices.
    inline virtual size_t NumVertices(void) const
      { return 3; }
    virtual const Pnt *GetVertice(const size_t &i) const;
    virtual void SetVertice(const size_t &,Pnt *);
    virtual BND3d Bnd(void) const;
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;
    virtual ID getKPoints(void) const;

    any_const_ptr GetProp(const std::string &cod) const;
  };

} //fin namespace XC
#endif
