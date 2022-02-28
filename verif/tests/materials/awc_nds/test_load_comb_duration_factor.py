# -*- coding: utf-8 -*-
''' Test duration factor values returned by the function 
    getLoadCombinationDurationFactor of the AWCNDS_materials module.
'''

from materials.awc_nds import AWCNDS_materials

deadLoad= [True]
liveLoad= [True, False]
snowLoad= [True, False]
windLoad= [True, False]

cdDict= dict()
for dl in deadLoad:
    for ll in liveLoad:
        for sl in snowLoad:
            for wl in windLoad:
                lcString= ''
                if(dl): lcString+= 'D'
                if(ll): lcString+= '+L'
                if(sl): lcString+= '+S'
                if(wl): lcString+= '+W'
                CD= AWCNDS_materials.getLoadCombinationDurationFactor(deadLoad= dl, liveLoad= ll, snowLoad= sl, windLoad= wl)
                cdDict[lcString]= CD

cdRefDict= {'D+L+S+W': 1.6, 'D+L+S': 1.15, 'D+L+W': 1.6, 'D+L': 1.0, 'D+S+W': 1.6, 'D+S': 1.15, 'D+W': 1.6, 'D': 0.9}


ok= (cdDict==cdRefDict)

'''
print(cdDict)
print(ok)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ok==True):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
