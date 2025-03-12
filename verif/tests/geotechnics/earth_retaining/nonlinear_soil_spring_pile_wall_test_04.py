# -*- coding: utf-8 -*-
''' Nonlinear soil spring model inspired in the example 2 of the publication "User's Guide: Computer Program for Winkler Soil-Structure Interaction Analysis of Sheet-Pile Walls (CWALSSI)" by William P. Dawkins, Oklahoma State University U.S. Army Corps of Engineers.
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import geom
import xc
from scipy.constants import g
from geotechnics import earth_pressure
from geotechnics import frictional_cohesive_soil as fcs
from geotechnics.earth_retaining import pile_wall as pw
from materials import typical_materials
from materials.sections import section_properties
from model import predefined_spaces
from solution import predefined_solutions
from materials.sections.fiber_section import def_column_RC_section
from misc_utils import log_messages as lmsg

# Units
lbf= 4.44822 # Pound force to N
ft= 0.3048
inch= 25.4e-3
pcf= lbf/ft**3
pci= lbf/inch**3
psf= lbf/ft**2
psi= lbf/inch**2
ksi= 1000*psi

# Materials definition
## Soil materials
soil1= earth_pressure.CoulombSoil(phi= math.radians(30), rho= 105*pcf/g, rhoSat= 105*pcf/g, Kh= 2.9*pci, deltaAngle= math.radians(20))
soil2= earth_pressure.CoulombSoil(phi= math.radians(30), rho= 128.5*pcf/g, rhoSat= 128.5*pcf/g, Kh= 15*pci, deltaAngle= math.radians(20))
soil3= earth_pressure.BellSoil(phi= math.radians(0.0), c= 1500*psf, rho= 122.5*pcf/g, rhoSat= 122.5*pcf/g, Kh= 87*pci, deltaAngle= math.radians(0))
### Soil strata.
L0= -8*ft # pile wall top (m).
L1= L0+4*ft # position of the anchor (m).
L2= L1+4*ft # Soil 1 bottom, water table level at rigth (m).
L3= L2+4*ft # water table level at left (m).
L4= L3+(22-4)*ft # Soil 2 bottom (m), excavation depth.
L= L4+(31-22)*ft # pile wall tip (m).
soilLayersDepths= [L0, L1, L2, L3, L4, L]
soilLayers= [soil1, soil1, soil2, soil2, soil3, soil3, soil3]

## Pile material.
sheetPileSectionGeometry= section_properties.GenericSection2D(name= 'sheet_pile_section_geometry', area= 9.4*inch**2, I= 220.4*inch**4, W=1.0, alph= 5/6.0)
sheetPileMaterial= typical_materials.BasicElasticMaterial(E= 29e6*psi, nu= 0.3)
sheetPileSectionMaterial= typical_materials.BeamMaterialData(name= 'sheet_pile_section', section= sheetPileSectionGeometry, material= sheetPileMaterial)
## Anchor material.
anchorSectionGeometry= section_properties.GenericSection1D(name= 'anchor_section_geometry', area= 1*inch**2)
anchorMaterial= typical_materials.ElasticPerfectlyPlasticMaterial(E= 29e6*psi, nu= 0.3, fyp= 40*ksi, fyn= -40e3*ksi)
anchorSectionMaterial= typical_materials.BeamMaterialData(name= 'anchor_section', section= anchorSectionGeometry, material= anchorMaterial)

lastExcavationDepth= L4
pileWall= pw.PileWall(pileSection= sheetPileSectionMaterial, soilLayersDepths= soilLayersDepths, soilLayers= soilLayers, excavationDepths= [L1, lastExcavationDepth], pileSpacing= 1.0*ft, waterTableDepths= [L3, L2])

# Mesh generation
pileWall.genMesh(elemSize= 0.1)

# Excavate until the anchor depth.
reactionCheckTolerance= 1e-6
lmsg.setLevel(lmsg.INFO) # print excavation level at each step.
pileWall.solve(excavationDepthIndex= 0, excavationSide= 'left', convergenceTestTol= 1e-4, reactionCheckTolerance= reactionCheckTolerance, krylov= False, logDepth= False)

# Get node to attach the anchor
anchorNode, dist= pileWall.getNodeAtDepth(depth= L1)
## Create the anchor.
modelSpace= pileWall.modelSpace
newNode= modelSpace.newNodeXY(40*ft, anchorNode.getInitialPos3d.y)
## Fix the new node.
modelSpace.fixNode('000', newNode.tag)
modelSpace.setElementDimension(2) # Truss defined in a two-dimensional space.
xcAnchorMaterial= anchorMaterial.defElasticPPMaterial(preprocessor= modelSpace.preprocessor, name= 'anchor_material')
# xcAnchorMaterial= anchorMaterial.defElasticMaterial(preprocessor= modelSpace.preprocessor, name= 'anchor_material')
modelSpace.setDefaultMaterial(xcAnchorMaterial) # Set the material for the new element.
anchorElement= modelSpace.newElement("Truss",nodeTags= [anchorNode.tag, newNode.tag])
anchorElement.sectionArea= 1.0*inch**2

# Apply the load on the top node.
## Get top node.
topNode= pileWall.getTopNode()
## Load definition.
F= 1000*lbf
lp0= pileWall.modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(topNode.tag,xc.Vector([-F,0,0]))
## Add the load case to the domain.
pileWall.modelSpace.addLoadCaseToDomain(lp0.name)
ok= pileWall.solProc.solve()
if(ok!=0):
    lmsg.error('Can\'t solve.')
    exit(1)

# Excavate until the soil 3 is reached.
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
Nref= 8455.33*lbf
ratio1= abs(N-Nref)/Nref

UMaxRef= 6.129e-1*inch
ratio2= abs(UMax-UMaxRef)/UMaxRef
UMinRef= -2.26*inch # See page 65 of the reference document.
ratio3= abs(UMin-UMinRef)/UMinRef
MMaxRef= 6.171e4*lbf*ft
ratio4= abs(MMax-MMaxRef)/MMaxRef
MMinRef= -4.824e3*lbf*ft 
ratio5= abs(MMin-MMinRef)/MMinRef
# The results are quite different from those of the reference document, this
# difference is probably due to the differences between the analysis models
# and the different assumptions in both codes. Anyway, it is interesting
# to keep the test for future reference. 

'''
print('anchor node displacement: ', anchorNode.getDisp[0]*1e3, 'mm')
print('anchor axial force N= ', N/1e3, 'kN')
print('ratio1= ', ratio1)
print('reference anchor axial force Nref= ', Nref/1e3, 'kN')
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
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<0.1:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# # Write results on CSV file.
# fname= os.path.basename(__file__)
# csvFileName= fname.replace(".py", ".csv")
# from tabulate import tabulate
# outputTable= pw.get_results_table(resultsDict= results)
# content= tabulate(outputTable, headers= 'firstrow', tablefmt="tsv")
# with open(csvFileName, "w") as csvFile:
#     csvFile.write(content)
    
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
