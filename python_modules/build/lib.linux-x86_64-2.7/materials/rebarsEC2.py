# -*- coding: utf-8 -*-
# rebars as defined in Eurocode 2.

import math

rebarsEC2={}

rebarsEC2['fi04']= {'nmb':'fi04','d':4e-3,'P':0.099}
rebarsEC2['fi05']= {'nmb':'fi05','d':5e-3,'P':0.154}
rebarsEC2['fi06']= {'nmb':'fi06','d':6e-3,'P':0.222}
rebarsEC2['fi07']= {'nmb':'fi07','d':7e-3,'P':0.302}
rebarsEC2['fi08']= {'nmb':'fi08','d':8e-3,'P':0.395}
rebarsEC2['fi10']= {'nmb':'fi10','d':10e-3,'P':0.617}
rebarsEC2['fi12']= {'nmb':'fi12','d':12e-3,'P':0.888}
rebarsEC2['fi14']= {'nmb':'fi14','d':14e-3,'P':1.208}
rebarsEC2['fi16']= {'nmb':'fi16','d':16e-3,'P':1.578}
rebarsEC2['fi20']= {'nmb':'fi20','d':20e-3,'P':2.466}
rebarsEC2['fi25']= {'nmb':'fi25','d':25e-3,'P':3.854}
rebarsEC2['fi32']= {'nmb':'fi32','d':32e-3,'P':6.313}
rebarsEC2['fi40']= {'nmb':'fi40','d':40e-3,'P':9.865}
rebarsEC2['fi50']= {'nmb':'fi50','d':50e-3,'P':15.413}


for item in rebarsEC2:
  bar= rebarsEC2[item]
  bar['r']= bar['d']/2
  bar['area']= math.pi*bar['r']*bar['r']

