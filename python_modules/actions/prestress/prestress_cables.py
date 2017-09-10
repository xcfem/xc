# -*- coding: utf-8 -*-
# Utils to prestress elements

def pre_stress_elem_line(preprocessor,tag_line, tension):
  '''Introduces a prestressing tension on the elements in a line

  :param preprocessor: preprocessor
  :param tag_line: line number
  :param tension: tension value
  '''
  line= preprocessor.getCad.getLine(tag_line)
  elems= line.getElems()
  for e in elems:
    e.getMaterial().prestress(tension)

def post_stress_elem_line(preprocessor,tag_line, tension):
  '''Introduces a post-tension on the elements in a line

  :param preprocessor: preprocessor
  :param tag_line: line number
  :param tension: tension value
  '''
  
  line= preprocessor.getCad.getLine(tag_line)
  elems= linea.getElems()
  for e in elems:
    pret= e.getPrestress
    e.getMaterial().prestress(tension+pret)

def re_stress_elem_lines(set_lines, umbral, tension):
  setsLines= preprocessor.getSets.getSet(set_lines)
  for s in setsLines:
    elems= linea.getElems()
    for e in elems:
      pret= e.getPrestress()
      if(pret<umbral):
        e.prestress(tension+pret)
