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
//AggregatorAdditions.h
//deque de punteros (se emplear en la clase Set).


#ifndef AGGREGATORADDITIONS_H
#define AGGREGATORADDITIONS_H

#include <material/uniaxial/DqUniaxialMaterial.h>

namespace XC {

class ResponseId;

//! \ingroup MATSCC
//
//! @brief Punteros a UniaxialMaterial con grado de libertad asociado a
//! a cada uno de dichos grados de libertad (se emplea en SectionAggregator).
class AggregatorAdditions: public DqUniaxialMaterial
  {
    ResponseId *matCodes; //!< Esfuerzo para el que cada UniaxialMaterial proporciona una respuesta.
  protected:
    void libera(void);
    void alloc_ptrs(const ResponseId &);
    void alloc_ptrs(const ResponseId *);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    virtual bool procesa_comando(CmdStatus &);
  public:
    AggregatorAdditions(EntCmd *owner= NULL);
    AggregatorAdditions(EntCmd *owner,const UniaxialMaterial &,int c);
    AggregatorAdditions(const AggregatorAdditions &otro);
    AggregatorAdditions(const AggregatorAdditions &otro,SectionForceDeformation *s);
    AggregatorAdditions &operator=(const AggregatorAdditions &otro);
    ~AggregatorAdditions(void);

    bool check_ptrs(void) const;
    void putMatCodes(const ResponseId &codes);

    void getType(ResponseId &retval,const size_t &offset) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

} //end of XC namespace
#endif
