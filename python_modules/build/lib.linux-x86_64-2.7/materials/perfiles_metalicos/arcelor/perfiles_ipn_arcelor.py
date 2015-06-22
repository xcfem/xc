# -*- coding: utf-8 -*-
# Section axis:


#    ARCELOR          XC

#                     ^ Y                    
#                     |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----

#       |
#       v Z

# Los ejes en los que se definen las características mecánicas de la sección
# en el catálogo original de Arcelor se han cambiado para acomodarlos a los
# habituales en XC (eje fuerte paralelo al eje Z). Para ello, como puede verse
# en la figura, basta intercambiar los ejes Y y Z.


# XXX Repasar coeficientes de distorsión
#   alpha-> alphaY,alphaZ}

perfilesIPN={}

perfilesIPN['IPN_80']= {'nmb':'IPN_80','P':5.9,'h':80e-3,'b':42e-3,'tw':3.9e-3,'tf':5.9e-3,'r1':3.9e-3,'r2':2.3e-3,'A':7.58e-4,'d':59e-3,'FI':'-','Pmin':0e-3,'Pmax':0e-3,'AL':0.304,'AG':51.09,'Iz':77.8e-8,'Wzel':19.5e-6,'Wzpl':22.8e-6,'iz':3.2e-2,'Avy':3.41e-4,'Iy':6.29e-8,'Wyel':3e-6,'Wypl':5e-6,'iy':0.91e-2,'Ss':21.6e-3,'It':0.87e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_100']= {'nmb':'IPN_100','P':8.3,'h':100e-3,'b':50e-3,'tw':4.5e-3,'tf':6.8e-3,'r1':4.5e-3,'r2':2.7e-3,'A':10.6e-4,'d':75.7e-3,'FI':'-','Pmin':0e-3,'Pmax':0e-3,'AL':0.37,'AG':44.47,'Iz':171e-8,'Wzel':34.2e-6,'Wzpl':39.8e-6,'iz':4.01e-2,'Avy':4.85e-4,'Iy':12.2e-8,'Wyel':4.88e-6,'Wypl':8.1e-6,'iy':1.07e-2,'Ss':25e-3,'It':1.6e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_120']= {'nmb':'IPN_120','P':11.1,'h':120e-3,'b':58e-3,'tw':5.1e-3,'tf':7.7e-3,'r1':5.1e-3,'r2':3.1e-3,'A':14.2e-4,'d':92.4e-3,'FI':'-','Pmin':0e-3,'Pmax':0e-3,'AL':0.439,'AG':39.38,'Iz':328e-8,'Wzel':54.7e-6,'Wzpl':63.6e-6,'iz':4.81e-2,'Avy':6.63e-4,'Iy':21.5e-8,'Wyel':7.41e-6,'Wypl':12.4e-6,'iy':1.23e-2,'Ss':28.4e-3,'It':2.71e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_140']= {'nmb':'IPN_140','P':14.3,'h':140e-3,'b':66e-3,'tw':5.7e-3,'tf':8.6e-3,'r1':5.7e-3,'r2':3.4e-3,'A':18.3e-4,'d':109.1e-3,'FI':'-','Pmin':0e-3,'Pmax':0e-3,'AL':0.502,'AG':34.94,'Iz':573e-8,'Wzel':81.9e-6,'Wzpl':95.4e-6,'iz':5.61e-2,'Avy':8.65e-4,'Iy':35.2e-8,'Wyel':10.7e-6,'Wypl':17.9e-6,'iy':1.4e-2,'Ss':31.8e-3,'It':4.32e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_160']= {'nmb':'IPN_160','P':17.9,'h':160e-3,'b':74e-3,'tw':6.3e-3,'tf':9.5e-3,'r1':6.3e-3,'r2':3.8e-3,'A':22.8e-4,'d':125.8e-3,'FI':'-','Pmin':0e-3,'Pmax':0e-3,'AL':0.575,'AG':32.13,'Iz':935e-8,'Wzel':117e-6,'Wzpl':136e-6,'iz':6.4e-2,'Avy':10.83e-4,'Iy':54.7e-8,'Wyel':14.8e-6,'Wypl':24.9e-6,'iy':1.55e-2,'Ss':35.2e-3,'It':6.57e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_180']= {'nmb':'IPN_180','P':21.9,'h':180e-3,'b':82e-3,'tw':6.9e-3,'tf':10.4e-3,'r1':6.9e-3,'r2':4.1e-3,'A':27.9e-4,'d':142.4e-3,'FI':'-','Pmin':0e-3,'Pmax':0e-3,'AL':0.64,'AG':29.22,'Iz':1450e-8,'Wzel':161e-6,'Wzpl':187e-6,'iz':7.2e-2,'Avy':13.35e-4,'Iy':81.3e-8,'Wyel':19.8e-6,'Wypl':33.2e-6,'iy':1.71e-2,'Ss':38.6e-3,'It':9.58e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_200']= {'nmb':'IPN_200','P':26.2,'h':200e-3,'b':90e-3,'tw':7.5e-3,'tf':11.3e-3,'r1':7.5e-3,'r2':4.5e-3,'A':33.4e-4,'d':159.1e-3,'FI':'-','Pmin':0e-3,'Pmax':0e-3,'AL':0.709,'AG':27.04,'Iz':2140e-8,'Wzel':214e-6,'Wzpl':250e-6,'iz':8e-2,'Avy':16.03e-4,'Iy':117e-8,'Wyel':26e-6,'Wypl':43.5e-6,'iy':1.87e-2,'Ss':42e-3,'It':13.5e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_220']= {'nmb':'IPN_220','P':31.1,'h':220e-3,'b':98e-3,'tw':8.1e-3,'tf':12.2e-3,'r1':8.1e-3,'r2':4.9e-3,'A':39.5e-4,'d':175.8e-3,'FI':'M 10','Pmin':50e-3,'Pmax':56e-3,'AL':0.775,'AG':24.99,'Iz':3060e-8,'Wzel':278e-6,'Wzpl':324e-6,'iz':8.8e-2,'Avy':19.06e-4,'Iy':162e-8,'Wyel':33.1e-6,'Wypl':55.7e-6,'iy':2.02e-2,'Ss':45.4e-3,'It':18.6e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_240']= {'nmb':'IPN_240','P':36.2,'h':240e-3,'b':106e-3,'tw':8.7e-3,'tf':13.1e-3,'r1':8.7e-3,'r2':5.2e-3,'A':46.1e-4,'d':192.5e-3,'FI':'M 10','Pmin':54e-3,'Pmax':60e-3,'AL':0.844,'AG':23.32,'Iz':4250e-8,'Wzel':354e-6,'Wzpl':412e-6,'iz':9.59e-2,'Avy':22.33e-4,'Iy':221e-8,'Wyel':41.7e-6,'Wypl':70e-6,'iy':2.2e-2,'Ss':48.9e-3,'It':25e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_260']= {'nmb':'IPN_260','P':41.9,'h':260e-3,'b':113e-3,'tw':9.4e-3,'tf':14.1e-3,'r1':9.4e-3,'r2':5.6e-3,'A':53.3e-4,'d':208.9e-3,'FI':'M 12','Pmin':62e-3,'Pmax':62e-3,'AL':0.906,'AG':21.65,'Iz':5740e-8,'Wzel':442e-6,'Wzpl':514e-6,'iz':10.4e-2,'Avy':26.08e-4,'Iy':288e-8,'Wyel':51e-6,'Wypl':85.9e-6,'iy':2.32e-2,'Ss':52.6e-3,'It':33.5e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_280']= {'nmb':'IPN_280','P':47.9,'h':280e-3,'b':119e-3,'tw':10.1e-3,'tf':15.2e-3,'r1':10.1e-3,'r2':6.1e-3,'A':61e-4,'d':225.1e-3,'FI':'M 12','Pmin':68e-3,'Pmax':68e-3,'AL':0.966,'AG':20.17,'Iz':7590e-8,'Wzel':542e-6,'Wzpl':632e-6,'iz':11.1e-2,'Avy':30.18e-4,'Iy':364e-8,'Wyel':61.2e-6,'Wypl':103e-6,'iy':2.45e-2,'Ss':56.4e-3,'It':44.2e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_300']= {'nmb':'IPN_300','P':54.2,'h':300e-3,'b':125e-3,'tw':10.8e-3,'tf':16.2e-3,'r1':10.8e-3,'r2':6.5e-3,'A':69e-4,'d':241.6e-3,'FI':'M 12','Pmin':70e-3,'Pmax':74e-3,'AL':1.03,'AG':19.02,'Iz':9800e-8,'Wzel':653e-6,'Wzpl':762e-6,'iz':11.9e-2,'Avy':34.58e-4,'Iy':451e-8,'Wyel':72.2e-6,'Wypl':121e-6,'iy':2.56e-2,'Ss':60.1e-3,'It':56.8e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_320']= {'nmb':'IPN_320','P':61,'h':320e-3,'b':131e-3,'tw':11.5e-3,'tf':17.3e-3,'r1':11.5e-3,'r2':6.9e-3,'A':77.7e-4,'d':257.9e-3,'FI':'M 12','Pmin':70e-3,'Pmax':80e-3,'AL':1.09,'AG':17.87,'Iz':12510e-8,'Wzel':782e-6,'Wzpl':914e-6,'iz':12.7e-2,'Avy':39.26e-4,'Iy':555e-8,'Wyel':84.7e-6,'Wypl':143e-6,'iy':2.67e-2,'Ss':63.9e-3,'It':72.5e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_340']= {'nmb':'IPN_340','P':68,'h':340e-3,'b':137e-3,'tw':12.2e-3,'tf':18.3e-3,'r1':12.2e-3,'r2':7.3e-3,'A':86.7e-4,'d':274.3e-3,'FI':'M 12','Pmin':78e-3,'Pmax':86e-3,'AL':1.15,'AG':16.9,'Iz':15700e-8,'Wzel':923e-6,'Wzpl':1080e-6,'iz':13.5e-2,'Avy':44.27e-4,'Iy':674e-8,'Wyel':98.4e-6,'Wypl':166e-6,'iy':2.8e-2,'Ss':67.6e-3,'It':90.4e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_360']= {'nmb':'IPN_360','P':76.1,'h':360e-3,'b':143e-3,'tw':13e-3,'tf':19.5e-3,'r1':13e-3,'r2':7.8e-3,'A':97e-4,'d':290.2e-3,'FI':'M 12','Pmin':78e-3,'Pmax':92e-3,'AL':1.21,'AG':15.89,'Iz':19610e-8,'Wzel':1090e-6,'Wzpl':1276e-6,'iz':14.2e-2,'Avy':49.95e-4,'Iy':818e-8,'Wyel':114e-6,'Wypl':194e-6,'iy':2.9e-2,'Ss':71.8e-3,'It':115e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_380']= {'nmb':'IPN_380','P':84,'h':380e-3,'b':149e-3,'tw':13.7e-3,'tf':20.5e-3,'r1':13.7e-3,'r2':8.2e-3,'A':107e-4,'d':306.7e-3,'FI':'M 16','Pmin':84e-3,'Pmax':86e-3,'AL':1.27,'AG':15.12,'Iz':24010e-8,'Wzel':1260e-6,'Wzpl':1482e-6,'iz':15e-2,'Avy':55.55e-4,'Iy':975e-8,'Wyel':131e-6,'Wypl':221e-6,'iy':3.02e-2,'Ss':75.4e-3,'It':141e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_400']= {'nmb':'IPN_400','P':92.4,'h':400e-3,'b':155e-3,'tw':14.4e-3,'tf':21.6e-3,'r1':14.4e-3,'r2':8.6e-3,'A':118e-4,'d':322.9e-3,'FI':'M 16','Pmin':86e-3,'Pmax':92e-3,'AL':1.33,'AG':14.36,'Iz':29210e-8,'Wzel':1460e-6,'Wzpl':1714e-6,'iz':15.7e-2,'Avy':61.69e-4,'Iy':1160e-8,'Wyel':149e-6,'Wypl':253e-6,'iy':3.13e-2,'Ss':79.3e-3,'It':170e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_450']= {'nmb':'IPN_450','P':115,'h':450e-3,'b':170e-3,'tw':16.2e-3,'tf':24.3e-3,'r1':16.2e-3,'r2':9.7e-3,'A':147e-4,'d':363.6e-3,'FI':'M 16','Pmin':92e-3,'Pmax':106e-3,'AL':1.48,'AG':12.83,'Iz':45850e-8,'Wzel':2040e-6,'Wzpl':2400e-6,'iz':17.7e-2,'Avy':77.79e-4,'Iy':1730e-8,'Wyel':203e-6,'Wypl':345e-6,'iy':3.43e-2,'Ss':88.9e-3,'It':267e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_500']= {'nmb':'IPN_500','P':141,'h':500e-3,'b':185e-3,'tw':18e-3,'tf':27e-3,'r1':18e-3,'r2':10.8e-3,'A':179e-4,'d':404.3e-3,'FI':'M 20','Pmin':102e-3,'Pmax':110e-3,'AL':1.63,'AG':11.6,'Iz':68740e-8,'Wzel':2750e-6,'Wzpl':3240e-6,'iz':19.6e-2,'Avy':95.6e-4,'Iy':2480e-8,'Wyel':268e-6,'Wypl':456e-6,'iy':3.72e-2,'Ss':98.5e-3,'It':402e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_550']= {'nmb':'IPN_550','P':166,'h':550e-3,'b':200e-3,'tw':19e-3,'tf':30e-3,'r1':19e-3,'r2':11.9e-3,'A':212e-4,'d':445.6e-3,'FI':'M 22','Pmin':112e-3,'Pmax':118e-3,'AL':1.8,'AG':10.8,'Iz':99180e-8,'Wzel':3610e-6,'Wzpl':4240e-6,'iz':21.6e-2,'Avy':111.3e-4,'Iy':3490e-8,'Wyel':349e-6,'Wypl':592e-6,'iy':4.02e-2,'Ss':107.3e-3,'It':544e-8,'E':2.1e+11,'nu':0.3}

perfilesIPN['IPN_600']= {'nmb':'IPN_600','P':199,'h':600e-3,'b':215e-3,'tw':21.6e-3,'tf':32.4e-3,'r1':21.6e-3,'r2':13e-3,'A':254e-4,'d':485.8e-3,'FI':'M 24','Pmin':126e-3,'Pmax':128e-3,'AL':1.97,'AG':9.89,'Iz':138800e-8,'Wzel':4627e-6,'Wzpl':5452e-6,'iz':23.39e-2,'Avy':138e-4,'Iy':4674e-8,'Wyel':435e-6,'Wypl':752e-6,'iy':4.29e-2,'Ss':117.6e-3,'It':787e-8,'E':2.1e+11,'nu':0.3}

for item in perfilesIPN:
  perfil= perfilesIPN[item]
  perfil['alpha']= perfil['Avy']/perfil['A']
  perfil['G']= perfil['E']/(2*(1+perfil['nu']))
  perfil['AreaQy']= 2*perfil['b']*perfil['tf']+(perfil['tw']+2*perfil['r1'])*perfil['tf']
  perfil['AreaQz']= perfil['A']-perfil['d']*perfil['tw']
