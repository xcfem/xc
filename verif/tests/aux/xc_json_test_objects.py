# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2024 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.sections.fiber_section import def_simple_RC_section

reinfRow= def_simple_RC_section.ReinfRow(areaRebar= 0.01, width= 0.25, nRebars= 4, nominalCover= 0.035, nominalLatCover= 0.04)

data= {'a list': [1, 42, 3.141, 1337, 'help', u'€'],
        'a string': 'bla',
        'another dict': {'foo': 'bar',
                         'key': 'value',
                         'the answer': 42},
       'reinforcement_row': reinfRow}
