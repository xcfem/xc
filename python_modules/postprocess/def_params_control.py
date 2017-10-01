# -*- coding: utf-8 -*-
import math

def defSteelShapeElasticRangeElementParameters(e,shape):
  e.setProp("nmbSecc",shape.sectionName)
  e.setProp("Area",shape.A())
  e.setProp("fyd",shape.steelType.fyd())
  e.setProp("fydV",shape.steelType.fydV())

  e.setProp("Wyel",shape.get('Wyel'))
  e.setProp("Wzel",shape.get('Wzel'))
  e.setProp("AreaQz",shape.get('AreaQz'))
  e.setProp("AreaQy",shape.get('AreaQy'))


def defSteelShapeElasticRangeParametersForSet(elems,shape):
  for e in elems:
    defSteelShapeElasticRangeElementParameters(e,shape)



