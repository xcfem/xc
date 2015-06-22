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
//BeamColumnWithSectionFDTrf2d.h

#ifndef BeamColumnWithSectionFDTrf2d_h
#define BeamColumnWithSectionFDTrf2d_h

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFD.h>

namespace XC {
class Node;

class CrdTransf;
class CrdTransf2d;

//! \ingroup ElemBarra
//
//! @brief Elemento barra con material de tipo SectionForceDeformation  para problemas bidimensionales.
class BeamColumnWithSectionFDTrf2d: public BeamColumnWithSectionFD
  {
    BeamColumnWithSectionFDTrf2d &operator=(const BeamColumnWithSectionFDTrf2d &);
  protected:  
    CrdTransf2d *theCoordTransf;

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

    void set_transf(const CrdTransf *trf);
    bool procesa_comando(CmdStatus &status);
  public:
    BeamColumnWithSectionFDTrf2d(int tag,int classTag,int numSec);
    BeamColumnWithSectionFDTrf2d(int tag,int classTag,int numSec,const Material *mat,const CrdTransf *trf);
    BeamColumnWithSectionFDTrf2d(int tag, int classTag,int numSec, int nodeI, int nodeJ,CrdTransf2d &coordTransf);
    BeamColumnWithSectionFDTrf2d(const BeamColumnWithSectionFDTrf2d &);
    ~BeamColumnWithSectionFDTrf2d(void);
    void initialize_trf(void);
    virtual CrdTransf *getCoordTransf(void);
    virtual const CrdTransf *getCoordTransf(void) const;
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC

#endif
