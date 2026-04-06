# -*- coding: utf-8 -*-
''' Verification of the fasteners according to EN 1992-4:2018. '''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
from materials.ec2 import EC2_materials
from postprocess.reports import common_formats as cf

# Data
gammaMs= 1.4 # Partial safety factor for steel.
rebarDiameter= 16e-3 # Bar diameter in meters.
steel= EC2_materials.ReinforcingSteel(steelName="S500B", fyk=500e6, emax=0.05,gammaS= gammaMs, k=1.08)
k1= 8.9 # cast-in headed fasteners (no value for hooked anchors).
concrete= EC2_materials.C30
rebar16= EC2_materials.Rebar(diam= 16e-3, steel= EC2_materials.S500B) 
rebar20= EC2_materials.Rebar(diam= 20e-3, steel= EC2_materials.S500B)

anchorReferences= [403, 394, 432, 428]
anchorRebars= [rebar16, rebar16, rebar20, rebar20]
anchorTd= [16.77e3, 52.92e3, 46.63e3, 57.7e3]
anchorQd= [3.3e3, 3.6e3, 6.18e3, 5.93e3]

h= 1.0 # Concrete element thickness (m).
hef= 0.3 # Effective anchor depth (m).
anchorPosition= geom.Pos2d(0,0) # Anchor position
baseMaterialContour= geom.Polygon2d([geom.Pos2d(-1,-1), geom.Pos2d(1,-1), geom.Pos2d(1,1), geom.Pos2d(-1,1)]) # Contour of concrete element.

# Check steel.
steelCheckResults= list()
for i, (Td, Qd) in enumerate(zip(anchorTd, anchorQd)):
    reference= anchorReferences[i]
    rebar= anchorRebars[i]
    CFTd, FtRd= rebar.getUltimateTensileStrengthEfficiency(Td)
    CFQd, FvRd= rebar.getUltimateShearStrengthEfficiency(Qd)
    CF= rebar.getCombinedShearAndTensionEfficiency(Td= Td, Qd= Qd)
    steelCheckResults.append([i, reference, Td, FtRd, CFTd, Qd, FvRd, CFQd, CF])

# Concrete results.
## Check pull-out resistance.
pullOutResults= list()
for i, Td in enumerate(anchorTd):
    reference= anchorReferences[i]
    rebar= anchorRebars[i]
    steelEfficiency= steelCheckResults[i][4]
    pullOutCf, pullOutResistance= rebar.getPullOutEfficiency(Td= Td, lb= hef, concrete= concrete, eta1= 1.0, steelEfficiency= steelEfficiency, alpha_1= 1.0, alpha_2= 1.0, alpha_3= 1.0, alpha_4= 1.0, alpha_5= 1.0, plainFastener= False)
    pullOutResults.append([i, reference, Td, steelEfficiency, pullOutResistance, pullOutCf])

# Check concrete cone failure resistance.
distToNearestEdge= 0.6
concreteConeResults= list()
for i, Td in enumerate(anchorTd):
    reference= anchorReferences[i]
    rebar= anchorRebars[i]
    NRkc0= rebar.getAxialInitialResistanceConeFailure(k1= k1, concrete= concrete, hEf= hef)
    refProjectedAreaContour= rebar.getReferenceProjectedAreaContour(anchorPosition= anchorPosition, hEf= hef) # contour of the reference projected area, i.e. area of concrete of an individual anchor with large spacing and edge distance at the concrete surface.
    AcN0= refProjectedAreaContour.getArea()
    # Compute the actual projected area, limited by overlapping concrete cones of
    # adjacent fasteners as well as by edges of the concrete member.
    side= rebar.getScrN(hef)
    # Compute the average area for each one of the three fasteners on one side
    # of the base plate.
    halfSide= side/2
    AcNLength= side+0.32
    AcNWidth= halfSide/2+min(halfSide, distToNearestEdge)
    AcN= (AcNLength*AcNWidth)/3
    # Compute the factor taking into account disturbance of the distribution
    # of stresses in the concrete due to the proximity of an edge of the
    # concrete member
    psi_s_N_factor= rebar.getPsi_s_N_factor(c= distToNearestEdge, hEf= hef)
    # Compute the shell spalling factor
    psi_re_N_factor= rebar.getPsi_re_N_factor(hEf= hef)
    NRkc= NRkc0*AcN/AcN0*psi_s_N_factor*psi_re_N_factor
    NRdc= NRkc/1.5
    coneResistanceCF= Td/NRdc
    concreteConeResults.append([i, reference, Td, NRdc, coneResistanceCF])
    

# Check concrete edge failure.
slabThickness= 0.5
concreteEdgeResults= list()
for i, Qd in enumerate(anchorQd):
    reference= anchorReferences[i]
    rebar= anchorRebars[i]
    c1= distToNearestEdge
    VRkc0= rebar.getVRkC0(concrete= concrete, hEf= hef, c1= c1, cracked= True)
    # Compute the reference projected area according to figure 7.13
    # of EN 1992-4.
    AcV0Height= 1.5*min(c1, slabThickness)
    AcV0Width= 2*1.5*c1
    AcV0= AcV0Height*AcV0Width
    AcVHeight= 1.5*min(c1, slabThickness)
    AcVWidth= 2*1.5*c1+0.32
    AcV= AcVHeight*AcVWidth/3 # Three fasteners.
    VRkc= VRkc0*(AcV/AcV0)
    psi_s_v= rebar.getPsi_s_v(c1= c1, c2= 5.0)
    VRkc*= psi_s_v
    psi_h_v= rebar.getPsi_h_v(h= slabThickness, c1= c1)
    VRkc*= psi_h_v
    psi_ec_v= rebar.getPsi_ec_v(ev= 0.0, c1= c1)
    VRkc*= psi_ec_v
    psi_alpha_v= rebar.getPsi_alpha_v(alpha_v= 0.0)
    VRkc*= psi_alpha_v
    psi_re_v= rebar.getPsi_re_v(edgeReinforcement= False)
    VRkc*= psi_re_v
    VRdc= VRkc/1.5
    edgeResistanceCF= Qd/VRdc
    concreteEdgeResults.append([i, reference, Qd, VRdc, edgeResistanceCF])

## Check pry-out resistance.
pryOutResults= list()
for i, Qd in enumerate(anchorQd):
    reference= anchorReferences[i]
    rebar= anchorRebars[i]
    steelEfficiency= steelCheckResults[i][4]
    pryOutCf, pryOutResistance= rebar.getPryOutFailureEfficiency(Qd= Qd, concrete= concrete, k1= k1, hEf= hef, supplementaryReinforcement= False)
    pryOutResults.append([i, reference, Qd, pryOutResistance, pryOutCf])


# Compute nteraction of tension and shear in concrete
## Compute the maximum shear efficiency in concrete
concreteMaxShearEfficiency= list()
for edgeR, pryOutR in zip(concreteEdgeResults, pryOutResults):
    maxConcreteShearCF= max(edgeR[-1], pryOutR[-1])
    concreteMaxShearEfficiency.append(maxConcreteShearCF)
## Compute the maximum tension efficiency in concrete
concreteMaxTensionEfficiency= list()
for pullOutR, coneR in zip(pullOutResults, concreteConeResults):
    maxConcreteTensionCF= max(pullOutR[-1], coneR[-1])
    concreteMaxTensionEfficiency.append(maxConcreteTensionCF)
## Compute the tension and shear interaction efficiency according to equation
## (7.55) in table 7.3 of clause 7.2.3.1 of EN 1992-4.
concreteMaxEfficiency= list()
for tensionCF, shearCF in zip(concreteMaxTensionEfficiency, concreteMaxShearEfficiency):
    concreteCF= math.pow(tensionCF, 1.5)+math.pow(shearCF, 1.5)
    concreteMaxEfficiency.append(concreteCF)

# Check results.
steelCheckResultsRef= [[0, 403, 16770.0, 94411.68878962021, 0.17762631105316828, 3300.0, 43429.3768432253, 0.07598543289977643, 0.2028613693663252], [1, 394, 52920.0, 94411.68878962021, 0.560523815201769, 3600.0, 43429.3768432253, 0.08289319952702882, 0.48326735324257813], [2, 432, 46630.0, 147518.26373378158, 0.316096453549309, 6180.0, 67858.40131753954, 0.09107199521369566, 0.31685517632034493], [3, 428, 57700.0, 147518.26373378158, 0.39113800921713765, 5930.0, 67858.40131753954, 0.08738785301249438, 0.3667721453104499]]
pullOutResultsRef= [[0, 403, 16770.0, 0.17762631105316828, 163909.18192642395, 0.10231275516662494], [1, 394, 52920.0, 0.560523815201769, 81819.17528939707, 0.6467921463742484], [2, 432, 46630.0, 0.316096453549309, 181359.18552278637, 0.2571140792542945], [3, 428, 57700.0, 0.39113800921713765, 146564.62427950653, 0.3936829933119675]]
concreteConeResultsRef= [[0, 403, 16770.0, 57226.68472351378, 0.29304510790766464], [1, 394, 52920.0, 57226.68472351378, 0.924743417440287], [2, 432, 46630.0, 57226.68472351378, 0.8148296590181516], [3, 428, 57700.0, 57226.68472351378, 1.0082708840949464]]
concreteEdgeResultsRef= [[0, 403, 3300.0, 75416.80424699602, 0.04375682625310187], [1, 394, 3600.0, 75416.80424699602, 0.0477347195488384], [2, 432, 6180.0, 78392.9313392036, 0.0788336383705228], [3, 428, 5930.0, 78392.9313392036, 0.07564457532964404]]
pryOutResultsRef= [[0, 403, 3300.0, 337731.2541059829, 0.009771082657823645], [1, 394, 3600.0, 337731.2541059829, 0.010659362899443976], [2, 432, 6180.0, 337731.2541059829, 0.018298572977378826], [3, 428, 5930.0, 337731.2541059829, 0.017558339442695216]]
concreteMaxEfficiencyRef= [0.16778909728719044, 0.8996955923338128, 0.7576641287665256, 1.0332368940571115]

err= 0.0
for row, rowRef in zip(steelCheckResults, steelCheckResultsRef):
    for v, vRef in zip(row, rowRef):
        err+= (v-vRef)**2
for row, rowRef in zip(pullOutResults, pullOutResultsRef):
    for v, vRef in zip(row, rowRef):
        err+= (v-vRef)**2
for row, rowRef in zip(concreteConeResults, concreteConeResultsRef):
    for v, vRef in zip(row, rowRef):
        err+= (v-vRef)**2
for row, rowRef in zip(concreteEdgeResults, concreteEdgeResultsRef):
    for v, vRef in zip(row, rowRef):
        err+= (v-vRef)**2
for row, rowRef in zip(pryOutResults, pryOutResultsRef):
    for v, vRef in zip(row, rowRef):
        err+= (v-vRef)**2
for v, vRef in zip(concreteMaxEfficiency, concreteMaxEfficiencyRef):
    err+= (v-vRef)**2
err= math.sqrt(err)

'''
# Report check steel results.
print('Check steel results.')
for row in steelCheckResults:
    print(row[1], cf.Force.format(row[2]/1e3), cf.Force.format(row[3]/1e3), cf.Factor.format(row[4]), cf.Force.format(row[5]/1e3), cf.Force.format(row[6]/1e3), cf.Factor.format(row[7]), cf.Factor.format(row[8]))
    
# Report concrete cone results.
print('Concrete cone results.')
for row in concreteConeResults:
    print(row[1], cf.Force.format(row[2]/1e3), cf.Force.format(row[3]/1e3), cf.Factor.format(row[4]))
    
# Report pull-out results.
print('Pull-out results.')
for row in pullOutResults:
    print(row[1], cf.Force.format(row[2]/1e3), cf.Factor.format(row[3]), cf.Force.format(row[4]/1e3), cf.Factor.format(row[5]))

# Report concrete edge results.
print('Concrete edge results.')
for row in concreteEdgeResults:
    print(row[1], cf.Force.format(row[2]/1e3), cf.Force.format(row[3]/1e3), cf.Factor.format(row[4]))
    
# Report concrete pry-out results.
print('Concrete pry-out results.')
for row in pryOutResults:
    print(row[1], cf.Force.format(row[2]/1e3), cf.Force.format(row[3]/1e3), cf.Factor.format(row[4]))
    
# Report interaction of tension and shear in concrete.
print('Concrete interaction of tension and shear results.')
for i, concreteCF in enumerate(concreteMaxEfficiency):
    reference= anchorReferences[i]
    print(reference, cf.Factor.format(concreteCF))

print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-6):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')

