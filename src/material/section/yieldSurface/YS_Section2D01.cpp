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
// @ rkaul@stanford.edu
// @ ggd@stanford.edu

#include "YS_Section2D01.h"
#include "material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h"
#define SEC_TAG_YS2D01 -1


XC::YS_Section2D01::YS_Section2D01(int tag)
  :YieldSurfaceSection2d(tag, SEC_TAG_YS2D01, 0, true), E(0), A(0), I(0)
{}

XC::YS_Section2D01::YS_Section2D01(void)
  :YieldSurfaceSection2d(0, SEC_TAG_YS2D01, 0, true), E(0), A(0), I(0)
{
}

XC::YS_Section2D01::YS_Section2D01(int tag, double E_in, double A_in, double I_in, YieldSurface_BC *ptrys, bool use_kr)
  :YieldSurfaceSection2d(tag, SEC_TAG_YS2D01, ptrys, use_kr), E(E_in), A(A_in), I(I_in)
  {
    if(E <= 0.0)
      {
        std::cerr << "XC::YS_Section2D01::YS_Section2D01 -- Input E <= 0.0 ... setting E to 1.0\n";
        E = 1.0;
      }
    if(A <= 0.0)
      {
        std::cerr << "XC::YS_Section2D01::YS_Section2D01 -- Input A <= 0.0 ... setting A to 1.0\n";
        A = 1.0;
      }
    if(I <= 0.0)
      {
        std::cerr << "XC::YS_Section2D01::YS_Section2D01 -- Input I <= 0.0 ... setting I to 1.0",
        I = 1.0;
      }
  }

void XC::YS_Section2D01::getSectionStiffness(XC::Matrix &Ks)
  {
    Ks(0,0) = E*A; Ks(0,1) = 0.0;
    Ks(1,0) = 0.0; Ks(1,1) = E*I;
  }

const XC::Matrix &XC::YS_Section2D01::getInitialTangent(void) const
  {
    ks(0,0) = E*A; ks(0,1) = 0.0;
    ks(1,0) = 0.0; ks(1,1) = E*I;
    return ks;
  }

XC::SectionForceDeformation* XC::YS_Section2D01::getCopy(void) const
  { return new YS_Section2D01(*this); }

void XC::YS_Section2D01::Print(std::ostream &s, int flag)
  {
    s << "YS_Section2D01, tag: " << this->getTag() << std::endl;
    s << "\tE: " << E << std::endl;
    s << "\tA: " << A << std::endl;
    s << "\tI: " << I << std::endl;
    XC::YieldSurfaceSection2d::Print(s, flag);
  }
