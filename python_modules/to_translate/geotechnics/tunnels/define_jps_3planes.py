# -*- coding: utf-8 -*-
# Define the «Joint Pyramids» corresponding to the planes  P1,P2 and P3
JP000= geom.BlockPyramid(half_spaces(P1U,P2U,P3U) )
JP001= geom.BlockPyramid(half_spaces(P1U,P2U,P3L) )
JP010= geom.BlockPyramid(half_spaces(P1U,P2L,P3U) )
JP011= geom.BlockPyramid(half_spaces(P1U,P2L,P3L) )
JP100= geom.BlockPyramid(half_spaces(P1L,P2U,P3U) )
JP101= geom.BlockPyramid(half_spaces(P1L,P2U,P3L) )
JP110= geom.BlockPyramid(half_spaces(P1L,P2L,P3U) )
JP111= geom.BlockPyramid(half_spaces(P1L,P2L,P3L) )
