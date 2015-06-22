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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/section/SectionRepres.cpp,v $
                                                                        
                                                                        
// File: SectionRepres.C
//
// Written by Remo M. de Souza
// November 1998

#include <material/section/repres/section/SectionRepres.h>
#include <material/section/repres/geom_section/GeomSection.h>
#include "modelador/loaders/MaterialLoader.h"

#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"

XC::SectionRepres::SectionRepres(int tag,MaterialLoader *ml)
  : TaggedObject(tag), material_loader(ml), gmSecc(nullptr) {}

//! @brief Constructor de copia.
XC::SectionRepres::SectionRepres(const SectionRepres &otro)
  : TaggedObject(otro), material_loader(otro.material_loader), gmSecc(otro.gmSecc) {}

//! @brief Operador asignación.
XC::SectionRepres &XC::SectionRepres::operator=(const SectionRepres &otro)
  {
    SectionRepres::operator=(otro);
    material_loader= otro.material_loader;
    gmSecc= otro.gmSecc;
    return *this;
  }

//! @brief Lee un objeto XC::SectionRepres desde archivo
bool XC::SectionRepres::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(SectionRepres) Procesando comando: " << cmd << std::endl;

    if(cmd == "geom") //Asigna una geometría a la seccion.
      {
        const std::string nmbGeom= interpretaString(status.GetString());
        setGeomNamed(nmbGeom);
        return true;
      }
    else
      return TaggedObject::procesa_comando(status);
  }


//! @brief Devuelve el número total de celdas.
int XC::SectionRepres::getNumCells(void) const
  {
    int ncells= 0;
    if(gmSecc)
      ncells= gmSecc->getRegiones().getNumCells();
    return ncells;
  }

void XC::SectionRepres::setGeomNamed(const std::string &nmbGeom)
  {
    const GeomSection *tmp= material_loader->find_ptr_geom_section(nmbGeom);
    if(tmp)
      gmSecc= tmp;
    else
     std::cerr << "(SectionRepres::setGeom;" 
                    << " no se encontró la geometría de nombre: '"
                    << nmbGeom << "'.\n";
  }

//! @brief Asigna la geometría a la sección.
void XC::SectionRepres::setGeom(const GeomSection *g)
  { gmSecc= g; }

//! @brief Devuelve un puntero a la geometría de la sección.
const XC::GeomSection *XC::SectionRepres::getGeom(void) const
  { return gmSecc; }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::SectionRepres::GetProp(const std::string &cod) const
  {
    if(cod=="numCells")
      {
        tmp_gp_szt= getNumCells();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod == "geom")
      {
        if(gmSecc)
          return any_const_ptr(gmSecc);
        else
          return any_const_ptr();
      }
    else
      return TaggedObject::GetProp(cod);
  }


//! @brief Imprime información sobre el objeto.
void XC::SectionRepres::Print(std::ostream &s, int flag)
  {
    s << "\nSection representation type: " << getType();
  }


std::ostream &XC::operator<<(std::ostream &s, XC::SectionRepres &sectionRepr)
  {
    sectionRepr.Print(s);
    return s;
  }
