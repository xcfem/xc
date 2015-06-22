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
//CalcPivotes.cc

#include "CalcPivotes.h"
#include "material/section/fiber_section/fiber/StoFibras.h"
#include "xc_utils/src/geom/d1/Segmento3d.h"
#include "material/section/fiber_section/fiber/Fiber.h"
#include "DefAgotPivotes.h"

inline Pos3d getPos3d(const XC::Fiber *t,const double &strain= 0.0)
  { return Pos3d(strain,t->getLocY(),t->getLocZ()); }

inline Pos3d getCDG(const XC::DqFibras &fs)
  { return Pos3d(0.0,fs.getYCdg(),fs.getZCdg()); }

Ref3d3d getRef3d(const XC::DqFibras &fs,const double &theta)
  {
    const Pos3d g(getCDG(fs));
    return Ref3d3d(g,Vector3d(1,0,0),Vector3d(0,cos(theta),sin(theta)));
  }

//! @brief Constructor.
XC::CalcPivotes::CalcPivotes(const DefAgotPivotes &ap,const StoFibras &fs,const DqFibras &fsC,const DqFibras &fsS,const double &theta)
  : Ref3d3d(getRef3d(fs,theta)), agot_pivotes(ap), fibras(fs),fibrasC(fsC),fibrasS(fsS) {}

const XC::Fiber *XC::CalcPivotes::getFiberSMinY(void) const
  {
    const size_t i= fibrasS.getFibraCooMin(*this,2);
    return fibrasS[i];
  }

const XC::Fiber *XC::CalcPivotes::getFiberCMinY(void) const
  {
    const size_t i= fibrasC.getFibraCooMin(*this,2);
    return fibrasC[i];
  }

const XC::Fiber *XC::CalcPivotes::getFiberCMaxY(void) const
  {
    const size_t i= fibrasC.getFibraCooMax(*this,2);
    return fibrasC[i];
  }


//! @brief Devuelve el punto que corresponde a deformación nula
//! en la fibra más traccionada de hormigón.
Pos3d XC::CalcPivotes::GetPuntoD(void) const
  {
    Pos3d retval;
    const Fiber *t= getFiberCMinY();
    retval= getPos3d(t,0.0);
    return retval;
  }

Pos3d XC::CalcPivotes::GetPivoteA(void) const
  {
    Pos3d retval;
    if(!fibrasS.empty()) //Hay armadura.
      {
        const Fiber *t= getFiberSMinY();
        const Pos3d pos_t= getPos3d(t,agot_pivotes.getDefAgotPivoteA()); //Deformación de agotamiento en pivote A.
        const double v_min_s= GetPosLocal(pos_t)(2);
        if(v_min_s<0) //La tesela está en la zona de tracciones.
          retval= pos_t;
        else //La tesela está en la zona de compresiones.
          retval= GetPuntoD();
      }
    else //no hay armadura.
      retval= GetPuntoD();
    return retval;
  }

Pos3d XC::CalcPivotes::GetPivoteB(void) const
  {
    const Fiber *t= getFiberCMaxY();
    Pos3d retval= getPos3d(t,agot_pivotes.getDefAgotPivoteB());
    return retval;
  }

Pos3d XC::CalcPivotes::GetPivoteC(void) const
  {
    const Pos3d D= GetPuntoD();
    const Pos3d B= GetPivoteB();
    const Recta3d s(D,B);
    GeomObj::list_Pos3d lp= s.Interseccion(1,agot_pivotes.getDefAgotPivoteC());
    assert(lp.size()>0); //La lista no puede estar vacía.
    const Pos3d retval= *lp.begin();
    return retval;
  }
