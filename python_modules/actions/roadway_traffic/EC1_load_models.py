# -*- coding: utf-8 -*-
from __future__ import division

'''Roadway traffic load model definition according to Eurocode 1 part 2.'''

__author__= "Luis C. Pérez Tato (LP) "
__copyright__= "Copyright 2015, LP"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from actions.roadway_traffic import load_model_base as lmb

tandem300LM1= lmb.TandemLoad(axleLoad= 300e3)
tandem200LM1= lmb.TandemLoad(axleLoad= 200e3)
tandem100LM1= lmb.TandemLoad(axleLoad= 100e3)


# pos: (transversal,longitudinal)
EC1_notional_lane1=lmb.LoadModel(wLoads= tandem300LM1.getWheelLoads())
EC1_notional_lane2=lmb.LoadModel(wLoads= tandem200LM1.getWheelLoads())
EC1_notional_lane3=lmb.LoadModel(wLoads= tandem100LM1.getWheelLoads())

#Carro EC1 concomitante con frenado (0.75*Q)
EC1_notional_lane1_brake=lmb.LoadModel(wLoads=tandem300LM1.getWheelLoads(loadFactor= 0.75))
EC1_notional_lane2_brake=lmb.LoadModel(wLoads=tandem200LM1.getWheelLoads(loadFactor= 0.75))

EC1_notional_lane3_brake=lmb.LoadModel(wLoads=tandem100LM1.getWheelLoads(loadFactor= 0.75))

def getAdditionalAmplificationFactor(D:float):
    ''' Return the additional amplification factor according to the expression
        4.7 of the clause 4.6.1 (6) of Eurocode 1 part 2.

    :param D: distance (m) of the cross-section under consideration from the 
              expansion joint.
    '''
    return max(1.3*(1-D/26.0),1)
