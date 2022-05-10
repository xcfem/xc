# -*- coding: utf-8 -*-
import geom

# Define the «Joint Pyramids» corresponding to the four planes: P1,P2,P3 y P4
JP0000= geom.BlockPyramid(half_spaces(P1U,P2U,P3U,P4U) )
JP0001= geom.BlockPyramid(half_spaces(P1U,P2U,P3U,P4L) )
JP0010= geom.BlockPyramid(half_spaces(P1U,P2U,P3L,P4U) )
JP0011= geom.BlockPyramid(half_spaces(P1U,P2U,P3L,P4L) )

JP0100= geom.BlockPyramid(half_spaces(P1U,P2L,P3U,P4U) )
JP0101= geom.BlockPyramid(half_spaces(P1U,P2L,P3U,P4L) )
JP0110= geom.BlockPyramid(half_spaces(P1U,P2L,P3L,P4U) )
JP0111= geom.BlockPyramid(half_spaces(P1U,P2L,P3L,P4L) )

JP1000= geom.BlockPyramid(half_spaces(P1L,P2U,P3U,P4U) )
JP1001= geom.BlockPyramid(half_spaces(P1L,P2U,P3U,P4L) )
JP1010= geom.BlockPyramid(half_spaces(P1L,P2U,P3L,P4U) )
JP1011= geom.BlockPyramid(half_spaces(P1L,P2U,P3L,P4L) )

JP1100= geom.BlockPyramid(half_spaces(P1L,P2L,P3U,P4U) )
JP1101= geom.BlockPyramid(half_spaces(P1L,P2L,P3U,P4L) )
JP1110= geom.BlockPyramid(half_spaces(P1L,P2L,P3L,P4U) )
JP1111= geom.BlockPyramid(half_spaces(P1L,P2L,P3L,P4L) )
