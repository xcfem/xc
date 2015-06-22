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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.6 $
// $Date: 2005/03/25 00:32:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/section/fiber_section/FiberSectionRepr.cpp,v $


// File: FiberSectionRepr.C
// Written by Remo M. de Souza
// November 1998

#include <material/section/repres/section/FiberSectionRepr.h>
#include <material/section/repres/section/FiberData.h>

#include "material/section/fiber_section/fiber/Fiber.h"

#include "material/section/fiber_section/FiberSection2d.h"
#include "material/section/fiber_section/FiberSection3d.h"
#include "material/section/fiber_section/FiberSectionGJ.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

void XC::FiberSectionRepr::vacia_fibras(void)
  {
    fibras.erase(fibras.begin(),fibras.end());// NOTE: don't delete fiber objects themselves
                                              // leave this to FiberSection destructor
  }

void XC::FiberSectionRepr::copia_fibras(const FiberSectionRepr &otro)
  {
    vacia_fibras();
    for(contenedor_fibras::const_iterator i= otro.fibras.begin();i!=otro.fibras.end();i++)
      addFiber(*(*i)->getCopy());
  }

//! @brief Constructor.
XC::FiberSectionRepr::FiberSectionRepr(int sectionID,MaterialLoader *ml)
  :SectionRepres(sectionID, ml), sectID(sectionID) {}

//! @brief Constructor de copia.
XC::FiberSectionRepr::FiberSectionRepr(const FiberSectionRepr &otro)
  :SectionRepres(otro), sectID(otro.sectID)
  { copia_fibras(otro); }

//! @brief Operador asignación.
XC::FiberSectionRepr &XC::FiberSectionRepr::operator=(const FiberSectionRepr &otro)
  {
    SectionRepres::operator=(otro);
    sectID= otro.sectID;
    copia_fibras(otro);
    return *this;
  }

XC::FiberSectionRepr *XC::FiberSectionRepr::getCopy(void) const
  { return new FiberSectionRepr(*this); }


//! @brief Lee un objeto XC::FiberSectionRepr desde archivo
bool XC::FiberSectionRepr::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(FiberSectionRepr) Procesando comando: " << cmd << std::endl;

    if(cmd == "for_each_fiber")
      {
        
        const std::string nmbBlq= nombre_clase()+":"+boost::lexical_cast<std::string>(getTag())+ ":for_each_fiber";
        const std::string bloque= status.GetBloque();
        for(contenedor_fibras::iterator i= fibras.begin();i!=fibras.end();i++)
          (*i)->EjecutaBloque(status,bloque,nmbBlq);
        return true;
      }
    else
      return SectionRepres::procesa_comando(status);
  }

XC::FiberSectionRepr::~FiberSectionRepr(void)
  { vacia_fibras(); }


#define SEC_TAG_FiberSectionRepr 1

int XC::FiberSectionRepr::getType(void) const
  { return SEC_TAG_FiberSectionRepr; }



//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::FiberSectionRepr::GetProp(const std::string &cod) const
  {
    if(cod=="numFibras")
      {
        tmp_gp_szt= getNumFibers();
        return any_const_ptr(tmp_gp_szt);
      }
    else
      return SectionRepres::GetProp(cod);
  }

void XC::FiberSectionRepr::Print(std::ostream &s, int flag)
  {
    SectionRepres::Print(s,flag);
  }


std::ostream &XC::operator<<(std::ostream &s, XC::FiberSectionRepr &fiberSectionRepr)
  {
    fiberSectionRepr.Print(s);
    return s;
  }


int XC::FiberSectionRepr::addFiber(Fiber &newFiber)
  {
    fibras.push_back(&newFiber);
    return 0;
  }

//! @brief Devuelve el número de fibras de la sección.
int XC::FiberSectionRepr::getNumFibers(void) const
  { return fibras.size(); }

//! @brief Devuelve los datos de las fibras correspondientes a celdas y armaduras.
XC::FiberData XC::FiberSectionRepr::getFiberData(void) const
  {
    assert(gmSecc);
    return FiberData(*gmSecc);
  }

XC::contenedor_fibras XC::FiberSectionRepr::getFibras2d(void) const
  {
    contenedor_fibras retval;
    if(!material_loader)
      {
        std::cerr << "XC::FiberSectionRepr::getFibras2d; el puntero a materiales es nulo.\n";
        return retval;
      }
    for(contenedor_fibras::const_iterator i= fibras.begin();i!=fibras.end();i++)
      retval.push_back(*i);

    FiberData fiberData= getFiberData();
    fiberData.getFibras2d(retval);
    return retval;
  }

XC::contenedor_fibras XC::FiberSectionRepr::getFibras3d(void) const
  {
    contenedor_fibras retval;
    if(!material_loader)
      {
        std::cerr << "XC::FiberSectionRepr::getFibras2d; el puntero a materiales es nulo.\n";
        return retval;
      }
    for(contenedor_fibras::const_iterator i= fibras.begin();i!=fibras.end();i++)
      retval.push_back(*i);

    FiberData fiberData= getFiberData();
    fiberData.getFibras3d(retval);
    return retval;
  }

XC::FiberSection2d XC::FiberSectionRepr::getFiberSection2d(int secTag) const
  { return FiberSection2d(secTag,fibras,material_loader); }

XC::FiberSection3d XC::FiberSectionRepr::getFiberSection3d(int secTag) const
  { return FiberSection3d(secTag,fibras,material_loader); }

XC::FiberSectionGJ XC::FiberSectionRepr::getFiberSectionGJ(int secTag,const double &GJ) const
  { return FiberSectionGJ(secTag,fibras,GJ,material_loader); }

