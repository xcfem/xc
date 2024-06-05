# -*- coding: utf-8 -*-
''' Check buckling computations using the results of a linear buckling 
analysis. Same test as test_buckling_check_08.py but with the Y and Z
axes swapped.
'''

from __future__ import division
from __future__ import print_function

import math
import os
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
from misc_utils import log_messages as lmsg

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Reinforced concrete section.
concr= EHE_materials.HA30
steel= EHE_materials.B500S
## Section geometry.
sectionWidth= 1.4
sectionDepth= 0.45
rcSection= def_simple_RC_section.RCRectangularSection(name= 'flatColumnsRCSection', sectionDescr= 'columns section', concrType= concr, reinfSteelType= steel, width= sectionWidth, depth= sectionDepth, swapReinforcementAxes= True)
## Reinforcement.
firstTopLayer, secondTopLayer, firstBottomLayer, secondBottomLayer= rcSection.defineMainReinforcement(nominalCover= 0.04, fiStirr= 12e-3, topLayersDiameters= [32e-3, None], bottomLayersDiameters= [32e-3, None], lateralLayersDiameters= [25e-3, None], nRebarsHoriz= 6, nRebarsVert= 11)
rcSection.defineShearReinforcementYZ(nShReinfBranchesY= 4, fiStirrY= 12e-3, spacingY= 0.15, nShReinfBranchesZ= 8, fiStirrZ= 12e-3, spacingZ= 0.15)

# Finite element model.
## Problem type
feProblem= xc.FEProblem()
feProblem.title= 'Building columns effective lengths.'
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Problem geometry
H= 20.36 # Buckling length.
p2= modelSpace.newKPoint(0,0,H)
p1= modelSpace.newKPoint(0,0,0)
l1= modelSpace.newLine(p1, p2)
l1.nDiv= 6

## Geometric transformations
corot= modelSpace.newCorotCrdTransf("corot", xzVector= xc.Vector([1,0,0]))
modelSpace.setDefaultCoordTransf(corot)

## Finite element material
xcSection= rcSection.defElasticShearSection3d(preprocessor)
modelSpace.setDefaultMaterial(xcSection)

## Finite element type.
modelSpace.newSeedElement("ElasticBeam3d")

## Mesh generation.
l1.genMesh(xc.meshDir.I)

## Constraints.
bottomNode= p1.getNode()
topNode= p2.getNode()
modelSpace.fixNode('000_FF0',bottomNode.tag) # pinned
modelSpace.fixNode('00F_FFF',topNode.tag) # pinned

## Loads
lp0= modelSpace.newLoadPattern(name= 'lp0')
Nd= -2990e3
MdzTop= 2000e3
MdzBottom= 1970e3
lp0.newNodalLoad(topNode.tag, xc.Vector([0, 0, Nd, MdzTop, 0, 0]))
lp0.newNodalLoad(bottomNode.tag, xc.Vector([0,0,0, MdzBottom, 0, 0]))
modelSpace.addLoadCaseToDomain(lp0.name)

# Perform linear buckling analysis.
linearBucklingAnalysis= predefined_solutions.SpectraLinearBucklingAnalysis(prb= feProblem, numModes= 4)
linearBucklingAnalysis.setup()
analOk= linearBucklingAnalysis.solve()

# Get results.
eulerBucklingLoadFactor1= linearBucklingAnalysis.analysis.getEigenvalue(1)
eulerBucklingLoadFactor2= linearBucklingAnalysis.analysis.getEigenvalue(2)
eulerBucklingLoadFactor3= linearBucklingAnalysis.analysis.getEigenvalue(3)
bucklingLoadFactors= [eulerBucklingLoadFactor1, eulerBucklingLoadFactor2, eulerBucklingLoadFactor3]

# Normalize eigenvectors if needed.
norms= preprocessor.getDomain.getMesh.normalizeEigenvectors()

xcTotalSet= modelSpace.getTotalSet()

# Check direction of eigenvectors for buckling mode 1.
dotProduct= 0.0
for e in xcTotalSet.elements:
    elementNodes= e.nodes
    eigenvector0= e.nodes[0].getEigenvector(1)
    eigenvector1= e.nodes[1].getEigenvector(1)
    dispEigenvector= 0.5*(eigenvector0+eigenvector1)
    dispEigenvector= xc.Vector(list(dispEigenvector)[0:3])
    elementWeakAxis= e.getVDirWeakAxisGlobalCoord(True) # initialGeometry= True
    dotProduct+= elementWeakAxis.dot(dispEigenvector)**2
dotProduct= math.sqrt(dotProduct)
normal= (dotProduct<1e-8) # buckling mode normal to weak axis.

avgLeff_1= 0.0 # Average effective length for the first mode.
avgMechLambda_1= 0.0 # Average mechanical slenderness for the first mode.
avgEf_1= 0.0 # Average fictitious eccentricity for the first mode.
avgStrongAxisBucklingPercent_1= 0.0 # dot product (projection) of the buckling eigenvector over the strong axis.

avgLeff_2= 0.0 # Average effective length for the second mode.
avgMechLambda_2= 0.0 # Average mechanical slenderness for the second mode.
avgEf_2= 0.0 # Average fictitious eccentricity for the second mode.
avgStrongAxisBucklingPercent_2= 0.0 # dot product (projection) of the buckling eigenvector over the strong axis.

results= dict()
for e in xcTotalSet.elements:
    # Critical axial load.
    reinforcementFactorZ= 1 # Rectangular section table 43.5.1 of EHE-08.
    reinforcementFactorY= 1 # Rectangular section table 43.5.1 of EHE-08.
    Cz= 0.24 # table 43.1.2 of EHE-08.
    Cy= 0.24
    sectionDepthZ= sectionWidth
    sectionDepthY= sectionDepth
    Leffi, mechLambdai, Efi, strongAxisBucklingPercent= EHE_limit_state_checking.get_buckling_parameters(element= e, rcSection= rcSection, bucklingLoadFactors= bucklingLoadFactors, sectionDepthZ= sectionDepthZ, Cz= Cz, reinforcementFactorZ= reinforcementFactorZ, sectionDepthY= sectionDepthY, Cy= Cy, reinforcementFactorY= reinforcementFactorY)
    # First mode
    avgLeff_1+= Leffi[0] # Effective length for the first mode Y axis.
    avgMechLambda_1+= mechLambdai[0] # Mechanical slenderness for the first mode.
    avgEf_1+= Efi[0][0] # Fictitious eccentricity for the first mode Y axis.
    avgStrongAxisBucklingPercent_1+= strongAxisBucklingPercent[0] # dot product (projection) of the buckling eigenvector over the strong axis.

    # Second mode.
    avgLeff_2+= Leffi[1] # Effective length for the second mode Y axis.
    avgMechLambda_2+= mechLambdai[1] # Mechanical slenderness for the second mode.
    avgEf_2+= Efi[1][0] # Fictitious eccentricity for the second mode Y axis.
    avgStrongAxisBucklingPercent_2+= strongAxisBucklingPercent[1] # dot product (projection) of the buckling eigenvector over the strong axis.
    
    z= e.getPosCentroid(False).z
    results[z]= (e.tag, Leffi, mechLambdai, Efi, strongAxisBucklingPercent)

sz= len(xcTotalSet.elements)
# First mode values.
avgLeff_1/=sz
ratio1= abs(avgLeff_1-24.562653042759433)/24.562653042759433
avgMechLambda_1/=sz
ratio2= abs(avgMechLambda_1-189.08339128331272)/189.08339128331272
avgEf_1/=sz
ratio3= abs(avgEf_1-1.0332958871738163)/1.0332958871738163
avgStrongAxisBucklingPercent_1/= sz
ratio7= abs(avgStrongAxisBucklingPercent_1)

# Second mode values (torsional buckling).
avgLeff_2/=sz
ratio4= abs(avgLeff_2)
avgMechLambda_2/=sz
ratio5= abs(avgMechLambda_2)
avgEf_2/=sz
ratio6= abs(avgEf_2-0.07)/.07
avgStrongAxisBucklingPercent_2/= sz
ratio8= abs(avgStrongAxisBucklingPercent_2-0.18932198192378955)/0.18932198192378955


'''
print('buckling mode normal to weak axis: ', normal)
print('first euler buckling load factor: ', eulerBucklingLoadFactor1)
print('second euler buckling load factor: ', eulerBucklingLoadFactor2)
print('average effective length (first buckling mode): ', avgLeff_1, 'm, ratio1= ', ratio1)
print('average mechanical slenderness (first buckling mode): ', avgMechLambda_1, ' ratio2= ', ratio2)

print('average fictitious eccentricity (first buckling mode): ', avgEf_1, 'm, ratio3= ', ratio3)
print('average strong axis buckling percentage (first buckling mode): ', avgStrongAxisBucklingPercent_1, ' ratio7= ', ratio7)

print('\naverage effective length (second buckling mode): ', avgLeff_2, 'm, ratio4= ', ratio4)
print('average mechanical slenderness (second buckling mode): ', avgMechLambda_2, ' ratio5= ', ratio5)

print('average fictitious eccentricity (second buckling mode): ', avgEf_2, 'm, ratio6= ', ratio6)
print('average strong axis buckling percentage (second buckling mode): ', avgStrongAxisBucklingPercent_2, ' ratio8= ', ratio8)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(normal and (ratio1<1e-3) and (ratio2<1e-3) and (ratio3<1e-5) and (ratio4<1e-5) and (ratio5<1e-3) and (ratio6<1e-4) and (ratio7<1e-5) and (ratio8<1e-4)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
'''
import matplotlib.pyplot as plt
Leff= dict()
mechLambda= dict()
efi= dict()
sorted_results= dict(sorted(results.items())) # sort on z.
for i, bucklingLoadFactor in enumerate(bucklingLoadFactors): # iterate through modes.
    mode= i+1
    zi= list()
    Leff[mode]= list()
    mechLambda[mode]= list()
    efi[mode]= list()
    for z in sorted_results: # For each z coordinate.
        zi.append(z)
        values= sorted_results[z]
        eTag= values[0]
        lz= values[1][i] # Results for mode i.
        Leff[mode].append(round(lz,1)) 
        ml= values[2][i]
        mechLambda[mode].append(round(ml,0)) 
        ef= values[3][i]
        efi[mode].append(round(ef,3)) 

    # Display results.
    plt.title('effective length for mode: '+str(mode))
    plt.ylabel('z')
    plt.xlabel('Leff')
    plt.plot(Leff[mode], zi)
    plt.show()
    plt.title('mechanical slenderness for mode: '+str(mode))
    plt.ylabel('z')
    plt.xlabel('mechLambda')
    plt.plot(mechLambda[mode], zi)
    plt.show()
    plt.title('additional eccentricity for mode: '+str(mode))
    plt.ylabel('z')
    plt.xlabel('efi')
    plt.plot(efi[mode], zi)
    plt.show()
'''



# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayLoads()
# for mode in [1, 2, 3]:
#     oh.displayEigenvectors(mode= mode)
# oh.displayIntForcDiag('Mz')
# # oh.displayEigenResult(1, defFScale= 10.0)
# # oh.displayLocalAxes()
# # oh.displayReactions()
