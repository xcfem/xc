# -*- coding: utf-8 -*-
''' Test for checking the shear-strength verificacion of a circular
    reinforced concrete section under PURE compression.
'''

from __future__ import division
from __future__ import print_function

from misc_utils import log_messages as lmsg
import xc
from misc import scc3d_testing_bench
from solution import predefined_solutions
from model import predefined_spaces
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section

from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking

import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

# Materials definition
concr= EHE_materials.HA40
concr.alfacc= 0.85 # f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
concr.gmmC= 1.0 # See tabla 5.11
steel= EHE_materials.B500S
steel.gammaS= 1.0 # See tabla 5.11

# Section geometry
diameter= 1000e-3 # Cross-section diameter [m]
cover= 0.07 # Cover [m]
section= def_column_RC_section.RCCircularSection(name='test',Rext= diameter/2.0, concrType=concr, reinfSteelType= steel)
section.nDivIJ= 10

# Longitudinal reinforcement
rebarDiam= 20e-3 # See 5.1.2 disposición de la armadura.
rebarArea= math.pi*(rebarDiam/2.0)**2 
numOfRebars= 18 # See 5.1.2 disposición de la armadura.


# Shear reinforcement
shearReinfDiam= 16e-3
shearReinfArea= math.pi*(shearReinfDiam/2.0)**2 
nBranches= 2
shearReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh",nShReinfBranches= nBranches, areaShReinfBranch= shearReinfArea, shReinfSpacing= 0.1, angAlphaShReinf= math.pi/2.0, angThetaConcrStruts= math.pi/4.0)

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
Nd= -6000e3 # Axial force when checking shear (last row of the table 5-11).
Myd= 0.0 # Y bending moment value when checking shear.
Mzd= 0.0 # Z bending moment value when checking shear.
Vd= 1460e3 # Shear value (last row, third column of the table 5-11).

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


shearController= EHE_limit_state_checking.ShearController('ULS_shear')

scc= zlElement.getSection()
shearCF= shearController.checkSection(sct= scc, elementDimension= zlElement.getDimension)

# Check results
# The reference values have been obtained solving the problem for a very
# small bending moment (My= 1 N.m). They are compared here with the results
# computed without bending (see calcVuEHE08SiAt method in ShearController
# controller class.
z= shearController.mechanicLeverArm
zRef= 0.426
ratio0= abs(z-zRef)/zRef
Vu1= shearController.Vu1
Vcu= shearController.Vcu
VcuRef= 693.63e3 # Obtained for Myd= 1 N.m
ratio1= abs(Vcu-VcuRef)/VcuRef
Vsu= shearController.Vsu
VsuRef= 1057.39e3 # Obtained for Myd= 1 N.m
ratio2= abs(Vsu-VsuRef)/VsuRef
VuRef= 1751.01e3 # Obtained for Myd= 1 N.m
Vu2= shearController.Vu2
ratio3= abs(Vu2-VuRef)/VuRef
Vu= shearController.Vu
ratio4= abs(Vu-VuRef)/VuRef

'''
print("\ntheta= ", math.degrees(shearController.theta))
print("Vu1= ",Vu1/1e3," kN")
print("z= ", z,'m', ratio0)
print("Vcu= ",Vcu/1e3," kN", ratio1)
print("Vsu= ",Vsu/1e3," kN", ratio2)
print("Vu= ",Vu2/1e3," kN", ratio3)
print("Vu= ",Vu/1e3," kN", ratio4)
'''

if ((abs(ratio0)<1e-2) & (abs(ratio1)<1e-3) & (abs(ratio2)<1e-3) & (abs(ratio3)<1e-3)):
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
