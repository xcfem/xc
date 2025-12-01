# -*- coding: utf-8 -*-
'''Check internal forces envelope display.

Inspired on the example obtained from:
http://ndl.ethernet.edu.et/bitstream/123456789/87977/12/Chapter%20two%20example%201%20.pdf
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
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
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Define problem geometry.
### k-points.
kPoints= list()
for x in [0, 6, 12, 18]:
    kPoints.append(modelSpace.newKPoint(x, 0, 0))

### lines.
lines= list()
p0= kPoints[0]
for p1 in kPoints[1:]:
    newLine= modelSpace.newLine(p0, p1)
    newLine.nDiv= 12
    lines.append(newLine)
    p0= p1
    
## Define materials.
E= 30e9 # Young modulus (MPa)
h= 0.45 # Beam cross-section depth (m).
b= 0.25 # Beam cross-section width (m).
A= b*h # Beam cross-section area.
I= 1/12*b*h**32 # Inertia of the beam cross-section.
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)
## Define geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")

## Define mesh.
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(scc)
modelSpace.newSeedElement('ElasticBeam2d')
for l in lines:
    l.genMesh(xc.meshDir.I)

## Define constraints.
for p in kPoints:
    n= p.getNode()
    pos= n.getInitialPos3d
    if(abs(pos.x)<1e-3):
        cDOFs= '00F'
    else:
        cDOFs= 'F0F'
    modelSpace.fixNode(cDOFs, n.tag)

## Define loads.
liveLoad= 29e3
deadLoad= 14.5e3
## Dead load.
g0= modelSpace.newLoadPattern(name= 'G0', setCurrent= True)
deadLoadVector= xc.Vector([0.0, -deadLoad])
for l in lines:
    for e in l.elements:
        e.vector2dUniformLoadGlobal(deadLoadVector)
        
## Live load on the whole beam.      
q1= modelSpace.newLoadPattern(name= 'Q1', setCurrent= True)
liveLoadVector= xc.Vector([0.0, -liveLoad])
for l in lines:
    for e in l.elements:
        e.vector2dUniformLoadGlobal(liveLoadVector)

## Live load on the first and third span.      
q2= modelSpace.newLoadPattern(name= 'Q2', setCurrent= True)
for l in [lines[0], lines[2]]:
    for e in l.elements:
        e.vector2dUniformLoadGlobal(liveLoadVector)
          
## Live load on the second span.      
q3= modelSpace.newLoadPattern(name= 'Q3', setCurrent= True)
for l in [lines[1]]:
    for e in l.elements:
        e.vector2dUniformLoadGlobal(liveLoadVector)
        
## Live load on the first and second spans.      
q4= modelSpace.newLoadPattern(name= 'Q4', setCurrent= True)
# eleLoad= q3.newElementalLoad("beam2d_uniform_load")
for l in [lines[1], lines[2]]:
    for e in l.elements:
        e.vector2dUniformLoadGlobal(liveLoadVector)

## Load combinations
combContainer= combs.CombContainer()
### Ultimate limit state load combinations.
combContainer.SLS.freq.add('combSLS01','1.0*G0+1.0*Q1')
combContainer.SLS.freq.add('combSLS02','1.0*G0+1.0*Q2')
combContainer.SLS.freq.add('combSLS03','1.0*G0+1.0*Q3')
combContainer.SLS.freq.add('combSLS04','1.0*G0+1.0*Q4')

slsFrequentCombinations= combContainer.getCorrespondingLoadCombinations(['sls_frequent'])

xcTotalSet= modelSpace.getTotalSet()
vc.def_vars_envelope_internal_forces_beam_elems_2d(elems= xcTotalSet.elements)
for slsCombs in slsFrequentCombinations:
    for key in slsCombs:
        combExpr= slsCombs[key].expr
        modelSpace.newLoadCombination(key, combExpr)
        modelSpace.addLoadCaseToDomain(key)
        result= modelSpace.analyze(calculateNodalReactions= True)
        if(result!=0):
            errMsg= 'Cannot solve.'
            lmsg.error(errMsg)
            exit(1)
        vc.update_envelope_internal_forces(xcTotalSet.elements, key)
        modelSpace.removeLoadCombination(key)
        modelSpace.revertToStart

# Check results.
MMax= -6.023e23
combMMax= None
MMin= -MMax
combMMin= None
for e in xcTotalSet.elements:
    M_max= max(e.getProp('Mz+')[0], e.getProp('Mz+')[1])
    if(M_max>MMax):
        MMax= M_max
        combMMax= e.getProp('comb_Mz+')
    M_min= min(e.getProp('Mz-')[0], e.getProp('Mz-')[1])
    if(M_min<MMin):
        MMin= M_min
        combMMin= e.getProp('comb_Mz-')

refMMax= 146.8125000000007e3
ratio1= abs(MMax-refMMax)/refMMax
refCombMMax= ['combSLS02', 'combSLS02']
refMMin= -173.99999999999991e3
ratio2= abs(MMin-refMMin)/-refMMin
refCombMMin= ['combSLS04', 'combSLS04']

testOK= (abs(ratio1)<1e-6) and (abs(ratio2)<1e-6) and (combMMax==refCombMMax) and (combMMin==refCombMMin)

'''
print('\nset: ', xcTotalSet.name)
print('  maximum bending moment:', MMax/1e3, 'kN.m; load combination: ', combMMax)
print('  ratio1= ', ratio1)
print('  minimum bending moment:', MMin/1e3, 'kN.m; load combination: ', combMMin)
print('  ratio2= ', ratio2)
'''


fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.png')
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)

# Write graphic file.
# oh.displayEnvelopeComponent(envelopeComponent= 'Mz+')
# oh.displayNodeValueDiagram(itemToDisp= 'Mz+')
# oh.displayEnvelopeComponent(envelopeComponent= 'Mz-')
# oh.displayNodeValueDiagram(itemToDisp= 'Mz-')
oh.displayNodeValueDiagram(itemToDisp= 'Mz+', fileName= outputFileName)
# Check if file exists.
testOK= testOK and os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName)


# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# # oh.displayEnvelopeComponent(envelopeComponent= 'N+')
# # oh.displayNodeValueDiagram(itemToDisp= 'N+')
# oh.displayEnvelopeComponent(envelopeComponent= 'Mz+')
# oh.displayNodeValueDiagram(itemToDisp= 'Mz+')
# oh.displayEnvelopeComponent(envelopeComponent= 'Mz-')
# oh.displayNodeValueDiagram(itemToDisp= 'Mz-')
# # oh.displayLoads()
# # oh.displayDispRot(itemToDisp='uY')

# # Matplotlib stuff.
# ## Sort elements on x coordinate.
# from operator import itemgetter
# import matplotlib.pyplot as plt

# sortedElements= list()
# for e in xcTotalSet.elements:
#     pos= e.getPosCentroid(True)
#     sortedElements.append((e, pos.x))
# sortedElements= sorted(sortedElements, key= itemgetter(1))
# ## Extract envelope values.
# enveloppes= dict()
# for item in ['N+', 'N-', 'Mz+', 'Mz-', 'Vy+', 'Vy-']:
#     xi= list()
#     vi= list()
#     (e0, x0)= sortedElements[0]
#     xn0= e0.getNodes[0].getInitialPos3d.x
#     xi.append(xn0)
#     v0= e0.getProp(item)[0]
#     vi.append(v0)
#     for (e1, x1) in sortedElements[1:]:
#         xn0= e1.getNodes[0].getInitialPos3d.x
#         xi.append(xn0)
#         v0= e1.getProp(item)[0]
#         vi.append(v0)
#     xn1= e1.getNodes[1].getInitialPos3d.x
#     xi.append(xn1)
#     v1= e1.getProp(item)[1]
#     vi.append(v1)
#     enveloppes[item]= (xi, vi)
#
# # Create figures
# ## Markers.
# x_markers= list()
# y_markers= list()
# for p in kPoints:
#     n= p.getNode()
#     pos= n.getInitialPos3d
#     x_markers.append(float(pos.x))
#     y_markers.append(float(0.0))

# for (vmax, vmin) in [('N+', 'N-'), ('Mz+', 'Mz-'), ('Vy+', 'Vy-')]:
#     xi= enveloppes[vmax][0]
#     vi_max= enveloppes[vmax][1]
#     vi_min= enveloppes[vmin][1]
#     fig, ax = plt.subplots()
#     ax.plot(x_markers, y_markers, marker= '^')
#     factor= 1e-3
#     units= '$kN$'
#     if(vmax[0]== 'M'): # if bending moment.
#         ax.yaxis.set_inverted(True)  # inverted axis with autoscaling
#         units= '$kN \cdot m$'
#     factoredValuesMax= [v * factor for v in vi_max]
#     ax.plot(xi, factoredValuesMax, label= vmax)
#     factoredValuesMin= [v * factor for v in vi_min]
#     ax.plot(xi, factoredValuesMin, label= vmin)
#     ax.legend(loc="upper right")

#     ax.set(xlabel='x (m)', ylabel= vmax[0]+' ('+units+')',
#            title='Internal forces enveloppe '+str(vmax)+','+str(vmin))
#     ax.grid()

# # Show graph
# #fig.savefig("test.png")
# plt.show()
