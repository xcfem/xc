      SUBROUTINE SKYPDS (M,N,A)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYPDS factorizes a diagonal submatrix                           *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    M (sii) : leading dimension of (A)                               *
C*    N (sii) : current dimension of the diagonal submatrix            *
C*    A (arb) : diagonal submatrix                                     *
C*                                                                     *
C*  - BLAS kernels :                                                   *
C*    DSCAL,DSYR                                                       *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          M,N
      DOUBLE PRECISION A(M,*)
C
C==== local variables ==================================================
C
      INTEGER          I
      DOUBLE PRECISION ALPHA
C
C**** executable statements ********************************************
C
      DO 010 I = 1,N-1
C
         IF ( A(I,I) .NE. 0.0D0 ) THEN
C
            ALPHA = 1.0D0/A(I,I)
C
C.......... scale the i-th line by 1.0D0/A(I,I) ........................
C
            CALL DSCAL (N-I,ALPHA,A(I,I+1),M)
C
C.......... perform a rank one updating in the upper triangle ..........
C
            CALL DSYR  ('U',N-I,-A(I,I),A(I,I+1),M,A(I+1,I+1),M)
C
         END IF
C
  010 CONTINUE
C
      RETURN
C
C**** end of SKYPDS ****************************************************
C
      END
