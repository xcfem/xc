# -*- coding: utf-8 -*-
''' Pull-out test on V3 reinforced concrete bar. 
From the article «An efficient tension-stiffening model for non-linear
analysis of reinforced concrete members», by Renata S.B. Stramandinoli
and Henriette L. La Rovere.
Test conducted by Rostasy et al.
Uses a bar of 6 m length and cross-section dimensions of
30 cm × 50 cm. The longitudinal steel ratio is 0.67%, and
the material properties are as follows: fct = 1.17 MPa; Ec =
10 GPa; fy = 526 MPa; Es = 197 GPa. 
'''

from __future__ import print_function

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "


import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials.ec2 import EC2_materials
from materials import concrete_base
from materials import typical_materials
from materials.sections.fiber_section import fiber_sets
import numpy as np

#Data from the experiment
width=0.3     #width (cross-section coordinate Y)
depth=0.5     #depth (cross-section coordinate Z)
ro_exp=0.67*1e-2  #longitudinal steel ratio 
fct_exp=1.17*1e6  #
Ec_exp=10*1e9
fy_exp=526e6
Es_exp=197*1e9
Lbar_exp=6   #not considered, 

#Calculated data
As_calc=ro_exp*width*depth   #reinforcement area in the cross-section

#Other data
nDivIJ= 20  #number of cells (fibers) in the IJ direction of the cross section
nDivJK= 20  #number of cells (fibers) in the JK direction of the cross section
l= 1e-7     # Distance between nodes
Flist=np.arange(0e3,650e3,25e3)     # axial force [N]


# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler     #nodes container
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)  #Defines the dimension of nodes  three coordinates (x,y,z) and six DOF for each node (Ux,Uy,theta)

n1= nodes.newNodeXY(1.0,0)
n2= nodes.newNodeXY(1.0+l,0)

# Materials definition
concrAux= EC2_materials.C25           #parameters only for the compression branche 

#Reinforcing steel.
rfSteel=concrete_base.ReinforcingSteel(steelName='rfSteel', fyk=fy_exp, emax=0.08, gammaS=1.15,k=1.05)
rfSteel.Es=Es_exp
steelDiagram= rfSteel.defDiagK(preprocessor) #Definition of steel stress-strain diagram in XC. 


#Parameters for tension stiffening of concrete
paramTS=concrete_base.paramTensStiffness(concrMat=concrAux,reinfMat=rfSteel,reinfRatio=ro_exp,diagType='K')
paramTS.E_c=Ec_exp  #concrete elastic modulus
paramTS.f_ct=fct_exp  #concrete tensile strength 
paramTS.E_ct=Ec_exp #concrete elastic modulus in the tensile linear-elastic range
paramTS.E_s=Es_exp 
paramTS.eps_y=fy_exp/Es_exp

#print('alfa=', paramTS.alfa())

ftdiag=paramTS.pointOnsetCracking()['ft']
ectdiag=paramTS.pointOnsetCracking()['eps_ct']
eydiag=paramTS.eps_y
#Etsdiag=ftdiag/(eydiag-ectdiag)
Etsdiag=paramTS.regresLine()['slope']
#Material for making concrete fibers: concrete02 with tension stiffening
concr= typical_materials.defConcrete02(preprocessor=preprocessor,name='concr',epsc0=concrAux.epsilon0(),fpc=concrAux.fmaxK(),fpcu=0.85*concrAux.fmaxK(),epscu=concrAux.epsilonU(),ratioSlope=0.1,ft=ftdiag,Ets=abs(Etsdiag))


# Section geometry (rectangular 0.3x0.5, 20x20 cells)
geomSectFibers= preprocessor.getMaterialHandler.newSectionGeometry("geomSectFibers")
y1= width/2.0
z1= depth/2.0
#concrete region
regions= geomSectFibers.getRegions
concrSect= regions.newQuadRegion('concr')
concrSect.nDivIJ= nDivIJ
concrSect.nDivJK= nDivJK
concrSect.pMin= geom.Pos2d(-y1,-z1)
concrSect.pMax= geom.Pos2d(+y1,+z1)

#reinforcement bar (1 single rebar, centered in the cross-section )
reinforcement= geomSectFibers.getReinfLayers
reinforcementA= reinforcement.newStraightReinfLayer(rfSteel.nmbDiagK) #Steel stress-strain diagram to use.
reinforcementA.numReinfBars= 1
reinforcementA.barArea= As_calc
reinforcementA.p1= geom.Pos2d(0.0,0.0) # 1 single rebar, centered in the cross-section 


#Section material 
#it is a generic section created to be assigned to the elements specified
#its stress and strain state is neutral (if we ask this section for stress or strain
#values the result is always 0)
materialHandler= preprocessor.getMaterialHandler
sctFibers= materialHandler.newMaterial("fiber_section_3d","sctFibers")
fiberSectionRepr= sctFibers.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(geomSectFibers.name)
sctFibers.setupFibers()

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial='sctFibers'
elements.dimElem= 1 # Dimension of element space
elem= elements.newElement("ZeroLengthSection",xc.ID([n1.tag,n2.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
#newSPConstraint(nodeTag,DOF,value)
spc= constraints.newSPConstraint(n1.tag,0,0.0) # Node 1, ux=0
spc= constraints.newSPConstraint(n1.tag,1,0.0) # Node 1, uy=0
spc= constraints.newSPConstraint(n1.tag,2,0.0) # Node 1, theta=0
spc= constraints.newSPConstraint(n2.tag,1,0.0) # Node 2, uy=0



# Loads definition
loadHandler= preprocessor.getLoadHandler   #loads container

lPatterns= loadHandler.getLoadPatterns

#section of element 1: it's the copy of the material section 'sctFibers' assigned
#to element 1 and specific of this element. It has the tensional state of the element
sccEl1= elem.getSection()         
fibersSccEl1= sccEl1.getFibers()

setsRCEl1= fiber_sets.fiberSectionSetupRCSets(scc=sccEl1,concrMatTag=concr.tag,concrSetName="concrSetFbEl1",reinfMatTag=rfSteel.matTagK,reinfSetName="reinfSetFbEl1")

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
dom= preprocessor.getDomain
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")

strain=list()
stress=list()
areaSec=width*depth
for F in Flist:
    pointLoad=xc.Vector([F,0,0])
    lp0.newNodalLoad(n2.tag,pointLoad)    #applies the point load on node 2 
    #We add the load case to domain.
    lPatterns.addToDomain(lp0.name)           #reads load pattern "0" and adds it to the domain
    # Solve
    #analysis= predefined_solutions.plain_newton_raphson(feProblem)
    analysis= predefined_solutions.plain_static_modified_newton(feProblem)
    analOk= analysis.analyze(1)
    fConcrMin= setsRCEl1.concrFibers.getFiberWithMinStrain()
    epsCMin= fConcrMin.getMaterial().getStrain() # minimum strain among concrete fibers
    strain.append(epsCMin*1E3)
    stress.append(F/areaSec/1e6)
    dom.revertToStart()
    lp0.clearLoads()
    lPatterns.removeFromDomain(lp0.name)

#Test comparison values
strainComp=[0.0, 0.006332344221165989, 0.012664688442331979, 0.018997032663498052, 0.025329376884663957, 0.03166172110582981, 0.037994065326996104, 0.08257991954761966, 0.21277820675132073, 0.34045049853978415, 0.47078740365672234, 0.603629650643874, 0.7320280242585642, 0.8604263978732545, 0.9888247714879446, 1.117223145102635, 1.245621518717326, 1.3740198923320153, 1.5024182659467054, 1.6308166395613952, 1.7761328779774574, 1.905796189052184, 2.0354595001269096, 2.1651228112016354, 2.2947861222763617, 2.4244494333510875]
stressComp=[0.0, 0.16666666666666669, 0.33333333333333337, 0.5, 0.66666666666666674, 0.83333333333333337, 1.0, 1.1666666666666667, 1.3333333333333335, 1.5, 1.6666666666666667, 1.8333333333333335, 2.0, 2.166666666666667, 2.3333333333333335, 2.5, 2.666666666666667, 2.8333333333333335, 3.0, 3.166666666666667, 3.3333333333333335, 3.5, 3.666666666666667, 3.8333333333333335, 4.0, 4.166666666666667]


residStress= (np.array(stress) - np.array(stressComp))
residStrain= (np.array(strain) - np.array(strainComp))

ratio1= np.linalg.norm(residStress)/-concrAux.fmaxK()
ratio2= np.linalg.norm(residStrain)/3.5e-3

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-5) and (ratio2<1e-5)) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

### FIGURES
# import matplotlib.pyplot as plt
# plt.plot(strain,stress)
# plt.show()
