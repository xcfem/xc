# -*- coding: utf-8 -*-
import math

def defSteelProfileElasticRangeElementParameters(e,profile):
  e.setProp("nmbSecc",profile.sectionName)
  e.setProp("Area",profile.A())
  e.setProp("fyd",profile.steelType.fyd())
  e.setProp("fydV",profile.steelType.fydV())

  e.setProp("Wyel",profile.get('Wyel'))
  e.setProp("Wzel",profile.get('Wzel'))
  e.setProp("AreaQz",profile.get('AreaQz'))
  e.setProp("AreaQy",profile.get('AreaQy'))


def defSteelProfileElasticRangeParametersForSet(elems,profile):
  for e in elems:
    defSteelProfileElasticRangeElementParameters(e,profile)



