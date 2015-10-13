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
//PlanoDeformacion

#ifndef PLANO_DEFORMACION_H
#define PLANO_DEFORMACION_H

#include <iostream>
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "utility/actor/actor/MovableObject.h"

class Pos2d;
class Pos3d;
class Recta2d;
class Semiplano2d;

namespace XC {

class Vector;
class ResponseId;

//! \ingroup MATSCC
//
//! @defgroup MATSCCDiagInt Diagrama de interacción de una sección.
//
//! @ingroup MATSCCDiagInt
//
//! @brief Plano de deformación de una sección.
class PlanoDeformacion: public Plano3d, public MovableObject
  {
  protected:
    bool check_positions(const Pos2d &,const Pos2d &, const Pos2d &);
    bool check_positions(const Pos3d &,const Pos3d &, const Pos3d &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    friend class SectionForceDeformation;
    friend class DqFibras;
    friend class FiberSectionBase; //Borrar esta línea.
    const Vector &getDeformation(void) const;
  public:
    PlanoDeformacion( const Pos3d &p1,const Pos3d &p2, const Pos3d &p3);
    explicit PlanoDeformacion(const Plano3d &);
    PlanoDeformacion( const Pos2d &yz1, const double &e_1, //Deformaciones en tres fibras de la sección.
                      const Pos2d &yz2, const double &e_2,
                      const Pos2d &yz3, const double &e_3);
    explicit PlanoDeformacion(const double &eps= 0.0);
    PlanoDeformacion(const Vector &e);

    double Deformacion(const Pos2d &p) const;
    const Vector &getDeformation(const size_t &order,const ResponseId &code) const;

    void ConstantStrain(const double &);

    Recta2d getFibraNeutra(void) const;
    Pos2d getPuntoSemiplanoTracciones(void) const;
    Pos2d getPuntoSemiplanoCompresiones(void) const;
    Semiplano2d getSemiplanoTracciones(void) const;
    Semiplano2d getSemiplanoTracciones(const Recta2d &) const;
    Semiplano2d getSemiplanoCompresiones(void) const;
    Semiplano2d getSemiplanoCompresiones(const Recta2d &) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

} // fin namespace XC

#endif
