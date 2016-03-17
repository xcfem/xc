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
//ProtoBeam2d.h

#ifndef ProtoBeam2d_h
#define ProtoBeam2d_h

#include "domain/mesh/element/Element1D.h"
#include "material/section/repres/CrossSectionProperties2d.h"
namespace XC {
class CrdTransf2d;

//! \ingroup ElemBarra
//
//! @brief Clase base para elementos barra en problemas bidimensionales.
class ProtoBeam2d : public Element1D
  {
  protected:
    CrossSectionProperties2d ctes_scc; //Section mechanical properties E,A,Iy,...
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    void set_material(const Material *m);

  public:
    ProtoBeam2d(int tag, int class_tag,const Material *m= NULL);
    ProtoBeam2d(int tag, int class_tag, double A, double E, double I, int Nd1, int Nd2);
    int getNumDOF(void) const;
    inline CrossSectionProperties2d getSectionProperties(void) const
      { return ctes_scc; }
    void setSectionProperties(const CrossSectionProperties2d &ctes)
      { ctes_scc= ctes; }

    Vector getVDirEjeFuerteLocales(void) const;
    Vector getVDirEjeDebilLocales(void) const;
    double getAnguloEjeFuerte(void) const;
    double getAnguloEjeDebil(void) const;

  };
} // end of XC namespace

#endif
