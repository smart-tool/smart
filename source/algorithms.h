#ifndef _ALGORITHMS_H
#define _ALGORITHMS_H

#ifdef __x64_86__
# define X64_ONLY 1
#else
# define X64_ONLY 0
#endif

#if !(defined(_WIN32) || defined(__AVR__))
# define HAVE_SHM
#endif

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
  _Colussi, // Colussi
  _Smith,   // Smith
  _GG,      // Galil-Giancarlo
  _Raita,   // Raita
  _SMOA,    // String Matching on Ordered Alphabets
  _RColussi,// Reverse Colussi
  _Skip,    // Skip Search
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
  _BLOCK,   // 2-Block Boyer-Moore (missing)
  _HASH3,   // Wu-Manber for Single Pattern Matching (q=3)
  _HASH5,   // Wu-Manber for Single Pattern Matching (q=5)
  _HASH8,   // Wu-Manber for Single Pattern Matching (q=8)
  _TSW,     // Two-Sliding-Window
  _BMH2,    // Boyer-Moore-Horspool with q-grams (missing)
  _BMH4,    // Boyer-Moore-Horspool with q-grams (missing)
  _GRASPm,  // Genomic Rapid Algorithm for String Pattern-match
  _SSEF,    // SSEF (K=7)
  // Algorithms based on automata
  _AUT,   // Automaton Matcher
  _RF,    // Reverse-Factor
  _TRF,   // Turbo Reverse factor
  _Simon, // Simon
  _FDM,   // Forward-DAWG-Match
  _BOM,   // BOM
  _BOM2,  // BOM2
  _DFDM,  // Double Forward DAWG Matching (in work)
  _WW,    // Wide Window
  _LDM,   // Linear DAWG Matching
  _ILDM1, // ILDM1
  _ILDM2, // ILDM2
  _EBOM,  // Extended Backward Oracle Matching
  _FBOM,  // Forward Backward Oracle Matching
  _SEBOM, // Simplified Extended Backward Oracle Matching
  _SFBOM, // Simplified Forward Backward Oracle Matching
  _SBDM,  // Succint Backward DAWG Matching
  _SKIP2,
  _SKIP3,
  _SKIP4,
  _SKIP5,
  _SKIP6,
  _SKIP7,
  _SKIP8,
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
  _SBNDM_BMH,// SBNDM with Horspool Shift
  _BMH_SBNDM,// Horspool with SBNDM test
  _FNDM,     // Forward Nondeterministic DAWG Matching
  _BWW,      // Bit parallel Wide Window
  _FAOSO2,   // Fast Average Optimal Shift-Or (u=2)
  _FAOSO4,   // Fast Average Optimal Shift-Or (u=2)
  _FAOSO6,   // Fast Average Optimal Shift-Or (u=2)
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
  
  // new algorithms, yet uncategorized
  _BSDM,
  _BSDM2,
  _BSDM3,
  _BSDM4,
  _BSDM5,
  _BSDM6,
  _BSDM7,
  _BSDM8,
  _BXS,
  _BXS1,
  _BXS2,
  _BXS3,
  _BXS4,
  _BXS6,
  _BXS8,
  _FS_W1,
  _FS_W2,
  _FS_W4,
  _FS_W6,
  _FS_W8,
  _FSBNDM_W1,
  _FSBNDM_W2,
  _FSBNDM_W4,
  _FSBNDM_W6,
  _FSBNDM_W8,
  _FSBNDMQ20,
  _FSBNDMQ21,
  _FSBNDMQ31,
  _FSBNDMQ32,
  _FSBNDMQ41,
  _FSBNDMQ42,
  _FSBNDMQ43,
  _FSBNDMQ61,
  _FSBNDMQ62,
  _FSBNDMQ64,
  _FSBNDMQ81,
  _FSBNDMQ82,
  _FSBNDMQ84,
  _FSBNDMQ86,
  _IOM,
  _JOM,
  _LWFR2,
  _LWFR3,
  _LWFR4,
  _LWFR5,
  _LWFR6,
  _LWFR7,
  _LWFR8,
  _QF23,
  _QF24,
  _QF26,
  _QF28,
  _QF33,
  _QF34,
  _QF36,
  _QF42,
  _QF43,
  _QF44,
  _QF62,
  _QF63,
  _QLQS,
  _SBNDM_W2,
  _SBNDM_W4,
  _SBNDM_W6,
  _SSM,
  _TSA,
  _TSA_Q2,
  _TSO5,
  _TUNEDBM,
  _TVSBS_W2,
  _TVSBS_W4,
  _TVSBS_W6,
  _TVSBS_W8,
  _WFR,      // Weak Factor Recognizer
  _WFR2,   
  _WFR3,
  _WFR4,
  _WFR5,
  _WFR6,
  _WFR7,
  _WFR8,
  _WFRQ2,    // Weak Factor Recognizer with q-grams
  _WFRQ3,
  _WFRQ4,
  _WFRQ5,
  _WFRQ6,
  _WFRQ7,
  _WFRQ8,
  _TWFR2,
  _TWFR3,
  _TWFR4,
  _TWFR5,
  _TWFR6,
  _TWFR7,
  _TWFR8,
  _TWFRQ2,
  _TWFRQ3,
  _TWFRQ4,
  _TWFRQ5,
  _TWFRQ6,
  _TWFRQ7,
  _TWFRQ8,
  _TWFR,
  _WC,       // Tailed Substring algorithm
  _WOM,      // Worst Occurrence Matcher
  // state of the art:
  _SSECP,    // SSE Crochemore-Perrin
  _EPSM,     // SSE4 Exact Packed String Matching
} ALGO_POS;

struct algo {
  const enum algo_id id;
  const int execute; // e.g. broken
  const char *name;
  const char *desc;
  const int minlen;
  const int missing;
};

// Note: This part may be edited by select
const struct algo ALGOS[] = {
    // Comparison based Algorithms
    [_BF] = {_BF, 1, "bf", "Brute Force", 0, 0},
    [_MP] = {_MP, 1, "mp", "Morris-Pratt", 0, 0},
    [_KMP] = {_KMP, 1, "kmp", "Knuth-Morris Pratt", 0, 0},
    [_BM] = {_BM, 1, "bm", "Boyer-Moore", 0, 0},
    [_HOR] = {_HOR, 1, "hor", "Hoorspool", 0, 0},
    [_GS] = {_GS, 1, "gs", "Galil Seiferas", 0, 0},
    [_AG] = {_AG, 1, "ag", "Apostolico-Giancarlo", 0, 0},
    [_KR] = {_KR, 1, "kr", "Karp-Rabin", 0, 0},
    [_ZT] = {_ZT, 1, "zt", "Zhu-Takaoka", 0, 0},
    [_AC] = {_AC, 1, "ac", "Apostolico-Crochemore", 0, 0},
    [_TW] = {_TW, 1, "tw", "Two-Way", 0, 0},
    [_OM] = {_OM, 1, "om", "Optimal Mismatch", 0, 0},
    [_MS] = {_MS, 1, "ms", "Maximal shift", 0, 0},
    [_QS] = {_QS, 1, "qs", "Quick-Search", 0, 0},
    [_TunBM] = {_TunBM, 0, "tunbm", "Tuned Boyer-Moore", 0, 0},
    [_NSN] = {_NSN, 0, "nsn", "not so naive", 2, 0},
    [_TBM] = {_TBM, 1, "tbm", "Turbo Boyer-Moore", 0, 0},
    [_Colussi] = {_Colussi, 1, "colussi", "Colussi", 0, 0},
    [_Smith] = {_Smith, 1, "smith", "Smith", 0, 0},
    [_GG] = {_GG, 1, "gg", "Galil-Giancarlo", 0, 0},
    [_Raita] = {_Raita, 1, "raita", "Raita", 0, 0},
    [_SMOA] = {_SMOA, 1, "smoa", "String Matching on Ordered Alphabets", 0, 0},
    [_RColussi] = {_RColussi, 1, "rcolussi", "Reverse Colussi", 0, 0},
    [_Skip] = {_Skip, 1, "skip", "Skip Search", 0, 0},
    [_KMPSkip] = {_KMPSkip, 1, "kmpskip", "KMP Skip Search", 0, 0},
    [_ASkip] = {_ASkip, 1, "askip", "Alpha Skip Search", 0, 0},
    [_BR] = {_BR, 1, "br", "Berry-Ravindran", 0, 0},
    [_AKC] = {_AKC, 1, "akc", "Ahmed-Kaykobad-Chowdhury", 0, 0},
    [_FS] = {_FS, 1, "fs", "Fast-Search", 0, 0},
    [_FFS] = {_FFS, 1, "ffs", "Forward-Fast-Search", 0, 0},
    [_BFS] = {_BFS, 1, "bfs", "Backward-Fast-Search, Fast Boyer-Moore", 0, 0},
    [_TS] = {_TS, 1, "ts", "Tailed-Substring", 0, 0},
    [_SSABS] = {_SSABS, 1, "ssabs", "Sheik-Sumit-Anindya-Balakrishnan-Sekar",
                0},
    [_TVSBS] = {_TVSBS, 1, "tvsbs", "Thathoo-Virmani-Sai-Balakrishnan-Sekar",
                0},
    [_PBMH] = {_PBMH, 1, "pbmh", "Boyer-Moore-Horspool using Probabilities", 0, 0},
    [_FJS] = {_FJS, 1, "fjs", "Franek-Jennings-Smyth", 0, 0},
    [_BLOCK] = {_BLOCK, 0, "block", "2-Block Boyer-Moore", 0, 1},
    [_HASH3] = {_HASH3, 1, "hash3",
                "Wu-Manber for Single Pattern Matching (q=3)", 0, 0},
    [_HASH5] = {_HASH5, 1, "hash5",
                "Wu-Manber for Single Pattern Matching (q=5)", 0, 0},
    [_HASH8] = {_HASH8, 1, "hash8",
                "Wu-Manber for Single Pattern Matching (q=8)", 0, 0},
    [_TSW] = {_TSW, 1, "tsw", "Two-Sliding-Window", 0, 0},
    [_BMH2] = {_BMH2, 0, "bmh2", "Boyer-Moore-Horspool with q-grams (BMH2)",
               2, 1},
    [_BMH4] = {_BMH4, 0, "bmh4", "Boyer-Moore-Horspool with q-grams (BMH4)",
               4, 1},
    [_GRASPm] = {_GRASPm, 1, "graspm",
                 "Genomic Rapid Algorithm for String Pattern-match", 0, 0},
    [_SSEF] = {_SSEF, X64_ONLY, "ssef", "SSEF (K=7)", 32, 0},
    // Algorithms based on automata
    [_AUT] = {_AUT, 1, "aut", "Automaton Matcher", 0, 0},
    [_RF] = {_RF, 1, "rf", "Reverse-Factor", 0, 0},
    [_TRF] = {_TRF, 1, "trf", "Turbo Reverse factor", 0, 0},
    [_Simon] = {_Simon, 1, "simon", "Simon", 0, 0},
    [_FDM] = {_FDM, 1, "fdm", "Forward-DAWG-Match", 0, 0},
    [_BOM] = {_BOM, 1, "bom", "BOM", 0, 0},
    [_BOM2] = {_BOM2, 1, "bom2", "BOM2", 0, 0},
    [_DFDM] = {_DFDM, 0, "dfdm", "Double Forward DAWG Matching", 0, 0}, // in work
    [_WW] = {_WW, 1, "ww", "Wide Window", 0, 0},
    [_LDM] = {_LDM, 0, "ldm", "Linear DAWG Matching", 0, 0},
    [_ILDM1] = {_ILDM1, 1, "ildm1", "ILDM1", 0, 0},
    [_ILDM2] = {_ILDM2, 1, "ildm2", "ILDM2", 0, 0},
    [_EBOM] = {_EBOM, 1, "ebom", "Extended Backward Oracle Matching", 0, 0},
    [_FBOM] = {_FBOM, 1, "fbom", "Forward Backward Oracle Matching", 0, 0},
    [_SEBOM] = {_SEBOM, 1, "sebom",
                "Simplified Extended Backward Oracle Matching", 0, 0},
    [_SFBOM] = {_SFBOM, 1, "sfbom",
                "Simplified Forward Backward Oracle Matching", 0, 0},
    [_SBDM] = {_SBDM, 0, "sbdm", "Succint Backward DAWG Matching", 0, 1},
    [_SKIP2] = {_SKIP2, 1, "skip2", "Skip Search with lookahead", 2, 0},
    [_SKIP3] = {_SKIP3, 1, "skip3", "Skip Search with lookahead", 3, 0},
    [_SKIP4] = {_SKIP4, 1, "skip4", "Skip Search with lookahead", 4, 0},
    [_SKIP5] = {_SKIP5, 1, "skip5", "Skip Search with lookahead", 5, 0},
    [_SKIP6] = {_SKIP6, 1, "skip6", "Skip Search with lookahead", 6, 0},
    [_SKIP7] = {_SKIP7, 1, "skip7", "Skip Search with lookahead", 7, 0},
    [_SKIP8] = {_SKIP8, 1, "skip8", "Skip Search with lookahead", 8, 0},
    // Algorithms based on bit-parallelism
    [_SO] = {_SO, 1, "so", "Shift-Or", 0, 0},
    [_SA] = {_SA, 1, "sa", "Shift-And", 0, 0},
    [_BNDM] = {_BNDM, 1, "bndm", "BNDM for Long patterns", 0, 0},
    [_BNDML] = {_BNDML, 1, "bndml", "BNDM for Long patterns", 0, 0},
    [_SBNDM] = {_SBNDM, 1, "sbndm", "Simplified BNDM", 0, 0},
    [_TNDM] = {_TNDM, 1, "tndm", "two-way nondeterministic dawg matching", 0, 0},
    [_TNDMa] = {_TNDMa, 1, "tndma",
                "Two-Way Nondeterministic DAWG Matching (version 2)", 0, 0},
    [_LBNDM] = {_LBNDM, 1, "lbndm", "long patterns bndm", 0, 0},
    [_SVM0] = {_SVM0, 1, "svm0", "shift vector matching (version 0)", 0, 0},
    [_SVM1] = {_SVM1, 1, "svm1", "shift vector matching (version 1)", 0, 0},
    [_SVM2] = {_SVM2, 1, "svm2", "shift vector matching (version 2)", 0, 0},
    [_SVM3] = {_SVM3, 1, "svm3", "shift vector matching (version 3)", 0, 0},
    [_SVM4] = {_SVM4, 1, "svm4", "shift vector matching (version 4)", 0, 0},
    [_SBNDM2] = {_SBNDM2, 1, "sbndm2", "simplified bndm with loop-unrolling",
                 0, 0},
    [_SBNDM_BMH] = {_SBNDM_BMH, 1, "sbndm-bmh", "sbndm with horspool shift", 2, 0},
    [_BMH_SBNDM] = {_BMH_SBNDM, 0, "bmh-sbndm", "Horspool with SBNDM test", 0, 0},
    [_FNDM] = {_FNDM, 1, "fndm", "forward nondeterministic dawg matching", 0, 0},
    [_BWW] = {_BWW, 1, "bww", "bit parallel wide window", 0, 0},
    [_FAOSO2] = {_FAOSO2, 1, "faoso2", "fast average optimal shift-or (u=2,m>2)",
                  3, 0},
    [_FAOSO4] = {_FAOSO4, 1, "faoso4", "fast average optimal shift-or (u=2,m>4)",
                  5, 0},
    [_FAOSO6] = {_FAOSO6, 0, "faoso6", "fast average optimal shift-or (u=2,m>6)",
                  7, 0},
    [_AOSO2] = {_AOSO2, 1, "aoso2", "average optimal shift-or (q=2)", 2, 0},
    [_AOSO4] = {_AOSO4, 1, "aoso4", "average optimal shift-or (q=4)", 4, 0},
    [_AOSO6] = {_AOSO6, 1, "aoso6", "average optimal shift-or (q=6)", 6, 0},
    [_BLIM] = {_BLIM, 1, "blim", "bit-parallel length invariant matcher", 0, 0},
    [_FSBNDM] = {_FSBNDM, 0, "fsbndm", "forward sbndm", 0, 0}, // broken m>31
    [_BNDMq2] = {_BNDMq2, 1, "bndmq2", "bndm with q-grams", 2, 0},
    [_BNDMq4] = {_BNDMq4, 1, "bndmq4", "bndm with q-grams", 4, 0},
    [_BNDMq6] = {_BNDMq6, 1, "bndmq6", "bndm with q-grams", 6, 0},
    [_SBNDMq2] = {_SBNDMq2, 1, "sbndmq2", "simplified bndm with q-grams", 2, 0},
    [_SBNDMq4] = {_SBNDMq4, 1, "sbndmq4", "simplified bndm with q-grams", 4, 0},
    [_SBNDMq6] = {_SBNDMq6, 1, "sbndmq6", "simplified bndm with q-grams", 6, 0},
    [_SBNDMq8] = {_SBNDMq8, 1, "sbndmq8", "simplified bndm with q-grams", 8, 0},
    [_UFNDMq2] = {_UFNDMq2, 1, "ufndmq2", "shift-or with q-grams", 2, 0},
    [_UFNDMq4] = {_UFNDMq4, 1, "ufndmq4", "shift-or with q-grams", 4, 0},
    [_UFNDMq6] = {_UFNDMq6, 1, "ufndmq6", "shift-or with q-grams", 6, 0},
    [_UFNDMq8] = {_UFNDMq8, 1, "ufndmq8", "shift-or with q-grams", 8, 0},
    [_SABP] = {_SABP, 1, "sabp", "Small Alphabet Bit Parallel", 0, 0},
    [_DBWW] = {_DBWW, 1, "dbww", "DOUBLE BWW", 0, 0},
    [_DBWW2] = {_DBWW2, 1, "dbww2", "DOUBLE BWW", 0, 0},
    [_KSA] = {_KSA, 1, "ksa", "Factorized SHIFT-AND", 0, 0},
    [_KBNDM] = {_KBNDM, 1, "kbndm", "Factorized BNDM", 0, 0},
    // new algorithms
    [_BSDM] = {_BSDM, 1, "bsdm", "Backward SNR DAWG Matching", 0, 0},
    [_BSDM2] = {_BSDM2, 1, "bsdm2", "Backward SNR DAWG Matching (m>=2)", 2, 0},
    [_BSDM3] = {_BSDM3, 1, "bsdm3", "Backward SNR DAWG Matching (m>=3)", 3, 0},
    [_BSDM4] = {_BSDM4, 1, "bsdm4", "Backward SNR DAWG Matching (m>=4)", 4, 0},
    [_BSDM5] = {_BSDM5, 1, "bsdm5", "Backward SNR DAWG Matching (m>=5)", 5, 0},
    [_BSDM6] = {_BSDM6, 1, "bsdm6", "Backward SNR DAWG Matching (m>=6)", 6, 0},
    [_BSDM7] = {_BSDM7, 1, "bsdm7", "Backward SNR DAWG Matching (m>=7)", 7, 0},
    [_BSDM8] = {_BSDM8, 1, "bsdm8", "Backward SNR DAWG Matching (m>=8)", 8, 0},
    [_BXS] = {_BXS, 1, "bxs", "BXS", 0, 0}, // inexact>32
    [_BXS1] = {_BXS1, 1, "bxs1", "BXS with q-grams limit", 1, 0},
    [_BXS2] = {_BXS2, 1, "bxs2", "BXS with q-grams limit", 2, 0},
    [_BXS3] = {_BXS3, 1, "bxs3", "BXS with q-grams limit", 3, 0},
    [_BXS4] = {_BXS4, 1, "bxs4", "BXS with q-grams limit", 4, 0},
    [_BXS6] = {_BXS6, 1, "bxs6", "BXS with q-grams limit", 6, 0},
    [_BXS8] = {_BXS8, 1, "bxs8", "BXS with q-grams limit", 8, 0},
    [_FS_W1] = {_FS_W1, 1, "fs-w1", "Multiple Sliding Windows", 0, 0},
    [_FS_W2] = {_FS_W2, 0, "fs-w2", "Multiple Sliding Windows", 0, 0},
    [_FS_W4] = {_FS_W4, 0, "fs-w4", "Multiple Sliding Windows", 0, 0}, // n>=6
    [_FS_W6] = {_FS_W6, 1, "fs-w6", "Multiple Sliding Windows", 0, 0}, // n>=8
    [_FS_W8] = {_FS_W8, 1, "fs-w8", "Multiple Sliding Windows", 0, 0},
    [_FSBNDM_W1] = {_FSBNDM_W1, 1, "fsbndm-w1", "", 0, 0}, //inexact >31
    [_FSBNDM_W2] = {_FSBNDM_W2, 1, "fsbndm-w2", "", 0, 0}, //inexact >31
    [_FSBNDM_W4] = {_FSBNDM_W4, 1, "fsbndm-w4", "", 0, 0}, //inexact >31
    [_FSBNDM_W6] = {_FSBNDM_W6, 1, "fsbndm-w6", "", 0, 0}, //inexact >31
    [_FSBNDM_W8] = {_FSBNDM_W8, 1, "fsbndm-w8", "", 11, 0}, //inexact >31
    [_FSBNDMQ20] = {_FSBNDMQ20, 1, "fsbndmq20", "", 2, 0}, // m: 2..32
    [_FSBNDMQ21] = {_FSBNDMQ21, 1, "fsbndmq21", "", 2, 0}, // m: 2..31
    [_FSBNDMQ31] = {_FSBNDMQ31, 1, "fsbndmq31", "", 3, 0}, // m: 3..31
    [_FSBNDMQ32] = {_FSBNDMQ32, 0, "fsbndmq32", "", 3, 0}, // m: 3..30
    [_FSBNDMQ41] = {_FSBNDMQ41, 1, "fsbndmq41", "", 4, 0}, // m: 4..31
    [_FSBNDMQ42] = {_FSBNDMQ42, 0, "fsbndmq42", "", 4, 0}, // m: 4..30
    [_FSBNDMQ43] = {_FSBNDMQ43, 0, "fsbndmq43", "", 4, 0}, // m: 4..29
    [_FSBNDMQ61] = {_FSBNDMQ61, 1, "fsbndmq61", "", 6, 0}, // m: 6..31
    [_FSBNDMQ62] = {_FSBNDMQ62, 0, "fsbndmq62", "", 6, 0}, // m: 6..30
    [_FSBNDMQ64] = {_FSBNDMQ64, 0, "fsbndmq64", "", 6, 0}, // m: 6..28
    [_FSBNDMQ81] = {_FSBNDMQ81, 1, "fsbndmq81", "", 8, 0}, // m: 8..31
    [_FSBNDMQ82] = {_FSBNDMQ82, 0, "fsbndmq82", "", 8, 0}, // m: 8..30
    [_FSBNDMQ84] = {_FSBNDMQ84, 0, "fsbndmq84", "", 8, 0}, // m: 8..28
    [_FSBNDMQ86] = {_FSBNDMQ86, 0, "fsbndmq86", "", 8, 0}, // m: 8..26
    [_IOM] = {_IOM, 1, "iom", "Improved Occurrence Heuristic", 0, 0},
    [_JOM] = {_JOM, 1, "jom", "Jumping Occurrence Matcher", 2, 0},
    [_LWFR2] = {_LWFR2, 1, "lwfr2", "Weak Factor Recognizer, Linear Version", 2, 0},
    [_LWFR3] = {_LWFR3, 1, "lwfr3", "Weak Factor Recognizer, Linear Version", 3, 0},
    [_LWFR4] = {_LWFR4, 1, "lwfr4", "Weak Factor Recognizer, Linear Version", 4, 0},
    [_LWFR5] = {_LWFR5, 1, "lwfr5", "Weak Factor Recognizer, Linear Version", 5, 0},
    [_LWFR6] = {_LWFR6, 1, "lwfr6", "Weak Factor Recognizer, Linear Version", 6, 0},
    [_LWFR7] = {_LWFR7, 1, "lwfr7", "Weak Factor Recognizer, Linear Version", 7, 0},
    [_LWFR8] = {_LWFR8, 1, "lwfr8", "Weak Factor Recognizer, Linear Version", 8, 0},
    [_QF23] = {_QF23, 1, "qf23", "Q-gram Filtering q=2 s=3", 3, 0},
    [_QF24] = {_QF24, 1, "qf24", "Q-gram Filtering q=2 s=4", 3, 0},
    [_QF26] = {_QF26, 1, "qf26", "Q-gram Filtering q=2 s=6", 3, 0},
    [_QF28] = {_QF28, 1, "qf28", "Q-gram Filtering q=2 s=8", 3, 0},
    [_QF33] = {_QF33, 1, "qf33", "Q-gram Filtering q=3 s=3", 4, 0},
    [_QF34] = {_QF34, 1, "qf34", "Q-gram Filtering q=3 s=4", 4, 0},
    [_QF36] = {_QF36, 1, "qf36", "Q-gram Filtering q=3 s=6", 4, 0},
    [_QF42] = {_QF42, 1, "qf42", "Q-gram Filtering q=4 s=2", 5, 0},
    [_QF43] = {_QF43, 1, "qf43", "Q-gram Filtering q=4 s=3", 5, 0},
    [_QF44] = {_QF44, 1, "qf44", "Q-gram Filtering q=4 s=4", 5, 0},
    [_QF62] = {_QF62, 1, "qf62", "Q-gram Filtering q=6 s=2", 7, 0},
    [_QF63] = {_QF63, 1, "qf63", "Q-gram Filtering q=6 s=3", 7, 0},
    [_QLQS] = {_QLQS, 1, "qlqs", "Quantum Leap Quick Search", 0, 0},
    [_SBNDM_W2] = {_SBNDM_W2, 1, "sbndm-w2", "", 0, 0}, //inexact >32
    [_SBNDM_W4] = {_SBNDM_W4, 1, "sbndm-w4", "", 0, 0}, //inexact >32
    [_SBNDM_W6] = {_SBNDM_W6, 1, "sbndm-w6", "", 0, 0}, //inexact >32
    [_SSM] = {_SSM, 1, "ssm", "SSM", 0, 0},
    [_TSA] = {_TSA, 1, "tsa", "word-wise popcount", 0, 0}, // max 64
    [_TSA_Q2] = {_TSA_Q2, 0, "tsa-q2", "word-wise popcount q=2", 2, 0}, // max: 64
    [_TSO5] = {_TSO5, 1, "tso5", "optimized word-wise popcount", 0, 0}, // max: 64
    [_TUNEDBM] = {_TUNEDBM, 1, "tunedbm", "Tuned Boyer-Moore", 0, 0},
    [_TVSBS_W2] = {_TVSBS_W2, 1, "tvsbs-w2", "TVSBS", 0, 0},
    [_TVSBS_W4] = {_TVSBS_W4, 0, "tvsbs-w4", "TVSBS", 2, 0},
    [_TVSBS_W6] = {_TVSBS_W6, 0, "tvsbs-w6", "TVSBS", 2, 0},
    [_TVSBS_W8] = {_TVSBS_W8, 0, "tvsbs-w8", "TVSBS", 2, 0},
    [_WFR] = {_WFR, 1, "wfr", "Weak Factor Recognizer", 0, 0},
    [_WFR2] = {_WFR2, 1, "wfr2", "Weak Factor Recognizer (m>=2)", 2, 0},
    [_WFR3] = {_WFR3, 1, "wfr3", "Weak Factor Recognizer (m>=3)", 3, 0},
    [_WFR4] = {_WFR4, 1, "wfr4", "Weak Factor Recognizer (m>=4)", 4, 0},
    [_WFR5] = {_WFR5, 1, "wfr5", "Weak Factor Recognizer (m>=5)", 5, 0},
    [_WFR6] = {_WFR6, 1, "wfr6", "Weak Factor Recognizer (m>=6)", 6, 0},
    [_WFR7] = {_WFR7, 1, "wfr7", "Weak Factor Recognizer (m>=7)", 7, 0},
    [_WFR8] = {_WFR8, 1, "wfr8", "Weak Factor Recognizer (m>=8)", 8, 0},
    [_WFRQ2] = {_WFRQ2, 1, "wfrq2", "Weak Factor Recognizer with q-grams", 2, 0},
    [_WFRQ3] = {_WFRQ3, 1, "wfrq3", "Weak Factor Recognizer with q-grams", 3, 0},
    [_WFRQ4] = {_WFRQ4, 1, "wfrq4", "Weak Factor Recognizer with q-grams", 4, 0},
    [_WFRQ5] = {_WFRQ5, 1, "wfrq5", "Weak Factor Recognizer with q-grams", 5, 0},
    [_WFRQ6] = {_WFRQ6, 1, "wfrq6", "Weak Factor Recognizer with q-grams", 6, 0},
    [_WFRQ7] = {_WFRQ7, 1, "wfrq7", "Weak Factor Recognizer with q-grams", 7, 0},
    [_WFRQ8] = {_WFRQ8, 1, "wfrq8", "Weak Factor Recognizer with q-grams", 8, 0},
    [_TWFR] = {_TWFR, 1, "twfr", "Tuned Weak Factor Recognizer", 0, 0},
    [_TWFR2] = {_TWFR2, 1, "twfr2", "Tuned Weak Factor Recognizer", 0, 0},
    [_TWFR3] = {_TWFR3, 1, "twfr3", "Tuned Weak Factor Recognizer", 0, 0},
    [_TWFR4] = {_TWFR4, 1, "twfr4", "Tuned Weak Factor Recognizer", 0, 0},
    [_TWFR5] = {_TWFR5, 1, "twfr5", "Tuned Weak Factor Recognizer", 0, 0},
    [_TWFR6] = {_TWFR6, 1, "twfr6", "Tuned Weak Factor Recognizer", 0, 0},
    [_TWFR7] = {_TWFR7, 1, "twfr7", "Tuned Weak Factor Recognizer", 0, 0},
    [_TWFR8] = {_TWFR8, 1, "twfr8", "Tuned Weak Factor Recognizer", 0, 0},
    [_TWFRQ2] = {_TWFRQ2, 1, "twfrq2", "Tuned Weak Factor Recognizer with q-grams", 2, 0},
    [_TWFRQ3] = {_TWFRQ3, 1, "twfrq3", "Tuned Weak Factor Recognizer with q-grams", 3, 0},
    [_TWFRQ4] = {_TWFRQ4, 1, "twfrq4", "Tuned Weak Factor Recognizer with q-grams", 4, 0},
    [_TWFRQ5] = {_TWFRQ5, 1, "twfrq5", "Tuned Weak Factor Recognizer with q-grams", 5, 0},
    [_TWFRQ6] = {_TWFRQ6, 1, "twfrq6", "Tuned Weak Factor Recognizer with q-grams", 6, 0},
    [_TWFRQ7] = {_TWFRQ7, 1, "twfrq7", "Tuned Weak Factor Recognizer with q-grams", 7, 0},
    [_TWFRQ8] = {_TWFRQ8, 1, "twfrq8", "Tuned Weak Factor Recognizer with q-grams", 8, 0},
    [_WC] = {_WC, 1, "wc", "Tailed Substring algorithm", 0, 0},
    [_WOM] = {_WOM, 1, "wom", "Worst Occurrence Matcher", 0, 0},
    // state of the art:
    [_SSECP] = {_SSECP, 0, "ssecp", "SSE Crochemore-Perrin", 0, 0}, // broken
    [_EPSM] = {_EPSM, X64_ONLY, "epsm", "SSE4 Exact Packed String Matching", 0, 0},
};

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define NumAlgo ARRAY_SIZE(ALGOS)

#endif // _ALGORITHMS_H
