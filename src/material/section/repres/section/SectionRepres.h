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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:01:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/section/SectionRepres.h,v $
                                                                        
                                                                        
// File: SectionRepres.h
//
// Written by Remo M. de Souza
// November 1998


// Purpose: This file contains the class definition for SectionRepres.  
// SectionRepres is an abstract base class and thus no objects of it's
// type can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.


#ifndef SectionRepres_h 
#define SectionRepres_h 

#include <utility/tagged/TaggedObject.h>
#include <list>
#include "boost/lexical_cast.hpp"


namespace XC {
class GeomSection;
class MaterialLoader;
class Material;

//! @ingroup MATSCCRepresGeom
//
//! @brief Representación de la distribución de los distintos
//! materiales en una sección.
class SectionRepres: public TaggedObject
  {

  protected:
    MaterialLoader *material_loader; //!< Gestor de materiales (búsqueda,...).
    const GeomSection *gmSecc; //!< Geometría de la sección.

    bool procesa_comando(CmdStatus &status);
  public:
    //Constructores
    SectionRepres(int tag,MaterialLoader *ml);
    SectionRepres(const SectionRepres &otro);

    SectionRepres &operator=(const SectionRepres &otro);

    // Section inquiring functions
    virtual int getType(void) const = 0;
    int getNumCells(void) const;
    const GeomSection *getGeom(void) const;
    void setGeom(const GeomSection *);
    void setGeomNamed(const std::string &);

    any_const_ptr GetProp(const std::string &cod) const;
    void Print(std::ostream &s, int flag =0);
    friend std::ostream &operator<<(std::ostream &s, SectionRepres &fiberSectionRepr);    
  };

} // end of XC namespace


#endif

