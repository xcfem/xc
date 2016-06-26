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
//PlanoDeformacion.cc

#include "PlanoDeformacion.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/d2/EcuacionGeneralPlano3d.h"
#include "utility/matrix/Vector.h"
#include "material/section/ResponseId.h"
#include "xc_utils/src/geom/d1/Recta3d.h"
#include "xc_utils/src/geom/d1/Recta2d.h"
#include "xc_utils/src/geom/d2/Semiplano2d.h"
#include "utility/actor/actor/MovableVector.h"

bool XC::PlanoDeformacion::check_positions(const Pos3d &pA,const Pos3d &pB, const Pos3d &pC)
  {
    const Pos2d p1= Pos2d(pA.y(),pA.z());
    const Pos2d p2= Pos2d(pB.y(),pB.z());
    const Pos2d p3= Pos2d(pC.y(),pC.z());
    return check_positions(p1,p2,p3);
  }

bool XC::PlanoDeformacion::check_positions(const Pos2d &p1,const Pos2d &p2, const Pos2d &p3)
  {
    bool retval= true;
    const GEOM_FT tol2= 1e-3;
    const GEOM_FT d12= p1.dist2(p2);
    if(d12<tol2)
      {
        retval= false;
        std::clog << "PlanoDeformacion; points p1= " << p1 << " and p2= " 
                  << p2 << " are too close d= " << sqrt(d12) << std::endl;
      }
    const GEOM_FT d13= p1.dist2(p3);
    if(d13<tol2)
      {
        retval= false;
        std::clog << "PlanoDeformacion; points p1= " << p1 << " and p3= " 
                  << p3 << " are too close d= " << sqrt(d13) << std::endl;
      }
    const GEOM_FT d23= p2.dist2(p3);
    if(d23<tol2)
      {
        retval= false;
        std::clog << "PlanoDeformacion; points p2= " << p2 << " and p3= " 
                  << p3 << " are too close d= " << sqrt(d23) << std::endl;
      }
    return retval;
  }

XC::PlanoDeformacion::PlanoDeformacion(const Pos3d &p1,const Pos3d &p2, const Pos3d &p3)
  : Plano3d(p1,p2,p3), MovableObject(0) { check_positions(p1,p2,p3); }

XC::PlanoDeformacion::PlanoDeformacion(const Plano3d &p)
  : Plano3d(p), MovableObject(0) {}


XC::PlanoDeformacion::PlanoDeformacion( const Pos2d &yz1, const double &e_1, //Deformaciones en tres fibras de la sección.
                  const Pos2d &yz2, const double &e_2,
                  const Pos2d &yz3, const double &e_3)
  : Plano3d(), MovableObject(0)
  {
    const Pos3d p1(e_1,yz1.x(),yz1.y());
    const Pos3d p2(e_2,yz2.x(),yz2.y());
    const Pos3d p3(e_3,yz3.x(),yz3.y());
    check_positions(p1,p2,p3);
    TresPuntos(p1,p2,p3);
  }
XC::PlanoDeformacion::PlanoDeformacion(const double &eps) //Deformación constante en toda la sección.
  :Plano3d(Pos3d(eps,100,0),Pos3d(eps,0,100),Pos3d(eps,100,100)), MovableObject(0) {}

XC::PlanoDeformacion::PlanoDeformacion(const Vector &e)
  : Plano3d(), MovableObject(0)
  {
    const double e_1= e(0); //Deformación en (0,0)
    const double e_2= e(0)+e(1); //Deformación en (1,0)
    double e_3= e(0); //Deformación en (0,1)
    if(e.Size()>2)
      e_3+= e(2); //Deformación en (0,1)
    
    const Pos3d p1(e_1,0.0,0.0);
    const Pos3d p2(e_2,1.0,0.0);
    const Pos3d p3(e_3,0.0,1.0);
    TresPuntos(p1,p2,p3);
  }

void XC::PlanoDeformacion::ConstantStrain(const double &e)
  {
    const Pos3d p1(e,0.0,0.0);
    const Pos3d p2(e,100.0,0.0);
    const Pos3d p3(e,0.0,100.0);
    TresPuntos(p1,p2,p3);
  }

//! @brief Devuelve la deformación que corresponde a la
//! fibra de posición "p"
double XC::PlanoDeformacion::Deformacion(const Pos2d &p) const
  { return Plano3d::x(p); }

//! @brief Devuelve el vector de deformaciones.
const XC::Vector &XC::PlanoDeformacion::getDeformation(void) const
  {
    static Vector retval(3);
    retval(0)= Deformacion(Pos2d(0,0));
    retval(1)= Deformacion(Pos2d(1,0))-retval(0);
    retval(2)= Deformacion(Pos2d(0,1))-retval(0);
    return retval;
  }

//! @brief Devuelve el vector de deformaciones.
const XC::Vector &XC::PlanoDeformacion::getDeformation(const size_t &order,const ResponseId &code) const
  {
    static Vector retval;
    retval.resize(order);
    retval.Zero();
    const Vector &tmp= getDeformation();
    for(register size_t i= 0;i<order;i++)
      {
        if(code(i) == SECTION_RESPONSE_P)
          retval[i]+= tmp(0);
        if(code(i) == SECTION_RESPONSE_MZ)
          retval[i]+= tmp(1);
        if(code(i) == SECTION_RESPONSE_MY)
          retval[i]+= tmp(2);
      }
    return retval;
  }

//! @brief Devuelve la fibra neutra.
Recta2d XC::PlanoDeformacion::getFibraNeutra(void)const
  {
    const double a= angulo(*this,PlanoYZ3d);
    Recta2d retval;
    if(a>1e-4)
      {
        Recta3d traza= TrazaYZ();
        if(traza.exists())
          retval= traza.ProyeccionYZ2d();
      }
    else //Casi paralelos: No es capaz de encontrar la intersección.
      retval.setExists(false);
    return retval;
  }


//! @brief Devuelve (si puede) un punto en el que las tensiones son de tracción.
Pos2d XC::PlanoDeformacion::getPuntoSemiplanoTracciones(void) const
  {
    Pos2d retval(0,0);
    const Recta2d fn(getFibraNeutra());
    bool exists= fn.exists();
    const double a= angulo(*this,PlanoYZ3d);
    if(exists && (a>mchne_eps_dbl)) //Neutral axis exists.
      {
        const Pos2d p0(fn.Punto());
        //const Vector2d v(RectaMaximaPendienteYZ().ProyeccionYZ2d().VDir());
        const Vector2d v= fn.VDir().Normal();
        retval= p0+1000*v;
        if(Deformacion(retval)<0) //Lado compresiones.
          retval= p0-1000*v;
      }
    else //La fibra neutra es impropia.
      retval.setExists(Deformacion(retval)>0.0);
    return retval;
  }

//! @brief Devuelve (si puede) un punto en el que las tensiones son de compresion.
Pos2d XC::PlanoDeformacion::getPuntoSemiplanoCompresiones(void) const
  {
    Pos2d retval(0,0);
    const Recta2d fn(getFibraNeutra());
    bool exists= fn.exists();
    const double a= angulo(*this,PlanoYZ3d);
    if(exists && (a>mchne_eps_dbl)) //Neutral axis exists.
      {
        const Pos2d p0(fn.Punto());
        //const Vector2d v(RectaMaximaPendienteYZ().ProyeccionYZ2d().VDir());
        const Vector2d v= fn.VDir().Normal();
        retval= p0+1000*v;
        if(Deformacion(retval)>0) //Lado tracciones.
          retval= p0-1000*v;
      }
    else //La fibra neutra es impropia.
      retval.setExists(Deformacion(retval)<0.0);
    return retval;
  }

//! @brief Devuelve el semiplano cuyo borde es la recta que se pasa
//! como parámetro y que está contenido en el semiplano de tracción.
Semiplano2d XC::PlanoDeformacion::getSemiplanoTracciones(const Recta2d &r) const
  {
    const Semiplano2d spt= getSemiplanoTracciones();
    assert(spt.exists());
    const Pos2d p0(r.Punto());
    const Vector2d v= r.VDir().Normal();
  
    Pos2d p(p0+1000*v);
    Semiplano2d retval;
    if(spt.In(p))
      retval= Semiplano2d(r,p);
    else
      {
        p= p0-1000*v;
        if(spt.In(p))
          retval= Semiplano2d(r,p);
        else
	  std::cerr << "PlanoDeformacion::getSemiplanoTracciones; no se pudo encontrar"
                    << " el semiplano traccionado cuyo borde es r= " << r << std::endl;
      }
    return retval;
  }

//! @brief Devuelve el semiplano cuyo borde es la fibra neutra
//! y en el que las tensiones son de tracción.
Semiplano2d XC::PlanoDeformacion::getSemiplanoTracciones(void) const
  {
    const Recta2d fn= getFibraNeutra();
    bool exists= fn.exists();
    const double a= angulo(*this,PlanoYZ3d);
    Pos2d tmp(0,0);
    if(exists && (a>mchne_eps_dbl)) //Neutral axis exists.
      {
        const Pos2d p0(fn.Punto());
        const Vector2d v= fn.VDir().Normal();
        tmp= p0+1000*v;
        if(Deformacion(tmp)<0) //Lado compresiones.
          tmp= p0-1000*v;
      }
    else //La fibra neutra es impropia.
      exists= (Deformacion(tmp)>0.0);

    Semiplano2d retval;
    if(exists)
      retval= Semiplano2d(fn,tmp);
    return retval;
  }

//! @brief Devuelve el semiplano cuyo borde es la recta que se pasa
//! como parámetro y en el que las tensiones son de compresión.
Semiplano2d XC::PlanoDeformacion::getSemiplanoCompresiones(const Recta2d &r) const
  {
    const Recta2d fn= getFibraNeutra();
    bool exists= fn.exists();
    const double a= angulo(*this,PlanoYZ3d);
    Pos2d tmp(0,0);
    if(exists && (a>mchne_eps_dbl)) //Neutral axis exists.
      {
        const Pos2d p0(fn.Punto());
        const Vector2d v= fn.VDir().Normal();
        tmp= p0+1000*v;
        if(Deformacion(tmp)>0) //Lado tracciones.
          tmp= p0-1000*v;
      }
    else //La fibra neutra es impropia.
      exists= (Deformacion(tmp)<0.0);

    Semiplano2d retval;
    if(exists)
      retval= Semiplano2d(fn,tmp);
    return retval;
  }

//! @brief Devuelve el semiplano cuyo borde es la fibra neutra
//! y en el que las tensiones son de compresión.
Semiplano2d XC::PlanoDeformacion::getSemiplanoCompresiones(void) const
  {
    const Recta2d fn= getFibraNeutra();
    bool exists= fn.exists();
    Semiplano2d retval;
    if(exists)
      retval= getSemiplanoCompresiones(fn);
    return retval;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::PlanoDeformacion::sendData(CommParameters &cp)
  {
    //setDbTagDataPos(0,getTag());
    Vector ec_gen(4);
    EcuacionGeneralPlano3d ec= GetEcuacionGeneral();
    ec_gen(0)= ec.a(); ec_gen(1)= ec.b(); ec_gen(2)= ec.c(); ec_gen(3)= ec.d(); 
    int res= cp.sendVector(ec_gen,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::PlanoDeformacion::recvData(const CommParameters &cp)
  {
    //setTag(getDbTagDataPos(0));
    Vector ec_gen(4);
    int res= cp.receiveVector(ec_gen,getDbTagData(),CommMetaData(1));
    EcuacionGeneralPlano3d ec(ec_gen(0),ec_gen(1),ec_gen(2),ec_gen(3));
    EcuacionGeneral(ec);
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::PlanoDeformacion::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::PlanoDeformacion::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
