# -*- coding: utf-8 -*-
''' Checks the computation of the interaction diagram according to Eurocode 2.
    This test uses the 3D interaction diagram (N, My, Mz).

   Results checked agoin those of the website:
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
from materials.sections.fiber_section import def_simple_RC_section

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

# Create interaction diagram.
feProblem.errFileName= "/tmp/erase.err" # Don't print error messages
                                          # regarding the precission of the
                                          # interaction diagram computation.
feProblem.logFileName= "/tmp/erase.log" # Don't print error messages
                                          # regarding the precission of the
                                          # interaction diagram computation.
interactionDiagram= rcSection.defInteractionDiagram(preprocessor= preprocessor, matDiagType= 'd')
feProblem.errFileName= "cerr" # Display errors if any.
feProblem.logFileName= "clog" # Display warnings if any.

# Compute capacity factor.
N= -639.0932159278339 # Axial force at section top
My= 39874.98268251148 # Y bending moment at section top
Mz= 0.0 # Z bending moment at section top
CF= interactionDiagram.getCapacityFactor(geom.Pos3d(N, My, Mz))

## Results from EurocodeApplied.com
Mu= 268.71e3

# Check results.
CFRef= My/Mu
ratio= abs(CF-CFRef)/CFRef

'''
print("CF= ",CF)
print("CFRef= ",CFRef)
print("ratio= ",ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio)<.05):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
