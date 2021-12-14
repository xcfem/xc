//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//uniaxialMaterialClassTags.h

#ifndef uniaxialMaterialClassTags_h
#define uniaxialMaterialClassTags_h

#define MAT_TAG_ElasticMaterial			1
#define MAT_TAG_ElasticPPMaterial		2
#define MAT_TAG_ParallelMaterial		3
#define MAT_TAG_Concrete01			4
#define MAT_TAG_Steel01				5

#define MAT_TAG_Hardening			6
#define MAT_TAG_Hysteretic			7
#define MAT_TAG_EPPGap				8
#define MAT_TAG_Viscous				9
#define MAT_TAG_Backbone			10
#define MAT_TAG_PathIndependent			11
#define MAT_TAG_SeriesMaterial			12
#define MAT_TAG_CableMaterial                   13
#define MAT_TAG_ENTMaterial			14
#define MAT_TAG_Penalty				15
#define MAT_TAG_MinMax				16
#define MAT_TAG_BoucWen				17
#define MAT_TAG_Pinching4			18
#define MAT_TAG_BarSlip				19
#define MAT_TAG_Fatigue			        20

// Terje:
#define MAT_TAG_SmoothSteel01			21
#define MAT_TAG_SmoothConcrete01		22

#define MAT_TAG_Steel03				23
#define MAT_TAG_ReinforcingSteel		24

#define MAT_TAG_Concrete02			25
#define MAT_TAG_Steel02				26


#define MAT_TAG_Concrete04                      30

//B Jeremic
#define MAT_TAG_PySimple1                       31
#define MAT_TAG_TzSimple1                       32
#define MAT_TAG_QzSimple1                       33
#define MAT_TAG_PyLiq1                          34
#define MAT_TAG_TzLiq1                          35

#define MAT_TAG_FedeasMaterial                  36
#define MAT_TAG_FedeasBond1                     37
#define MAT_TAG_FedeasBond2                     38
#define MAT_TAG_FedeasConcrete1                 39
#define MAT_TAG_FedeasConcrete2                 40
#define MAT_TAG_FedeasConcrete3                 41
#define MAT_TAG_FedeasHardening                 42
#define MAT_TAG_FedeasHysteretic1               43
#define MAT_TAG_FedeasHysteretic2               44
#define MAT_TAG_FedeasSteel1                    45
#define MAT_TAG_FedeasSteel2                    46

#define MAT_TAG_DrainMaterial		        48
#define MAT_TAG_DrainHardening		        49
#define MAT_TAG_DrainBilinear		        50
#define MAT_TAG_DrainClough1		        51
#define MAT_TAG_DrainClough2		        52
#define MAT_TAG_DrainPinch1			53
#define MAT_TAG_DrainPinch2			54

#define MAT_TAG_SnapMaterial		        55
#define MAT_TAG_SnapBilinear		        56
#define MAT_TAG_SnapClough		        57
#define MAT_TAG_SnapPinch		        58
#define MAT_TAG_SnapCloughDamage	        59
#define MAT_TAG_SnapPinchingDamage	        60

#define MAT_TAG_Clough1	                        61
#define MAT_TAG_Clough2	                        62
#define MAT_TAG_Pinch1	                        63
#define MAT_TAG_BiLinear	                64
#define MAT_TAG_Pinching	                65

#endif
