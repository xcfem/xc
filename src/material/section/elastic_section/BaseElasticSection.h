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
//BaseElasticSection.h

#ifndef BaseElasticSection_h
#define BaseElasticSection_h

#include <material/section/SeccionBarraPrismatica.h>
#include <utility/matrix/Vector.h>

namespace XC {

//! \ingroup MATSCC
//
//! @defgroup MATSCCElastica Secciones de material elástico.
//
//! \ingroup MATSCCElastica
//
//! @brief Clase base para las secciones transversales formadas
//! por un material elástico lineal.
class BaseElasticSection: public SeccionBarraPrismatica
  {
  protected:
    Vector eTrial; //!< section trial deformations
    Vector eInic; //!< section initial deformations
    Vector eCommit; //!< section commited deformations
    
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);

  public:
    BaseElasticSection(int tag,int classTag,const size_t &dim,MaterialLoader *mat_ldr= NULL);    

    int commitState(void);
    int revertToLastCommit (void);
    int revertToStart (void);

    virtual void sectionGeometry(const std::string &)= 0;

    int setInitialSectionDeformation(const Vector&);
    int setTrialSectionDeformation(const Vector&);
    inline const Vector &getInitialSectionDeformation(void) const
      { return eInic; }
    const Vector &getSectionDeformation(void) const;

    any_const_ptr GetProp(const std::string &cod) const;
    void Print(std::ostream &s, int flag =0) const;
  };
} // fin namespace XC

#endif
