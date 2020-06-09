# -*- coding: utf-8 -*-
''' Test for checking the shear-strength verificacion of a circular
    reinforced concrete section.

    Results are compared with those of the PhD thesis:
    "Efectos de los esfuerzos cortantes biaxiales en la respuesta sísmica de
     columnas de hormigón armado" by Edison Osorio Bustamante. Barcelona
     november 2012. Universitat Politècnica de Catalunya. Table 5-11 
'''

from __future__ import division
from __future__ import print_function

from misc_utils import log_messages as lmsg
import xc_base
import geom
import xc
from misc import scc3d_testing_bench
from solution import predefined_solutions
from model import predefined_spaces
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section

from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking

from materials import typical_materials

import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

# Materials definition
concr= EHE_materials.HA40
concr.alfacc= 0.85 # f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
concr.gmmC= 1.0
steel= EHE_materials.B500S
steel.gammaS= 1.0

# Section geometry
diameter= 350e-3 # Cross-section diameter [m]
cover= 0.025 # Cover [m]
section= def_column_RC_section.RCCircularSection(name='test',Rext= diameter/2.0, concrType=concr, reinfSteelType= steel)
section.nDivIJ= 10

# Longitudinal reinforcement
rebarDiam= 16e-3
rebarArea= EHE_materials.Fi16
numOfRebars= 12


# Shear reinforcement
shearReinfArea= EHE_materials.Fi6
shearReinfDiam= 6e-3
nBranches= 2
shearReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh",nShReinfBranches= nBranches, areaShReinfBranch= shearReinfArea, shReinfSpacing= 0.2, angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)

nCover= cover

mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars= numOfRebars, width= math.pi*(diameter-2*cover), nominalCover= nCover)])


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
materialHandler= preprocessor.getMaterialHandler

# Section geometry
section.mainReinf= mainReinf
section.shReinf= shearReinf

section.defRCSection(preprocessor,matDiagType= 'd')

scc3d_testing_bench.sectionModel(preprocessor, section.sectionName)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)

# Loads definition
Nd= -550e3 # Axial force when checking shear.
Myd= 190e3# 223.1e3 # Y bending moment when checking shear. Reduced
          # to achieve convergence.
Mzd= 0.0 # Z bending moment value when checking shear.
Vd= 148.7e3 # Shear value.

lPatterns= preprocessor.getLoadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")

lp0.newNodalLoad(2,xc.Vector([Nd,0,Vd,0,Myd,Mzd]))

#We add the load case to domain.
lPatterns.addToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.simple_newton_raphson(feProblem, mxNumIter= 10)
analOk= analysis.analyze(10)

import os
fname= os.path.basename(__file__)
if(analOk<0):
    lmsg.error(fname+' ERROR. Failed to converge.')
    quit()


shearController= EHE_limit_state_checking.ShearController('ULS_shear')
secHAParamsTorsion= EHE_limit_state_checking.computeEffectiveHollowSectionParametersRCSection(section)

elements= preprocessor.getElementHandler
scc= elements.getElement(1).getSection()
shearCF= shearController.checkSection(scc,secHAParamsTorsion)

Vu1= shearController.Vu1
Vu1Ref= 1.32e6
Vcu= shearController.Vcu
VcuRef= 84.2e3+67.2e3
Vsu= shearController.Vsu
VsuRef= 25.2e3*400e6/steel.fyd() # They don't reduce the stress on shear reinf.
Vu2= shearController.Vu2
Vu= shearController.Vu
VuRef= 176.7e3

ratio1= ((Vcu-VcuRef)/VcuRef)
ratio2= ((Vsu-VsuRef)/VsuRef)
ratio3= ((Vu-VuRef)/VuRef)

'''
print("Vcu= ",Vcu/1e3," kN")
print("VcuRef= ",VcuRef/1e3," kN")
print("ratio1= ",ratio2)
print("Vsu= ",Vsu/1e3," kN")
print("VsuRef= ",VsuRef/1e3," kN")
print("ratio2= ",ratio3)
print("Vu= ",Vu/1e3," kN")
print("VuRef= ",VuRef/1e3," kN")
print("ratio3= ",ratio4)
'''


if ((abs(ratio1)<0.1) & (abs(ratio2)<0.1) & (abs(ratio3)<0.01)):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
