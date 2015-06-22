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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/numberer/DOF_Numberer.h,v $
                                                                        
                                                                        
// File: ~/analysis/numberer/DOF_Numberer.h
// 
// Written: fmk 
// Created: 9/96
// Revision: A
//
// Description: This file contains the class definition for DOF_Numberer.
// DOF_Numberer is an abstract base class, i.e. no objects of it's
// type can be created. 
//
// What: "@(#) DOF_Numberer.h, revA"

#ifndef DOF_Numberer_h
#define DOF_Numberer_h

#include <utility/actor/actor/MovableObject.h>
#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {
class AnalysisModel;
class GraphNumberer;
class FEM_ObjectBroker;
class ID;
class ModelWrapper;

//! @ingroup Analisis
//
//! @defgroup Numerador Numeración de grados de libertad.
//
//! @ingroup Numerador
//
//! @brief Clase base para los numeradores de grados de libertad.
//! Para realizar su función esta clase contiene un numerador de grafos
//! y un apuntador al modelo de análisis.
//! Este es el objeto encargado de crear, almacenar y destruir el
//! numerador de grafos.
class DOF_Numberer: public MovableObject, public EntCmd
  {
  private:
    ModelWrapper *getModelWrapper(void);
    const ModelWrapper *getModelWrapper(void) const;

    GraphNumberer *theGraphNumberer; //!< Numerador de grados de libertad.
  protected:
    AnalysisModel *getAnalysisModelPtr(void);
    GraphNumberer *getGraphNumbererPtr(void);
    const AnalysisModel *getAnalysisModelPtr(void) const;
    const GraphNumberer *getGraphNumbererPtr(void) const;

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    void alloc(const std::string &);
    void copia(const GraphNumberer &);
    void libera(void);

    friend class ModelWrapper;
    friend class FEM_ObjectBroker;
    DOF_Numberer(ModelWrapper *,int classTag);
    DOF_Numberer(ModelWrapper *);
    DOF_Numberer(const DOF_Numberer &);
    DOF_Numberer &operator=(const DOF_Numberer &);
    virtual DOF_Numberer *getCopy(void) const;
  public:
    virtual ~DOF_Numberer(void);

    // pure virtual functions
    virtual int numberDOF(int lastDOF_Group = -1);
    virtual int numberDOF(ID &lastDOF_Groups);

    void useAlgorithm(const std::string &);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };
} // fin namespace XC

#endif

