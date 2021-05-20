      SUBROUTINE SKYPF2 (INDEX,JMAX,JMIN,MCOLS,MROWS,DGPNT,A,RW,TW)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYPF2 factors (A) into (L)*(D)*(L') in a partitioned way        *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    INDEX  (aiw) : work array, max(MCOLS,MROWS), for storing indexes *
C*    JMAX   (sii) : index of the last  column of (A)                  *
C*    JMIN   (sii) : index of the first column of (A)                  *
C*    MCOLS  (sii) : maximum number of columns stored in (RW)          *
C*    MROWS  (sii) : maximum number of rows    stored in (RW)          *
C*    DGPNT  (aii) : addresses of the diagonal coefficients            *
C*    A      (arb) : the coefficients of the matrix (A)                *
C*    RW     (arw) : work array, MROWSxMCOLS, for storing submatrices  *
C*    TW     (arw) : work array, MROWSxMROWS, for storing submatrices  *
C*                                                                     *
C*  - SUBPROGRAMS :                                                    *
C*    SKYPCI,SKYPDS,SKYPRI,SKYPSB,SKYPSP                               *
C*                                                                     *
C*  - BLAS kernels :                                                   *
C*    DGEMV,DSCAL,DTRSM                                                *
C*                                                                     *
C*  - INTRINSIC FUNCTIONS :                                            *
C*    MAX0,MIN0                                                        *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          DGPNT(*),INDEX(*),JMAX,JMIN,MCOLS,MROWS
      DOUBLE PRECISION A(*),RW(MROWS,*),TW(MROWS,*)
C
C==== local variables ==================================================
C
      INTEGER          J,JLEFT,JLOWER,JNEXT,JRIGHT,JSIZE,
     &                 JSPAN,JSTART,JUPPER,NCOLS,NROWS
      DOUBLE PRECISION ALPHA
C
C==== subprograms ======================================================
C
      INTEGER          SKYPCI,SKYPRI
C
C==== intrinsic functions ==============================================
C
      INTEGER          MAX0,MIN0
C
C**** executable statements ********************************************
C
C.... loop on MROWS at a time ..........................................
C
      DO 010 JLOWER = JMIN,JMAX,MROWS
C
         JUPPER = MIN0(JLOWER+MROWS-1,JMAX) 
C
C....... copy the diagonal submatrix into (TW) .........................
C
         CALL SKYPSB (DGPNT,JLOWER,JUPPER,INDEX,JLOWER,JUPPER,JMIN,
     &                JUPPER,MROWS,MROWS,NROWS,A,TW,'T')
C
C....... factorize the diagonal submatrix ..............................
C
         CALL SKYPDS (MROWS,NROWS,TW)
C
C....... copy (TW) back into (A) .......................................
C
         CALL SKYPSP (DGPNT,JLOWER,JUPPER,INDEX,JMIN,
     &                MROWS,MROWS,NROWS,A,TW,'T')
C
         IF ( JMAX .GT. JUPPER ) THEN      
C
            JRIGHT = SKYPCI(DGPNT,JUPPER,JMAX,JUPPER+1,JMIN)
C
            IF ( JRIGHT .GT. 0 ) THEN
C
C............. update the nondiagonal submatrices ......................
C
               JNEXT = JUPPER + 1
C
               DO 030 WHILE ( JNEXT .LE. JRIGHT )
C
                  JLEFT = SKYPCI(DGPNT,JUPPER,JNEXT,JMAX,JMIN)
                  JNEXT = JLEFT
C
C................ copy the nondiagonal submatrix into (RW) .............
C
                  CALL SKYPSB (DGPNT,JLOWER,JUPPER,INDEX,JNEXT,JRIGHT,
     &                         JMIN,JUPPER,MCOLS,MROWS,NCOLS,A,RW,'R')
C
C................ updating: (RW)=(TW')**(-1)*(RW) ......................
C
                  CALL DTRSM  ('L','U','T','U',NROWS,NCOLS,1.0D00,
     &                         TW,MROWS,RW,MROWS)
C
C................ copy (RW) back into (A) ..............................
C
                  CALL SKYPSP (DGPNT,JLOWER,JUPPER,INDEX,JMIN,
     &                         MCOLS,MROWS,NCOLS,A,RW,'R')
C
  030          CONTINUE 
C
               JNEXT = JUPPER + 1
C
               DO 040 WHILE ( JNEXT .LE. JRIGHT )
C
C................ factorize the nondiagonal submatrix ..................
C
                  JLEFT = SKYPCI(DGPNT,JUPPER,JNEXT,JMAX,JMIN)
                  JNEXT = JLEFT
C
C................ copy the nondiagonal submatrix into (RW) .............
C
                  CALL SKYPSB (DGPNT,JLOWER,JUPPER,INDEX,JNEXT,JRIGHT,
     &                         JMIN,JMAX,MCOLS,MROWS,NCOLS,A,RW,'R')
C
C................ scale (RW) ...........................................
C
                  DO 050 J = JLOWER,JUPPER
                     IF ( A(DGPNT(J)) .NE. 0.0D0 ) THEN
                        ALPHA = 1.0D0/A(DGPNT(J))
                        CALL DSCAL (NCOLS,ALPHA,RW(J-JLOWER+1,1),MROWS)
                     END IF
  050             CONTINUE
C
C................ update the trailing submatrices ......................
C
                  DO 060 J = JLEFT,JRIGHT
                     JSTART = SKYPRI(DGPNT,J,JMIN)
                     JSTART = MAX0(JSTART,JLOWER)
                     JSIZE  = JLOWER + NROWS - JSTART
                     IF ( JSIZE .GT. 0 ) THEN
                        JSPAN = MIN0(NCOLS,J-JLEFT+1)
                        CALL DGEMV ('T',JSIZE,JSPAN,-1.0D0,
     &                              RW(JSTART-JLOWER+1,1),MROWS,
     &                              A(DGPNT(J)+JSTART-J),1,+1.0D0,
     &                              A(DGPNT(J)+JLEFT -J),1)
                     END IF
  060             CONTINUE
C
C................ copy (RW) back into (A) ..............................
C
                  CALL SKYPSP (DGPNT,JLOWER,JUPPER,INDEX,JMIN,
     &                         MCOLS,MROWS,NCOLS,A,RW,'R')
C
  040          CONTINUE
C
            END IF
C
         END IF
C
  010 CONTINUE
C
      RETURN
C
C**** end of SKYPF2 ****************************************************
C
      END
