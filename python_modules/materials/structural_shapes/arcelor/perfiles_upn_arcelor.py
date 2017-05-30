# -*- coding: utf-8 -*-
'''
Ejes de la sección:

   ARCELOR          XC

                    ^ Y                    
                    |

      ---           ---
      |             | 
      | -> Y        | -> Z
      |             |
      ---           ---

      |
      v Z

Los ejes en los que se definen las características mecánicas de la sección
en el catálogo original de Arcelor se han cambiado para acomodarlos a los
habituales en XC (eje fuerte paralelo al eje Z). Para ello, como puede verse
en la figura, basta intercambiar los ejes Y y Z.
  
'''
perfilesUPN= {}

perfilesUPN['UPN_80']= {'nmb':'UPN_80', 'P':8.65, 'h':80e-3, 'b':45e-3, 'tw':6e-3, 'tf':8e-3, 'r1':8e-3, 'r2':4e-3, 'A':11.02e-4, 'd':47e-3, 'FI':'-', 'Pmin': None, 'Pmax': None, 'AL':0.321, 'AG':37.1, 'Iz':106e-8, 'Wzel':26.6e-6, 'Wzpl':32.3e-6, 'iz':3.1e-2, 'Avy':4.9e-4, 'Iy':19.4e-8, 'Wyel':6.38e-6, 'Wypl':11.9e-6, 'iy':1.33e-2, 'Ss':19.4e-3, 'It':2.2e-8, 'Iw':0.18e-16, 'zs':1.42e-2, 'zm':2.65e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_100']= {'nmb':'UPN_100', 'P':10.6, 'h':100e-3, 'b':50e-3, 'tw':6e-3, 'tf':8.5e-3, 'r1':8.5e-3, 'r2':4.5e-3, 'A':13.5e-4, 'd':64e-3, 'FI':'-', 'Pmin': None, 'Pmax': None, 'AL':0.372, 'AG':35.1, 'Iz':206e-8, 'Wzel':41.2e-6, 'Wzpl':49e-6, 'iz':3.91e-2, 'Avy':6.46e-4, 'Iy':29.3e-8, 'Wyel':8.49e-6, 'Wypl':16.2e-6, 'iy':1.47e-2, 'Ss':20.3e-3, 'It':2.81e-8, 'Iw':0.41e-16, 'zs':1.55e-2, 'zm':2.93e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_120']= {'nmb':'UPN_120', 'P':13.4, 'h':120e-3, 'b':55e-3, 'tw':7e-3, 'tf':9e-3, 'r1':9e-3, 'r2':4.5e-3, 'A':17e-4, 'd':82e-3, 'FI':'-', 'Pmin': None, 'Pmax': None, 'AL':0.434, 'AG':32.52, 'Iz':364e-8, 'Wzel':60.7e-6, 'Wzpl':72.6e-6, 'iz':4.62e-2, 'Avy':8.8e-4, 'Iy':43.2e-8, 'Wyel':11.1e-6, 'Wypl':21.2e-6, 'iy':1.59e-2, 'Ss':22.2e-3, 'It':4.15e-8, 'Iw':0.9e-16, 'zs':1.6e-2, 'zm':3.03e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_140']= {'nmb':'UPN_140', 'P':16, 'h':140e-3, 'b':60e-3, 'tw':7e-3, 'tf':10e-3, 'r1':10e-3, 'r2':5e-3, 'A':20.4e-4, 'd':98e-3, 'FI':'M 12', 'Pmin':33e-3, 'Pmax':37e-3, 'AL':0.489, 'AG':30.54, 'Iz':605e-8, 'Wzel':86.4e-6, 'Wzpl':103e-6, 'iz':5.45e-2, 'Avy':10.41e-4, 'Iy':62.7e-8, 'Wyel':14.8e-6, 'Wypl':28.3e-6, 'iy':1.75e-2, 'Ss':23.9e-3, 'It':5.68e-8, 'Iw':1.8e-16, 'zs':1.75e-2, 'zm':3.37e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_160']= {'nmb':'UPN_160', 'P':18.8, 'h':160e-3, 'b':65e-3, 'tw':7.5e-3, 'tf':10.5e-3, 'r1':10.5e-3, 'r2':5.5e-3, 'A':24e-4, 'd':115e-3, 'FI':'M 12', 'Pmin':34e-3, 'Pmax':42e-3, 'AL':0.546, 'AG':28.98, 'Iz':925e-8, 'Wzel':116e-6, 'Wzpl':138e-6, 'iz':6.21e-2, 'Avy':12.6e-4, 'Iy':85.3e-8, 'Wyel':18.3e-6, 'Wypl':35.2e-6, 'iy':1.89e-2, 'Ss':25.3e-3, 'It':7.39e-8, 'Iw':3.26e-16, 'zs':1.84e-2, 'zm':3.56e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_180']= {'nmb':'UPN_180', 'P':22, 'h':180e-3, 'b':70e-3, 'tw':8e-3, 'tf':11e-3, 'r1':11e-3, 'r2':5.5e-3, 'A':28e-4, 'd':133e-3, 'FI':'M 16', 'Pmin':38e-3, 'Pmax':41e-3, 'AL':0.611, 'AG':27.8, 'Iz':1350e-8, 'Wzel':150e-6, 'Wzpl':179e-6, 'iz':6.95e-2, 'Avy':15.09e-4, 'Iy':114e-8, 'Wyel':22.4e-6, 'Wypl':42.9e-6, 'iy':2.02e-2, 'Ss':26.7e-3, 'It':9.55e-8, 'Iw':5.57e-16, 'zs':1.92e-2, 'zm':3.75e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_200']= {'nmb':'UPN_200', 'P':25.3, 'h':200e-3, 'b':75e-3, 'tw':8.5e-3, 'tf':11.5e-3, 'r1':11.5e-3, 'r2':6e-3, 'A':32.2e-4, 'd':151e-3, 'FI':'M 16', 'Pmin':39e-3, 'Pmax':46e-3, 'AL':0.661, 'AG':26.15, 'Iz':1910e-8, 'Wzel':191e-6, 'Wzpl':228e-6, 'iz':7.7e-2, 'Avy':17.71e-4, 'Iy':148e-8, 'Wyel':27e-6, 'Wypl':51.8e-6, 'iy':2.14e-2, 'Ss':28.1e-3, 'It':11.9e-8, 'Iw':9.07e-16, 'zs':2.01e-2, 'zm':3.94e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_220']= {'nmb':'UPN_220', 'P':29.4, 'h':220e-3, 'b':80e-3, 'tw':9e-3, 'tf':12.5e-3, 'r1':12.5e-3, 'r2':6.5e-3, 'A':37.4e-4, 'd':167e-3, 'FI':'M 16', 'Pmin':40e-3, 'Pmax':51e-3, 'AL':0.718, 'AG':24.46, 'Iz':2690e-8, 'Wzel':245e-6, 'Wzpl':292e-6, 'iz':8.48e-2, 'Avy':20.62e-4, 'Iy':197e-8, 'Wyel':33.6e-6, 'Wypl':64.1e-6, 'iy':2.3e-2, 'Ss':30.3e-3, 'It':16e-8, 'Iw':14.6e-16, 'zs':2.14e-2, 'zm':4.2e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_240']= {'nmb':'UPN_240', 'P':33.2, 'h':240e-3, 'b':85e-3, 'tw':9.5e-3, 'tf':13e-3, 'r1':13e-3, 'r2':6.5e-3, 'A':42.3e-4, 'd':184e-3, 'FI':'M 20', 'Pmin':46e-3, 'Pmax':50e-3, 'AL':0.775, 'AG':23.34, 'Iz':3600e-8, 'Wzel':300e-6, 'Wzpl':358e-6, 'iz':9.22e-2, 'Avy':23.71e-4, 'Iy':248e-8, 'Wyel':39.6e-6, 'Wypl':75.7e-6, 'iy':2.42e-2, 'Ss':31.7e-3, 'It':19.7e-8, 'Iw':22.1e-16, 'zs':2.23e-2, 'zm':4.39e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_260']= {'nmb':'UPN_260', 'P':37.9, 'h':260e-3, 'b':90e-3, 'tw':10e-3, 'tf':14e-3, 'r1':14e-3, 'r2':7e-3, 'A':48.3e-4, 'd':200e-3, 'FI':'M 22', 'Pmin':50e-3, 'Pmax':52e-3, 'AL':0.834, 'AG':22, 'Iz':4820e-8, 'Wzel':371e-6, 'Wzpl':442e-6, 'iz':9.99e-2, 'Avy':27.12e-4, 'Iy':317e-8, 'Wyel':47.7e-6, 'Wypl':91.6e-6, 'iy':2.56e-2, 'Ss':33.9e-3, 'It':25.5e-8, 'Iw':33.3e-16, 'zs':2.36e-2, 'zm':4.66e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_280']= {'nmb':'UPN_280', 'P':41.8, 'h':280e-3, 'b':95e-3, 'tw':10e-3, 'tf':15e-3, 'r1':15e-3, 'r2':7.5e-3, 'A':53.3e-4, 'd':216e-3, 'FI':'M 22', 'Pmin':52e-3, 'Pmax':57e-3, 'AL':0.89, 'AG':21.27, 'Iz':6280e-8, 'Wzel':448e-6, 'Wzpl':532e-6, 'iz':10.9e-2, 'Avy':29.28e-4, 'Iy':399e-8, 'Wyel':57.2e-6, 'Wypl':109e-6, 'iy':2.74e-2, 'Ss':35.6e-3, 'It':31e-8, 'Iw':48.5e-16, 'zs':2.53e-2, 'zm':5.02e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_300']= {'nmb':'UPN_300', 'P':46.2, 'h':300e-3, 'b':100e-3, 'tw':10e-3, 'tf':16e-3, 'r1':16e-3, 'r2':8e-3, 'A':58.8e-4, 'd':232e-3, 'FI':'M 24', 'Pmin':55e-3, 'Pmax':59e-3, 'AL':0.95, 'AG':20.58, 'Iz':8030e-8, 'Wzel':535e-6, 'Wzpl':632e-6, 'iz':11.7e-2, 'Avy':31.77e-4, 'Iy':495e-8, 'Wyel':67.8e-6, 'Wypl':130e-6, 'iy':2.9e-2, 'Ss':37.3e-3, 'It':37.4e-8, 'Iw':69.1e-16, 'zs':2.7e-2, 'zm':5.41e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_320']= {'nmb':'UPN_320', 'P':59.5, 'h':320e-3, 'b':100e-3, 'tw':14e-3, 'tf':17.5e-3, 'r1':17.5e-3, 'r2':8.75e-3, 'A':75.8e-4, 'd':246e-3, 'FI':'M 22', 'Pmin':58e-3, 'Pmax':62e-3, 'AL':0.982, 'AG':16.5, 'Iz':10870e-8, 'Wzel':679e-6, 'Wzpl':826e-6, 'iz':12.1e-2, 'Avy':47.11e-4, 'Iy':597e-8, 'Wyel':80.6e-6, 'Wypl':152e-6, 'iy':2.81e-2, 'Ss':43e-3, 'It':66.7e-8, 'Iw':96.1e-16, 'zs':2.6e-2, 'zm':4.82e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_350']= {'nmb':'UPN_350', 'P':60.6, 'h':350e-3, 'b':100e-3, 'tw':14e-3, 'tf':16e-3, 'r1':16e-3, 'r2':8e-3, 'A':77.3e-4, 'd':282e-3, 'FI':'M 22', 'Pmin':56e-3, 'Pmax':62e-3, 'AL':1.047, 'AG':17.25, 'Iz':12840e-8, 'Wzel':734e-6, 'Wzpl':918e-6, 'iz':12.9e-2, 'Avy':50.84e-4, 'Iy':570e-8, 'Wyel':75e-6, 'Wypl':143e-6, 'iy':2.72e-2, 'Ss':40.7e-3, 'It':61.2e-8, 'Iw':114e-16, 'zs':2.4e-2, 'zm':4.45e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_380']= {'nmb':'UPN_380', 'P':63.1, 'h':380e-3, 'b':102e-3, 'tw':13.5e-3, 'tf':16e-3, 'r1':16e-3, 'r2':8e-3, 'A':80.4e-4, 'd':313e-3, 'FI':'M 24', 'Pmin':59e-3, 'Pmax':60e-3, 'AL':1.11, 'AG':17.59, 'Iz':15760e-8, 'Wzel':829e-6, 'Wzpl':1014e-6, 'iz':14e-2, 'Avy':53.23e-4, 'Iy':615e-8, 'Wyel':78.7e-6, 'Wypl':148e-6, 'iy':2.77e-2, 'Ss':40.3e-3, 'It':59.1e-8, 'Iw':146e-16, 'zs':2.38e-2, 'zm':4.58e-2, 'E':2.1e+11, 'nu':0.3}
perfilesUPN['UPN_400']= {'nmb':'UPN_400', 'P':71.8, 'h':400e-3, 'b':110e-3, 'tw':14e-3, 'tf':18e-3, 'r1':18e-3, 'r2':9e-3, 'A':91.5e-4, 'd':324e-3, 'FI':'M 27', 'Pmin':61e-3, 'Pmax':62e-3, 'AL':1.182, 'AG':16.46, 'Iz':20350e-8, 'Wzel':1020e-6, 'Wzpl':1240e-6, 'iz':14.9e-2, 'Avy':58.55e-4, 'Iy':846e-8, 'Wyel':102e-6, 'Wypl':190e-6, 'iy':3.04e-2, 'Ss':44e-3, 'It':81.6e-8, 'Iw':221e-16, 'zs':2.65e-2, 'zm':5.11e-2, 'E':2.1e+11, 'nu':0.3}

for item in perfilesUPN:
  perfil= perfilesUPN[item]
  Avy= perfil['Avy']
  A= perfil['A']
  E= perfil['E']
  nu= perfil['nu']
  b= perfil['b']
  d= perfil['d']
  tf= perfil['tf']
  tw= perfil['tw']
  r1= perfil['r1']
  perfil['alpha']= Avy/A
  perfil['G']= E/(2*(1+nu))
  perfil['AreaQy']= A-2*b*tf+(tw+r1)*tf
  perfil['AreaQz']= A-d*tw
