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
//ClosedTriangleMesh.h

#ifndef CLOSEDTRIANGLEMESH_H
#define CLOSEDTRIANGLEMESH_H

#include "xc_utils/src/geom/d2/Triedro3d.h"
#include <set>
#include <deque>
#include "utility/actor/actor/MovableObject.h"

class MallaTriang3d;

namespace XC {

class Vector;
class FiberSectionBase;

//! \@ingroup MATSCCDiagInt
//
//! @brief Diagrama de interacción (N,Mx,My) de una sección.
class ClosedTriangleMesh: public GeomObj3d, public MovableObject
  {
  protected:
    typedef std::vector<Triedro3d> v_triedros;
    typedef v_triedros::iterator iterator;
    typedef v_triedros::const_iterator const_iterator;
    
    v_triedros triedros;
    double tol;
    double rMax; //! Radio de una esfera circunscrita al convex-hull.
    double rMin; //! Radio de una esfera que toca al vértice más cercano.

    GeomObj::list_Pos3d get_interseccion(const Pos3d &p) const;
  protected:
    void getMatrizPosiciones(Matrix &);
    virtual void setMatrizPosiciones(const Matrix &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    ClosedTriangleMesh(void);
    ClosedTriangleMesh(const Pos3d &org,const MallaTriang3d &mll);
    ClosedTriangleMesh(const ClosedTriangleMesh &otro);
    ClosedTriangleMesh &operator=(const ClosedTriangleMesh &otro);
    virtual ClosedTriangleMesh *clon(void) const;

    virtual double GetMax(short unsigned int i) const;
    virtual double GetMin(short unsigned int i) const;
    virtual Pos3d Cdg(void) const;
    virtual double Longitud(void) const;
    virtual double Area(void) const;
    virtual double Volumen(void) const;
    virtual double Ix(void) const; 
    virtual double Iy(void) const;
    virtual double Iz(void) const;
    short unsigned int Dimension() const
      { return 3; }

    size_t GetNumFacetas(void) const;

    iterator begin(void);
    iterator end(void);
    const_iterator begin() const;
    const_iterator end() const;
    size_t size(void) const;
    
    const_iterator BuscaTriedro(const Pos3d &p) const;
    const Triedro3d *BuscaPtrTriedro(const Pos3d &p) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void write(std::ofstream &);
    void read(std::ifstream &);
    void writeTo(const std::string &);
    void readFrom(const std::string &);
    void Print(std::ostream &os) const;
  };

} // end of XC namespace

#endif
