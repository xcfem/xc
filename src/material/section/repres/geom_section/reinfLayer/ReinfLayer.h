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
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/geom_section/reinfLayer/ReinfLayer.h,v $
                                                                        
                                                                        
// File: ReinfLayer.h
// Written by Remo M. de Souza
// December 1998

#ifndef ReinfLayer_h 
#define ReinfLayer_h 

#include "material/section/repres/DiscretBase.h" 
#include <material/section/repres/geom_section/reinfBar/VectorReinfBar.h>

class Semiplano2d;
class Poligono2d;

namespace XC {
class ReinfBar;
class ListReinfLayer;
class GeomSection;

//! @ingroup MATSCCArmaduras
//
//! @brief Conjunto de armaduras que forma una capa.
class ReinfLayer: public DiscretBase
  {
  private:
    int nReinfBars; //!< Número de barras de la capa.
    double barDiam; //!< Diámetro de las barras.
    double area; //!< Área de las barras.
  protected:
    mutable VectorReinfBar reinfBars;



    friend class ListReinfLayer;
    ReinfLayer(ListReinfLayer *,Material *m);
    ReinfLayer(ListReinfLayer *,Material *mat,const int &numReinfBars,const double &bDiam=0.0,const double &bArea= 0.0);
    virtual ReinfLayer *getCopy(void) const= 0;
  public:
    virtual ~ReinfLayer(void) {}

    // edition functions
    virtual void setNumReinfBars(int numReinfBars);
    virtual int getNumReinfBars(void) const;
    virtual void setReinfBarDiameter(double reinfBarDiameter);
    virtual const double &getReinfBarDiameter(void) const;
    virtual void setReinfBarArea(double reinfBarArea);
    virtual const double &getReinfBarArea(void) const;

    // reinforcing layer inquiring functions
    //void Cumplen(const std::string &,ListReinfLayer &,bool );
    void getBarrasIn(const Poligono2d &,ListReinfLayer &,bool );
    void getBarrasIn(const Semiplano2d &,ListReinfLayer &,bool );
    
    const GeomSection *getGeomSection(void) const;
    double getRecubrimiento(void) const;
    Vector getCdg(void) const;

    inline double getArea(void) const
      { return area*nReinfBars; }
    VectorReinfBar &getReinfBars(void);  
    virtual const VectorReinfBar &getReinfBars(void) const= 0;  


    virtual void Print(std::ostream &s, int flag =0) const;   
    friend std::ostream &operator<<(std::ostream &, const ReinfLayer &);    
  };

std::ostream &operator<<(std::ostream &s, const ReinfLayer &);
   
} // end of XC namespace


#endif

