# -*- coding: utf-8 -*-
'''Computation of the axial forces envelope on truss elements.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from postprocess import def_vars_control as vc
from misc_utils import log_messages as lmsg
from materials import typical_materials

# Problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

## Define mesh.
nodePairs= list()
for i in range(1,10):
    n0= modelSpace.newNode(0.0, i)
    n1= modelSpace.newNode(5.0, i)
    nodePairs.append((n0, n1))
### Define elements
#### Define materials.
elast= typical_materials.defElasticMaterial(preprocessor, "elast",E= 1e9)
modelSpace.setDefaultMaterial(elast)
modelSpace.setElementDimension(2)
### Define elements.
for (n0, n1) in nodePairs:
    truss= modelSpace.newElement('Truss', [n0.tag, n1.tag])
    truss.sectionArea= 0.1
### Define constraints.
for (n0, n1) in nodePairs:
    modelSpace.fixNode('00', n0.tag)
    modelSpace.fixNode('F0', n1.tag)

# Define load cases.
loadCases= list()
for i in range(0, 5):
    loadCombName= 'ELU0'+str(i)
    loadComb= modelSpace.newLoadPattern(name= loadCombName)
    loadCases.append(loadCombName)
    modelSpace.setCurrentLoadPattern(loadCombName)
    for (n0, n1) in nodePairs:
        pos= n0.getInitialPos3d
        y= pos.y
        if i % 2 == 0:
            loadValue= (pos.y+i+1)*1e3
        else:
            loadValue= -(pos.y+i+1)*0.5e3
        loadComb.newNodalLoad(n1.tag, xc.Vector([loadValue, 0.0]))

xcTotalSet= modelSpace.getTotalSet()
vc.def_vars_envelope_internal_forces_truss_elems(elems= xcTotalSet.elements)
        
for loadCase in loadCases:
    modelSpace.removeAllLoadsAndCombinationsFromDomain()
    modelSpace.revertToStart()
    modelSpace.addLoadCaseToDomain(loadCase)
    result= modelSpace.analyze(calculateNodalReactions= True)
    if(result!=0):
        lmsg.error("Can't solve.")
    vc.update_envelope_internal_forces(xcTotalSet.elements, loadCase)
    
# Check results.
results= dict()
for e in xcTotalSet.elements:
    Nmax= e.getProp('N+')[0]
    Nmin= e.getProp('N-')[0]
    results[e.tag]= {'N+':int(Nmax), 'N-':int(Nmin)}
    
refResults= {0: {'N+': 6000, 'N-': -2500}, 1: {'N+': 7000, 'N-': -3000}, 2: {'N+': 8000, 'N-': -3500}, 3: {'N+': 8999, 'N-': -4000}, 4: {'N+': 10000, 'N-': -4499}, 5: {'N+': 11000, 'N-': -5000}, 6: {'N+': 12000, 'N-': -5500}, 7: {'N+': 13000, 'N-': -6000}, 8: {'N+': 14000, 'N-': -6500}}

testOK= (results==refResults)

import os
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    


# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
oh.displayFEMesh()
oh.displayEnvelopeComponent(envelopeComponent= 'N+')
oh.displayNodeValueDiagram(itemToDisp= 'N+')
# oh.displayLoads()
# oh.displayDispRot(itemToDisp='uY')
