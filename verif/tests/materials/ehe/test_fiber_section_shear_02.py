# -*- coding: utf-8 -*-
''' Test for checking the shear-strength verificacion of a reinforced concrete section.
   Some comparison values are obtained from example ER-CA-02 of
   www.areadecalculo.com. Calculation is carried on according to EHE-08 standard. '''

from __future__ import division
from __future__ import print_function

import xc_base
import geom
import xc
from misc import scc3d_testing_bench
from solution import predefined_solutions
from model import predefined_spaces
from materials.sections.fiber_section import def_simple_RC_section

from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking

from materials import typical_materials

import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"


# Data

width= 0.4  # Cross-section width [m]
depth= 0.6  # Cross-section depth [m]
cover= 0.05 # Cover [m]

NDato= 0         # Axial force when checking shear.
MyDato= 1.9606e5 # Moment to check shear it's value is forced to obtain Vcu around 82.607 kN.
MzDato= 0        # Moment value when checking shear.
VDato= 125e3     # Shear value.

# Longitudinal reinforcement
rebarArea= EHE_materials.Fi20
rebarDiam= 20e-3
numOfRebars= 3
lowerRow= def_simple_RC_section.ReinfRow(areaRebar= rebarArea, width= width, nRebars= numOfRebars, nominalCover= cover-rebarDiam/2.0, nominalLatCover= cover-rebarDiam/2.0)
upperRow= def_simple_RC_section.ReinfRow(areaRebar= rebarArea, width= width, nRebars= numOfRebars, nominalCover= cover-rebarDiam/2.0, nominalLatCover= cover-rebarDiam/2.0)

# Transverse reinforcement
diamATrsv= 6e-3
numRamas= 4
shearReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh",nShReinfBranches= numRamas, areaShReinfBranch= EHE_materials.Fi6, shReinfSpacing= 0.2, angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
materialHandler= preprocessor.getMaterialHandler

# Materials definition
concr= EHE_materials.HA30
concr.alfacc= 0.85 # f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
B500S= EHE_materials.B500S

# Section geometry
section= def_simple_RC_section.RCRectangularSection(name='test', width= width, depth= depth, concrType= concr, reinfSteelType= B500S)
section.positvRebarRows= def_simple_RC_section.LongReinfLayers([lowerRow])
section.negatvRebarRows= def_simple_RC_section.LongReinfLayers([upperRow])
section.shReinfY= shearReinf

section.defRCSection(preprocessor,matDiagType= 'd')

scc3d_testing_bench.sectionModel(preprocessor, section.sectionName)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)

# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([NDato,0,VDato,0,MyDato,MzDato]))

#We add the load case to domain.
lPatterns.addToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.simple_newton_raphson(feProblem)
analOk= analysis.analyze(10)


shearController= EHE_limit_state_checking.ShearController('ULS_shear')
secHAParamsTorsion= EHE_limit_state_checking.computeEffectiveHollowSectionParameters(section.geomSection,depth/2.0,cover)


elements= preprocessor.getElementHandler
scc= elements.getElement(1).getSection()
shearCF= shearController.checkSection(scc,secHAParamsTorsion)

Vu1A= shearController.Vu1
Vu1ATeor= 1.32e6
VcuA= shearController.Vcu
VcuATeor= 82.607e3
VsuA= shearController.Vsu
Vu2A= shearController.Vu2

ratio1= ((Vu1A-Vu1ATeor)/Vu1ATeor)
ratio2= ((VcuA-VcuATeor)/VcuATeor)
ratio3= ((VsuA-111.966e3)/111.966e3)
ratio4= ((Vu2A-194.574e3)/194.574e3)

'''
print("Vu1A= ",Vu1A/1e3," kN")
print("Vu1ATeor= ",Vu1ATeor/1e3," kN")
print("ratio1= ",ratio1)
print("VcuA= ",VcuA/1e3," kN")
print("VcuATeor= ",VcuATeor/1e3," kN")
print("ratio2= ",ratio2)
print("VsuA= ",VsuA/1e3," kN")
print("Vu2A= ",Vu2A/1e3," kN")
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
#if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-4) & (abs(ratio4)<1e-4):
if (abs(ratio1)<0.1) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-4) & (abs(ratio4)<1e-4):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
