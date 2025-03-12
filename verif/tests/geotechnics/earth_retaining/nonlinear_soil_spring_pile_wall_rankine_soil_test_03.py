# -*- coding: utf-8 -*-
''' Nonlinear soil spring model inspired in the example 14.1 of the book "Principles of Foundation Engineering" of Braja M. Das. Eight Edition. CENGAGE Learning. 2016.
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from scipy.constants import g
from geotechnics import earth_pressure
from geotechnics.earth_retaining import pile_wall as pw
from materials import typical_materials
from model import predefined_spaces
from solution import predefined_solutions
from materials.sections.fiber_section import def_column_RC_section
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking

# Materials definition
## Soil materials
### Dry soil.
drySoil= earth_pressure.RankineSoil(phi= math.radians(32), rho= 15.90e3/g, rhoSat= 19.33e3/g, Kh= 30e6)
### Wet soil.
wetSoil= earth_pressure.RankineSoil(phi= math.radians(32), rho= 15.90e3/g, rhoSat= 19.33e3/g, Kh= 30e6)
### Soil strata.
L1= 2.0 # Water table depth (m).
L2= L1+3.0 # Dredge line depth (m).
Dtheory= 5.46 # theoretical depth of embedment
L= L2+1.3*Dtheory # Total lenght (m)
soilLayersDepths= [0.0, L1, L2, L]
soilLayers= [drySoil, wetSoil, wetSoil, wetSoil]

## Pile material.
concr= EHE_materials.HA30
steel= EHE_materials.B500S
diameter= 450e-3 # Cross-section diameter [m]
pileSection= def_column_RC_section.RCCircularSection(name='test',Rext= diameter/2.0, concrType=concr, reinfSteelType= steel)

pileWall= pw.PileWall(pileSection= pileSection, soilLayersDepths= soilLayersDepths, soilLayers= soilLayers, excavationDepths= [L2], pileSpacing= 1.0, waterTableDepths= [L1, L1])

# Mesh generation
pileWall.genMesh()

# Solve 
reactionCheckTolerance= 1e-6
pileWall.solve(excavationSide= 'left', reactionCheckTolerance= reactionCheckTolerance)

# Get results.
results= pileWall.getResultsDict()


# Compute maximum bending moment.
MMin= 6.023e23
MMax= -MMin
for nodeTag in results:
    nodeResults= results[nodeTag]
    depth= nodeResults['depth']
    M= nodeResults['M']
    MMin= min(MMin, M)
    MMax= max(MMax, M)
    
refValue= -209.57330322152927e3
err= abs(max(abs(MMax), abs(MMin))+refValue)/refValue

'''
print('\nASCII output:')
outputTable= pw.get_results_table(resultsDict= results)
from tabulate import tabulate
print(tabulate(outputTable, headers= 'firstrow'))
print('MMax= ', MMax/1e3, 'kN.m')
print('MMin= ', MMin/1e3, 'kN.m')
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<.05:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
    
# Matplotlib output.
# pw.plot_results(resultsDict= results, title= 'Based on the example 14.1 of the book "Principles of Foundation Engineering" of Braja M. Das.')

# # VTK Graphic output.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(pileWall.modelSpace)
# # oh.displayBlocks()
# # oh.displayFEMesh(setsToDisplay= [pileSet])
# # oh.displayLocalAxes()
# #oh.displayLoads()
# oh.displayReactions(reactionCheckTolerance= reactionCheckTolerance)
# oh.displayDispRot('uX', defFScale= 10.0)
# oh.displayIntForcDiag('M')
# oh.displayIntForcDiag('V')
