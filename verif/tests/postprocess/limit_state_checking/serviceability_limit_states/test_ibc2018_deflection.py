# -*- coding: utf-8 -*-
''' Test deflection values returned by the function getDeflectionLimit
    of the ibc_2018_sls module.'''

from serviceability_limit_states import ibc_2018_sls

memberTypes= ['Roof', 'Floor', 'ExteriorWall', 'InteriorWall', 'FarmMember' ,'GreenhouseMember']

roofMemberSubTypes= ['PlasterCeiling', 'NonPlasterCeiling', 'NoCeiling']
wallMemberSubTypes= ['PlasterFinish', 'NonPlasterFinish', 'FlexibleFinish']

deadLoad= [True, False]
liveLoad= [True, False]
snowLoad= [True, False]
windLoad= [True, False]

limits= list()
for mt in memberTypes:
    memberSubTypes= [None]
    if(mt=='Roof'):
        memberSubTypes= roofMemberSubTypes
    elif(mt in ['ExteriorWall', 'InteriorWall']):
        memberSubTypes= wallMemberSubTypes
    for st in memberSubTypes:
        for dl in deadLoad:
            for ll in liveLoad:
                for sl in snowLoad:
                    for wl in windLoad:
                        limit= ibc_2018_sls.getDeflectionLimit(span= 1.0, memberType= mt, memberSubType= st, deadLoad= dl, liveLoad= ll, snowLoad= sl, windLoad= wl)
                        limits.append(int(1.0/limit))
                        
refLimits= [240, 240, 240, 240, 360, 360, 360, 240, 360, 360, 360, 360, 360, 360, 360, 360, 180, 180, 180, 180, 240, 240, 240, 180, 240, 240, 240, 240, 240, 240, 240, 360, 120, 120, 120, 120, 180, 180, 180, 120, 180, 180, 180, 180, 180, 180, 180, 360, 240, 240, 240, 240, 360, 360, 360, 240, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 360, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120]

error= 0
for l1, l2 in zip(limits, refLimits):
    error+= (l1-l2)**2
    
# print(limits)
# print(error)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (error==0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')



