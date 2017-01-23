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
//ProtoBeam3d.h

#ifndef ProtoBeam3d_h
#define ProtoBeam3d_h

#include "domain/mesh/element/Element1D.h"
#include "material/section/repres/CrossSectionProperties3d.h"

namespace XC {
//! \ingroup OneDimensionalElem
//
//! @brief Clase base para elementos viga en
//! problemas tridimensionales.
class ProtoBeam3d : public Element1D
  {
  protected:
    CrossSectionProperties3d ctes_scc; //Section mechanical properties E,A,Iy,...
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    void set_material(const Material *m);

  public:
    ProtoBeam3d(int tag, int class_tag,const Material *m= NULL);
    ProtoBeam3d(int tag, int class_tag, int Nd1, int Nd2);
    ProtoBeam3d(int tag, int class_tag, double A, double E, double G, double Jx, double Iy, double Iz, int Nd1, int Nd2);
    int getNumDOF(void) const;
    inline CrossSectionProperties3d getSectionProperties(void) const
      { return ctes_scc; }
    void setSectionProperties(const CrossSectionProperties3d &ctes)
      { ctes_scc= ctes; }

    Vector getVDirEjeFuerteLocales(void) const;
    Vector getVDirEjeDebilLocales(void) const;
    double getAnguloEjeFuerte(void) const;
    double getAnguloEjeDebil(void) const;
  };
} // end of XC namespace

#endif
