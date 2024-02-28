#ifndef _ALGORITHMS_H
#define _ALGORITHMS_H
enum algo_id {
  // Comparison based Algorithms
  _BF,      // Brute Force
  _MP,      // Morris-Pratt
  _KMP,     // Knuth-Morris Pratt
  _BM,      // Boyer-Moore
  _HOR,     // Hoorspool
  _GS,      // Galil Seiferas
  _AG,      // Apostolico-Giancarlo
  _KR,      // Karp-Rabin
  _ZT,      // Zhu-Takaoka
  _AC,      // Apostolico-Crochemore
  _TW,      // Two-Way
  _OM,      // Optimal Mismatch
  _MS,      // Maximal shift
  _QS,      // Quick-Search
  _TunBM,   // Tuned Boyer-Moore
  _NSN,     // not so naive
  _TBM,     // Turbo Boyer-Moore
  _Col,     // Colussi (missing)
  _Smith,   // Smith
  _GG,      // Galil-Giancarlo (missing)
  _Raita,   // Raita
  _SMOA,    // String Mtahcing on Ordered Alphabets
  _RCol,    // Reverse Colussi
  _Skip,    // Skip Serach
  _KMPSkip, // KMP skip search
  _ASkip,   // Alpha-Skip-Search
  _BR,      // Berry-ravindran
  _AKC,     // AKC algorithm
  _FS,      // Fast-Search
  _FFS,     // Forward-Fast-Search
  _BFS,     // Backward-Fast-Search, Fast-Boyer-Moore
  _TS,      // Tailed-Substring
  _SSABS,   // Sheik-Sumit-Anindya-Balakrishnan-Sekar
  _TVSBS,   // Thathoo-Virmani-Sai-Balakrishnan-Sekar
  _PBMH,    // Boyer-Moore-Horspool using Probabilities
  _FJS,     // Franek-Jennings-Smyth
  _BLOCK,   // 2-Block Boyer-Moore
  _HASH3,   // Wu-Manber for Single Pattern Matching (q=3)
  _HASH5,   // Wu-Manber for Single Pattern Matching (q=5)
  _HASH8,   // Wu-Manber for Single Pattern Matching (q=8)
  _TSW,     // Two-Sliding-Window
  _BMH2,    // Boyer-Moore-Horspool with q-grams  (BMH2)
  _BMH4,    // Boyer-Moore-Horspool with q-grams  (BMH4)
  _GRASPm,  // Genomic Rapid Algorithm for String Pattern-match
  _SSEF,    // SSEF (K=7)
  // Algorithms based on automata
  _AUT,   // Automaton Matcher
  _RF,    // Revrese-Factor
  _TRF,   // Turbo Reverse factor
  _Simon, // Simon
  _FDM,   // Forward-DAWG-Match
  _BOM,   // BOM
  _BOM2,  // BOM2
  _DFDM,  // Double Forward DAWG Matching (missing)
  _WW,    // Wide Window
  _LDM,   // Linear DAWG Matching
  _ILDM1, // ILDM1
  _ILDM2, // ILDM2
  _EBOM,  // Extended Backward Oracle Matching
  _FBOM,  // Forward Backward Oracle Matching
  _SEBOM, // Simplified Extended Backward Oracle Matching
  _SFBOM, // Simplified Forward Backward Oracle Matching
  _SBDM,  // Succint Backward DAWG Matching
  // Algorithms based on bit-parallelism
  _SO,       // Shift-Or
  _SA,       // Shift-And
  _BNDM,     // BNDM for Long patterns
  _BNDML,    // BNDM for Long patterns
  _SBNDM,    // Simplified BNDM
  _TNDM,     // Two-Way Nondeterministic DAWG Matching
  _TNDMa,    // Two-Way Nondeterministic DAWG Matching (version 2)
  _LBNDM,    // Long patterns BNDM
  _SVM0,     // Shift Vector Matching (version 0)
  _SVM1,     // Shift Vector Matching (version 1)
  _SVM2,     // Shift Vector Matching (version 2)
  _SVM3,     // Shift Vector Matching (version 3)
  _SVM4,     // Shift Vector Matching (version 4)
  _SBNDM2,   // Simplified BNDM with loop-unrolling
  _SBNDMBMH, // SBNDM with Horspool Shift
  _BMHSBNDM, // Horspool with SBNDM test
  _FNDM,     // Forward Nondeterministic DAWG Matching
  _BWW,      // Bit parallel Wide Window
  _FAOSOq2,  // Fast Average Optimal Shift-Or (u=2)
  _FAOSOq4,  // Fast Average Optimal Shift-Or (u=2)
  _FAOSOq6,  // Fast Average Optimal Shift-Or (u=2)
  _AOSO2,    // Average Optimal Shift-Or (q=2)
  _AOSO4,    // Average Optimal Shift-Or (q=4)
  _AOSO6,    // Average Optimal Shift-Or (q=6)
  _BLIM,     // Bit-Parallel Length Invariant Matcher
  _FSBNDM,   // Forward SBNDM
  _BNDMq2,   // BNDM with q-grams
  _BNDMq4,   // BNDM with q-grams
  _BNDMq6,   // BNDM with q-grams
  _SBNDMq2,  // Simplified BNDM with q-grams
  _SBNDMq4,  // Simplified BNDM with q-grams
  _SBNDMq6,  // Simplified BNDM with q-grams
  _SBNDMq8,  // Simplified BNDM with q-grams
  _UFNDMq2,  // Shift-Or with q-grams
  _UFNDMq4,  // Shift-Or with q-grams
  _UFNDMq6,  // Shift-Or with q-grams
  _UFNDMq8,  // Shift-Or with q-grams
  _SABP,     // Small Alphabet Bit Parallel
  _DBWW,     // DOUBLE BWW
  _DBWW2,    // DOUBLE BWW
  _KSA,      // Factorized SHIFT-AND
  _KBNDM,    // Factorized BNDM
  _SSECP,    // SSE Crochemore-Perrin
  _EPSM,     // SSE4 Exact Packed String Matching
} ALGO_POS;

struct algo {
  const enum algo_id id;
  const int execute;
  const char *name;
  const char *desc;
  const int missing;
};

// Note: This part may be edited by select
const struct algo ALGOS[] = {
    // Comparison based Algorithms
    [_BF] = {_BF, 1, "bf", "Brute Force", 0},
    [_MP] = {_MP, 1, "mp", "Morris-Pratt", 0},
    [_KMP] = {_KMP, 1, "kmp", "Knuth-Morris Pratt", 0},
    [_BM] = {_BM, 1, "bm", "Boyer-Moore", 0},
    [_HOR] = {_HOR, 1, "hor", "Hoorspool", 0},
    [_GS] = {_GS, 1, "gs", "Galil Seiferas", 1},
    [_AG] = {_AG, 1, "ag", "Apostolico-Giancarlo", 0},
    [_KR] = {_KR, 1, "kr", "Karp-Rabin", 0},
    [_ZT] = {_ZT, 1, "zt", "Zhu-Takaoka", 0},
    [_AC] = {_AC, 1, "ac", "Apostolico-Crochemore", 0},
    [_TW] = {_TW, 1, "tw", "Two-Way", 0},
    [_OM] = {_OM, 1, "om", "Optimal Mismatch", 0},
    [_MS] = {_MS, 1, "ms", "Maximal shift", 0},
    [_QS] = {_QS, 1, "qs", "Quick-Search", 0},
    [_TunBM] = {_TunBM, 1, "tunbm", "Tuned Boyer-Moore", 1},
    [_NSN] = {_NSN, 1, "nsn", "not so naive", 0},
    [_TBM] = {_TBM, 1, "tbm", "Turbo Boyer-Moore", 0},
    [_Col] = {_Col, 1, "col", "Colussi", 1},
    [_Smith] = {_Smith, 1, "smith", "Smith", 0},
    [_GG] = {_GG, 1, "gg", "Galil-Giancarlo", 1},
    [_Raita] = {_Raita, 1, "raita", "Raita", 0},
    [_SMOA] = {_SMOA, 1, "smoa", "String Mtahcing on Ordered Alphabets", 0},
    [_RCol] = {_RCol, 1, "rcol", "Reverse Colussi", 1},
    [_Skip] = {_Skip, 1, "skip", "Skip Serach", 0},
    [_KMPSkip] = {_KMPSkip, 1, "kmpskip", "KMP skip search", 0},
    [_ASkip] = {_ASkip, 1, "askip", "Alpha-Skip-Search", 0},
    [_BR] = {_BR, 1, "br", "Berry-ravindran", 0},
    [_AKC] = {_AKC, 1, "akc", "AKC algorithm", 0},
    [_FS] = {_FS, 1, "fs", "Fast-Search", 0},
    [_FFS] = {_FFS, 1, "ffs", "Forward-Fast-Search", 0},
    [_BFS] = {_BFS, 1, "bfs", "Backward-Fast-Search, Fast-Boyer-Moore", 0},
    [_TS] = {_TS, 1, "ts", "Tailed-Substring", 0},
    [_SSABS] = {_SSABS, 1, "ssabs", "Sheik-Sumit-Anindya-Balakrishnan-Sekar",
                0},
    [_TVSBS] = {_TVSBS, 1, "tvsbs", "Thathoo-Virmani-Sai-Balakrishnan-Sekar",
                0},
    [_PBMH] = {_PBMH, 1, "pbmh", "Boyer-Moore-Horspool using Probabilities", 0},
    [_FJS] = {_FJS, 1, "fjs", "Franek-Jennings-Smyth", 0},
    [_BLOCK] = {_BLOCK, 1, "block", "2-Block Boyer-Moore", 1},
    [_HASH3] = {_HASH3, 1, "hash3",
                "Wu-Manber for Single Pattern Matching (q=3)", 0},
    [_HASH5] = {_HASH5, 1, "hash5",
                "Wu-Manber for Single Pattern Matching (q=5)", 0},
    [_HASH8] = {_HASH8, 1, "hash8",
                "Wu-Manber for Single Pattern Matching (q=8)", 0},
    [_TSW] = {_TSW, 1, "tsw", "Two-Sliding-Window", 0},
    [_BMH2] = {_BMH2, 1, "bmh2", "Boyer-Moore-Horspool with q-grams  (BMH2)",
               1},
    [_BMH4] = {_BMH4, 1, "bmh4", "Boyer-Moore-Horspool with q-grams  (BMH4)",
               1},
    [_GRASPm] = {_GRASPm, 1, "graspm",
                 "Genomic Rapid Algorithm for String Pattern-match", 0},
    [_SSEF] = {_SSEF, 1, "ssef", "SSEF (K=7)", 0},
    // Algorithms based on automata
    [_AUT] = {_AUT, 1, "aut", "Automaton Matcher", 0},
    [_RF] = {_RF, 1, "rf", "Revrese-Factor", 0},
    [_TRF] = {_TRF, 1, "trf", "Turbo Reverse factor", 0},
    [_Simon] = {_Simon, 1, "simon", "Simon", 0},
    [_FDM] = {_FDM, 1, "fdm", "Forward-DAWG-Match", 0},
    [_BOM] = {_BOM, 1, "bom", "BOM", 0},
    [_BOM2] = {_BOM2, 1, "bom2", "BOM2", 0},
    [_DFDM] = {_DFDM, 1, "dfdm", "Double Forward DAWG Matching", 1},
    [_WW] = {_WW, 1, "ww", "Wide Window", 0},
    [_LDM] = {_LDM, 1, "ldm", "Linear DAWG Matching", 0},
    [_ILDM1] = {_ILDM1, 1, "ildm1", "ILDM1", 0},
    [_ILDM2] = {_ILDM2, 1, "ildm2", "ILDM2", 0},
    [_EBOM] = {_EBOM, 1, "ebom", "Extended Backward Oracle Matching", 0},
    [_FBOM] = {_FBOM, 1, "fbom", "Forward Backward Oracle Matching", 0},
    [_SEBOM] = {_SEBOM, 1, "sebom",
                "Simplified Extended Backward Oracle Matching", 0},
    [_SFBOM] = {_SFBOM, 1, "sfbom",
                "Simplified Forward Backward Oracle Matching", 0},
    [_SBDM] = {_SBDM, 1, "sbdm", "Succint Backward DAWG Matching", 1},
    // Algorithms based on bit-parallelism
    [_SO] = {_SO, 1, "so", "Shift-Or", 0},
    [_SA] = {_SA, 1, "sa", "Shift-And", 0},
    [_BNDM] = {_BNDM, 1, "bndm", "BNDM for Long patterns", 0},
    [_BNDML] = {_BNDML, 1, "bndml", "BNDM for Long patterns", 0},
    [_SBNDM] = {_SBNDM, 1, "sbndm", "Simplified BNDM", 0},
    [_TNDM] = {_TNDM, 1, "tndm", "two-way nondeterministic dawg matching", 0},
    [_TNDMa] = {_TNDMa, 1, "tndma",
                "Two-Way Nondeterministic DAWG Matching (version 2)", 0},
    [_LBNDM] = {_LBNDM, 1, "lbndm", "long patterns bndm", 0},
    [_SVM0] = {_SVM0, 1, "svm0", "shift vector matching (version 0)", 0},
    [_SVM1] = {_SVM1, 1, "svm1", "shift vector matching (version 1)", 0},
    [_SVM2] = {_SVM2, 1, "svm2", "shift vector matching (version 2)", 0},
    [_SVM3] = {_SVM3, 1, "svm3", "shift vector matching (version 3)", 0},
    [_SVM4] = {_SVM4, 1, "svm4", "shift vector matching (version 4)", 0},
    [_SBNDM2] = {_SBNDM2, 1, "sbndm2", "simplified bndm with loop-unrolling",
                 1},
    [_SBNDMBMH] = {_SBNDMBMH, 1, "sbndmbmh", "sbndm with horspool shift", 1},
    [_BMHSBNDM] = {_BMHSBNDM, 1, "bmhsbndm", "horspool with sbndm test", 1},
    [_FNDM] = {_FNDM, 1, "fndm", "forward nondeterministic dawg matching", 0},
    [_BWW] = {_BWW, 1, "bww", "bit parallel wide window", 0},
    [_FAOSOq2] = {_FAOSOq2, 1, "faosoq2", "fast average optimal shift-or (u=2)",
                  1},
    [_FAOSOq4] = {_FAOSOq4, 1, "faosoq4", "fast average optimal shift-or (u=2)",
                  1},
    [_FAOSOq6] = {_FAOSOq6, 1, "faosoq6", "fast average optimal shift-or (u=2)",
                  1},
    [_AOSO2] = {_AOSO2, 1, "aoso2", "average optimal shift-or (q=2)", 0},
    [_AOSO4] = {_AOSO4, 1, "aoso4", "average optimal shift-or (q=4)", 0},
    [_AOSO6] = {_AOSO6, 1, "aoso6", "average optimal shift-or (q=6)", 0},
    [_BLIM] = {_BLIM, 0, "blim", "bit-parallel length invariant matcher", 0},
    [_FSBNDM] = {_FSBNDM, 1, "fsbndm", "forward sbndm", 0},
    [_BNDMq2] = {_BNDMq2, 1, "bndmq2", "bndm with q-grams", 0},
    [_BNDMq4] = {_BNDMq4, 1, "bndmq4", "bndm with q-grams", 0},
    [_BNDMq6] = {_BNDMq6, 1, "bndmq6", "bndm with q-grams", 0},
    [_SBNDMq2] = {_SBNDMq2, 1, "sbndmq2", "simplified bndm with q-grams", 0},
    [_SBNDMq4] = {_SBNDMq4, 1, "sbndmq4", "simplified bndm with q-grams", 0},
    [_SBNDMq6] = {_SBNDMq6, 1, "sbndmq6", "simplified bndm with q-grams", 0},
    [_SBNDMq8] = {_SBNDMq8, 1, "sbndmq8", "simplified bndm with q-grams", 0},
    [_UFNDMq2] = {_UFNDMq2, 1, "ufndmq2", "shift-or with q-grams", 0},
    [_UFNDMq4] = {_UFNDMq4, 1, "ufndmq4", "shift-or with q-grams", 0},
    [_UFNDMq6] = {_UFNDMq6, 1, "ufndmq6", "shift-or with q-grams", 0},
    [_UFNDMq8] = {_UFNDMq8, 1, "ufndmq8", "shift-or with q-grams", 0},
    [_SABP] = {_SABP, 1, "sabp", "Small Alphabet Bit Parallel", 0},
    [_DBWW] = {_DBWW, 1, "dbww", "DOUBLE BWW", 0},
    [_DBWW2] = {_DBWW2, 1, "dbww2", "DOUBLE BWW", 0},
    [_KSA] = {_KSA, 1, "ksa", "Factorized SHIFT-AND", 0},
    [_KBNDM] = {_KBNDM, 1, "kbndm", "Factorized BNDM", 0},
    [_SSECP] = {_SSECP, 0, "ssecp", "SSE Crochemore-Perrin", 0},
    [_EPSM] = {_EPSM, 1, "epsm", "SSE4 Exact Packed String Matching", 0},
};

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define NumAlgo ARRAY_SIZE(ALGOS)
#endif // _ALGORITHMS_H
