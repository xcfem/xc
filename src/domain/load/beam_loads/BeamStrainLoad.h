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
//BeamStrainLoad.h
                                                                        
#ifndef BeamStrainLoad_h
#define BeamStrainLoad_h


#include <domain/load/beam_loads/BeamLoad.h>
#include "material/section/diag_interaccion/PlanoDeformacion.h"

namespace XC {
class Matrix;
class ConstantesSecc2d;
class ConstantesSecc3d;

//! @ingroup ElemLoads
//
//! @brief Carga debida a deformaciones impuestas sobre elementos de tipo viga.
class BeamStrainLoad : public BeamLoad
  {
    PlanoDeformacion planoDefDorsal; //!< Deformaciones impuestas en el extremo dorsal.
    PlanoDeformacion planoDefFrontal; //!< Deformaciones impuestas en el extremo frontal.
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    BeamStrainLoad(int tag, const ID &theElementTags);
    BeamStrainLoad(int tag= 0);

    inline const PlanoDeformacion &getPlanoDeformacion1(void) const
      { return planoDefDorsal; }
    inline void setPlanoDeformacion1(const PlanoDeformacion &p)
      { planoDefDorsal= p; }
    inline const PlanoDeformacion &getPlanoDeformacion2(void) const
      { return planoDefFrontal; }
    inline void setPlanoDeformacion2(const PlanoDeformacion &p)
      { planoDefFrontal= p; }
    const Vector &getSection1Deformation(const size_t &order,const ResponseId &code) const;
    const Vector &getSection2Deformation(const size_t &order,const ResponseId &code) const;

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace

#endif

