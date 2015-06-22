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
//DiagInteraccion.h

#ifndef DIAG_INTERACCION_H
#define DIAG_INTERACCION_H

#include "xc_utils/src/geom/d2/Triedro3d.h"
#include <set>
#include <deque>
#include "utility/actor/actor/MovableObject.h"

class MallaTriang3d;

namespace XC {

class Vector;
class FiberSectionBase;
class DatosDiagInteraccion;

//! \@ingroup MATSCCDiagInt
//
//! @brief Diagrama de interacción (N,Mx,My) de una sección.
class DiagInteraccion: public GeomObj3d, public MovableObject
  {

    typedef std::vector<Triedro3d> v_triedros;
    typedef v_triedros::iterator iterator;
    typedef v_triedros::const_iterator const_iterator;

    typedef std::set<const Triedro3d *> set_ptr_triedros;

    
    v_triedros triedros;
    set_ptr_triedros triedros_cuadrante[8];
    double tol;
    double rMax; //! Radio de una esfera circunscrita al convex-hull.
    double rMin; //! Radio de una esfera que toca al vértice más cercano.

    void clasifica_triedro(const Triedro3d &tdro);
    void clasifica_triedros(void);
    GeomObj::list_Pos3d get_interseccion(const Pos3d &p) const;
  protected:
    void getMatrizPosiciones(Matrix &);
    void setMatrizPosiciones(const Matrix &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    DiagInteraccion(void);
    DiagInteraccion(const Pos3d &org,const MallaTriang3d &mll);
    DiagInteraccion(const DiagInteraccion &otro);
    DiagInteraccion &operator=(const DiagInteraccion &otro);
    virtual DiagInteraccion *clon(void) const;

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
    double FactorCapacidad(const Pos3d &esf_d) const;
    Vector FactorCapacidad(const GeomObj::list_Pos3d &lp) const;

    any_const_ptr GetProp(const std::string &cod) const;
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void write(std::ofstream &);
    void read(std::ifstream &);
    void writeTo(const std::string &);
    void readFrom(const std::string &);
    void Print(std::ostream &os) const;
  };

DiagInteraccion calc_diag_interaccion(const FiberSectionBase &scc,const DatosDiagInteraccion &datos);

} // fin namespace XC

#endif
