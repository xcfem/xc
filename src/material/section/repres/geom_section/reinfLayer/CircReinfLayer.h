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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:01:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/geom_section/reinfLayer/CircReinfLayer.h,v $
                                                                        
                                                                        
// File: CircReinfLayer.h
// Written by Remo M. de Souza
// December 1998


#ifndef CircReinfLayer_h 
#define CircReinfLayer_h 

#include <material/section/repres/geom_section/reinfLayer/ReinfLayer.h>
#include "xc_utils/src/geom/pos_vec/Pos2d.h"


namespace XC {
class ReinfBar;

//! @ingroup MATSCCArmaduras
//
//! @brief Capa de armaduras en forma de arco de circulo.
class CircReinfLayer : public ReinfLayer
  {
  private:
    Pos2d centerPosit;
    double arcRad;
    double initAng;
    double finalAng;
  protected:
    bool procesa_comando(CmdStatus &status);

    friend class ListReinfLayer;
    CircReinfLayer(ListReinfLayer *,Material *);
    // Constructor for an arc
    CircReinfLayer(ListReinfLayer *,Material *, int numReinfBars, double  reinfBarArea,
                   const Pos2d &centerPosition, double arcRadius, double
                   initialAngle, double finalAngle);
    // Constructor for full circle
    CircReinfLayer(ListReinfLayer *,Material *, int numReinfBars, double  reinfBarArea,
                   const Pos2d &centerPosition, double radius);
    ReinfLayer *getCopy(void) const;
  public:
    
    // edition functions
    void setCenterPosition(const Pos2d &centerPosition);
    void setArcRadius(const double &);
    void setInitAngle(const double &);
    void setFinalAngle(const double &);  

    // inquiring functions
    const VectorReinfBar &getReinfBars(void) const;
  
    Pos2d getCenterPosition(void) const;
    double getArcRadius(void) const;    
    double getInitAngle(void) const;
    double getFinalAngle(void) const;  
    
    double getMaxY(void) const;
    double getMaxZ(void) const;
    double getMinY(void) const;
    double getMinZ(void) const;

    virtual any_const_ptr GetProp(const std::string &cod) const;
    void Print(std::ostream &s, int flag =0) const;   
  };
} // end of XC namespace


#endif




