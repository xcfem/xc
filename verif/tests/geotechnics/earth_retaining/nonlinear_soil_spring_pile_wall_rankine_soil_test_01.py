# -*- coding: utf-8 -*-
''' Nonlinear soil spring model inspired in the example 14.2 of the book "Principles of Foundation Engineering" of Braja M. Das. Eight Edition. CENGAGE Learning. 2016.'''

import math
import geom
import xc
from scipy.constants import g
from geotechnics import earth_pressure
from geotechnics.earth_retaining import pile_wall as pw
from materials.sections.fiber_section import def_column_RC_section
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking

# Materials definition
## Soil material
soil= earth_pressure.RankineSoil(phi= math.radians(32), rho= 15.90e3/g, rhoSat= 19.33e3/g)
soil.Kh= 30e6
### Soil strata.
L1= 5.0 #5.0 # Excavation depth (m)
Dteory= 4.7
L= L1+1.3*Dteory # Total lenght (m)
soilLayersDepths= [0.0, L]
soilLayers= [soil, soil]

## Pile material.
concr= EHE_materials.HA30
steel= EHE_materials.B500S
diameter= 450e-3 # Cross-section diameter [m]
pileSection= def_column_RC_section.RCCircularSection(name='test',Rext= diameter/2.0, concrType=concr, reinfSteelType= steel)

## Pile wall.
pileWall= pw.PileWall(pileSection= pileSection, soilLayersDepths= soilLayersDepths, soilLayers= soilLayers, excavationDepths= [L1], pileSpacing= 1.0, waterTableDepth= [None, None])

# Mesh generation
pileWall.genMesh()

# Solve 
reactionCheckTolerance= 1e-6
pileWall.solve(excavationSide= 'left', reactionCheckTolerance= reactionCheckTolerance)

# Get results.
results= pileWall.getResultsDict()

outputTable= pw.get_results_table(resultsDict= results)

# Compute maximum bending moment.
MMin= 6.023e23
MMax= -MMin
for nodeTag in results:
    nodeResults= results[nodeTag]
    depth= nodeResults['depth']
    M= nodeResults['M']
    MMin= min(MMin, M)
    MMax= max(MMax, M)
    
refValue= -212.0886594633984e3
err= abs(max(abs(MMax), abs(MMin))+refValue)/refValue

'''
import os
fname= os.path.basename(__file__)
from tabulate import tabulate
content= tabulate(outputTable, headers= 'firstrow', tablefmt="tsv")
print('\nASCII output:')
print(content)
csvFileName= fname.replace(".py", ".csv")
with open(csvFileName, "w") as csvFile:
    csvFile.write(content)
print('MMax= ', MMax/1e3, 'kN.m')
print('MMin= ', MMin/1e3, 'kN.m')
print('err= ', err)
'''

import os
fname= os.path.basename(__file__)
from misc_utils import log_messages as lmsg
if abs(err)<.05:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# Matplotlib output.
# pw.plot_results(resultsDict= results, title= 'Based on the example 14.2 of the book "Principles of Foundation Engineering" of Braja M. Das.')

# # VTK Graphic output.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(pileWall.modelSpace)
# # oh.displayFEMesh(setsToDisplay= [pileSet])
# #oh.displayLocalAxes()
# #oh.displayLoads()
# oh.displayReactions(reactionCheckTolerance= reactionCheckTolerance)
# oh.displayDispRot('uX', defFScale= 10.0)
# oh.displayIntForcDiag('M')
# oh.displayIntForcDiag('V')
