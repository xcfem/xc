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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.12 $
// $Date: 2005/11/22 19:44:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/LoadPattern.h,v $
                                                                        
                                                                        
#ifndef LoadPattern_h
#define LoadPattern_h

// Written: fmk 
// Created: 07/99
// Revision: A
//
// Purpose: This file contains the class definition for LoadPattern.
// LoadPattern is a concrete class. A LoadPattern object is used to 
// to store reference loads and single point constraints and a TimeSeries function
// which is used to determine the load factor given the pseudo-time
// to the model. 
//
// What: "@(#) LoadPattern.h, revA"

#include "NodeLocker.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/utils_any.h"

namespace XC {
class NodalLoad;
class TimeSeries;
class ElementalLoad;
class NodalLoadIter;
class ElementalLoadIter;
class GroundMotion;
class Vector;

//! \ingroup Loads
//!
//!
//! @defgroup LPatterns Pautas (casos) de carga.
//
//! @ingroup LPatterns
//
//! @brief Contiene las cargas que actúan sobre la estructura
//! en determinado caso o hipótesis simple. Además define la
//! variación en el tiempo de dichas cargas (constante, lineal,
//! acelerograma,...).
class LoadPattern: public NodeLocker
  {
  private:
    double loadFactor; //!< factor de ponderación obtenido del TimeSeries (ver applyLoad).
    double gamma_f; //!< Factor de ponderación impuesto por la combinación de hipótesis.

    TimeSeries *theSeries; //!< puntero a la función de variación en el tiempo.

    // storage objects for the loads.
    TaggedObjectStorage  *theNodalLoads; //!< Almacenamiento de cargas nodales.
    TaggedObjectStorage  *theElementalLoads; //!< Almacenamiento de cargas sobre elementos.

    // iterator objects for the objects added to the storage objects
    NodalLoadIter       *theNodIter; //!< Iterador sobre cargas nodales.
    ElementalLoadIter   *theEleIter; //!< Iterador sobre cargas sobre elementos.

    // AddingSensitivity:BEGIN //////////////////////////////////////
    Vector *randomLoads;
    bool RVisRandomProcessDiscretizer;
    // AddingSensitivity:END ////////////////////////////////////////

    void libera_contenedores(void);
    void libera_iteradores(void);
    void alloc_contenedores(void);
    void alloc_iteradores(void);
    void libera(void);
  protected:
    int isConstant;     // to indicate whether setConstant has been called
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);
  public:
    LoadPattern(int tag);
    LoadPattern(void);                  // for FEM_ObjectBroker
    LoadPattern(int tag, int classTag); // for subclasses
    virtual ~LoadPattern(void);

    // method to set the associated TimeSeries and Domain
    virtual void setTimeSeries(TimeSeries *theSeries);
    virtual void setDomain(Domain *theDomain);
    bool addToDomain(void);
    void removeFromDomain(void);

    // methods to add loads
    virtual bool addNodalLoad(NodalLoad *);
    NodalLoad *newNodalLoad(const int &,const Vector &);
    virtual bool addElementalLoad(ElementalLoad *);
    bool newElementalLoad(ElementalLoad *);
    ElementalLoad *newElementalLoad(const std::string &);
    virtual bool addSP_Constraint(SP_Constraint *theSp);

    virtual NodalLoadIter &getNodalLoads(void);
    virtual ElementalLoadIter &getElementalLoads(void);
    int getNumNodalLoads(void) const;
    int getNumElementalLoads(void) const;
    int getNumLoads(void) const;

    // methods to remove loads
    virtual void clearAll(void);
    virtual bool removeNodalLoad(int tag);
    virtual bool removeElementalLoad(int tag);

    // methods to apply loads
    virtual void applyLoad(double pseudoTime = 0.0);
    virtual void setLoadConstant(void);


    virtual const double &getLoadFactor(void) const;
    const double &GammaF(void) const;
    double &GammaF(void);
    void setGammaF(const double &);
       

    // methods for o/p
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    virtual void Print(std::ostream &s, int flag =0);
    virtual any_const_ptr GetProp(const std::string &cod) const;

    virtual LoadPattern *getCopy(void);

    virtual int addMotion(GroundMotion &theMotion, int tag);
    virtual GroundMotion *getMotion(int tag);

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual void applyLoadSensitivity(double pseudoTime = 0.0);
    virtual int  setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int  updateParameter(int parameterID, Information &info);
    virtual int  activateParameter(int parameterID);
    virtual const Vector &getExternalForceSensitivity(int gradNumber);
    // AddingSensitivity:END ///////////////////////////////////////////
  };

} // fin namespace XC

#endif







