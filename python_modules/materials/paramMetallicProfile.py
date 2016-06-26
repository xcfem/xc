# -*- coding: utf-8 -*-

'''
Rutinas para definir las características mecánicas de
   las secciones a partir de los datos del perfil metálico
   (ver archivos perfiles_ipe_arcelor.lcmd, perfiles_he_arcelor.lcmd,...).
'''

'''
Define una sección elástica para elementos 3d a partir de los datos
 del perfil.
'''
def defSeccPerfilMetalicoElastica3d(defPerfil):
  print "DEPRECATED; call defSeccElastica3d."
  return defSeccElastica3d(preprocessor,defPerfil)

'''
Define una sección elástica para elementos 3d a partir de los datos
 del perfil.
'''
def defSeccPerfilMetalicoShElastica3d(defPerfil):
  print "DEPRECATED; call defSeccShElastica3d."
  return defSeccShElastica3d(preprocessor,defPerfil)

'''
Define una sección elástica para elementos 2d a partir de los datos
 del perfil.
'''
def defSeccPerfilMetalicoElastica2d(defPerfil):
  print "DEPRECATED; call defSeccElastica2d."
  return defSeccElastica2d(preprocessor,defPerfil)


'''
Define una sección elástica para elementos 2d a partir de los datos
 del perfil.
'''
def defSeccPerfilMetalicoShElastica2d(defPerfil):
  print "DEPRECATED; call defSeccShElastica2d."
  return defSeccShElastica2d(preprocessor,defPerfil)

