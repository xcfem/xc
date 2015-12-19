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
//BeamColumnWithSectionFDs.h

#ifndef BeamColumnWithSectionFD_h
#define BeamColumnWithSectionFD_h

#include <domain/mesh/element/Element1D.h>
#include "material/section/VectorSeccionesBarraPrismatica.h"
#include "SectionMatrices.h"


namespace XC {

//! \ingroup ElemBarra
//
//! @brief Elemento barra con material SeccionBarraPrismatica
class BeamColumnWithSectionFD : public Element1D
  {
  protected:
    VectorSeccionesBarraPrismatica theSections; //!< pointers to the SeccionBarraPrismatica objects
    bool procesa_comando(CmdStatus &status);

    SectionMatrices section_matrices;

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc= 1);
    BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc,const Material *m);
    BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc,const SeccionBarraPrismatica *sccModel);
    BeamColumnWithSectionFD(int tag, int classTag,const size_t &numSecc,const SeccionBarraPrismatica *sccModel,int Nd1,int Nd2);

    const SeccionBarraPrismatica *getSectionPtr(const size_t &i) const;

    void setTrialSectionDeformations(const std::vector<Vector> &vs);

    virtual void setSection(const SeccionBarraPrismatica *sccModel);
    bool setSections(const std::vector<SeccionBarraPrismatica *> &sectionPtrs);
    inline size_t getNumSections(void) const
      { return theSections.size(); }
    inline VectorSeccionesBarraPrismatica &getSections(void)
      { return theSections; }
 
    Response *setSectionResponse(SeccionBarraPrismatica *,const std::vector<std::string> &,const size_t &,Information &);
    int setSectionParameter(SeccionBarraPrismatica *,const std::vector<std::string> &,const size_t &, Parameter &);

    // public methods to set the state of the element
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    any_const_ptr GetProp(const std::string &) const;
  };

} //end of XC namespace
#endif
