# -*- coding: utf-8 -*-
''' Check buckling computations using the results of a linear buckling 
analysis. Same test as test_buckling_check_06.py but with the Y and Z
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

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Reinforced concrete section.
concr= EHE_materials.HA30
steel= EHE_materials.B500S
## Section geometry.
diameter= 1.25 # Section diameter expressed in meters.
cover= 0.05 # Concrete cover expressed in meters.
rcSection= def_column_RC_section.RCCircularSection(name='test',Rext= diameter/2.0, concrType=concr, reinfSteelType= steel)
## Reinforcement.
rebarDiam= 25e-3
rebarArea= math.pi*(rebarDiam/2.0)**2
numOfRebars= 24
shearRebarsDiam= 16e-3
mechCover= cover-shearRebarsDiam-rebarDiam/2.0
mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars= numOfRebars, width= math.pi*(diameter-2*mechCover), nominalCover= cover+shearRebarsDiam)])
## Put the reinforcement in the section.
rcSection.mainReinf= mainReinf

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
modelSpace.fixNode('000_FF0',bottomNode.tag)
modelSpace.fixNode('00F_FFF',topNode.tag)

## Loads
lp0= modelSpace.newLoadPattern(name= 'lp0')
Nd= -2990e3
MdyTop= 2000e3
MdyBottom= 1970e3
lp0.newNodalLoad(topNode.tag, xc.Vector([0, 0, Nd, 0, MdyTop, 0]))
lp0.newNodalLoad(bottomNode.tag, xc.Vector([0,0,0, 0, MdyBottom, 0]))
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

xcTotalSet= modelSpace.getTotalSet()

avgLeff= 0.0 # Average effective length.
avgMechLambda= 0.0 # Average mechanical slenderness.
avgEf= 0.0 # Average fictitious eccentricity.
results= dict()
for e in xcTotalSet.elements:
    # Critical axial load.
    reinforcementFactorZ= 2 # Circular section table 43.5.1 of EHE-08.
    reinforcementFactorY= 2 # Circular section table 43.5.1
    Cz= 0.2 # table 43.1.2 of EHE-08.
    Cy= 0.2
    Leffi, mechLambdai, Efi= EHE_limit_state_checking.get_buckling_parameters(element= e, rcSection= rcSection, bucklingLoadFactors= bucklingLoadFactors, sectionDepthZ= diameter, Cz= Cz, reinforcementFactorZ= reinforcementFactorZ, sectionDepthY= diameter, Cy= Cy, reinforcementFactorY= reinforcementFactorY)
    avgLeff+= Leffi[0][1] # Effective length for the first mode Y axis.
    avgMechLambda+= mechLambdai[0][1] # Mechanical slenderness for the first mode.
    avgEf+= Efi[0][1] # Fictitious eccentricity for the first mode Y axis.
    z= e.getPosCentroid(False).z
    results[z]= (e.tag, Leffi, mechLambdai, Efi)

sz= len(xcTotalSet.elements)
avgLeff/=sz
ratio1= abs(avgLeff-22.111953551507593)/22.111953551507593
avgMechLambda/=sz
ratio2= abs(avgMechLambda-70.75825136482429)/70.75825136482429
avgEf/=sz
ratio3= abs(avgEf-0.2318153467323909)/0.2318153467323909

'''
import matplotlib.pyplot as plt
LeffZ= dict()
LeffY= dict()
mechLambdaZ= dict()
mechLambdaY= dict()
efiZ= dict()
efiY= dict()
sorted_results= dict(sorted(results.items())) # sort on z.
for i, bucklingLoadFactor in enumerate(bucklingLoadFactors): # iterate through modes.
    mode= i+1
    zi= list()
    LeffZ[mode]= list()
    LeffY[mode]= list()
    mechLambdaZ[mode]= list()
    mechLambdaY[mode]= list()
    efiZ[mode]= list()
    efiY[mode]= list()
    for z in sorted_results: # For each z coordinate.
        zi.append(z)
        values= sorted_results[z]
        eTag= values[0]
        lz= values[1][i][0] # Results for mode i.
        ly= values[1][i][1]
        LeffZ[mode].append(lz) 
        LeffY[mode].append(ly) 
        mlz= values[2][i][0]
        mly= values[2][i][1]
        mechLambdaZ[mode].append(mlz) 
        mechLambdaY[mode].append(mly) 
        ez= values[3][i][0]
        ey= values[3][i][1]
        efiZ[mode].append(ez) 
        efiY[mode].append(ey) 

    # Display results.
    plt.title('effective length for mode: '+str(mode)+' (z axis)')
    plt.ylabel('z')
    plt.xlabel('Leff_z')
    plt.plot(LeffZ[mode], zi)
    plt.show()
    plt.title('effective length for mode: '+str(mode)+' (y axis)')
    plt.ylabel('z')
    plt.xlabel('Leff_y')
    plt.plot(LeffY[mode], zi)
    plt.show()
    plt.title('mechanical slendernes for mode: '+str(mode)+' (z axis)')
    plt.ylabel('z')
    plt.xlabel('mechLambda_z')
    plt.plot(mechLambdaZ[mode], zi)
    plt.show()
    plt.title('mechanical slendernes for mode: '+str(mode)+' (y axis)')
    plt.ylabel('z')
    plt.xlabel('mechLambda_y')
    plt.plot(mechLambdaY[mode], zi)
    plt.show()
    plt.title('additional eccentricity for mode: '+str(mode)+' (z axis)')
    plt.ylabel('z')
    plt.xlabel('efi_z')
    plt.plot(efiZ[mode], zi)
    plt.show()
    plt.title('additional eccentricity for mode: '+str(mode)+' (y axis)')
    plt.ylabel('z')
    plt.xlabel('efi_y')
    plt.plot(efiY[mode], zi)
    plt.show()
        

print('first euler buckling load factor: ', eulerBucklingLoadFactor1)
print('second euler buckling load factor: ', eulerBucklingLoadFactor2)
print('average effective length (first buckling mode): ', avgLeff, 'm', ratio1)
print('average mechanical slenderness (first buckling mode): ', avgMechLambda, ratio2)

print('average fictitious eccentricity (first buckling mode): ', avgEf, 'm', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-3) and (ratio2<1e-3) and (ratio3<1e-3)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayEigenvectors(1)
# oh.displayIntForcDiag('Mz')
# # oh.displayEigenResult(1, defFScale= 10.0)
# # oh.displayLocalAxes()
# # oh.displayReactions()
