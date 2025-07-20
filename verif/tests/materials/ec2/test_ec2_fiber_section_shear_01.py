# -*- coding: utf-8 -*-
''' Test for checking the shear-strength verificacion of a reinforced concrete
    section without shear reinforcement.

   Results checked agoinst those of the website:
   https://eurocodeapplied.com/design/en1992/uls-design-rectangular-section
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from materials import concrete_base
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from misc import scc3d_testing_bench
from model import predefined_spaces
from solution import predefined_solutions

width= 1.0 # Cross-section width expressed in meters.
depth= 0.6 # Cross-section depth expressed in meters.
bottomRebarsDiam= 16e-3 # Main reinforcement diameter expressed in meters.
topRebarsDiam= 4e-3 # Top rebars diameter expressed in meters.
bottomCover= 0.074 # Concrete cover of bottom reinforcement expressed in meters.
bottomNominalCover= bottomCover-bottomRebarsDiam/2.0
topCover= 0.068 # Concrete cover of top reinforcement expressed in meters.
topNominalCover= topCover-topRebarsDiam/2.0
numRebars= 6 # Number of rebars.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
concrete= EC2_materials.C25 # Concrete.
steel= EC2_materials.S500B # Reinforcing steel.

# Concrete section.
rcSection= def_simple_RC_section.RCRectangularSection(name='rcSection', width= width, depth= depth, concrType= concrete, reinfSteelType= steel)
## Concrete section reinforcement.
bottomRow= def_simple_RC_section.ReinfRow(rebarsDiam= bottomRebarsDiam, width= width, nRebars= numRebars, nominalCover= bottomNominalCover)
topRow= def_simple_RC_section.ReinfRow(rebarsDiam= topRebarsDiam, width= width, nRebars= numRebars, nominalCover= topNominalCover)
rcSection.positvRebarRows= def_simple_RC_section.LongReinfLayers([bottomRow])
rcSection.negatvRebarRows= def_simple_RC_section.LongReinfLayers([topRow])

# Create FE model.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Create the section material
materialHandler= preprocessor.getMaterialHandler
rcSection.defRCSection(preprocessor, matDiagType= 'd')

# Create ZeroLength element and assign the section to it.
zlElement, nodA, nodB= scc3d_testing_bench.section_model(preprocessor, rcSection.name)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode('000_000', nodA.tag)

# Load definition
Nd= 1975.6311869991625 # Axial load.
Myd= 81969.7049652187 # Y bending moment value when checking shear.
Vd= 188426.60757563822 # Shear value.
## Load pattern definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nodB.tag, xc.Vector([Nd, 0, Vd, 0, Myd, 0]))

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

limitState= EC2_limit_state_checking.shearResistance
shearController= limitState.getController()
shearController.nationalAnnex= 'Spain'

scc= zlElement.getSection()
shearCF= shearController.checkSection(sct= scc, elementDimension= zlElement.getDimension)

bw= shearController.strutWidth
d= shearController.effectiveDepth
dRef= 0.526 # See spreadsheet with the same name in this folder.
ratio0= abs(d-dRef)/dRef
z= shearController.innerLeverArm
cf= shearCF[0]
Vu= shearCF[1]
VuRef= 270.059698625942e3 # See spreadsheet with the same name in this folder.
ratio1= (abs(Vu-VuRef)/VuRef)

'''
print("bw= ", bw,'m')
print("d= ", d,'m')
print("ratio0= ", ratio0)
print("z= ", z,'m')
print("Vu= ", Vu/1e3, 'kN')
print("ratio1= ", ratio1)
print("CF= ", shearCF)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio0<1e-4) and (ratio1<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
