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
//BaseElasticSection2d.h

#ifndef BaseElasticSection2d_h
#define BaseElasticSection2d_h

#include "BaseElasticSection.h"
#include <utility/matrix/Vector.h>
#include "material/section/repres/CrossSectionProperties2d.h"

namespace XC {

//! \ingroup MATSCCElastica
//
//! @brief Clase base para las secciones transversales formadas
//! por un material elástico lineal en un problema bidimensional
//! (3 esfuerzos por sección).
class BaseElasticSection2d: public BaseElasticSection
  {
  protected:
    CrossSectionProperties2d ctes_scc; //!< Características mecánicas de la seccion E,A,I.

    int sendData(CommParameters &);
    int recvData(const CommParameters &);


  public:
    BaseElasticSection2d(int tag,int classTag,const size_t &dim, const double &E, const double &A, const double &I,double G, const double &alpha);
    BaseElasticSection2d(int tag,int classTag,const size_t &dim,MaterialLoader *mat_ldr= NULL);    
    BaseElasticSection2d(int classTag,const size_t &dim);

    const CrossSectionProperties2d &getCrossSectionProperties(void) const;   
    void setCrossSectionProperties(const CrossSectionProperties2d &);  
    void sectionGeometry(const std::string &);


    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif
