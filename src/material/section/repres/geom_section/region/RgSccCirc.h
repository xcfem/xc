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
//RgSccCirc.h
// Written by Remo M. de Souza
// December 1998

#ifndef RgSccCirc_h 
#define RgSccCirc_h 

#include <material/section/repres/geom_section/region/RgQuadCell.h>
#include <utility/matrix/Vector.h>

class SectorAnilloCircular2d;

namespace XC {
class Matrix;

//! @ingroup MATSCCRegiones
//
//! @brief Region en forma de sector de anillo empleada
//! para la discretización.
class RgSccCirc: public RgQuadCell
  {
  private:
    Vector centerPosit;
    double intRad, extRad;
    double initAng, finalAng;
  protected:
    bool procesa_comando(CmdStatus &status);
  public:
    RgSccCirc(Material *mat);
    RgSccCirc(Material *, int numSubdivCircunf, int numSubdivRadial,
              const Vector &centerPosition, double internRadius, 
              double externRadius, double initialAngle, double finalAngle);
    
    //edition functions
    void setCenterPosition(const Vector &);
    void setCenterPos(const Pos2d &);
    Pos2d getCenterPos(void) const;
    inline double getInteriorRadius(void) const
      { return intRad; }
    inline double getExteriorRadius(void) const
      { return extRad; }
    inline double getInitialAngle(void) const
      { return initAng; }
    inline double getFinalAngle(void) const
      { return finalAng; }
    void setInteriorRadius(const double &d)
      { intRad= d; }
    void setExteriorRadius(const double &d)
      { extRad= d; }
    void setInitialAngle(const double &d)
      { initAng= d; }
    void setFinalAngle(const double &d)
      { finalAng= d; }
    void setRadii(double internRadius, double externRadius);
    void setAngles(double initialAngle, double finalAngle);

    //inquiring functions
    const Rejilla2d &Malla(void) const;
    const VectorCells &getCells(void) const;
    RegionSecc *getCopy(void) const;

    void getRadii(double &internRadius, double &externRadius)  const;
    void getAngles(double &initialAngle, double &finalAngle)    const;
    const Vector &getCenterPosition(void) const;
    const Matrix &getVertCoords(void) const;

    double getMaxY(void) const;
    double getMaxZ(void) const;
    double getMinY(void) const;
    double getMinZ(void) const;

    Poligono2d getPoligono(void) const;
    SectorAnilloCircular2d &getSector(void) const;

    virtual any_const_ptr GetProp(const std::string &cod) const;
    void Print(std::ostream &s, int flag =0) const;   
    friend std::ostream &operator<<(std::ostream &s, RgSccCirc &rg_scc_circ);    
  };
} // end of XC namespace


#endif

 
