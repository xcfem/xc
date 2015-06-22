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
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/geom_section/reinfBar/ReinfBar.h,v $
                                                                        
                                                                        
// File: ReinfBar.h
//
// Written by Remo M. de Souza
// November 1998


#ifndef ReinfBar_h 
#define ReinfBar_h 

#include "utility/matrix/Vector.h"
#include "material/section/repres/DiscretBase.h" 

class Pos2d;

namespace XC {
class Matrix;

//! @ingroup MATSCCRepresGeom
//
//! @defgroup MATSCCArmaduras Representación de las barras de refuerzo (armaduras).
//
//! @ingroup MATSCCArmaduras
//
//! @brief Barra de refuerzo (armadura).
class ReinfBar: public DiscretBase
  {
  private:
    double diameter;
    double area;
    Vector posit;
  public:
    ReinfBar(Material *mat= NULL);
    ReinfBar(const double &barArea, Material *, const Vector &position);
    ReinfBar(const double &barArea,const double &barDiameter, Material *, const Vector &);
    virtual ReinfBar *getCopy(void) const;
    virtual ~ReinfBar(void) {}
    
    // edition functions

    void setDiameter(const double &barDiameter);
    void setArea(const double &barArea);
    void setMaterial(int materialID);
    void setPosition(const Vector &position);

    // reinforcing bar inquiring functions
    
    double getDiameter(void) const;
    double getArea(void) const;
    int getMaterial(void) const; 
    double Iy(void) const;
    double Iz(void) const;
    double Pyz(void) const;
    double Ix(void) const;
    double Theta_p(void) const;
    const Vector &DirEjeI_a(void) const;
    const Vector &DirEjeI_b(void) const;
    double getI1(void) const;
    double getI2(void) const;
    double getI(const unsigned short int &i,const unsigned short int &j) const;
    double getI(const unsigned short int i,const unsigned short int j,const Pos2d &o) const;
    double getI(const Pos2d &O,const Vector &e) const;
    double getIO(const Pos2d &o) const;
    Matrix &getI(void) const;
    Matrix &getI(const Pos2d &o) const;

    const Vector &getPosition(void) const;
    Pos2d getPos2d(void) const;

    double getMaxY(void) const;
    double getMaxZ(void) const;
    double getMinY(void) const;
    double getMinZ(void) const;

    virtual void Print(std::ostream &s, int flag =0) const;   
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC


#endif

