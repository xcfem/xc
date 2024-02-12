# -*- coding: utf-8 -*-
''' Check buckling computations using the results of a linear buckling 
analysis. 3D structural analysis model.
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
MdzTop= 2000e3
MdzBottom= 1970e3
lp0.newNodalLoad(topNode.tag, xc.Vector([0, 0, Nd, MdzTop, 0, 0]))
lp0.newNodalLoad(bottomNode.tag, xc.Vector([0,0,0, MdzBottom, 0, 0]))
modelSpace.addLoadCaseToDomain(lp0.name)

# Perform linear buckling analysis.
linearBucklingAnalysis= predefined_solutions.LinearBucklingAnalysis(prb= feProblem, numModes= 4, constraintHandlerType= 'transformation', numberingMethod= 'rcm', convTestType= "norm_disp_incr_conv_test", convergenceTestTol= 1e-8, maxNumIter= 1000, soeType= "band_gen_lin_soe", solverType= "band_gen_lin_lapack_solver", solutionAlgorithmType= 'krylov_newton_soln_algo', eigenSOEType= "spectra_soe", eigenSolverType= "spectra_solver")
linearBucklingAnalysis.setup()
analOk= linearBucklingAnalysis.solve()

# Get results.
eulerBucklingLoadFactor1= linearBucklingAnalysis.analysis.getEigenvalue(1)
eulerBucklingLoadFactor2= linearBucklingAnalysis.analysis.getEigenvalue(2)
eulerBucklingLoadFactor3= linearBucklingAnalysis.analysis.getEigenvalue(3)

xcTotalSet= modelSpace.getTotalSet()

avgLeff= 0.0 # Average effective length.
avgMechLambda= 0.0 # Average mechanical slenderness.
avgEf= 0.0 # Average fictitious eccentricity.
for e in xcTotalSet.elements:
    N= e.getN()
    Mz1= e.getMz1; Mz2= e.getMz2
    if(Mz1>Mz2):
        ez1, ez2= EHE_limit_state_checking.get_buckling_e1_e2_eccentricities(Nd= Nd, MdMax= Mz1, MdMin= Mz2)
    else:
        ez1, ez2= EHE_limit_state_checking.get_buckling_e1_e2_eccentricities(Nd= Nd, MdMax= Mz2, MdMin= Mz1)
    My1= e.getMy1; My2= e.getMy2
    if(My1>My2):
        ey1, ey2= EHE_limit_state_checking.get_buckling_e1_e2_eccentricities(Nd= Nd, MdMax= My1, MdMin= My2)
    else:
        ey1, ey2= EHE_limit_state_checking.get_buckling_e1_e2_eccentricities(Nd= Nd, MdMax= My2, MdMin= My1)
    nu= rcSection.getNonDimensionalAxialForce(N) # Non-dimensional axial force.

    # Lower slenderness limit.
    lowerSlendernessLimitZ= EHE_limit_state_checking.get_lower_slenderness_limit(C= 0.2, nonDimensionalAxialForce= nu, e1= ez1, e2= ez2, sectionDepth= diameter)
    lowerSlendernessLimitY= EHE_limit_state_checking.get_lower_slenderness_limit(C= 0.2, nonDimensionalAxialForce= nu, e1= ey1, e2= ey2, sectionDepth= diameter)
    # Critical axial load.
    Ncri= [eulerBucklingLoadFactor1*N, eulerBucklingLoadFactor2*N, eulerBucklingLoadFactor3*N]
    # Mechanical properties.
    section= e.physicalProperties.getVectorMaterials[0]
    EIz= section.sectionProperties.EIz()
    EIy= section.sectionProperties.EIy()
    iz= section.sectionProperties.iz # Radius of gyration about z axis.
    iy= section.sectionProperties.iy # Radius of gyration about y axis.
    Leffi= list() # Effective lengths for each mode.
    mechLambdai= list() # Mechanical slenderness for each mode.
    Efi= list() # Fictitious eccentricity for each mode.
    for mode, Ncr in enumerate(Ncri):
        Leffz= math.sqrt((EIz*math.pi**2)/abs(Ncr)) # Effective length.
        Leffy= math.sqrt((EIy*math.pi**2)/abs(Ncr)) # Effective length.
        if(Ncr>0):
            Leffz= -Leffz
            Leffy= -Leffy
        Leffi.append((Leffz, Leffy))
        mechLambdaZ= Leffz/iz # Compute mechanical slenderness
        mechLambdaY= Leffy/iy # Compute mechanical slenderness
        mechLambdai.append((mechLambdaZ, mechLambdaY))
        if(mechLambdaZ<lowerSlendernessLimitZ):
            efz= 0.0
        else:
            reinforcementFactorZ= 2 # Circular section table 43.5.1
            efz= EHE_limit_state_checking.get_fictitious_eccentricity(sectionDepth= diameter, firstOrderEccentricity= ez2, reinforcementFactor= reinforcementFactorZ, epsilon_y= steel.eyd(), radiusOfGyration= iz, bucklingLength= Leffz)
        if(mechLambdaY<lowerSlendernessLimitY):
            efy= 0.0
        else:
            reinforcementFactorY= 2 # Circular section table 43.5.1
            efy= EHE_limit_state_checking.get_fictitious_eccentricity(sectionDepth= diameter, firstOrderEccentricity= ey2, reinforcementFactor= reinforcementFactorY, epsilon_y= steel.eyd(), radiusOfGyration= iy, bucklingLength= Leffy)
        Efi.append((efz, efy))
    avgLeff+= Leffi[0][0] # Effective length for the first mode Z axis.
    avgMechLambda+= mechLambdai[0][0] # Mechanical slenderness for the first mode.
    avgEf+= Efi[0][0] # Fictitious eccentricity for the first mode Z axis.

sz= len(xcTotalSet.elements)
avgLeff/=sz
ratio1= abs(avgLeff-22.111953551507593)/22.111953551507593
avgMechLambda/=sz
ratio2= abs(avgMechLambda-70.75825136482429)/70.75825136482429
avgEf/=sz
ratio3= abs(avgEf-0.20056596377576272)/0.20056596377576272

'''
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
