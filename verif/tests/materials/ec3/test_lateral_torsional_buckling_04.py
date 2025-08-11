# -*- coding: utf-8 -*-
''' Lateral torsional buckling of steel beams. Test based on the example
    at section 4.3 of the article:

    "Stability Study of Cantilever-Beams – Numerical Analysis and Analytical Calculation (LTB)" Matthias Kraus, Nicolae-Andrei Crișan, Björn Witto
URL: https://d-nb.info/125069664X/34
'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import numpy as np
from materials.ec3 import EC3_materials
from geom_utils import interpolation as intp
from rough_calculations import ng_cantilever as cv

def get_simple_beam_moments(cantileverLength, uniformLoad):
    ''' Applies a simmetry to the bending moments in a cantilever under uniform
        load to obtain the "equivalent" law in a simply supported beam.

    :param cantileverLength: length of the cantilever.
    :param uniformLoad: value of the uniform load.
    '''
    twiceL= 2.0*cantileverLength
    li= np.linspace(0, 1, 11)
    xi= [x*twiceL for x in li] # points along the beam.
    M= list() # bending moments along the beam.
    for x in xi:
        if(x<L):
            xx= x
        else:
            xx= (twiceL-x)
        M.append(cantilever.getBendingMomentUnderUniformLoad(q= qz, x= xx))
    return intp.interpEquidistPoints(xi= xi, yi=M, nDiv=4)
    

# Material.
S355JR= EC3_materials.S355JR
gammaM0= 1.00
S355JR.gammaM= gammaM0 
shape= EC3_materials.IPEShape(S355JR,"IPE_200")
shape.sectionClass= 1

# Compute bending moments along the cantilever.
L= 4.0
cantilever= cv.Cantilever(E= shape.steelType.E, I= shape.Iz(), l= L)

qz= 6e3 # Uniform load.
twiceL= 2*L # length of the "equivalent" simply supported beam.
Mi= get_simple_beam_moments(cantileverLength= L, uniformLoad= qz)

# The value of the critical bending moment obtained in the article
# is 69.93 kN.m greater than the value obtained here (59.17 kN.m), so
# the result is on the safety side.
Mcr= shape.getMcr(L= twiceL, Mi= Mi)
McrRef= 59.16608074865926e3
ratio0= abs(Mcr-McrRef)/McrRef
# The value of the non dimensional beam slenderness obtained in the article
# is 1.05 smaller than the value obtained here (1.15), so
# the result is on the safety side.
overlineLambdaLT= shape.getLateralBucklingNonDimensionalBeamSlenderness(L= twiceL, Mi= Mi)
overlineLambdaLTRef= 1.1504838580042076
ratio1= abs(overlineLambdaLT-overlineLambdaLTRef)/overlineLambdaLTRef

# The value of the lateral torsional buckling reduction factor obtained in
# the article is 0.672 greater than the value obtained here (0.562), so
# the result is on the safety side.
chiLT= shape.getLateralBucklingReductionFactor(L= twiceL, Mi= Mi)
chiLTRef= 0.5620082928939704
ratio2= abs(chiLT-chiLTRef)/chiLTRef

# The value of the bending moment capacity obtained in the article is 52.6 kN.m
# greater than the value obtained here (44.01 kN.m), so the result is on the
# safety side.
MbRd= shape.getLateralTorsionalBucklingResistance(L= twiceL, Mi= Mi)
MbRdRef= 44.0125554414055e3 # Reference value.
ratio3= abs(MbRd-MbRdRef)/MbRdRef

'''
print('Mcr= ', Mcr/1e3, 'kN.m', ratio0)
print('overlineLambdaLT= ', overlineLambdaLT, ratio1)
print('chiLT= ', chiLT, ratio2)
print('MbRd= ', MbRd/1e3, 'kN.m', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio0)<1e-6 and abs(ratio1)<1e-6 and abs(ratio2)<1e-6 and abs(ratio3)<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
