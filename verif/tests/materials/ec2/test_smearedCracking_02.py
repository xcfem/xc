# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

''' Pull-out test on the example 7 reinforced concrete bar. 
From the article «An efficient tension-stiffening model for non-linear
analysis of reinforced concrete members», by Renata S.B. Stramandinoli
and Henriette L. La Rovere.
Test conducted by Hwang and Riskalla, apud Gupta and Maestrini in a study 
of tension members.
Uses a bar of 76.2 m length and cross-section dimensions of
17.8 cm × 30.5 cm. The longitudinal steel ratio is 1.476%, and
the material properties are as follows: fct = 2.62 MPa; Ec =
27.8 GPa; fy = 469 MPa; Es = 199 GPa. 
'''

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials.ehe import EHE_materials
from materials import concrete_base
from materials import typical_materials
from materials.sections.fiber_section import fiber_sets
import matplotlib.pyplot as plt
import numpy as np

#Data from the experiment
width=0.178   #width (cross-section coordinate Y)
depth=0.305   #depth (cross-section coordinate Z)
ro_exp=1.476*1e-2  #longitudinal steel ratio 
fct_exp=2.62*1e6  # concrete strength [Pa]
Ec_exp=27.8*1e9   # concrete elastic modulus [Pa]
fy_exp=469e6      # reinforcing steel yield strength [Pa]
Es_exp=199*1e9    # reinforcing steel elastic modulus [Pa]
Lbar_exp=0.762   #not considered, 

#Calculated data
As_calc=ro_exp*width*depth   #reinforcement area in the cross-section

#Other data
nDivIJ= 20  #number of cells (fibers) in the IJ direction of the cross section
nDivJK= 20  #number of cells (fibers) in the JK direction of the cross section
l= 1e-7     # Distance between nodes
Flist=np.arange(0e3,400e3,10e3)     # axial force [N]


# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler     #nodes container
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)  #Defines the dimension of nodes  three coordinates (x,y,z) and six DOF for each node (Ux,Uy,theta)


nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(1.0,0)
nod= nodes.newNodeXY(1.0+l,0)

# Materials definition
concrAux= EHE_materials.HA25           #parameters only for the compression branche 

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

# print('alfa=', paramTS.alfa())

ftdiag=paramTS.pointOnsetCracking()['ft']
ectdiag=paramTS.pointOnsetCracking()['eps_ct']
eydiag=paramTS.eps_y
Etsdiag=ftdiag/(eydiag-ectdiag)
#Material for making concrete fibers: concrete02 with tension stiffening
concr= typical_materials.defConcrete02(preprocessor=preprocessor,name='concr',epsc0=concrAux.epsilon0(),fpc=concrAux.fmaxK(),fpcu=0.85*concrAux.fmaxK(),epscu=concrAux.epsilonU(),ratioSlope=0.1,ft=ftdiag,Ets=Etsdiag)


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
elements.defaultTag= 1
elem= elements.newElement("ZeroLengthSection",xc.ID([1,2]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
#newSPConstraint(nodeTag,DOF,value)
spc= constraints.newSPConstraint(1,0,0.0) # Node 1, ux=0
spc= constraints.newSPConstraint(1,1,0.0) # Node 1, uy=0
spc= constraints.newSPConstraint(1,2,0.0) # Node 1, theta=0
spc= constraints.newSPConstraint(2,1,0.0) # Node 2, uy=0



# Loads definition
loadHandler= preprocessor.getLoadHandler   #loads container

lPatterns= loadHandler.getLoadPatterns


elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
#section of element 1: it's the copy of the material section 'sctFibers' assigned
#to element 1 and specific of this element. It has the tensional state of the element
sccEl1= ele1.getSection()         
fibersSccEl1= sccEl1.getFibers()

elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
#section of element 1: it's the copy of the material section 'sctFibers' assigned
#to element 1 and specific of this element. It has the tensional state of the element
sccEl1= ele1.getSection()         
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
  lp0.newNodalLoad(2,pointLoad)    #applies the point load on node 2 
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
strainComp=[0.0, 0.0065932062196386605, 0.013186412439277321, 0.01977961865891579, 0.026372824878554642, 0.032966031098193065, 0.03955923731783158, 0.04615244353747067, 0.052745649757109284, 0.05933885597674767, 0.06593206219638613, 0.07252526841602461, 0.07911847463566316, 0.08571168085530234, 0.09230488707494135, 0.13665065609791816, 0.2229427130873582, 0.30835503872027314, 0.39379041892836375, 0.4810193814046467, 0.5646635488321492, 0.6544212595855019, 0.7389893341098774, 0.823557408634254, 0.9081254831586301, 0.9926935576830064, 1.0772616322073816, 1.1726965964252059, 1.2581259567561134, 1.3435553170870205, 1.4289846774179273, 1.5144140377488369, 1.599843398079743, 1.6852727584106504, 1.7707021187415581, 1.8561314790724655, 1.9415608394033725, 2.0269901997342794, 2.1124195600651863, 2.197848920396095]

stressComp=[0.0, 0.1841959845275373, 0.3683919690550746, 0.55258795358261181, 0.73678393811014919, 0.92097992263768647, 1.1051759071652236, 1.289371891692761, 1.4735678762202984, 1.6577638607478358, 1.8419598452753729, 2.0261558298029105, 2.2103518143304473, 2.3945477988579849, 2.578743783385522, 2.7629397679130596, 2.9471357524405968, 3.1313317369681339, 3.3155277214956715, 3.4997237060232087, 3.6839196905507459, 3.8681156750782835, 4.0523116596058211, 4.2365076441333587, 4.4207036286608945, 4.6048996131884321, 4.7890955977159697, 4.9732915822435073, 5.157487566771044, 5.3416835512985816, 5.5258795358261192, 5.710075520353656, 5.8942715048811936, 6.0784674894087312, 6.2626634739362679, 6.4468594584638055, 6.6310554429913431, 6.8152514275188798, 6.9994474120464174, 7.183643396573955]

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

###  FIGURES & REPORTS
# #report concrete material
# from postprocess.reports import report_material

# report_material.report_concrete02(concrDiag=concr,paramTensStiffening=paramTS,grTitle='Test #7. Concrete $\sigma-\epsilon$ curve',grFileName='figures/material/testEx7',texFileName='figures/material/testEx7.tex')

# plt.plot(strain,stress)
# plt.show()
