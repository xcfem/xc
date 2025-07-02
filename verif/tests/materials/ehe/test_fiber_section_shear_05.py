# -*- coding: utf-8 -*-
''' Test for checking the shear-strength verificacion of a reinforced concrete
    section.

   Test based on the example ER-CA-02 of www.areadecalculo.com. Calculation is
   carried on according to EHE-08 standard.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

import math
import xc
from misc import scc3d_testing_bench
from solution import predefined_solutions
from model import predefined_spaces
from materials.sections.fiber_section import def_simple_RC_section

from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from postprocess import limit_state_data as lsd

# Data
width= 1.0  # Cross-section width [m]
depth= 0.5  # Cross-section depth [m]
cover= 0.03 # Cover [m]

NDato= -170e3   # Axial force when checking shear.
MyDato= 440e3 # Moment to check shear
MzDato= 0     # Moment value when checking shear.
VDato= 1067e3 # Shear value.

# Longitudinal reinforcement
rebarArea= EHE_materials.Fi20
rebarDiam= 20e-3
numOfRebars= 10
lowerRow= def_simple_RC_section.ReinfRow(areaRebar= rebarArea, width= width, nRebars= numOfRebars, nominalCover= cover-rebarDiam/2.0, nominalLatCover= cover-rebarDiam/2.0)
upperRow= def_simple_RC_section.ReinfRow(areaRebar= rebarArea, width= width, nRebars= numOfRebars, nominalCover= cover-rebarDiam/2.0, nominalLatCover= cover-rebarDiam/2.0)

# Transverse reinforcement
diamATrsv= 12e-3
numRamas= 5
shearReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh", nShReinfBranches= numRamas, areaShReinfBranch= EHE_materials.Fi12, shReinfSpacing= 0.2, angAlphaShReinf= math.pi/2.0, angThetaConcrStruts= math.pi/4.0)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Materials definition
concr= EHE_materials.HA35
# concr.alfacc= 0.85 # f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
B500S= EHE_materials.B500S

# Section geometry
section= def_simple_RC_section.RCRectangularSection(name='test', width= width, depth= depth, concrType= concr, reinfSteelType= B500S)
section.positvRebarRows= def_simple_RC_section.LongReinfLayers([lowerRow])
section.negatvRebarRows= def_simple_RC_section.LongReinfLayers([upperRow])
section.shReinfY= shearReinf

section.defRCSection(preprocessor, matDiagType= 'd')

zlElement, nodA, nodB= scc3d_testing_bench.section_model(preprocessor, section.name)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(nodA.tag)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nodB.tag,xc.Vector([NDato,0,VDato,0,MyDato,MzDato]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem)
analOk= analysis.analyze(10)

limitState= EHE_limit_state_checking.shearResistance
shearController= limitState.getController()

secHAParamsTorsion= EHE_limit_state_checking.computeEffectiveHollowSectionParametersRCSection(section)

scc= zlElement.getSection()
shearCF= shearController.checkSection(sct= scc, elementDimension= zlElement.getDimension, torsionParameters= secHAParamsTorsion)

b0= shearController.strutWidth
d= shearController.effectiveDepth
z= shearController.mechanicLeverArm
Nc= shearController.concreteAxialForce
K= shearController.getKEHE08()
theta= shearController.theta
cracksAngle= shearController.thetaFisuras
cracked= shearController.isCracked()
fcv= shearController.fckH
Vu1= shearController.Vu1
Vcu= shearController.Vcu
VcuRef= 17.069431559261e3 # See spreadsheet with the same name in this folder.
ratio1= (abs(Vcu-VcuRef)/VcuRef)
Vsu= shearController.Vsu
Vu2= shearController.Vu2
Vu2Ref= 962.943207724092e3 # See spreadsheet with the same name in this folder.
ratio2= (abs(Vu2-Vu2Ref)/Vu2Ref)


'''
# section.plot(preprocessor) # Display section geometry
# section.pdfReport(preprocessor= preprocessor, showPDF= True, keepPDF= False)
print('Nc= ', Nc/1e3, 'kN')
print("K= ", K)
print("b0= ", b0,'m')
print("d= ", d,'m')
print("z= ", z,'m')
print("bending: ", shearController.isBending)
print("cracked: ", cracked)
print("theta= ", math.degrees(theta))
print("cracks angle: ", math.degrees(cracksAngle))
print("cotg(theta)= ", 1.0/math.tan(theta))
print("fcv= ", fcv/1e6," MPa")
print("Vu1= ", Vu1/1e3," kN")
print("Vcu= ", Vcu/1e3," kN")
print("VcuRef= ", VcuRef/1e3," kN")
print("ratio1= ", ratio1)
print("Vsu= ", Vsu/1e3," kN")
print("Vu2= ", Vu2/1e3," kN")
print("ratio2= ", ratio2)
print("shearCF= ", shearCF[0])
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-5) and (ratio2<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

  
