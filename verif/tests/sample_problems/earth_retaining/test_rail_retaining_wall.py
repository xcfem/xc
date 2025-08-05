# -*- coding: utf-8 -*-
''' Rail retaining wall test. Test based on a problem solved with the module
called "Muros Pantalla" of the CYPE Ingenieros program.

See: https://info.cype.com/es/tema/muros-pantalla/
'''

import math
import geom
import xc
from scipy.constants import g
from geotechnics import earth_pressure
from geotechnics.earth_retaining import pile_wall as pw
from materials import typical_materials
from materials.sections.structural_shapes import bs_en_13674_1_rail_sections
from materials.en_13674_1 import EN_13674_1_materials
from model import predefined_spaces
from solution import predefined_solutions
from materials.sections.fiber_section import def_column_RC_section
from misc_utils import log_messages as lmsg

def apply_surcharge(pileWall, q, Q):
    ''' Apply the given load at the backfill surface.

    :param q: load to apply.
    :param Q: load at the top node.
    '''
    # Define load pattern.
    lp0= pileWall.modelSpace.newLoadPattern(name= '0')
    # Define loads.
    pileWall.addUniformLoad(lp= lp0, q= q)
    
    # Get top node and load it.
    topNode= pileWall.getTopNode()
    lp0.newNodalLoad(topNode.tag, xc.Vector([Q, 0, 0]))

    # Add the load case to the domain.
    pileWall.modelSpace.addLoadCaseToDomain(lp0.name)
    # Solve.
    if(pileWall.solProc is None):
        pileWall.defineSolutionProcedure(convergenceTestTol= 1e-6, krylov= False)
    ok= pileWall.solProc.solve()
    if(ok!=0):
        lmsg.error('Can\'t solve.')
        exit(1)
    return lp0

# Partial safety factors for loads.
gammaG= 1.35 # partial safety factor for permanent loads.
gammaQ= 1.5 # partial safety factor for variable loads.

# Materials definition
## Soil materials
beta= math.radians(15) # angle of the backfill with the horizontal
## Partial safety factors for soil.
gammaMc= 1.25 # partial safety factor for soil cohesion.
gammaRe= 1.0 # partial safety factor for passive earth resistance.
gammaReFict= gammaRe*gammaG

coluvialSup= earth_pressure.BellSoil(phi= math.radians(15), c= 700*g, beta= beta, rho= 1.8e3, rhoSat= 1.8e3, Kh= 30e6, gammaMc= gammaMc, gammaRe= gammaReFict)
coluvialSup.thickness= 2.5
coluvialInf= earth_pressure.BellSoil(phi= math.radians(28), c= 3e3*g, rho= 2e3, rhoSat= 2e3, Kh= 30e6, gammaMc= gammaMc, gammaRe= gammaReFict)

h_cr=  h0= coluvialSup.getCoulombTensionCrackDepth(sg_v= 0.0, alphaAngle= 0.0) # height of the tension zone, hcr

# Excavation geometry
L= 9.40 # longitud pantalla.
excavationDepth= 1.0
embedmentDepth= L-excavationDepth
waterTableDepths= [None, None] # No water table

### Soil strata.
L1= coluvialSup.thickness # depth coluvial superior.
soilLayersDepths= [0.0, L1, L]
soilLayers= [coluvialSup, coluvialInf, coluvialInf]

## Pile material.
gammaM0= 1.05 # Partial safety factor.
steel= EN_13674_1_materials.R260
steel.gammaM= gammaM0 
pileSection= bs_en_13674_1_rail_sections.RailShape(steel= steel, name= "RAIL_UIC54") # Section geometry.

# Pile wall.
pileSpacing= 0.5
pileWall= pw.PileWall(pileSection= pileSection, soilLayersDepths= soilLayersDepths, soilLayers= soilLayers, excavationDepths= [excavationDepth], pileSpacing= pileSpacing, waterTableDepths= waterTableDepths, designValue= True)

# Mesh generation
pileWall.genMesh()


# Solve
lmsg.setLevel(lmsg.INFO) # print excavation level at each step.
reactionCheckTolerance= 1e-5
pileWall.solve(excavationSide= 'left', convergenceTestTol= 1e-5, reactionCheckTolerance= reactionCheckTolerance, krylov= True, logDepth= False)

# Apply surcharge.
qd= gammaQ/gammaG*1500*g
Q= -pileSpacing*3000.0*g
lp= apply_surcharge(pileWall= pileWall, q= qd, Q= Q)

# Get results.
results= pileWall.getResultsDict()

# Compute maximum bending moment.
MkMin= 6.023e23
MkMax= -MkMin
QkMin= MkMin
QkMax= MkMax
UMax= MkMax
UMin= MkMin
for nodeTag in results:
    nodeResults= results[nodeTag]
    depth= nodeResults['depth']
    M= nodeResults['M']
    MkMin= min(MkMin, M)
    MkMax= max(MkMax, M)
    Q= nodeResults['V']
    QkMin= min(QkMin, Q)
    QkMax= max(QkMax, Q)    
    U= nodeResults['Ux']
    UMin= min(UMin, U)
    UMax= max(UMax, U)

UMaxRef= -65.7295070960316e-3
ratio1= abs(UMax-UMaxRef)/UMaxRef
MkMaxRef= 2.2398731135622656e3
ratio2= abs(MkMax-MkMaxRef)/MkMaxRef
MkMinRef= -54.27597625732579e3
ratio3= abs(MkMin-MkMinRef)/MkMinRef
QkMaxRef= 24.01744718024069e3
ratio4= abs(QkMax-QkMaxRef)/QkMaxRef
QkMinRef= 36.97638444024668e3
ratio5= abs(QkMin-QkMinRef)/QkMinRef

'''
title= 'Rail pile wall test.'
print(title)
print('depths= ', soilLayersDepths)
print('height of the tension zone, h_cr= ',h_cr)
if(waterTableDepths[1]):
    print('hydrostatic pressure gradient: ', abs(waterTableDepths[1]-waterTableDepths[0]))
else:
     print('hydrostatic pressure gradient: ', 0.0)
print('\nUMax= ', UMax*1e3, 'mm')
print('UMin= ', UMin*1e3, 'mm')
print('\nMkMax= ', MkMax/1e3, 'kN.m')
print('MkMin= ', MkMin/1e3, 'kN.m')
print('QkMax= ', QkMax/1e3, 'kN')
print('QkMin= ', QkMin/1e3, 'kN')
print('\nMdMax= ', gammaG*MkMax/1e3, 'kN.m')
print('MdMin= ', gammaG*MkMin/1e3, 'kN.m')
print('QdMax= ', gammaG*QkMax/1e3, 'kN')
print('QdMin= ', gammaG*QkMin/1e3, 'kN')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-15) & (ratio3<0.03)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# Spreadsheet output.
# import os
# fname= os.path.basename(__file__)
# from tabulate import tabulate
# outputTable= pw.get_results_table(resultsDict= results)
# content= tabulate(outputTable, headers= 'firstrow', tablefmt="tsv")
# csvFileName= fname.replace(".py", ".csv")
# with open(csvFileName, "w") as csvFile:
#     csvFile.write(content)
    
# Matplotlib output.
# pw.plot_results(resultsDict= results, title= title)

# # VTK output
# from postprocess import output_handler
# oh= output_handler.OutputHandler(pileWall.modelSpace)
# oh.displayFEMesh()
# oh.displayLoads()
