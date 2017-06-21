# -*- coding: utf-8 -*-

'''
Cross section axis:

   ARCELOR                   XC

                             ^ Y                    
                             |

      -----                -----
     /     \              /     \
    /       \ -> Y       /       \ | -> Z
   /         \          /         \
---           ---    ---           ---

        |
        v Z

The axis used in Arcelor documentation are different from those used in XC
(strong axis parallel to z axis) in other words: values for Y and Z axis 
are swapped with respect to those in the catalog.

Warning: area in m2/m, inertia in m4/m, Wz in m3/m and so on

'''


AU_profiles={}



AU_profiles['AU_14']= {'nmb':'AU_14', 'P':77.9, 'h':408e-3, 'b':750e-3, 's':8.3e-3, 't':10.0e-3, 'A':132.3e-4, 'Iz':28880e-8, 'Wzel':1405e-6, 'Wzpl':1663e-8, 'Szel':820e-6, 'E':2.1e+11, 'nu':0.3}
AU_profiles['AU_16']= {'nmb':'AU_16', 'P':86.3, 'h':411e-3, 'b':750e-3, 's':9.3e-3, 't':11.5e-3, 'A':146.5e-4, 'Iz':32850e-8, 'Wzel':1600e-6, 'Wzpl':1891e-8, 'Szel':935e-6, 'E':2.1e+11, 'nu':0.3}
AU_profiles['AU_18']= {'nmb':'AU_18', 'P':88.5, 'h':441e-3, 'b':750e-3, 's':9.1e-3, 't':10.5e-3, 'A':150.3e-4, 'Iz':39300e-8, 'Wzel':1780e-6, 'Wzpl':2082e-8, 'Szel':1030e-6, 'E':2.1e+11, 'nu':0.3}
AU_profiles['AU_20']= {'nmb':'AU_20', 'P':96.9, 'h':444e-3, 'b':750e-3, 's':10.0e-3, 't':12.0e-3, 'A':164.6e-4, 'Iz':44440e-8, 'Wzel':2000e-6, 'Wzpl':2339e-8, 'Szel':1155e-6, 'E':2.1e+11, 'nu':0.3}
AU_profiles['AU_23']= {'nmb':'AU_23', 'P':102.1, 'h':447e-3, 'b':750e-3, 's':9.5e-3, 't':13.0e-3, 'A':173.4e-4, 'Iz':50700e-8, 'Wzel':2270e-6, 'Wzpl':2600e-8, 'Szel':1285e-6, 'E':2.1e+11, 'nu':0.3}
AU_profiles['AU_25']= {'nmb':'AU_25', 'P':110.4, 'h':450e-3, 'b':750e-3, 's':10.2e-3, 't':14.5e-3, 'A':187.5e-4, 'Iz':56240e-8, 'Wzel':2500e-6, 'Wzpl':2866e-8, 'Szel':1420e-6, 'E':2.1e+11, 'nu':0.3}

#Code to import data.
# dataList= [('P',6,''),('h',2,'e-3'),('b',1,'e-3'),('s',4,'e-3'),('t',3,'e-3'),('A',5,'e-4'),('Iz',8,'e-8'),('Wzel',9,'e-6'),('Wzpl',11,'e-8'),('Szel',10,'e-6')]
# import csv
# import sys

# f = open('aux/au_sheet_pile.csv', 'rt')
# try:
#     reader = csv.reader(f)
#     for row in reader:
#         output= "AU_profiles['"+row[0]+"']= {'nmb':'"+row[0]+"', "
#         for t in dataList:
#             output+= "'"+t[0]+"':"+row[t[1]]+t[2]+', '
#         output+= "'E':2.1e+11, 'nu':0.3}"
#         print output
# finally:
#     f.close()

for item in AU_profiles:
  shape= AU_profiles[item]
  h= shape['h'] #Height
  tf= shape['t'] #Flange thickness
  tw= shape['s'] #Web thickness
  b= shape['b']
  Avy= tw*(h-tf)/b #Projected shear area.
  shape['Avy']= Avy
  A= shape['A']
  E= shape['E']
  nu= shape['nu']
  shape['alpha']= Avy/A
  shape['G']= E/(2*(1+nu))

from materials import structural_steel

class AUProfile(structural_steel.SteelProfile):
  def __init__(self,steel,name):
    super(AUProfile,self).__init__(steel,name,AU_profiles)
  def getRho(self):
    ''' Returns mass per unit lenght. '''
    return self.get('P')
