# -*- coding: utf-8 -*-
''' Nonlinear soil spring model inspired in the example in the Example 7 of 
the document "Proceedings of the International Workshop on the EVALUATION OF 
EUROCODE 7"  Trinity Colllege Dublin, ISSMGE – ERTC 10 and Geotechnet.

The numerical data are taken from the figura 16 in the page 16 of the PDF.

Concerning the values of the results see the section 6 "Human error" in the same
document.
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
from materials.sections import section_properties
from misc_utils import log_messages as lmsg
from materials.sections.fiber_section import def_column_RC_section
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking

def create_anchor():
    '''Create the anchor.'''
    ## Anchor material.
    anchorEA= 150e6
    anchorE= 200e9
    anchorK= 10e3/1e-3 # 10 kN/mm
    anchorLength= anchorEA/anchorK
    anchorArea= anchorEA/anchorE
    anchorSectionGeometry= section_properties.GenericSection1D(name= 'anchor_section_geometry', area= anchorArea)
    anchorMaterial= typical_materials.ElasticPerfectlyPlasticMaterial(E= anchorE, nu= 0.3, fyp= 275e6, fyn= -275e6)
    anchorSectionMaterial= typical_materials.BeamMaterialData(name= 'anchor_section', section= anchorSectionGeometry, material= anchorMaterial)
    ## Create the anchor.
    modelSpace= pileWall.modelSpace
    newNode= modelSpace.newNodeXY(anchorLength, anchorNode.getInitialPos3d.y)
    ## Fix the new node.
    modelSpace.fixNode('000', newNode.tag)
    modelSpace.setElementDimension(2) # Truss defined in a two-dimensional space.
    xcAnchorMaterial= anchorMaterial.defElasticPPMaterial(preprocessor= modelSpace.preprocessor, name= 'anchor_material')
    # xcAnchorMaterial= anchorMaterial.defElasticMaterial(preprocessor= modelSpace.preprocessor, name= 'anchor_material')
    modelSpace.setDefaultMaterial(xcAnchorMaterial) # Set the material for the new element.
    anchorElement= modelSpace.newElement("Truss",nodeTags= [anchorNode.tag, newNode.tag])
    anchorElement.sectionArea= anchorSectionGeometry.area
    
    ## Solve.
    ok= pileWall.solProc.solve()
    if(ok!=0):
        lmsg.error('Can\'t solve.')
        exit(1)
        
    return anchorElement
    
def apply_surcharge():
    # Apply the load at the backfill surface.
    # Define load pattern.
    lp0= pileWall.modelSpace.newLoadPattern(name= '0')
    # Define loads.
    pileWall.addUniformLoad(lp= lp0, q= surcharge)
    # Add the load case to the domain.
    pileWall.modelSpace.addLoadCaseToDomain(lp0.name)
    # Solve.
    if(pileWall.solProc is None):
        pileWall.defineSolutionProcedure(convergenceTestTol= 1e-5, krylov= False)
    ok= pileWall.solProc.solve()
    if(ok!=0):
        lmsg.error('Can\'t solve.')
        exit(1)
    
# Materials definition
## Soil materials (page 6 of the PDF file).
gravellySand= earth_pressure.CoulombSoil(phi= math.radians(29.3), rho= 18e3/g, rhoSat= 20e3/g, Kh= 26.5e6, deltaAngle= 2/3.0*math.radians(29.3))

### Soil strata.
D= 5.0
L0= 0.0 # pile wall top (m).
L1= L0+1.5 # position of the anchor (m).
L2= L1+0.5 # first excavation step.
L3= L0+8.5 # last excavation step.
L= L3+D # pile wall tip (m).
soilLayersDepths= [L0, L1, L2, L3, L]
soilLayers= [gravellySand, gravellySand, gravellySand, gravellySand, gravellySand, gravellySand]

## Diaphragm section.
diaphragmEI= 100000e3 # N.m2/m
diaphragmE= 200e9
diaphragmI= diaphragmEI/diaphragmE
diaphragmArea= 0.1
diaphragmSectionGeometry= section_properties.GenericSection2D(name= 'diaphragm_section_geometry', area= diaphragmArea, I= diaphragmI, W=1.0, alph= 5/6.0)
diaphragmMaterial= typical_materials.BasicElasticMaterial(E= diaphragmE, nu= 0.3)
diaphragmSectionMaterial= typical_materials.BeamMaterialData(name= 'diaphragm_section', section= diaphragmSectionGeometry, material= diaphragmMaterial)

lastExcavationDepth= L3
pileWall= pw.PileWall(pileSection= diaphragmSectionMaterial, soilLayersDepths= soilLayersDepths, soilLayers= soilLayers, excavationDepths= [L2, lastExcavationDepth], pileSpacing= 1.0, waterTableDepths= [5.0, 4.7], waterUnitWeight= 10e3)

# Mesh generation
pileWall.genMesh(elemSize= 0.2)

# Get node to attach the anchor
anchorNode, dist= pileWall.getNodeAtDepth(depth= L1)

    
# Apply the surcharge.
apply_surcharge()

# Excavate until the anchor depth.
reactionCheckTolerance= 1e-5
lmsg.setLevel(lmsg.INFO) # print excavation level at each step.
pileWall.solve(excavationDepthIndex= 0, excavationSide= 'left', convergenceTestTol= 1e-3, reactionCheckTolerance= reactionCheckTolerance, krylov= False, logDepth= False)


anchorElement= create_anchor()

anchorElement.getResistingForce()
N0= anchorElement.getN()
    
# Perform the rest of the excavation.
reactionCheckTolerance= 1e-5
pileWall.solve(excavationDepthIndex= 1, excavationSide= 'left', reactionCheckTolerance= reactionCheckTolerance, krylov= True, logDepth= False)

# Get results.
results= pileWall.getResultsDict()

# Compute maximum bending moment and displacement.
MMin= 6.023e23
MMax= -MMin
UMax= MMax
UMin= MMin
for nodeTag in results:
    nodeResults= results[nodeTag]
    depth= nodeResults['depth']
    M= nodeResults['M']
    MMin= min(MMin, M)
    MMax= max(MMax, M)
    U= nodeResults['Ux']
    UMin= min(UMin, U)
    UMax= max(UMax, U)

# Compare results.
anchorElement.getResistingForce()
N= anchorElement.getN()

UMaxRef= 5e-3
ratio2= abs(UMax-UMaxRef)/UMaxRef
UMinRef= -51.96e-3 # See page 65 of the reference document.
ratio3= abs(UMin-UMinRef)/UMinRef
MMaxRef= 257.9e3
ratio4= abs(MMax-MMaxRef)/MMaxRef
MMinRef= 1.0
ratio5= abs(MMin-MMinRef)/MMinRef


'''
import os
fname= os.path.basename(__file__)
csvFileName= fname.replace(".py", ".csv")
from tabulate import tabulate
outputTable= pw.get_results_table(resultsDict= results)
content= tabulate(outputTable, headers= 'firstrow', tablefmt="tsv")
with open(csvFileName, "w") as csvFile:
    csvFile.write(content)
'''
print('\ndiaphragm depth: ', L, 'm')
print('anchor node displacement: ', anchorNode.getDisp[0]*1e3, 'mm')
print('anchor axial force N= ', N/1e3, 'kN')
print('\nUMax= ', UMax*1e3, 'mm')
print('UMaxRef= ', UMaxRef*1e3, 'mm')
print('ratio2= ', ratio2)
print('UMin= ', UMin*1e3, 'mm')
print('UMinRef= ', UMinRef*1e3, 'mm')
print('ratio3= ', ratio3)
print('\nMMax= ', MMax/1e3, 'kN.m')
print('MMaxRef= ', MMaxRef/1e3, 'kN.m')
print('ratio4= ', ratio4)
print('MMin= ', MMin/1e3, 'kN.m')
print('MMinRef= ', MMinRef/1e3, 'kN.m')
print('ratio5= ', ratio5)

# Matplotlib output.
# pw.plot_results(resultsDict= results, title= 'Test based on the example 2 of the CWALSSI program manual.')

# # VTK Graphic output.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(pileWall.modelSpace)
# oh.displayFEMesh()
# # oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayReactions(reactionCheckTolerance= reactionCheckTolerance)
# oh.displayDispRot('uX', defFScale= 10.0)
# oh.displayIntForcDiag('M')
# # oh.displayIntForcDiag('V')
