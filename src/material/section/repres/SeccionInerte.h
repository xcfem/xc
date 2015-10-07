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
//SeccionInerte.h

#ifndef SeccionInerte_h 
#define SeccionInerte_h 

#include "xc_utils/src/nucleo/EntCmd.h"

class Pos2d;
class Recta2d;
class EjesPrincInercia2d;

namespace XC {
class ConstantesSecc3d;
class ConstantesSecc2d;
class Vector;
class Matrix;

//! @ingroup MATSCCRepres
//
//! @brief Seccion capaz de devolver parámetros mecánicos:
//! area, momentos de inercia, ...
class SeccionInerte: public EntCmd
  {
  public:
    SeccionInerte(EntCmd *owr= NULL);

    virtual double getAreaSeccBruta(void) const= 0;
    virtual Vector getCdgSeccBruta(void) const= 0;
    virtual double getIySeccBruta(void) const= 0;
    virtual double getIzSeccBruta(void) const= 0;
    virtual double getPyzSeccBruta(void) const= 0;

    double getIxSeccBruta(void) const;
    double getTheta_pSeccBruta(void) const;
    Vector getDirEjeI_aSeccBruta(void) const;
    Vector getDirEjeI_bSeccBruta(void) const;
    double getI1SeccBruta(void) const;
    double getI2SeccBruta(void) const;
    EjesPrincInercia2d getEjesInerciaSeccBruta(void) const;
    Vector2d getVDirEje1SeccBruta(void) const;
    Vector2d getVDirEjeFuerteSeccBruta(void) const;
    Vector2d getVDirEje2SeccBruta(void) const;
    Vector2d getVDirEjeDebilSeccBruta(void) const;
    double getISeccBruta(const unsigned short int &,const unsigned short int &) const;
    double getISeccBruta(const unsigned short int &,const unsigned short int &,const Pos2d &) const;
    double getISeccBruta(const Pos2d &,const Vector &) const;
    double getISeccBruta(const Recta2d &) const;
    double getIOSeccBruta(const Pos2d &) const;
    Matrix getISeccBruta(void) const;
    Matrix getISeccBruta(const Pos2d &) const;

    virtual double getAreaSeccHomogeneizada(const double &) const= 0;
    virtual Vector getCdgSeccHomogeneizada(const double &) const= 0;
    virtual double getIySeccHomogeneizada(const double &) const= 0;
    virtual double getIzSeccHomogeneizada(const double &) const= 0;
    virtual double getPyzSeccHomogeneizada(const double &) const= 0;

    double getIxSeccHomogeneizada(const double &) const;
    double getTheta_pSeccHomogeneizada(void) const;
    Vector getDirEjeI_aSeccHomogeneizada(void) const;
    Vector getDirEjeI_bSeccHomogeneizada(void) const;
    double getI1SeccHomogeneizada(const double &) const;
    double getI2SeccHomogeneizada(const double &) const;
    EjesPrincInercia2d getEjesInerciaSeccHomogeneizada(const double &) const;
    Vector2d getVDirEje1SeccHomogeneizada(const double &) const;
    Vector2d getVDirEjeFuerteSeccHomogeneizada(const double &) const;
    Vector2d getVDirEje2SeccHomogeneizada(const double &) const;
    Vector2d getVDirEjeDebilSeccHomogeneizada(const double &) const;
    double getISeccHomogeneizada(const double &,const unsigned short int &,const unsigned short int &) const;
    double getISeccHomogeneizada(const double &,const unsigned short int &,const unsigned short int &,const Pos2d &) const;
    double getISeccHomogeneizada(const double &,const Pos2d &,const Vector &) const;
    double getISeccHomogeneizada(const double &,const Recta2d &) const;
    double getIOSeccHomogeneizada(const double &,const Pos2d &) const;
    Matrix getISeccHomogeneizada(const double &) const;
    Matrix getISeccHomogeneizada(const double &,const Pos2d &) const;

    ConstantesSecc3d getConstantesSecc3d(const ConstantesSecc3d &) const;
    ConstantesSecc2d getConstantesSecc2d(const ConstantesSecc2d &) const;

  };

} // fin namespace XC


#endif

