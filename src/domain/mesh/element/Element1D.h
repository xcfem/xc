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
//Element1D.h

#ifndef Element1D_h
#define Element1D_h

#include <domain/mesh/element/ElementBase.h>

class Segmento3d;

namespace XC {
class Node;
class Material;
class Domain;
class CrdTransf;
class CrdTransf2d;
class CrdTransf3d;
class DeformationPlane;

//! \ingroup Elem
//
//! @brief Elemento unidimensional (beam,truss,...)
class Element1D : public ElementBase<2>
  {
    void malla_unidimensional(const TritrizPtrNod &,TritrizPtrElem &) const;
  protected:
    mutable std::vector<double> longsTributarias;

    TritrizPtrElem coloca_en_malla(const TritrizPtrNod &,dir_mallado dm) const;
    TritrizPtrElem cose(const SetEstruct &f1,const SetEstruct &f2) const;

    int sendCoordTransf(int posFlag,const int &,const int &,CommParameters &);
    CrdTransf *recvCoordTransf(int posFlag,const int &,const int &,const CommParameters &);
    CrdTransf2d *recvCoordTransf2d(int posFlag,const int &,const int &,const CommParameters &);

    CrdTransf3d *recvCoordTransf3d(int posFlag,const int &,const int &,const CommParameters &);

  public:
    Element1D(int tag, int classTag);
    Element1D(int tag, int classTag,int Nd1,int Nd2);
    void setDomain(Domain *theDomain);
    virtual CrdTransf *getCoordTransf(void);
    Matrix getLocalAxes(bool) const;
    virtual const CrdTransf *getCoordTransf(void) const;
    virtual const Matrix &getCooPuntos(const size_t &ndiv) const;
    virtual const Vector &getCooPunto(const double &xrel) const;
    Segmento3d getSegmento(bool initialGeometry= true) const;
    double getDist2(const Pos2d &p,bool initialGeometry= true) const;
    double getDist(const Pos2d &p,bool initialGeometry= true) const;
    double getDist2(const Pos3d &p,bool initialGeometry= true) const;
    double getDist(const Pos3d &p,bool initialGeometry= true) const;

    size_t getDimension(void) const;
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

    virtual void calculaLongsTributarias(bool initialGeometry= true) const;
    double getLongTributaria(const Node *) const;

    void vector2dUniformLoadGlobal(const Vector &);
    void vector2dUniformLoadLocal(const Vector &);
    void vector2dPointByRelDistLoadGlobal(const double &,const Vector &);
    void vector2dPointByRelDistLoadLocal(const double &,const Vector &);
    void vector2dPointLoadGlobal(const Vector &,const Vector &);
    void vector2dPointLoadLocal(const Vector &,const Vector &);
    void vector3dUniformLoadGlobal(const Vector &);
    void vector3dUniformLoadLocal(const Vector &);
    void vector3dPointByRelDistLoadGlobal(const double &,const Vector &);
    void vector3dPointByRelDistLoadLocal(const double &,const Vector &);
    void vector3dPointLoadGlobal(const Vector &,const Vector &);
    void vector3dPointLoadLocal(const Vector &,const Vector &);
    void strainLoad(const DeformationPlane &p1, const DeformationPlane &p2);


  };

} //end of XC namespace
#endif
