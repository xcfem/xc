# -*- coding: utf-8 -*-
'''Computation of the internal forces envelope on 2D elastic beam elements.

Inspired on the example obtained from:
http://ndl.ethernet.edu.et/bitstream/123456789/87977/12/Chapter%20two%20example%201%20.pdf
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from postprocess import def_vars_control as vc
from misc_utils import log_messages as lmsg
from materials import typical_materials
from actions import combinations as combs

# Problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Define problem geometry.
### k-points.
p0= modelSpace.newKPoint(0, 0, 0)
p1= modelSpace.newKPoint(4, 0, 0)

### lines.
l0= modelSpace.newLine(p0, p1)
    
## Define materials.
E= 30e9 # Young modulus (MPa)
h= 0.45 # Beam cross-section depth (m).
b= 0.25 # Beam cross-section width (m).
A= b*h # Beam cross-section area.
I= 1/12*b*h**32 # Inertia of the beam cross-section.
nu= 0.3
G= E/(2*(1+nu))
scc= typical_materials.defElasticShearSection3d(preprocessor, "scc",A= A, E=E, G= G, Iz= I, Iy= I, J= I/10.0, alpha_y= 5/6.0, alpha_z= 5/6.0)

## Define geometric transformations
lin= modelSpace.newLinearCrdTransf("lin", xc.Vector([0, 0, 1]))

## Define mesh.
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(scc)
modelSpace.newSeedElement('ElasticBeam3d')
l0.genMesh(xc.meshDir.I)

## Define constraints.
n0= p0.getNode()
modelSpace.fixNode('000_000', n0.tag)

## Define loads.
P= 1e3

n1= p1.getNode()
loads= list()
## Maximum axial load.
pNp= modelSpace.newLoadPattern(name= 'PN+', setCurrent= True)
pNp.newNodalLoad(n1.tag, xc.Vector([P, 0, 0, 0, 0, 0]))
loads.append(pNp)
## Minimum axial load.
pNm= modelSpace.newLoadPattern(name= 'PN-', setCurrent= True)
pNm.newNodalLoad(n1.tag, xc.Vector([-P, 0, 0, 0, 0, 0]))
loads.append(pNm)
## Maximum y shear.
pVyp= modelSpace.newLoadPattern(name= 'PVy+', setCurrent= True)
pVyp.newNodalLoad(n1.tag, xc.Vector([0, P, 0, 0, 0, 0]))
loads.append(pVyp)
## Minimum y shear.
pVym= modelSpace.newLoadPattern(name= 'PVy-', setCurrent= True)
pVym.newNodalLoad(n1.tag, xc.Vector([0, -P, 0, 0, 0, 0]))
loads.append(pVym)
## Maximum z shear.
pVzp= modelSpace.newLoadPattern(name= 'PVz+', setCurrent= True)
pVzp.newNodalLoad(n1.tag, xc.Vector([0, 0, P, 0, 0, 0]))
loads.append(pVzp)
## Minimum z shear.
pVzm= modelSpace.newLoadPattern(name= 'PVz-', setCurrent= True)
pVzm.newNodalLoad(n1.tag, xc.Vector([0, 0, -P, 0, 0, 0]))
loads.append(pVzm)
## Maximum torque.
pTp= modelSpace.newLoadPattern(name= 'PT+', setCurrent= True)
pTp.newNodalLoad(n1.tag, xc.Vector([0, 0, 0, P, 0, 0]))
loads.append(pTp)
## Minimum torque.
pTm= modelSpace.newLoadPattern(name= 'PT-', setCurrent= True)
pTm.newNodalLoad(n1.tag, xc.Vector([0, 0, 0, -P, 0, 0]))
loads.append(pTm)
               
xcTotalSet= modelSpace.getTotalSet()
vc.def_vars_envelope_internal_forces_beam_elems_3d(elems= xcTotalSet.elements)
for lp in loads:
    modelSpace.addLoadCaseToDomain(lp.name)
    result= modelSpace.analyze(calculateNodalReactions= True)
    if(result!=0):
        errMsg= 'Cannot solve.'
        lmsg.error(errMsg)
        exit(1)
    vc.update_envelope_internal_forces(xcTotalSet.elements, lp.name)
    modelSpace.removeAllLoadsAndCombinationsFromDomain()
    modelSpace.revertToStart

# Store the results in a dictionary.
propertiesToStore= ['My+', 'My-', 'Mz+', 'Mz-', 'N+', 'N-', 'T+', 'T-', 'Vy+', 'Vy-', 'Vz+', 'Vz-', 'comb_My+', 'comb_My-', 'comb_Mz+', 'comb_Mz-', 'comb_N+', 'comb_N-', 'comb_T+', 'comb_T-', 'comb_Vy+', 'comb_Vy-', 'comb_Vz+', 'comb_Vz-']
envelopeDict= dict()
for e in xcTotalSet.elements:
    propValues= dict()
    for propName in propertiesToStore:
        propValues[propName]= e.getProp(propName)
    x= e.getPosCentroid(True).x
    propValues['x']= x
    envelopeDict[e.tag]= propValues

expectedResult= {'comb_My+':'PVz-', 'comb_My-':'PVz+', 'comb_Mz+':'PVy+', 'comb_Mz-':'PVy-', 'comb_N+':'PN+', 'comb_N-':'PN-', 'comb_T+':'PT+', 'comb_T-':'PT-', 'comb_Vy+':'PVy+', 'comb_Vy-':'PVy-', 'comb_Vz+':'PVz+', 'comb_Vz-':'PVz-'}
# Check results.
testOK= True
for eTag in envelopeDict:
    elementEnvelope= envelopeDict[eTag]
    for key in expectedResult:
        valueKey= key.removeprefix('comb_')
        comb0= elementEnvelope[key][0]
        value0= elementEnvelope[valueKey][0]
        comb1= elementEnvelope[key][1]
        value1= elementEnvelope[valueKey][1]
        expectedComb= expectedResult[key]
        # Don't consider zero bending moment values because any of the loads
        # gives a null bending moment at the cantilever end.
        if((valueKey[0]=='M') and (abs(value0)>P/1e6) and (abs(value1)>P/1e6)):
            if(comb0!= expectedComb):
                testOK= False
                # print('failed at: ',eTag, key, 0, comb0, expectedComb, value0)
            if(comb1!= expectedComb):
                testOK= False
                # print('failed at: ',eTag, key, 1, comb1, expectedComb, value1)
import os
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
 

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# oh.displayLocalAxes()
# # oh.displayEnvelopeComponent(envelopeComponent= 'N+')
# # oh.displayNodeValueDiagram(itemToDisp= 'N+')
# # oh.displayEnvelopeComponent(envelopeComponent= 'Mz+')
# # oh.displayNodeValueDiagram(itemToDisp= 'Mz+')
# # oh.displayEnvelopeComponent(envelopeComponent= 'Mz-')
# # oh.displayNodeValueDiagram(itemToDisp= 'Mz-')
# # oh.displayLoads()
# # oh.displayDispRot(itemToDisp='uY')

