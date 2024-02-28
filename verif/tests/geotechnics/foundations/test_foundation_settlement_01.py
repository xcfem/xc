from geotechnics.foundations import settlement_found as settle


# Example based on settlement calculated for section R2 LAV-VP pág. 91 Anejo Nª 6 - Geotecnia Proyecto de construcción de plataforma de la integración del ferrocarril en la ciudad de Palencia. Tramo: acceso sur, fase II, pag.
#The settlement calculated in the project is very similar to the one obtained here, but not all the variables introduced in their calculation are known

# Data
soilLayers={
    'QT3L':{'zTop':0,'zBottom':3,'E':20e6,'nu':0.30},
    'QT3G':{'zTop':3,'zBottom':5,'E':35e6,'nu':0.25},
    'TDA_alterada': {'zTop':5,'zBottom':7,'E':15e6,'nu':0.30},
    'TDA_sana':{'zTop':7,'zBottom':20,'E':30e6,'nu':0.28},
    } # z are positive depths from the base of the foundation

Hembank=6 # embankment height
gammaEmbank=20e3 # unit weight of the embankment
Wembank= 7.2 # width of the top of the embankment
Wfound=Wembank+2*Hembank
# end data

# average pressure transmitted by the embankment
Lembank=10*Wfound-0.2 # lenght of the embankment
weigth=gammaEmbank*Hembank*(Wembank+2/1/2*Hembank)
press=weigth/Wfound
# settlement
sNotLongFound=settle.settlementFlexibleRectFound(soilLayers,Lembank,Wembank,press)
Lembank=10*Wfound+0.2 # long foundation
sLongFound=settle.settlementFlexibleRectFound(soilLayers,Lembank,Wembank,press) 

ratio1=(sNotLongFound-0.06796912)/sNotLongFound
ratio2=(sLongFound-0.06799527)/sLongFound

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5 and abs(ratio2)<1e-5:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

