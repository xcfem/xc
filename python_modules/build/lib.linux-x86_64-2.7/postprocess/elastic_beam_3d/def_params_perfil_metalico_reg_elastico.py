# -*- coding: utf-8 -*-
import math

def defParamsPerfilMetalicoRegElasticoElem(e,perfil):
  e.setProp("nmbSecc",perfil.nmb)
  e.setProp("Area",perfil.A())
  e.setProp("fyd",perfil.steelType.fyd())
  e.setProp("fydV",perfil.steelType.fydV())

  e.setProp("Wyel",perfil.get('Wyel'))
  e.setProp("Wzel",perfil.get('Wzel'))
  e.setProp("AreaQz",perfil.get('AreaQz'))
  e.setProp("AreaQy",perfil.get('AreaQy'))


def defParamsPerfilMetalicoRegElasticoSet(elems,perfil):
  for e in elems:
    defParamsPerfilMetalicoRegElasticoElem(e,perfil)



