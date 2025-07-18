# -*- coding: utf-8 -*-
''' Test for checking the shear-strength verificacion of a circular
    reinforced concrete section under PURE compression.
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
## Section geometry
section.mainReinf= mainReinf
section.shReinf= shearReinf
## Define section material.
section.defRCSection(preprocessor, matDiagType= 'd')
## Create ZeroLength element and assign the section to it.
zlElement, nodA, nodB= scc3d_testing_bench.section_model(preprocessor, section.name)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(nodA.tag)

# Loads definition
Nd= -6000e3 # Axial force when checking shear.
Myd= 0.0 # Y bending moment value when checking shear.
Mzd= 0.0 # Z bending moment value when checking shear.
Vd= 1800e3 # Shear value.

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')

lp0.newNodalLoad(nodB.tag,xc.Vector([Nd,0,Vd,0,Myd,Mzd]))

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

# Check results
# The reference values have been changed the 3/12/2024. The previous ones
# were computed when the error in XC::CircReinfLayer::getReinfBars(void)
# was not fixed yet. As a result those values were not correct.
# On the other hand, fixing that error has revealed another one in
# XC::FiberPtrDeque::getLeverArmSegment when the bending moments are zero.
# The calculation of the lever arm in those cases was wrong. It has been
# fixing using the inertia values of the section to get an approximation
# of the lever arm as the tratidional one: 0.8*depth.
z= shearController.mechanicLeverArm
zRef= 0.683 # No documentary support (check that all runs as usual). 
ratio0= abs(z-zRef)/zRef
Vu1= shearController.Vu1
Vcu= shearController.Vcu
VcuRef= 713.75e3 # No documentary support (check that all runs as usual). 
ratio1= abs(Vcu-VcuRef)/VcuRef
Vsu= shearController.Vsu
VsuRef= 1682.01e3 # No documentary support (check that all runs as usual).
ratio2= abs(Vsu-VsuRef)/VsuRef
VuRef= 2395.75e3 # 2593.07e3 # No documentary support (check that all runs as usual).
Vu2= shearController.Vu2
ratio3= abs(Vu2-VuRef)/VuRef
Vu= shearController.Vu
ratio4= abs(Vu-VuRef)/VuRef

'''
print("\ntheta= ", math.degrees(shearController.theta))
print("Vu1= ",Vu1/1e3," kN")
print("z= ", z,'m', ratio0)
print("Vcu= ",Vcu/1e3," kN VcuRef= ", VcuRef/1e3, ratio1)
print("Vsu= ",Vsu/1e3," kN VsuRef= ", VsuRef/1e3, ratio2)
print("Vu2= ",Vu2/1e3," kN", ratio3)
print("Vu= ",Vu/1e3," kN Vu2Ref= ", VuRef/1e3, ratio4)
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
