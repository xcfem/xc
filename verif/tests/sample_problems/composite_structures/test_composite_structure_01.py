# -*- coding: utf-8 -*-
'''Composite structure verification test.

Inspired on the example A3 of the book:
Dujmovic, Darko., Androic, Boris., Lukacevic, Ivan. Composite Structures According to Eurocode 4: Worked Examples. Germany: Wiley, 2015.
ISBN 978-3-433-03107-0
'''
from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from scipy.constants import g
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from materials.ec2 import EC2_materials
from materials.ec3 import EC3_materials
from materials.sections.fiber_section import def_simple_RC_section

# Geometry
L= 10.0 # span.
b= 2.5 # beam spacing.
hc= 0.12 # deck slab thickness.

# Materials.
## Steel beams.
steel= EC3_materials.S235JR
steelShape= EC3_materials.IPEShape(steel= steel,name='IPE_400')
ba= steelShape.b() # Flange width.

## Concrete
concrete= EC2_materials.EC2Concrete("C25/30", -25e6, 1.5)
concrete.cemType='N'# class N cement
RH= 70 # ambient relative humidity(%)
ts= 1 # drying shrinkage begins at the age 1 day.
t0= 1 # age of concrete at loading time.
rfSteel= EC2_materials.S500B

# Actions
Ac= 2.5*hc # Concrete slab area per m width.
gk1= Ac*25e3 # Concrete self weight.
gk2= steelShape.getRho()*g # Steel beam self weigth per unit length.
gk3= 3.75e3 # Floor finishes dead load.
q= 3.0e3*2.5 # Imposed floor load, category of use C1

# Creep and shrinkage.
## Determination of final creep coefficient.
### Perimeter of the concrete part which is exposed to drying, u
u= 2*b - ba
h0= 2*Ac/u  # notional size of the member h0.
t_inf= 1e4 # ten thousand days
# t: age of concrete in days at the moment considered.
# t0: age of concrete in days at loading.
# RH: ambient relative humidity(%)
# notional size of the member
phi_inf_t0= concrete.getCreepFitt0(t= t_inf, t0= t0, RH= RH, h0= h0)
## Shrinkage.
### Autogenous shrinkage strain.
Epsca_inf= concrete.getShrEpsca(t= t_inf)
### Basic drying shrinkage strain.
Epscd0= concrete.getShrEpscd0(RH)
### Drying shrinkage strain
Epscd_inf= concrete.getShrEpscd(t= t_inf, ts= ts, RH= RH, h0= h0)
### Total shrinkage 
Epscs_inf= concrete.getShrEpscs(t= t_inf, ts= ts, RH= RH, h0= h0)

# Effective width of the concrete flange
## At mid-span.
L_eff_mid= 0.85*L
b_e1_mid= L_eff_mid/8.0
b_eff_mid= min(2*b_e1_mid, b)
## At support.
L_eff_supp= 0.5*L
b_e1_supp= L_eff_supp/8.0
b_eff_supp= min(2*b_e1_supp, b)

# Finite element model.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics2D(nodeHandler)

# Materials definition
## Material for the steel shape web.
Es= steelShape.steelType.E
quadMaterial= typical_materials.defElasticIsotropic3d(preprocessor, "quadMaterial",E= Es, nu= 0.3, rho= 0.0)
tf= steelShape.tf()
## Material for the flanges of the steel shape.
beamFlangesArea= tf*ba
beamFlangesInertia= 1/12.0*ba*tf**3
beamFlangesSection= typical_materials.defElasticSection2d(preprocessor, "beamFlangesSection", A= beamFlangesArea, E= Es, I= beamFlangesInertia)
## Material for the connectors.
connectorDiameter= 32e-3
connectorArea= math.pi*(connectorDiameter/2.0)**2
connectorInertia= math.pi*(connectorDiameter/2.0)**4/4.0
twoConnectorsSection= typical_materials.defElasticSection2d(preprocessor, "twoConnectorsSection", A= connectorArea, E= Es, I= 2*connectorInertia)

## Material for the deck.
rebarDiam= 12e-3
rebarArea= math.pi*rebarDiam**2/4.0
midSpanNRebar= int(b_eff_mid/.20)
midSpanRebarRow= def_simple_RC_section.ReinfRow(areaRebar= rebarArea, width= b_eff_mid, nRebars= midSpanNRebar, nominalCover= .035, nominalLatCover= .035)
midSpanDeckSection= def_simple_RC_section.RCRectangularSection(name= 'mid_span_creep_deck_section', width= b_eff_mid, depth= hc, concrType= concrete, reinfSteelType= rfSteel)
midSpanDeckSection.positvRebarRows= def_simple_RC_section.LongReinfLayers([midSpanRebarRow])
midSpanDeckSection.negatvRebarRows= def_simple_RC_section.LongReinfLayers([midSpanRebarRow])
supportNRebar= int(b_eff_supp/.20)
supportRebarRow= def_simple_RC_section.ReinfRow(areaRebar= rebarArea, width= b_eff_supp, nRebars= supportNRebar, nominalCover= .035, nominalLatCover= .035)
supportDeckSection= def_simple_RC_section.RCRectangularSection(name= 'support_creep_deck_section', width= b_eff_supp, depth= hc, concrType= concrete, reinfSteelType= rfSteel)
supportDeckSection.positvRebarRows= def_simple_RC_section.LongReinfLayers([supportRebarRow])
supportDeckSection.negatvRebarRows= def_simple_RC_section.LongReinfLayers([supportRebarRow])
### Creep
creepOnDeck= True

if(creepOnDeck):
    concrete.defTDConcreteParameters(beta= 0.4, cement= '42.5R', h0= h0, T= 21, RH= RH, ts= ts, t0= t0)
    
    midSpanDeckSection.defRCSection2d(preprocessor, matDiagType= 'td')
    # midSpanDeckSection.pdfReport()
    xcMidSpanDeckSection= midSpanDeckSection.fiberSection
    supportDeckSection.defRCSection2d(preprocessor, matDiagType= 'td')
    # supportDeckSection.pdfReport()
    xcSupportDeckSection= supportDeckSection.fiberSection
else:
    midSpanDeckSection.defRCSection2d(preprocessor, matDiagType= 'd')
    xcMidSpanDeckSection= midSpanDeckSection.fiberSection
    supportDeckSection.defRCSection2d(preprocessor, matDiagType= 'd')
    xcSupportDeckSection= supportDeckSection.fiberSection

# Create mesh.
## Define points.
websHeight= steelShape.h()-steelShape.tf()
p0= modelSpace.newKPoint(-L/2,0)
p1= modelSpace.newKPoint(0,0)
p2= modelSpace.newKPoint(L,0)
p3= modelSpace.newKPoint(L, websHeight)
p4= modelSpace.newKPoint(0, websHeight)
p5= modelSpace.newKPoint(-L/2, websHeight)
## Create surfaces.
s1= modelSpace.newQuadSurface(p0, p1, p4, p5)
s2= modelSpace.newQuadSurface(p1, p2, p3, p4)
### Set element sizes.
for s in [s1, s2]:
    # s.setElemSizeIJ(0.2, 0.1)
    s.setElemSizeIJ(0.4, 0.2)
# Create mesh.
modelSpace.conciliaNDivs()
## Create web mesh.
modelSpace.setDefaultMaterial(quadMaterial)
modelSpace.newSeedElement('FourNodeQuad')
for s in [s1, s2]:
    s.genMesh(xc.meshDir.I)
## Create flanges mesh.
modelSpace.setDefaultMaterial(beamFlangesSection)
modelSpace.setNumberOfDOFs(3)
transfCooHandler= preprocessor.getTransfCooHandler
lin= transfCooHandler.newLinearCrdTransf2d("lin")
modelSpace.setDefaultCoordTransf(lin)
flange_nodes= list()

def find_flange_nodes(pos):
    ''' Find a node in the same position if it exists, otherwise return None.

    :param pos: position to search the node.
    '''
    retval= None
    for n in flange_nodes:
        nPos= n.getInitialPos2d
        dist= nPos.dist(pos)
        if(dist<1e-3):
            retval= n
            break
    return retval

def new_flange_node(modelSpace, pos):
    ''' If not already done, create a new three DOF node in the given position.

    :param modelSpace: wrapper of the preprocessor of the finite element 
                       problem.
    :param pos: position to search the node.
    '''
    newNode= False
    retval= find_flange_nodes(pos)
    if(not retval):
        retval= modelSpace.newNodeXY(pos.x, pos.y)
        newNode= True
    flange_nodes.append(retval)
    return retval, newNode

flange_elements= list()
for s in [s1, s2]:
    for l in s.getSides:
        lLength= l.getLength()
        if(lLength>steelShape.h()):
            edge= l.getEdge
            nodesOnSide= list()
            for n in edge.nodes:
                pos= n.getInitialPos2d
                newFlangeNode, isNew= new_flange_node(modelSpace, pos)
                nodesOnSide.append((pos.x, newFlangeNode))
                if(isNew):
                    modelSpace.newEqualDOF(n.tag, newFlangeNode.tag, dofs= xc.ID([0, 1]))
            # Create flange elements.
            sorted_by_x= sorted(nodesOnSide, key=lambda tup: tup[0])
            n0=  sorted_by_x[0][1] # First node.
            for tp in sorted_by_x[1:]:
                n1= tp[1]
                elem= modelSpace.newElement("ElasticBeam2d",xc.ID([n0.tag,n1.tag]))
                flange_elements.append(elem)
                n0= n1

# Create connector elements.            
modelSpace.setDefaultMaterial(twoConnectorsSection)
connector_nodes= list()
already_visited= set()
for n in flange_nodes:
    pos= n.getInitialPos2d
    if(abs(pos.y-websHeight)<1e-3):
        nTag= n.tag
        if(not nTag in already_visited):
          newConnectorNode= modelSpace.newNodeXY(pos.x, pos.y+hc/2.0)
          newConnectorElement= modelSpace.newElement("ElasticBeam2d",xc.ID([n.tag,newConnectorNode.tag]))
          connector_nodes.append((pos.x, newConnectorNode))
          already_visited.add(nTag)

# Create deck elements.
deck_elements= list()
modelSpace.setDefaultMaterial(xcMidSpanDeckSection)
sorted_by_x= sorted(connector_nodes, key=lambda tup: tup[0])
n0= sorted_by_x[0][1] # First node.
for tp in sorted_by_x[1:]:
    x= tp[0]
    if((abs(x)<0.15*L) or (abs(x-L)<0.15*L)):
        modelSpace.setDefaultMaterial(xcSupportDeckSection)
    else:
        modelSpace.setDefaultMaterial(xcMidSpanDeckSection)
    n1= tp[1]
    elem= modelSpace.newElement("ForceBeamColumn2d",xc.ID([n0.tag,n1.tag]))
    if(creepOnDeck):
        elem.tol= 1e-12
        # elem.maxSubdivisions= 20
        # elem.subdivideFactor= 20
    deck_elements.append(elem)
    n0= n1

# Constraints.
modelSpace.fixNode("00", p0.getNode().tag)
modelSpace.fixNode("F0", p2.getNode().tag)

# Locate offending elements.
offendingElementTags= [218] # 527, 537, 538, 542, 543, 548, 549, 550, 551, 552, 553, 558, 559, 560, 564, 572, 579, 591, 592, 596, 597, 599]
for tag in offendingElementTags:
    elem= modelSpace.getElement(tag)
    centroid= elem.getPosCentroid(True)
    material= elem.getSection(0)
    print(tag, centroid, material.name)

# Loads.
loads= list()
lg1= modelSpace.newLoadPattern(name= 'G1', setCurrent= True)
for e in deck_elements:
    e.vector2dUniformLoadGlobal(xc.Vector([0, -gk1]))
loads.append(lg1)
lg2= modelSpace.newLoadPattern(name= 'G2', setCurrent= True)
for e in deck_elements:
    e.vector2dUniformLoadGlobal(xc.Vector([0, -gk2]))
loads.append(lg2)
lg3= modelSpace.newLoadPattern(name= 'G3', setCurrent= True)
for e in deck_elements:
    e.vector2dUniformLoadGlobal(xc.Vector([0, -gk3]))
loads.append(lg3)
lq1= modelSpace.newLoadPattern(name= 'Q1', setCurrent= True)
for e in deck_elements:
    e.vector2dUniformLoadGlobal(xc.Vector([0, -q]))
loads.append(lq1)

# Design bending moment for internal support.
## Load combination.
e_d= '1.35*G1+1.35*G2+1.35*G3+1.5*Q1'
modelSpace.addNewLoadCaseToDomain(loadCaseName= 'Ed', loadCaseExpression= e_d, reset= True)

# Compute solution.
if(creepOnDeck):
    solProc= predefined_solutions.PlainKrylovNewton(prb= feProblem, convergenceTestTol= 1e-2, convTestType= 'energy_incr_conv_test', maxNumIter= 50, printFlag= 1)
else:
    solProc= predefined_solutions.PlainNewtonRaphson(prb= feProblem, convergenceTestTol= 1e-3, printFlag= 1)

solProc.setup()
analysis= solProc.analysis

if(creepOnDeck):
    Tcr = 28 # creep model age (in days)
    modelSpace.setCurrentTime(Tcr)
    # Set the load control integrator with dt=0 so that the domain time doesn’t advance.
    solProc.integrator.dLambda1= 0.0  
    result= solProc.analysis.analyze(1)
    modelSpace.setCreepOn() # Turn creep on
    
# Compute solution
dt= 100 # time increment in days
solProc.integrator.dLambda1= dt # set new increment for the integrator.
solProc.integrator.setNumIncr(dt) # IMPORTANT! otherwise it got stuck.

lastT= 10000 
t= 0
if(creepOnDeck):
    t= Tcr
while t < lastT:
    print('solve for time: '+str(t)+' days.')
    ok = solProc.analysis.analyze(1)
    t+= dt
    if(ok!=0):
        exit(1)

# print(t)

print('Loads.')
print('  Concrete self weigth per unit length: gk1= ', gk1/1e3, 'kN/m')
print('  Steel beam self weigth per unit length: gk2= ', gk2/1e3, 'kN/m')
print('  Floor finishes dead load: gk3= ', gk3/1e3, 'kN/m')
print('  Imposed floor load, category of use C1: q= ', q/1e3, 'kN/m')
print('Creep and shrinkage.')
print('  Creep.')
print('    Perimeter of the concrete part which is exposed to drying: u=', u, 'm')
print('    Notional size of the member: h0=', h0, 'm')
print('    Final creep coefficient: phi_inf_t0=', phi_inf_t0)
print('  Shrinkage.')
print('    Autogenous shrinkage strain at t= inf: ', Epsca_inf)
print('    Basic shrinkage strain: ', Epscd0)
print('    Drying shrinkage strain at t= inf: ', Epscd_inf)
print('    Total shrinkage strain at t= inf: ', Epscs_inf)
print('Effective width of the concrete flange.')
print('  At mid-span: b_eff_mid= ', b_eff_mid, ' m')
print('  At mid-span: b_eff_supp= ', b_eff_supp, ' m')

# # Output stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
# Uncomment to display the mesh
# oh.displayBlocks()
# oh.displayFEMesh()
# for load in loads:
#     modelSpace.addLoadCaseToDomain(load.name)
#     oh.displayLoads()
#     modelSpace.removeLoadCaseFromDomain(load.name)
# oh.displayReactions()
oh.displayDispRot(itemToDisp='uY', defFScale= 100.0)
