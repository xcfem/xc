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
//SeccionInerte.cpp

#include "SeccionInerte.h"
#include "xc_basic/src/util/inercia.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/sis_ref/Ref2d2d.h"
#include "xc_utils/src/geom/sis_ref/EjesPrincInercia2d.h"
#include "material/section/repres/CrossSectionProperties3d.h"
#include "material/section/repres/CrossSectionProperties2d.h"
#include "xc_utils/src/geom/d1/Segmento2d.h"
#include "xc_utils/src/base/utils_any.h"

XC::SeccionInerte::SeccionInerte(EntCmd *owr)
  : EntCmd(owr) {}

//! @brief Inertia of the homogenized section about an axis parallel to x through his center of gravity.
//! @param E0: Elastic modulus used as reference.
double XC::SeccionInerte::getIxSeccHomogeneizada(const double &E0) const
  { return getIySeccHomogeneizada(E0)+getIzSeccHomogeneizada(E0); }



//! @brief Agle of the principal axis of inertia of the homogenized section with respect of current axis.
double XC::SeccionInerte::getTheta_pSeccHomogeneizada(void) const
  { return theta_inercia(getIySeccHomogeneizada(1.0),getIzSeccHomogeneizada(1.0),getPyzSeccHomogeneizada(1.0)); }

//! @brief Direction of a principal axis of inertia of the homogenized section with respect of current axis.
//! @param E0: Elastic modulus used as reference.
XC::Vector XC::SeccionInerte::getDirEjeI_aSeccHomogeneizada(void) const
  {
    const Dir2d p(getTheta_pSeccHomogeneizada());
    Vector retval(2);
    retval[0]= p.dx();
    retval[1]= p.dy();
    return retval;
  }

//! @brief Direction of the other axis of inertia of the homogenized section with respect of current axis.
//! @param E0: Elastic modulus used as reference.
XC::Vector XC::SeccionInerte::getDirEjeI_bSeccHomogeneizada(void) const
  {
    const Dir2d p= Dir2d(getTheta_pSeccHomogeneizada()).Perpendicular();
    Vector retval(2);
    retval[0]= p.dx();
    retval[1]= p.dy();
    return retval;
  }

//! @brief Major principal moment of inertia of the homogenized section.
double XC::SeccionInerte::getI1SeccHomogeneizada(const double &E0) const
  { 
    const double iy= getIySeccHomogeneizada(E0);
    const double iz= getIzSeccHomogeneizada(E0);
    const double pyz= getPyzSeccHomogeneizada(E0);
    return I1_inercia(iy,iz,pyz);
  }

//! @brief Minor principal moment of inertia of the homogenized section.
double XC::SeccionInerte::getI2SeccHomogeneizada(const double &E0) const
  {
    const double iy= getIySeccHomogeneizada(E0);
    const double iz= getIzSeccHomogeneizada(E0);
    const double pyz= getPyzSeccHomogeneizada(E0);
    return I2_inercia(iy,iz,pyz);
  }

//! @brief Principal axis of inertia of the homogenized section.
EjesPrincInercia2d XC::SeccionInerte::getEjesInerciaSeccHomogeneizada(const double &E0) const
  {
    const Vector v= getCdgSeccHomogeneizada(E0);
    const Pos2d cdg(v[0],v[1]);
    return EjesPrincInercia2d(cdg,getIySeccHomogeneizada(E0),getIzSeccHomogeneizada(E0),getPyzSeccHomogeneizada(E0));
  }
//! @brief Direction of the major principal axis of inertia of the homogenized section.
Vector2d XC::SeccionInerte::getVDirEje1SeccHomogeneizada(const double &E0) const
  { return getEjesInerciaSeccHomogeneizada(E0).getVDirEje1(); }
//! @brief Direction of the major principal axis of inertia of the homogenized section.
Vector2d XC::SeccionInerte::getVDirEjeFuerteSeccHomogeneizada(const double &E0) const
  { return getEjesInerciaSeccHomogeneizada(E0).getVDirEje1(); }
//! @brief Direction of the minor principal axis of inertia of the homogenized section.
Vector2d XC::SeccionInerte::getVDirEje2SeccHomogeneizada(const double &E0) const
  { return getEjesInerciaSeccHomogeneizada(E0).getVDirEje2(); }
//! @brief Direction of the minor principal axis of inertia of the homogenized section.
Vector2d XC::SeccionInerte::getVDirEjeDebilSeccHomogeneizada(const double &E0) const
  { return getEjesInerciaSeccHomogeneizada(E0).getVDirEje2(); }

//! @brief Inertia tensor (i,j) component of the homogenized section.
double XC::SeccionInerte::getISeccHomogeneizada(const double &E0,const unsigned short int &i,const unsigned short int &j) const
  {
    unsigned short int k= i + (j-1)*2;
    double retval= 0.0;
    switch(k)
      {
        case 1: //(1,1)
          retval= getIySeccHomogeneizada(E0);
          break;
        case 2: //(2,1)
        case 3: //(1,2)
          retval= -getPyzSeccHomogeneizada(E0);
          break;
	case 4:
          retval= getIzSeccHomogeneizada(E0);
          break;
      }
    return retval;
  }

//! @brief Devuelve el momento de inercia respecto al eje que pasa por O con dirección la de e.
double XC::SeccionInerte::getISeccHomogeneizada(const double &E0,const Pos2d &O,const Vector &e) const
  {
    const Matrix Io(getISeccHomogeneizada(E0,O));
    return dot(e,Io*e)/e.Norm2();
  }

//! @brief Devuelve el momento de inercia respecto a la recta que se pasa
//! como parámetro.
double XC::SeccionInerte::getISeccHomogeneizada(const double &E0,const Recta2d &r) const
  { return getISeccHomogeneizada(E0,r.Punto(),Vector(r.VDir()));  }

//! @brief Devuelve la componente i,j del tensor de inercia calculado respecto al punto "o".
double XC::SeccionInerte::getISeccHomogeneizada(const double &E0,const unsigned short int &i,const unsigned short int &j,const Pos2d &o) const
  {
    const double Iij= getISeccHomogeneizada(E0,i,j);
    const Vector cdg= getCdgSeccHomogeneizada(E0);

    Pos2d pp(cdg[0],cdg[1]);
    Ref2d2d ejes(pp);
    Pos2d pos_local= ejes.GetPosLocal(o);
    return Iij + getAreaSeccHomogeneizada(E0) * pos_local(i) * pos_local(j);
  }

//! @brief Devuelve el momento polar de inercia respecto al punto o.
double XC::SeccionInerte::getIOSeccHomogeneizada(const double &E0,const Pos2d &o) const
  { return (getISeccHomogeneizada(E0,1,1,o)+getISeccHomogeneizada(E0,2,2,o)+getISeccHomogeneizada(E0,3,3,o))/2; }


//! @brief Devuelve el tensor de inercia calculado desde el centro de gravedad del objeto.
XC::Matrix XC::SeccionInerte::getISeccHomogeneizada(const double &E0) const
  {
    Matrix i(2,2);
    i(0,0)= getIySeccHomogeneizada(E0); i(0,1)= -getPyzSeccHomogeneizada(E0);
    i(1,0)= i(0,1);   i(1,1)= getIzSeccHomogeneizada(E0);
    return i;
  }

//! @brief Devuelve el tensor de inercia respector al punto o.
XC::Matrix XC::SeccionInerte::getISeccHomogeneizada(const double &E0,const Pos2d &o) const
  {
    Matrix retval(2,2);
    const Matrix Ig= getISeccHomogeneizada(E0);
    Vector O(2); O[0]= o.x(); O[1]= o.y();
    const Vector og= getCdgSeccHomogeneizada(E0) - O;
    const double m= getAreaSeccHomogeneizada(E0);
    retval= Ig+m*(og.Norm2()*identity(Ig)-(og & og));
    return retval;
  }

XC::CrossSectionProperties3d XC::SeccionInerte::getCrossSectionProperties3d(const CrossSectionProperties3d &base) const
  {
    CrossSectionProperties3d retval;
    retval= base; //Asignamos E, G y alpha.
    retval.A()= getAreaSeccHomogeneizada(base.E()); 
    retval.Iy()= getIySeccHomogeneizada(base.E());
    retval.Iz()= getIzSeccHomogeneizada(base.E());
    retval.J()= getIxSeccHomogeneizada(base.E());
    return retval;
  }

XC::CrossSectionProperties2d XC::SeccionInerte::getCrossSectionProperties2d(const CrossSectionProperties2d &base) const
  {
    static CrossSectionProperties2d retval;
    retval= base; //Asignamos E, G y alpha.
    retval.A()= getAreaSeccHomogeneizada(base.E()); 
    retval.I()= getIzSeccHomogeneizada(base.E());
    return retval;
  }

//! @brief Inertia of the gross section about an axis parallel to x through his center of gravity.
double XC::SeccionInerte::getIxSeccBruta(void) const
  { return getIySeccBruta()+getIzSeccBruta(); }

//! @brief Agle of the principal axis of inertia of the gross section with respect of current axis.
double XC::SeccionInerte::getTheta_pSeccBruta(void) const
  { return theta_inercia(getIySeccBruta(),getIzSeccBruta(),getPyzSeccBruta()); }

//! @brief Direction of a principal axis of inertia of the gross section with respect of current axis.
XC::Vector XC::SeccionInerte::getDirEjeI_aSeccBruta(void) const
  {
    const Dir2d p(getTheta_pSeccBruta());
    Vector retval(2);
    retval[0]= p.dx();
    retval[1]= p.dy();
    return retval;
  }

//! @brief Direction of the other principal axis of inertia of the gross section with respect of current axis.
XC::Vector XC::SeccionInerte::getDirEjeI_bSeccBruta(void) const
  {
    const Dir2d p= Dir2d(getTheta_pSeccBruta()).Perpendicular();
    Vector retval(2);
    retval[0]= p.dx();
    retval[1]= p.dy();
    return retval;
  }

//! @brief Major principal moment of inertia of the gross section.
double XC::SeccionInerte::getI1SeccBruta(void) const
  {
    const double iy= getIySeccBruta();
    const double iz= getIzSeccBruta();
    const double pyz= getPyzSeccBruta();
    return I1_inercia(iy,iz,pyz);
  }

//! @brief Minor principal moment of inertia of the gross section.
double XC::SeccionInerte::getI2SeccBruta(void) const
  {
    const double iy= getIySeccBruta();
    const double iz= getIzSeccBruta();
    const double pyz= getPyzSeccBruta();
    return I2_inercia(iy,iz,pyz);
  }

//! @brief Principal axis of inertia of the gross section.
EjesPrincInercia2d XC::SeccionInerte::getEjesInerciaSeccBruta(void) const
  {
    const Vector v= getCdgSeccBruta();
    const Pos2d cdg(v[0],v[1]);
    return EjesPrincInercia2d(cdg,getIySeccBruta(),getIzSeccBruta(),getPyzSeccBruta());
  }
//! @brief Direction of the major principal axis of inertia of the gross section.
Vector2d XC::SeccionInerte::getVDirEje1SeccBruta(void) const
  { return getEjesInerciaSeccBruta().getVDirEje1(); }
//! @brief Direction of the major principal axis of inertia of the gross section.
Vector2d XC::SeccionInerte::getVDirEjeFuerteSeccBruta(void) const
  { return getEjesInerciaSeccBruta().getVDirEje1(); }
//! @brief Direction of the minor principal axis of inertia of the gross section.
Vector2d XC::SeccionInerte::getVDirEje2SeccBruta(void) const
  { return getEjesInerciaSeccBruta().getVDirEje2(); }
//! @brief Direction of the minor principal axis of inertia of the gross section.
Vector2d XC::SeccionInerte::getVDirEjeDebilSeccBruta(void) const
  { return getEjesInerciaSeccBruta().getVDirEje2(); }

//! @brief Devuelve la componente i,j del tensor de inercia calculado respecto al CDG.
double XC::SeccionInerte::getISeccBruta(const unsigned short int &i,const unsigned short int &j) const
  {
    unsigned short int k= i + (j-1)*2;
    double retval= 0.0;
    switch(k)
      {
        case 1: //(1,1)
          retval= getIySeccBruta();
          break;
        case 2: //(2,1)
        case 3: //(1,2)
          retval= -getPyzSeccBruta();
          break;
	case 4:
          retval= getIzSeccBruta();
          break;
      }
    return retval;
  }

//! @brief Devuelve el momento de inercia respecto al eje que pasa por O con dirección la de e.
double XC::SeccionInerte::getISeccBruta(const Pos2d &O,const Vector &e) const
  {
    const Matrix Io(getISeccBruta(O));
    return dot(e,Io*e)/e.Norm2();
  }

//! @brief Devuelve el momento de inercia respecto a la recta que se pasa
//! como parámetro.
double XC::SeccionInerte::getISeccBruta(const Recta2d &r) const
  { return getISeccBruta(r.Punto(),Vector(r.VDir()));  }

//! @brief Devuelve la componente i,j del tensor de inercia calculado respecto al punto "o".
double XC::SeccionInerte::getISeccBruta(const unsigned short int &i,const unsigned short int &j,const Pos2d &o) const
  {
    const double Iij= getISeccBruta(i,j);
    const Vector cdg= getCdgSeccBruta();

    Pos2d pp(cdg[0],cdg[1]);
    Ref2d2d ejes(pp);
    Pos2d pos_local= ejes.GetPosLocal(o);
    return Iij + getAreaSeccBruta() * pos_local(i) * pos_local(j);
  }

//! @brief Devuelve el momento polar de inercia respecto al punto o.
double XC::SeccionInerte::getIOSeccBruta(const Pos2d &o) const
  { return (getISeccBruta(1,1,o)+getISeccBruta(2,2,o)+getISeccBruta(3,3,o))/2; }


//! @brief Devuelve el tensor de inercia calculado desde el centro de gravedad del objeto.
XC::Matrix XC::SeccionInerte::getISeccBruta(void) const
  {
    Matrix i(2,2);
    i(0,0)= getIySeccBruta(); i(0,1)= -getPyzSeccBruta();
    i(1,0)= i(0,1);   i(1,1)= getIzSeccBruta();
    return i;
  }

//! @brief Devuelve el tensor de inercia respector al punto o.
XC::Matrix XC::SeccionInerte::getISeccBruta(const Pos2d &o) const
  {
    Matrix retval(2,2);
    const Matrix Ig= getISeccBruta();
    Vector O(2); O[0]= o.x(); O[1]= o.y();
    const Vector og= getCdgSeccBruta() - O;
    const double m= getAreaSeccBruta();
    retval= Ig+m*(og.Norm2()*identity(Ig)-(og & og));
    return retval;
  }

