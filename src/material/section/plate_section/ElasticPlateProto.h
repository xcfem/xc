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
//ElasticPlateProto.h

#ifndef ElasticPlateProto_h
#define ElasticPlateProto_h


#include "ElasticPlateBase.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "utility/actor/actor/MovableVector.h"


namespace XC {
//! \ingroup MATPLAC
//
//! @brief ??.
template <int SZ>
class ElasticPlateProto : public ElasticPlateBase
  {
  protected:
    Vector trialStrain;
    Vector initialStrain;
    static Vector stress;
    static Matrix tangent;

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public : 
    ElasticPlateProto(int tag,int classTag);
    ElasticPlateProto(int classTag);
    ElasticPlateProto(int tag,int classTag, double E, double nu, double h);

    //send back order of strain in vector form
    int getOrder(void) const;

    //get the strain and integrate plasticity equations
    int setInitialSectionDeformation(const Vector &strain);
    int setTrialSectionDeformation(const Vector &strain_from_element);
    //send back the strain
    inline const Vector &getInitialSectionDeformation(void) const
      { return initialStrain; }
    const Vector& getSectionDeformation(void) const;

    int revertToStart(void);

  }; //end of ElasticPlateProto declarations

//static vector and matrices
template <int SZ>
XC::Vector XC::ElasticPlateProto<SZ>::stress(SZ);
template <int SZ>
XC::Matrix XC::ElasticPlateProto<SZ>::tangent(SZ,SZ);


template <int SZ>
XC::ElasticPlateProto<SZ>::ElasticPlateProto(int tag,int classTag)
  : ElasticPlateBase(tag, classTag), trialStrain(SZ), initialStrain(SZ) {}

//null constructor
template <int SZ>
XC::ElasticPlateProto<SZ>::ElasticPlateProto(int classTag)
  : ElasticPlateBase( 0, classTag), trialStrain(SZ), initialStrain(SZ) {}

//full constructor
template <int SZ>
XC::ElasticPlateProto<SZ>::ElasticPlateProto(int tag, int classTag,
                                           double young,
                                           double poisson,
                                           double thickness)
  : ElasticPlateBase(tag,classTag,young,poisson,thickness), trialStrain(SZ), initialStrain(SZ) {}

template <int SZ>
int XC::ElasticPlateProto<SZ>::getOrder(void) const
  { return SZ; }

//get the strain 
template <int SZ>
int XC::ElasticPlateProto<SZ>::setInitialSectionDeformation(const Vector &strain)
  {
    initialStrain= strain;
    return 0;
  }

//get the strain 
template <int SZ>
int XC::ElasticPlateProto<SZ>::setTrialSectionDeformation(const Vector &strain)
  {
    trialStrain= strain;
    return 0;
  }

//send back the strain
template <int SZ>
const XC::Vector &XC::ElasticPlateProto<SZ>::getSectionDeformation(void) const
  {
    static Vector retval;
    retval= trialStrain-initialStrain;
    return retval;
  }

//@ brief revert to start
template <int SZ>
int XC::ElasticPlateProto<SZ>::revertToStart(void)
  {
    initialStrain.Zero();
    return ElasticPlateBase::revertToStart();
  }

//! @brief Envía los datos through the channel being passed as parameter.
template <int SZ>
int XC::ElasticPlateProto<SZ>::sendData(CommParameters &cp)
  {
    int res= ElasticPlateBase::sendData(cp);
    res+= cp.sendVector(trialStrain,getDbTagData(),CommMetaData(6));
    res+= cp.sendVector(initialStrain,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Recibe los datos through the channel being passed as parameter.
template <int SZ>
  int XC::ElasticPlateProto<SZ>::recvData(const CommParameters &cp)
  {
    int res= ElasticPlateBase::recvData(cp);
    res+= cp.receiveVector(trialStrain,getDbTagData(),CommMetaData(6));
    res+= cp.receiveVector(initialStrain,getDbTagData(),CommMetaData(7));
    return res;
  }

} // end of XC namespace

#endif
