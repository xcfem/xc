# -*- coding: utf-8 -*-
''' Trivial test for a 2D fiber section with a TDConcreteMC10 material 
    constitutive model. 

This test is inspired in the example A3 of the book:
Dujmovic, Darko., Androic, Boris., Lukacevic, Ivan. Composite Structures According to Eurocode 4: Worked Examples. Germany: Wiley, 2015.
ISBN 978-3-433-03107-0 
'''

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces
from materials import typical_materials
from materials.ec2 import EC2_materials
from materials.sections.fiber_section import def_simple_RC_section
from solution import predefined_solutions
from misc_utils import log_messages as lmsg

# XXX: This test does not work yet.

# Geometry
b= 2.5 # beam spacing.
hc= 0.12 # deck slab thickness.
Ac= b*hc # Concrete slab area per m width.
ba= 0.18 # Steel beam flange width.

# Materials.
## Concrete
concrete= EC2_materials.EC2Concrete("C25/30", -25e6, 1.5)
concrete.cemType='N'# class N cement
RH= 70 # ambient relative humidity(%)
ts= 0 # drying shrinkage begins at the age 1 day.
t0= 7 # age of concrete at loading time.
beta= 0.4 #
## Reinforcing steel.
rfSteel= EC2_materials.S500B

# Actions
N= 500.688e3 # Axial force.
M= -367.658 # Bending moment.

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
b_eff_mid= 2.125
## At supports.
b_eff_supp= 1.25

# Finite element model.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.StructuralMechanics2D(nodeHandler) # Problem space.

## Material for the deck.
midSpanSection= False
rebarDiam= 12e-3
rebarArea= math.pi*rebarDiam**2/4.0

if(midSpanSection):
    b_eff= b_eff_mid
else:
    b_eff= b_eff_supp

nRebar= int(b_eff/.20)
rebarRow= def_simple_RC_section.ReinfRow(areaRebar= rebarArea, width= b_eff, nRebars= nRebar, nominalCover= .035, nominalLatCover= .035)
deckSection= def_simple_RC_section.RCRectangularSection(name= 'creep_deck_section', width= b_eff, depth= hc, concrType= concrete, reinfSteelType= rfSteel)
deckSection.positvRebarRows= def_simple_RC_section.LongReinfLayers([rebarRow])
deckSection.negatvRebarRows= def_simple_RC_section.LongReinfLayers([rebarRow])
totalReinforcementArea= 2*nRebar*rebarArea
avgReinfStress= N/totalReinforcementArea

### Creep
creepOnDeck= True

if(creepOnDeck):
    concrete.defTDConcreteParameters(beta= beta, cement= '42.5R', h0= h0, T= 21, RH= RH, ts= ts, t0= t0)
    
    deckSection.defRCSection2d(preprocessor, matDiagType= 'td')
    deckSection.pdfReport()
    xcDeckSection= deckSection.fiberSection
else:
    deckSection.defRCSection2d(preprocessor, matDiagType= 'd')
    xcDeckSection= deckSection.fiberSection

# Define mesh.
## Define nodes.
n1= modelSpace.newNode(0, 0)
n2= modelSpace.newNode(0, 0)

## Define constraints.
modelSpace.fixNode000(n1.tag)

## Create element.
modelSpace.setDefaultMaterial(xcDeckSection)
zl= modelSpace.newElement("ZeroLengthSection", [n1.tag, n2.tag])

# Loads.
loads= list()
lp0= modelSpace.newLoadPattern(name= 'lp0', setCurrent= True)
modelSpace.setCurrentLoadPattern(lp0.name)
lp0.newNodalLoad(n2.tag, xc.Vector([N, 0, M]))
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution.
solProc= predefined_solutions.PlainNewtonRaphson(feProblem, convergenceTestTol= 1e-2, printFlag= 1)
solProc.setup()

if(creepOnDeck):
    Tcr = 28 # creep model age (in days)
    modelSpace.setCurrentTime(Tcr)
    # Set the load control integrator with dt=0 so that the domain time doesn’t advance.
    solProc.integrator.dLambda1= 0.0  
    result= solProc.analysis.analyze(1)
    if(result!=0):
        lmsg.error("Can't solve for the initial state.")
        exit(1)
    modelSpace.setCreepOn() # Turn creep on
    
# Compute solution
dt= 2 # time increment in days
solProc.integrator.dLambda1= dt # set new increment for the integrator.
solProc.integrator.setNumIncr(dt) # IMPORTANT! otherwise it got stuck.

lastT= 10000 
t= 0
if(creepOnDeck):
    t= Tcr
while t < lastT:
    print('solve for time: '+str(t)+' days.', flush= True)
    ok = solProc.analysis.analyze(1)
    t+= dt
    if(ok!=0):
        exit(1)
        
# # Output stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# Uncomment to display the mesh
# oh.displayBlocks()
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayReactions()

