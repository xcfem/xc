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
from materials.sections.fiber_section import def_column_RC_section

from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from postprocess import limit_state_data as lsd
from misc_utils import log_messages as lmsg

# Materials definition
concr= EHE_materials.HA40
concr.alfacc= 0.85 # f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
concr.gmmC= 1.0 # See tabla 5.11
steel= EHE_materials.B500S
steel.gammaS= 1.0 # See tabla 5.11

# Section geometry
diameter= 350e-3 # Cross-section diameter [m] (see 5.1.1 geometría de los...)
cover= 0.025 # Cover [m] (see 5.1.2 disposición de la armadura).
section= def_column_RC_section.RCCircularSection(name='test',Rext= diameter/2.0, concrType=concr, reinfSteelType= steel)
section.nDivIJ= 10

# Longitudinal reinforcement
rebarDiam= 16e-3 # See 5.1.2 disposición de la armadura.
rebarArea= math.pi*(rebarDiam/2.0)**2 
numOfRebars= 12 # See 5.1.2 disposición de la armadura.


# Shear reinforcement
shearReinfDiam= 6e-3
shearReinfArea= math.pi*(shearReinfDiam/2.0)**2 
nBranches= 2
shearReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh",nShReinfBranches= nBranches, areaShReinfBranch= shearReinfArea, shReinfSpacing= 0.2, angAlphaShReinf= math.pi/2.0, angThetaConcrStruts= math.pi/4.0)

mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars= numOfRebars, width= math.pi*(diameter-2*cover), nominalCover= cover)])


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
materialHandler= preprocessor.getMaterialHandler

# Section geometry
section.mainReinf= mainReinf
section.shReinf= shearReinf

section.defRCSection(preprocessor, matDiagType= 'd')

zlElement, nodA, nodB= scc3d_testing_bench.section_model(preprocessor, section.name)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(nodA.tag)

# Loads definition
Nd= -550e3 # Axial force when checking shear (last row of the table 5-11).
Myd= 190e3# 223.1e3 # Y bending moment when checking shear. Reduced
          # to achieve convergence.
Mzd= 0.0 # Z bending moment value when checking shear.
Vd= 148.7e3 # Shear value (last row, third column of the table 5-11).

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')

lp0.newNodalLoad(nodB.tag,xc.Vector([Nd,0,Vd,0,Myd,Mzd]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem, maxNumIter= 10)
analOk= analysis.analyze(10)

import os
fname= os.path.basename(__file__)
if(analOk<0):
    lmsg.error(fname+' ERROR. Failed to converge.')
    quit()


limitState= lsd.shearResistance
shearController= limitState.getController(code_limit_state_checking= EHE_limit_state_checking)

secHAParamsTorsion= EHE_limit_state_checking.computeEffectiveHollowSectionParametersRCSection(section)

scc= zlElement.getSection()
shearCF= shearController.checkSection(sct= scc, elementDimension= zlElement.getDimension, torsionParameters= secHAParamsTorsion)

# Check results
z= shearController.mechanicLeverArm
zRef= 204.2e-3 # lever arm (last row, fifth column of the table 5-11).
Vu1= shearController.Vu1
Vcu= shearController.Vcu
VcuRef= 84.2e3+67.2e3 # Sum of the values of the 7th and 8th columns, third row.
Vsu= shearController.Vsu
VsuRef= 25.2e3*400e6/steel.fyd() # (9th column) they don't reduce the stress on shear reinf.
Vu2= shearController.Vu2
Vu= shearController.Vu
VuRef= 176.7e3

ratio0= ((z-zRef)/zRef)
ratio1= ((Vcu-VcuRef)/VcuRef)
ratio2= ((Vsu-VsuRef)/VsuRef)
ratio3= ((Vu-VuRef)/VuRef)

print("\ntheta= ", math.degrees(shearController.theta))
print("Vu1= ",Vu1/1e3," kN")
print("z= ", z,'m')
print("zRef= ", zRef,'m')
print("ratio0= ",ratio0)
print("Vcu= ",Vcu/1e3," kN")
print("VcuRef= ",VcuRef/1e3," kN")
print("ratio1= ",ratio1)
print("Vsu= ",Vsu/1e3," kN")
print("VsuRef= ",VsuRef/1e3," kN")
print("ratio2= ",ratio2)
print("Vu= ",Vu/1e3," kN")
print("VuRef= ",VuRef/1e3," kN")
print("ratio3= ",ratio3)
'''
'''


if ((abs(ratio1)<0.1) & (abs(ratio2)<0.1) & (abs(ratio3)<0.01)):
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
