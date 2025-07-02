# -*- coding: utf-8 -*-
''' Test for checking the shear-strength verificacion of a circular RC section.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

import math
import xc
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
from misc import scc3d_testing_bench
from model import predefined_spaces
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

# Materials definition
concr= EHE_materials.HA30
concr.alfacc=1.0
steel= EHE_materials.B500S

# Section geometry
radius= 0.5 # Cross-section radius expressed in meters.
nCover= 0.025+12e-3
rebarDiam= 32e-3 # Bar diameter expressed in meters.
cover= nCover+rebarDiam/2.0 # Concrete cover expressed in meters.
rebarArea= math.pi*(rebarDiam/2.0)**2 # Rebar area expressed in square meters.
section1= def_column_RC_section.RCCircularSection(name='Section1',Rext= radius, concrType=concr, reinfSteelType= steel)
# Longitudinal reinforcement
section1.mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars=24, width= 2*math.pi*(radius-cover), nominalCover= nCover)])

# Shear reinforcement
stirrups= EHE_materials.rebarsEHE['fi12']
shearReinfArea= stirrups['area']
nBranches= 4
section1.shReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh",nShReinfBranches= nBranches, areaShReinfBranch= shearReinfArea, shReinfSpacing= 0.25, angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Create the section material
section1.defRCSection(preprocessor, matDiagType= 'd')

# Create ZeroLength element and assign the section to it.
zlElement, nodA, nodB= scc3d_testing_bench.section_model(preprocessor, section1.name)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode('000_000', nodA.tag)

# Load definition
Myd= 1400e3 # Y bending moment value when checking shear.
Vd= 1090.24e3 # Shear value.
## Load pattern definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nodB.tag, xc.Vector([0.0,0.0,Vd,0,Myd,0.0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem, maxNumIter= 10, convergenceTestTol= 1e-5)
analOk= analysis.analyze(10)

import os
fname= os.path.basename(__file__)
if(analOk<0):
    lmsg.error(fname+' ERROR. Failed to converge.')
    quit()
    
limitState= EHE_limit_state_checking.shearResistance
shearController= limitState.getController()

scc= zlElement.getSection()
shearCF= shearController.checkSection(sct= scc, elementDimension= zlElement.getDimension)

# Check results (see the spreadsheet with the same name in this folder).
theta= shearController.theta
b0= shearController.strutWidth
d= shearController.effectiveDepth
z= shearController.mechanicLeverArm
zRef= 0.671 # No documentary support (check that all runs as usual). 
ratio0= abs(z-zRef)/zRef
Nc= shearController.concreteAxialForce
K= shearController.getKEHE08()
Vu1= shearController.Vu1
Vu1Ref= 3514.20085047592e3  # See spreadsheet with the same name in this folder.
ratio1= (abs(Vu1-Vu1Ref)/Vu1Ref)
thetaFisuras= shearController.thetaFisuras
Vcu= shearController.Vcu
VcuRef= 492.944570034517e3 # See spreadsheet with the same name in this folder.
ratio2= (abs(Vcu-VcuRef)/VcuRef)
Vsu= shearController.Vsu
VsuRef= 616.617223367593e3 # See spreadsheet with the same name in this folder.
ratio3= (abs(Vsu-VsuRef)/VsuRef)
Vu2= shearController.Vu2
Vu= shearController.Vu
VuRef= 1109.56179340211e3 # See spreadsheet with the same name in this folder.
ratio4= (abs(Vu-VuRef)/VuRef)
cf= shearCF[0]

'''
print('Nc= ', Nc/1e3, 'kN')
print("K= ", K)
print("theta= ", math.degrees(theta),'º')
print("b0= ", b0,'m')
print("d= ", d,'m')
print("z= ", z,'m')
print("ratio0= ", ratio0)
print('thetaFisuras= ', math.degrees(thetaFisuras),'º')
print("Vu1= ",Vu1/1e3," kN")
print("ratio1= ", ratio1)
print("Vcu= ",Vcu/1e3," kN")
print("ratio2= ", ratio2)
print("Vsu= ",Vsu/1e3," kN")
print("VsuRef= ",VsuRef/1e3," kN")
print("ratio3= ", ratio3)
print("Vu2= ",Vu2/1e3," kN")
print("Vu= ",Vu/1e3," kN")
print("VuRef= ",VuRef/1e3," kN")
print("ratio4= ",ratio4)
print("CF= ",cf)
'''

if ((abs(ratio0)<1e-2) & (abs(ratio1)<1e-4) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-4) & (abs(ratio4)<1e-4)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Matplotlib output
# section.plot(preprocessor= preprocessor)
    
# # DXF output
# import ezdxf
# doc = ezdxf.new("R2000")
# msp = doc.modelspace()
# section.writeDXF(modelSpace= msp)
# dxfFileName= fname.removesuffix('.py')
# doc.saveas(dxfFileName+'.dxf')
