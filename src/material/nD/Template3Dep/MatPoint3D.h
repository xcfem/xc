//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//=============================================================================
//
// COPYRIGHT (C):     :-))
// PROJECT:           Object Oriented Finite Element Program
// FILE:              MatPoint3D.h
// CLASS:             MatPoint3D
// MEMBER FUNCTIONS:
//
// MEMBER VARIABLES
//
// PURPOSE:           Gauss Point
// RETURN:
// VERSION:
// LANGUAGE:          C++.ver >= 3.0 (Borland.C++.ver=3.1||SUN.C++.ver=3.0.1)
// TARGET OS:         DOS || UNIX || . . .
// DESIGNER:          Boris Jeremic
// PROGRAMMER:        Boris Jeremic
// DATE:              17 October 1994.
// UPDATE HISTORY:
//
//
//#                    Aug 2000 porting to OpenSees                            #
//=============================================================================
//
//
#ifndef MATPOINT3D_H
#define MATPOINT3D_H

#include <string>
#include "domain/mesh/element/utils/gauss_models/GaussPoint.h"

namespace XC {
  class NDMaterial;
  class stresstensor;
  class straintensor;

//! @ingroup NDMat
//
//! @brief Punto de integración.
class MatPoint3D: public GaussPoint
  {
  private:
    short int  r_direction_point_number;
    short int  s_direction_point_number;
    short int  t_direction_point_number;

  public: // dobro ovo su samo ustvari pointeri pa nema ekstra kopiranja !

    // *gpEPS is used to hold all state parameters and internal vars, instead of the 
    // following stresses, strain, and internal vars __Zhaohui 09-30-2000
    
    //Now no EPState needed. Each MatPoint has an NDMaterial
    //EPState        *gpEPS;
    
    //stresstensor * p_stress;
    //stresstensor * p_iterative_stress; // to be used for iterative nodal forces
    //double         q_ast_iterative;
    //straintensor * p_strain;
    //tensor  * TangentE;
    
    //Might be ElasticIsotropic3D or Template3Dep
    NDMaterial *matmodel;

  public:
    //default constructor
    MatPoint3D(  short int INr_direction_point_number = 0,
               short int INs_direction_point_number = 0,
               short int INt_direction_point_number = 0,
               double r_coord = 0,
               double s_coord = 0,
               double t_coord = 0,
               double r_weight = 0,
               double s_weight = 0,
               double t_weight = 0,
               //EPState *eps    = 0,
               NDMaterial * p_mmodel = 0   
	       //stresstensor * p_INstress = 0,
               //stresstensor * p_INiterative_stress = 0,
               //double         IN_q_ast_iterative = 0.0,
               //straintensor * p_INstrain = 0,
               //tensor * p_Tangent_E_tensor = 0,
               );
        
    // Constructor 1
    ~MatPoint3D(void);

    void Initialize(short int INr_direction_point_number,
                    short int INs_direction_point_number,
                    short int INt_direction_point_number,
                    double r_coord,
                    double s_coord,
                    double t_coord,
                    double r_weight,
                    double s_weight,
                    double t_weight,
                    //stresstensor * p_INstress,
                    //stresstensor * p_INiterative_stress,
                    //double         IN_q_ast_iterative,
                    //straintensor * p_INstrain,
                    //tensor * p_Tangent_E_tensor,
                    //EPState  * EPS,
		    NDMaterial * p_mmodel
                   );                             
  public:
    short int GP_number_r(void) const;
    short int GP_number_s(void) const;
    short int GP_number_t(void) const;
   
    MatPoint3D * GP(void);

    double r_coordinate() const;
    double s_coordinate() const;
    double t_coordinate() const;
    
    double r_weight() const;
    double s_weight() const;
    double t_weight() const;
    
    //void setEPS(EPState *eps);
    //EPState *getEPS() const;
    NDMaterial* getNDMat() const;
    const std::string &getType(void) const;
    int getTag(void) const;
    double getrho() const;
    const stresstensor &getStressTensor(void) const;
    const straintensor &getStrainTensor(void) const;
    //Added Aug. 13, 2001 Joey
    const straintensor &getPlasticStrainTensor(void) const;
    //Added 02-18-03 Joey
    double getpsi() const; //state parameter
    
    int commitState(void) ;
    int revertToLastCommit(void) ;
    int revertToStart(void) ;

    void report(const std::string &msg) const;
    void reportpqtheta(const std::string &msg) const;
    void reportTensor(const std::string &msg) const;
  };
} // end of XC namespace

#endif 
//
