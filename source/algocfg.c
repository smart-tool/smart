/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2024 Reini Urban
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sets.h"

/*
 * This program is used for the build-system to provide proper cmdline args per algo.
 * For now for make verify. make sanitize asan.lst maybe also.
 * cbmc can be much faster if given the proper depth and unwind args. best no depth at all
 * for sound proofs, which should terminate.
 */

enum verify_status {
  VFY_PASS,
  UNSATISFIABLE, // but VERIFICATION SUCCESSFUL
  VFY_FAIL,
  VFY_TIMEOUT,
};

// the first flags are exclusive, the rest bitmasks can be added
#define FLAGS_MASK 31
enum cbmc_args {
  cadical = 1,
  z3 = 2,
  cvc5 = 3,
  boolector = 4,
  yices = 5,
  glucose = 6,
  cprover_smt2 = 7,
  ipasir_cadical = 8,
  ipasir_custom = 9,
  bitwuzla = 10,
  mathsat = 11,
  picosat = 12,
  chaff = 13,
  kissat = 14,   // see also github.com/rurban/cmbc -b new-sat-solvers
  squolem = 15,  // see also github.com/rurban/cmbc -b new-sat-solvers
  stp = 16,      // requires github.com/rurban/cmbc -b stp
  no_ua = 32,    // no --unwinding-assertions
  no_sf = 64,    // no --slice-formula
  rs = 128,      // --refine-strings
};
#define LAST_SAT stp

const char *cbmc_flags[] = {
  [cadical] = "--sat-solver cadical",
  [z3] = "--z3",
  [cvc5] = "--cvc5",
  [boolector] = "--boolector",
  [yices] = "--yices",
  [cprover_smt2] = "--cprover-smt2",
  [ipasir_cadical] = "--sat-solver cadical",
  [ipasir_custom] = "--sat-solver ipasir",
  [glucose] = "--sat-solver glucose",
  [bitwuzla] = "--bitwuzla",
  [mathsat] = "--mathsat",
  [picosat] = "--external-sat-solver picosat",
  [chaff] = "--external-sat-solver chaff",
  [kissat] = "--external-sat-solver kissat --dimacs",
  [squolem] = "--external-sat-solver squolem --smt2",
  [stp] = "--stp",
};

/* UNSATIFIABLE means cbmc it ran into --depth limit. redo with higher --depth,
   maybe enlarge the timeout also. or use the builtin memcmp/memset with CBMC.
 */

struct algocfg {
  const enum algo_id id;
  const int good; // passes all tests
  const int asan; // passes asan tests
  const enum verify_status vfy;
  const int minlen;
  const int maxlen;
  const int depth; // MAX_M 10 * MAX_N 36
  const int unwind; // SIGMA 256
  const enum cbmc_args flags;
  const int timeout_min;
};

#define GOOD 1
#define ASAN 1
#define FAIL 0
#define ASSERTS 0

/* unwind:
  preMp: MAX_M * MAX_M = 64
  timeout 30s cbmc -DCBMC --unwind 64 --verbosity 9 --slice-formula source/algos/mp.c | tee mp.vfy
  grep -c Unwinding mp.vfy => 12118
 */

const struct algocfg ALGOCFGS[] = {
    // clang-format off
  // Comparison based Algorithms
  [_BF] = {_BF, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 1}, // 0.26s
  [_MP] = {_MP, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 1}, // 1024: 51s
  [_KMP] = {_KMP, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BM] = {_BM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_HOR] = {_HOR, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_GS] = {_GS, FAIL, FAIL, VFY_PASS, 0, 0, 0, 0, 0, 0}, // random m=32 failures
  [_AG] = {_AG, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_KR] = {_KR, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_ZT] = {_ZT, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_AC] = {_AC, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_TW] = {_TW, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_OM] = {_OM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_MS] = {_MS, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_QS] = {_QS, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_NSN] = {_NSN, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_TBM] = {_TBM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_Colussi] = {_Colussi, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_Smith] = {_Smith, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_GG] = {_GG, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_Raita] = {_Raita, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_SMOA] = {_SMOA, GOOD, ASAN, VFY_PASS, 0, 0, 512, 0, 0, 0},
  [_RColussi] = {_RColussi, FAIL, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0}, // random fails
  [_Skip] = {_Skip, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_KMPSkip] = {_KMPSkip, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_ASkip] = {_ASkip, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BR] = {_BR, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_AKC] = {_AKC, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_FS] = {_FS, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_FFS] = {_FFS, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BFS] = {_BFS, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_TS] = {_TS, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_SSABS] = {_SSABS, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_TVSBS] = {_TVSBS, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_PBMH] = {_PBMH, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_FJS] = {_FJS, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_2BLOCK] = {_2BLOCK, GOOD, ASAN, VFY_FAIL, 0, 0, 0, 0, 0, 0},
  [_SCUT] = {_SCUT, GOOD, ASAN, VFY_FAIL, 0, 0, 0, 0, 0, 0},
  [_HASH3] = {_HASH3, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_HASH5] = {_HASH5, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_HASH8] = {_HASH8, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_SM] = {_SM, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_SWM] = {_SWM, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_TSW] = {_TSW, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0}, // m < n - 3
  [_BMH2] = {_BMH2, GOOD, ASAN, VFY_FAIL, 2, 0, 0, 0, 0, 0},
  [_BMH4] = {_BMH4, GOOD, ASAN, VFY_FAIL, 4, 0, 0, 0, 0, 0},
  [_GRASPm] = {_GRASPm, FAIL, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_SSEF] = {_SSEF, X64_ONLY, ASAN, VFY_FAIL, 32, 0, 0, 0, 0, 0},
  [_AUT] = {_AUT, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_RF] = {_RF, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_TRF] = {_TRF, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_Simon] = {_Simon, FAIL, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_FDM] = {_FDM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BOM] = {_BOM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BOM2] = {_BOM2, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_DFDM] = {_DFDM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_WW] = {_WW, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_LDM] = {_LDM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_ILDM1] = {_ILDM1, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_ILDM2] = {_ILDM2, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_EBOM] = {_EBOM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_FBOM] = {_FBOM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_SEBOM] = {_SEBOM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_SFBOM] = {_SFBOM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_SBDM] = {_SBDM, GOOD, ASAN, VFY_FAIL, 0, 0, 0, 0, 0, 0},
  [_BSOM] = {_BSOM, GOOD, ASAN, VFY_FAIL, 0, 0, 0, 0, 0, 0},
  [_SKIP2] = {_SKIP2, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_SKIP3] = {_SKIP3, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_SKIP4] = {_SKIP4, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_SKIP5] = {_SKIP5, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_SKIP6] = {_SKIP6, GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_SKIP7] = {_SKIP7, GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_SKIP8] = {_SKIP8, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_SO]    = {_SO, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_SA]    = {_SA, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BNDM]  = {_BNDM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BM_BNDM] = {_BM_BNDM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_Turbo_BNDM] = {_Turbo_BNDM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BNDML] = {_BNDML, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_SBNDM] = {_SBNDM, GOOD, ASAN, VFY_PASS, 2, 32, 0, 0, 0, 0},
  [_TNDM]  = {_TNDM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_TNDMa] = {_TNDMa, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_LBNDM] = {_LBNDM, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_SVM0]  = {_SVM0, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_SVM1]  = {_SVM1, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_SVM2]  = {_SVM2, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_SVM3]  = {_SVM3, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_SVM4]  = {_SVM4, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_SBNDM2] = {_SBNDM2, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_SBNDM_BMH] = {_SBNDM_BMH, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_BMH_SBNDM] = {_BMH_SBNDM, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_FNDM]  = {_FNDM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BWW]   = {_BWW, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_FAOSO2] = {_FAOSO2, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_FAOSO4] = {_FAOSO4, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_FAOSO6] = {_FAOSO6, GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_AOSO2] = {_AOSO2, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_AOSO4] = {_AOSO4, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_AOSO6] = {_AOSO6, GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_BLIM]  = {_BLIM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_FSBNDM] = {_FSBNDM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BNDMq2] = {_BNDMq2, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_BNDMq4] = {_BNDMq4, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_BNDMq6] = {_BNDMq6, GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_SBNDMq2] = {_SBNDMq2, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_SBNDMq4] = {_SBNDMq4, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_SBNDMq6] = {_SBNDMq6, GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_SBNDMq8] = {_SBNDMq8, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_UFNDMq2] = {_UFNDMq2, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_UFNDMq4] = {_UFNDMq4, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_UFNDMq6] = {_UFNDMq6, GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_UFNDMq8] = {_UFNDMq8, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_SABP]  = {_SABP, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_DBWW]  = {_DBWW, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_DBWW2] = {_DBWW2, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_KSA]   = {_KSA, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_KBNDM] = {_KBNDM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BSDM]  = {_BSDM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BSDM2] = {_BSDM2, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_BSDM3] = {_BSDM3, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_BSDM4] = {_BSDM4, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_BSDM5] = {_BSDM5, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_BSDM6] = {_BSDM6, GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_BSDM7] = {_BSDM7, GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_BSDM8] = {_BSDM8, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_BXS]  = {_BXS,  GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_BXS1] = {_BXS1, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BXS2] = {_BXS2, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_BXS3] = {_BXS3, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_BXS4] = {_BXS4, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_BXS6] = {_BXS6, GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_BXS8] = {_BXS8, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_BQL] = {_BQL, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0}, // new
  [_FS_W1] = {_FS_W1, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_FS_W2] = {_FS_W2, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_FS_W4] = {_FS_W4, FAIL, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0}, // m>=6
  [_FS_W6] = {_FS_W6, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0}, // m>=8
  [_FS_W8] = {_FS_W8, GOOD, ASAN, VFY_PASS, 10, 0, 0, 0, 0, 0}, // needs m space at the end of T
  [_FSBNDM_W1] = {_FSBNDM_W1, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_FSBNDM_W2] = {_FSBNDM_W2, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_FSBNDM_W4] = {_FSBNDM_W4, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_FSBNDM_W6] = {_FSBNDM_W6, GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_FSBNDM_W8] = {_FSBNDM_W8, ASSERTS, FAIL, VFY_PASS, 11, 0, 0, 0, 0, 0},
  [_FSBNDMQ20] = {_FSBNDMQ20, GOOD, ASAN, VFY_PASS, 2, 32, 0, 0, 0, 0},
  [_FSBNDMQ21] = {_FSBNDMQ21, GOOD, ASAN, VFY_PASS, 2, 31, 0, 0, 0, 0},
  [_FSBNDMQ31] = {_FSBNDMQ31, GOOD, ASAN, VFY_PASS, 3, 31, 0, 0, 0, 0},
  [_FSBNDMQ32] = {_FSBNDMQ32, FAIL, FAIL, VFY_PASS, 3, 30, 0, 0, 0, 0},
  [_FSBNDMQ41] = {_FSBNDMQ41, GOOD, ASAN, VFY_PASS, 4, 31, 0, 0, 0, 0},
  [_FSBNDMQ42] = {_FSBNDMQ42, FAIL, FAIL, VFY_PASS, 4, 30, 0, 0, 0, 0},
  [_FSBNDMQ43] = {_FSBNDMQ43, FAIL, FAIL, VFY_PASS, 4, 29, 0, 0, 0, 0},
  [_FSBNDMQ61] = {_FSBNDMQ61, GOOD, ASAN, VFY_PASS, 4, 31, 0, 0, 0, 0},
  [_FSBNDMQ62] = {_FSBNDMQ62, FAIL, FAIL, VFY_PASS, 6, 30, 0, 0, 0, 0},
  [_FSBNDMQ64] = {_FSBNDMQ64, FAIL, FAIL, VFY_PASS, 6, 28, 0, 0, 0, 0},
  [_FSBNDMQ81] = {_FSBNDMQ81, GOOD, ASAN, VFY_PASS, 8, 31, 0, 0, 0, 0},
  [_FSBNDMQ82] = {_FSBNDMQ82, FAIL, FAIL, VFY_PASS, 8, 30, 0, 0, 0, 0},
  [_FSBNDMQ84] = {_FSBNDMQ84, FAIL, FAIL, VFY_PASS, 8, 28, 0, 0, 0, 0},
  [_FSBNDMQ86] = {_FSBNDMQ86, FAIL, FAIL, VFY_PASS, 8, 26, 0, 0, 0, 0},
  [_IOM]   = {_IOM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_JOM]   = {_JOM, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_LWFR2] = {_LWFR2, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_LWFR3] = {_LWFR3, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_LWFR4] = {_LWFR4, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_LWFR5] = {_LWFR5, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_LWFR6] = {_LWFR6, GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_LWFR7] = {_LWFR7, GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_LWFR8] = {_LWFR8, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_QF23] = {_QF23, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_QF24] = {_QF24, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_QF26] = {_QF26, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_QF28] = {_QF28, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_QF33] = {_QF33, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_QF34] = {_QF34, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_QF36] = {_QF36, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_QF42] = {_QF42, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_QF43] = {_QF43, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_QF44] = {_QF44, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_QF53] = {_QF53, GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_QF62] = {_QF62, GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_QF63] = {_QF63, GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_QF72] = {_QF72, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_QF82] = {_QF82, GOOD, ASAN, VFY_PASS, 9, 0, 0, 0, 0, 0},
  [_QLQS] = {_QLQS, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_SBNDM_W2] = {_SBNDM_W2, FAIL, ASAN, VFY_PASS, 2, 32, 0, 0, 0, 0},
  [_SBNDM_W4] = {_SBNDM_W4, FAIL, FAIL, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_SBNDM_W6] = {_SBNDM_W6, GOOD, ASAN, VFY_PASS, 6, 32, 0, 0, 0, 0},
  [_SSM]    = {_SSM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_TSA]    = {_TSA, GOOD, ASAN, VFY_PASS, 2, 63, 0, 0, 0, 0},
  [_TSA_Q2] = {_TSA_Q2, GOOD, ASAN, VFY_PASS, 2, 63, 0, 0, 0, 0},
  [_TSO5]   = {_TSO5, GOOD, ASAN, VFY_PASS, 0, 0, 63, 0, 0, 0},
  [_TUNEDBM] = {_TUNEDBM, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_TVSBS_W2] = {_TVSBS_W2, GOOD, FAIL, VFY_PASS, 0, XSIZE, 0, 0, 0, 0},
  [_TVSBS_W4] = {_TVSBS_W4, FAIL, FAIL, VFY_PASS, 2, XSIZE, 0, 0, 0, 0},
  [_TVSBS_W6] = {_TVSBS_W6, GOOD, ASAN, VFY_PASS, 2, XSIZE, 0, 0, 0, 0},
  [_TVSBS_W8] = {_TVSBS_W8, FAIL, FAIL, VFY_PASS, 2, XSIZE, 0, 0, 0, 0},
  [_WFR]  = {_WFR,  GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_WFR2] = {_WFR2, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_WFR3] = {_WFR3, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_WFR4] = {_WFR4, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_WFR5] = {_WFR5, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_WFR6] = {_WFR6, GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_WFR7] = {_WFR7, GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_WFR8] = {_WFR8, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_WFRQ2] = {_WFRQ2, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_WFRQ3] = {_WFRQ3, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_WFRQ4] = {_WFRQ4, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_WFRQ5] = {_WFRQ5, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_WFRQ6] = {_WFRQ6, GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_WFRQ7] = {_WFRQ7, GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_WFRQ8] = {_WFRQ8, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_TWFR]  = {_TWFR,  GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_TWFR2] = {_TWFR2, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_TWFR3] = {_TWFR3, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_TWFR4] = {_TWFR4, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_TWFR5] = {_TWFR5, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_TWFR6] = {_TWFR6, GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_TWFR7] = {_TWFR7, GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_TWFR8] = {_TWFR8, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_TWFRQ2] = {_TWFRQ2, GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_TWFRQ3] = {_TWFRQ3, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_TWFRQ4] = {_TWFRQ4, GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_TWFRQ5] = {_TWFRQ5, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_TWFRQ6] = {_TWFRQ6, GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_TWFRQ7] = {_TWFRQ7, GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_TWFRQ8] = {_TWFRQ8, GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_WC]     = {_WC,     GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_WOM]    = {_WOM,    GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_DOUBLEHASH] = {_DOUBLEHASH, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_BRAM3] = {_BRAM3, GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_BRAM5] = {_BRAM5, GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_BRAM7] = {_BRAM7, GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_FT3]   = {_FT3,   GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_HPBM] = {_HPBM, GOOD, ASAN, VFY_FAIL, 0, 0, 0, 0, 0, 0}, // OpenMP
  [_SSECP] = {_SSECP, FAIL, ASAN, VFY_FAIL, 0, 0, 0, 0, 0, 0}, // not cbmc, but esbmc
  [_SIMDKR] = {_SIMDKR, GOOD, ASAN, VFY_FAIL, 0, 0, 0, 0, 0, 0}, // not cbmc, but esbmc
  [_KRSIMD] = {_KRSIMD, GOOD, ASAN, VFY_FAIL, 0, 0, 0, 0, 0, 0}, // AVX2
  [_LIBC]  = {_LIBC,  FAIL, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0}, // no \0
  [_LIBC1] = {_LIBC1, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0}, // esbmc fails
  [_MUSL]  = {_MUSL,  FAIL, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0}, // no \0
  [_MUSL1] = {_MUSL1, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0}, //7s, upto d=820 ok
  [_MUSL2] = {_MUSL2, GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_EPSM]  = {_EPSM, GOOD, ASAN, VFY_FAIL, 0, 0, 0, 0, 0, 0}, // not cbmc, but esbmc
  [_HC1]  =  {_HC1,   GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_HC2]  =  {_HC2,   GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_HC3]  =  {_HC3,   GOOD, ASAN, VFY_PASS, 3, 4194304, 0, 0, 0, 0}, // maxlen
  [_HC4]  =  {_HC4,   GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_HC5]  =  {_HC5,   GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_HC6]  =  {_HC6,   GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_HC7]  =  {_HC7,   GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_HC8]  =  {_HC8,   GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_LHC1] =  {_LHC1,  GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_LHC2] =  {_LHC2,  GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_LHC3] =  {_LHC3,  GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_LHC4] =  {_LHC4,  GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_LHC5] =  {_LHC5,  GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_LHC6] =  {_LHC6,  GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_LHC7] =  {_LHC7,  GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_LHC8] =  {_LHC8,  GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_SHC1] =  {_SHC1,  GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_SHC2] =  {_SHC2,  GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_SHC3] =  {_SHC3,  GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_SHC4] =  {_SHC4,  GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_SHC5] =  {_SHC5,  GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_SHC6] =  {_SHC6,  GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_SHC7] =  {_SHC7,  GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_SHC8] =  {_SHC8,  GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
  [_FHC1] =  {_FHC1,  GOOD, ASAN, VFY_PASS, 0, 0, 0, 0, 0, 0},
  [_FHC2] =  {_FHC2,  GOOD, ASAN, VFY_PASS, 2, 0, 0, 0, 0, 0},
  [_FHC3] =  {_FHC3,  GOOD, ASAN, VFY_PASS, 3, 0, 0, 0, 0, 0},
  [_FHC4] =  {_FHC4,  GOOD, ASAN, VFY_PASS, 4, 0, 0, 0, 0, 0},
  [_FHC5] =  {_FHC5,  GOOD, ASAN, VFY_PASS, 5, 0, 0, 0, 0, 0},
  [_FHC6] =  {_FHC6,  GOOD, ASAN, VFY_PASS, 6, 0, 0, 0, 0, 0},
  [_FHC7] =  {_FHC7,  GOOD, ASAN, VFY_PASS, 7, 0, 0, 0, 0, 0},
  [_FHC8] =  {_FHC8,  GOOD, ASAN, VFY_PASS, 8, 0, 0, 0, 0, 0},
    // clang-format on
};

int main(int argc, char **argv) {
  if (argc < 2 || argc > 4) {
    fprintf(stderr, "Usage: ./algocfg [algo] [cfg [value]]\n");
    return 1;
  }
  char *algo = argv[1];
  getAlgo(ALGO_NAME, EXECUTE);
  int id = search_ALGO(ALGO_NAME, algo);
  if (id < 0) { // not an algo, so a cfg
    const char *cfg = algo;
    if (strcmp(cfg, "good") == 0) {
      int value = (argc == 3) ? atoi(argv[2]) : 1;
      for (unsigned i = 0; i < NumAlgo; i++)
        if (ALGOCFGS[i].id == i && ALGOCFGS[i].good == value)
          printf("%s ", ALGOS[i].name);
      printf("\n");
    } else if (strcmp(cfg, "fail") == 0) {
      for (unsigned i = 0; i < NumAlgo; i++)
        if (ALGOCFGS[i].id == i && ALGOCFGS[i].good == FAIL)
          printf("%s ", ALGOS[i].name);
      printf("\n");
    } else if (strcmp(cfg, "asan") == 0) {
      int value = (argc == 3) ? atoi(argv[2]) : 1;
      for (unsigned i = 0; i < NumAlgo; i++)
        if (ALGOCFGS[i].id == i && ALGOCFGS[i].asan == value)
          printf("%s ", ALGOS[i].name);
      printf("\n");
    } else if (strcmp(cfg, "VFY_PASS") == 0) {
      for (unsigned i = 0; i < NumAlgo; i++)
        if (ALGOCFGS[i].id == i && ALGOCFGS[i].vfy == VFY_PASS)
          printf("%s ", ALGOS[i].name);
      printf("\n");
    } else if (strcmp(cfg, "VFY_FAIL") == 0) {
      for (unsigned i = 0; i < NumAlgo; i++)
        if (ALGOCFGS[i].id == i && ALGOCFGS[i].vfy == VFY_FAIL)
          printf("%s ", ALGOS[i].name);
      printf("\n");
    } else if (strcmp(cfg, "VFY_TIMEOUT") == 0) {
      for (unsigned i = 0; i < NumAlgo; i++)
        if (ALGOCFGS[i].id == i && ALGOCFGS[i].vfy == VFY_TIMEOUT)
          printf("%s ", ALGOS[i].name);
      printf("\n");
    } else if (strcmp(cfg, "UNSATISFIABLE") == 0) {
      for (unsigned i = 0; i < NumAlgo; i++)
        if (ALGOCFGS[i].id == i && ALGOCFGS[i].vfy == UNSATISFIABLE)
          printf("%s ", ALGOS[i].name);
      printf("\n");
    } else if (strcmp(cfg, "depth") == 0) {
      int value = (argc == 3) ? atoi(argv[2]) : 320;
      for (unsigned i = 0; i < NumAlgo; i++)
        if (ALGOCFGS[i].id == i && ALGOCFGS[i].depth == value)
          printf("%s ", ALGOS[i].name);
      printf("\n");
    } else if (strcmp(cfg, "unwind") == 0) {
      int value = (argc == 3) ? atoi(argv[2]) : 0;
      for (unsigned i = 0; i < NumAlgo; i++)
        if (ALGOCFGS[i].id == i && ALGOCFGS[i].unwind == value)
          printf("%s ", ALGOS[i].name);
      printf("\n");
    } else if (strcmp(cfg, "timeout") == 0) {
      int value = (argc == 3) ? atoi(argv[2]) : 0;
      for (unsigned i = 0; i < NumAlgo; i++)
        if (ALGOS[i].id == i && ALGOCFGS[i].timeout_min == value)
          printf("%s ", ALGOS[i].name);
      printf("\n");
    } else if (strcmp(cfg, "minlen") == 0) {
      int value = (argc == 3) ? atoi(argv[2]) : 0;
      // TODO which op? == < > ...
      for (unsigned i = 0; i < NumAlgo; i++)
        if (ALGOCFGS[i].id == i && ALGOCFGS[i].minlen == value)
          printf("%s ", ALGOS[i].name);
      printf("\n");
    } else if (strcmp(cfg, "maxlen") == 0) {
      int value = (argc == 3) ? atoi(argv[2]) : 0;
      for (unsigned i = 0; i < NumAlgo; i++)
        if (ALGOCFGS[i].id == i && ALGOCFGS[i].maxlen == value)
          printf("%s ", ALGOS[i].name);
      printf("\n");
    } else {
      printf("Unknown cfg or algo %s\n", cfg);
      exit(1);
    }
  } else {
    if (argc < 3 || argc > 4) {
      fprintf(stderr, "Usage: ./algocfg algo cfg [value]\n");
      return 1;
    }
    const char *cfg = argv[2];
    unsigned solver_flags = ALGOCFGS[id].flags & FLAGS_MASK;
    if (strcmp(cfg, "good") == 0)
      printf("%d\n", ALGOCFGS[id].good);
    else if (strcmp(cfg, "asan") == 0)
      printf("%d\n", ALGOCFGS[id].asan);
    else if (strcmp(cfg, "vfy") == 0)
      printf("%s\n", ALGOCFGS[id].vfy == VFY_PASS        ? "VFY_PASS"
                     : ALGOCFGS[id].vfy == VFY_FAIL      ? "VFY_FAIL"
                     : ALGOCFGS[id].vfy == VFY_TIMEOUT   ? "VFY_TIMEOUT"
                     : ALGOCFGS[id].vfy == UNSATISFIABLE ? "UNSATISFIABLE"
                                                         : "?");
    else if (strcmp(cfg, "VFY_PASS") == 0)
      printf("%d\n", ALGOCFGS[id].vfy == VFY_PASS);
    else if (strcmp(cfg, "VFY_FAIL") == 0)
      printf("%d\n", ALGOCFGS[id].vfy == VFY_FAIL);
    else if (strcmp(cfg, "VFY_TIMEOUT") == 0)
      printf("%d\n", ALGOCFGS[id].vfy == VFY_TIMEOUT);
    else if (strcmp(cfg, "UNSATISFIABLE") == 0)
      printf("%d\n", ALGOCFGS[id].vfy == UNSATISFIABLE);
    else if (strcmp(cfg, "depth") == 0)
      printf("%d\n", ALGOCFGS[id].depth);
    else if (strcmp(cfg, "unwind") == 0)
      printf("%d\n", ALGOCFGS[id].unwind);
    else if (strcmp(cfg, "timeout") == 0)
      printf("%d\n", ALGOCFGS[id].timeout_min);
    else if (strcmp(cfg, "cbmc") == 0) {
      char *cbmc = getenv("CBMC");
      if (!cbmc)
        cbmc = "cbmc";
      // cmbc 5.12 has too high default verbosity 8, we want 6 + unwind summaries
      if (ALGOCFGS[id].timeout_min)
        printf("timeout %dm %s --verbosity 7 ", ALGOCFGS[id].timeout_min, cbmc);
      else
        printf("timeout 4m %s --verbosity 7 ", cbmc);
      if (strcmp(cbmc, "esbmc") == 0) {
        printf("--k-induction\n");
        return 0;
      }
      if (ALGOCFGS[id].depth)
        printf("--depth %d ", ALGOCFGS[id].depth);
      if (ALGOCFGS[id].unwind)
        printf("--unwind %d ", ALGOCFGS[id].unwind);
      if (!(ALGOCFGS[id].flags & no_ua))
        printf("--unwinding-assertions ");
      if (!(ALGOCFGS[id].flags & no_sf))
        printf("--slice-formula ");
      if (solver_flags && (solver_flags <= LAST_SAT))
        printf("%s ", cbmc_flags[solver_flags]);
      printf("\n");
    } else if (strcmp(cfg, "minlen") == 0)
      printf("%d\n", ALGOCFGS[id].minlen);
    else if (strcmp(cfg, "maxlen") == 0)
      printf("%d\n", ALGOCFGS[id].maxlen);
    else {
      printf("Unknown cfg %s\n", cfg);
      exit(1);
    }
    return 0;
  }
}
