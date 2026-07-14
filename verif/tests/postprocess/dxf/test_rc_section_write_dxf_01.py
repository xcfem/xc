# -*- coding: utf-8 -*-
''' Test for checking the writeDXF method  of a circular reinforced concrete 
    section.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

import math
import xc
from model import predefined_spaces
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section

from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from postprocess import limit_state_data as lsd
from misc_utils import log_messages as lmsg

# Materials definition
concr= EHE_materials.HA40
concr.alfacc= 1.0 # f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
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

fiberSection= section.defRCSection(preprocessor, matDiagType= 'd')

import os
fname= os.path.basename(__file__)
    
# # Matplotlib output
# section.plot(preprocessor= preprocessor)
    
# DXF output
import ezdxf
doc = ezdxf.new("R2000")
msp = doc.modelspace()
section.writeDXF(modelSpace= msp)
# dxfFileName= fname.removesuffix('.py')+'.dxf'
dxfFileName= '/tmp/'+fname.removesuffix('.py')+'.dxf'
doc.saveas(dxfFileName)

# Check that file exists
testOK= os.path.isfile(dxfFileName)

import os
from misc_utils import log_messages as lmsg
if(testOK) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(dxfFileName) # Clean after yourself.
