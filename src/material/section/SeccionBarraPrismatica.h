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
//SeccionBarraPrismatica.h
                                                                        
                                                                        
#ifndef SeccionBarraPrismatica_h
#define SeccionBarraPrismatica_h

class EjesPrincInercia2d;

#include "material/section/SectionForceDeformation.h"

class Semiplano2d;
class Recta2d;
class Pos2d;
class Vector2d;

namespace XC {

class DeformationPlane;

//! \ingroup Mat
//!
//!
//! \defgroup MATSCC Modelización de una sección transversal.
//!
//! \ingroup MATSCC
//
//! @brief Clase base para las secciones de barras prismáticas.
//! Representa las ecuaciones consititutivas de la sección.
class SeccionBarraPrismatica: public SectionForceDeformation
  {
  public:
    SeccionBarraPrismatica(int tag,int classTag,MaterialLoader *mat_ldr= NULL);
    SeccionBarraPrismatica(const SeccionBarraPrismatica &otro);
    SeccionBarraPrismatica &operator=(const SeccionBarraPrismatica &otro);

    int setTrialDeformationPlane(const DeformationPlane &);
    int setInitialDeformationPlane(const DeformationPlane &);
    virtual const Vector &getVectorDeformacion(const DeformationPlane &) const;
    DeformationPlane getDeformationPlane(void) const;
    DeformationPlane getInitialDeformationPlane(void) const;
    virtual double getStrain(const double &y,const double &z= 0) const;

    double getN(void) const;
    double getMy(void) const;
    double getMz(void) const;

    virtual double getCdgY(void) const;
    virtual double getCdgZ(void) const;
    Pos2d getCdg(void) const;
    Recta2d getFibraNeutra(void) const;
    Recta2d getEjeEsfuerzos(void) const;
    Pos2d getPuntoSemiplanoTracciones(void) const;
    Pos2d getPuntoSemiplanoCompresiones(void) const;
    Semiplano2d getSemiplanoTracciones(void) const;
    Semiplano2d getSemiplanoTracciones(const Recta2d &) const;
    Semiplano2d getSemiplanoCompresiones(void) const;
    Semiplano2d getSemiplanoCompresiones(const Recta2d &) const;

    const double &EA(void) const;
    const double &EIz(void) const;
    const double &EIy(void) const;
    const double &EIyz(void) const;
    double getTheta(void) const;
    double getEI1(void) const;
    double getEI2(void) const;
    EjesPrincInercia2d getEjesInercia(void) const;
    Vector2d getVDirEje1(void) const;
    Vector2d getVDirEjeFuerte(void) const;
    Vector2d getVDirEje2(void) const;
    Vector2d getVDirEjeDebil(void) const;

    bool hayAxil(const double &tol=1e-4) const;
    bool hayCortante(const double &tol=1e-4) const;
    bool hayMomento(const double &tol=1e-4) const;
    bool hayTorsor(const double &tol=1e-4) const;

  };

} // end of XC namespace


#endif
