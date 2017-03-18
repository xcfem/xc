//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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

    const SectorCircular3d get_sector_circular3d(void) const;
    MatrizPos3d get_posiciones(void) const;
    bool check_points(void) const;
  public:
    ArcoCircunf(Preprocessor *m);
    ArcoCircunf(const std::string &nombre= "",Preprocessor *m= nullptr);
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
    double getLambda(const Pos3d &) const;

    //! @brief Returns the number of vertices.
    inline virtual size_t NumVertices(void) const
      { return 3; }
    virtual const Pnt *GetVertice(const size_t &i) const;
    virtual void SetVertice(const size_t &,Pnt *);
    virtual BND3d Bnd(void) const;
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;
    virtual ID getKPoints(void) const;


  };

} //end of XC namespace
#endif
