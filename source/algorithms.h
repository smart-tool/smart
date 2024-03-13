#ifndef _ALGORITHMS_H
#define _ALGORITHMS_H

#ifdef __x86_64__
#define X64_ONLY 1
#else
#define X64_ONLY 0
#endif

#if !(defined(_WIN32) || defined(__AVR__))
#define HAVE_SHM
#endif

enum algo_id {
  // Comparison based Algorithms
  _BF,       // Brute Force
  _MP,       // Morris-Pratt
  _KMP,      // Knuth-Morris Pratt
  _BM,       // Boyer-Moore
  _HOR,      // Hoorspool
  _GS,       // Galil Seiferas
  _AG,       // Apostolico-Giancarlo
  _KR,       // Karp-Rabin
  _ZT,       // Zhu-Takaoka
  _AC,       // Apostolico-Crochemore
  _TW,       // Two-Way
  _OM,       // Optimal Mismatch
  _MS,       // Maximal shift
  _QS,       // Quick-Search
  _TunBM,    // Tuned Boyer-Moore
  _NSN,      // not so naive
  _TBM,      // Turbo Boyer-Moore
  _Colussi,  // Colussi
  _Smith,    // Smith
  _GG,       // Galil-Giancarlo
  _Raita,    // Raita
  _SMOA,     // String Matching on Ordered Alphabets
  _RColussi, // Reverse Colussi
  _Skip,     // Skip Search
  _KMPSkip,  // KMP skip search
  _ASkip,    // Alpha-Skip-Search
  _BR,       // Berry-ravindran
  _AKC,      // AKC algorithm
  _FS,       // Fast-Search
  _FFS,      // Forward-Fast-Search
  _BFS,      // Backward-Fast-Search, Fast-Boyer-Moore
  _TS,       // Tailed-Substring
  _SSABS,    // Sheik-Sumit-Anindya-Balakrishnan-Sekar
  _TVSBS,    // Thathoo-Virmani-Sai-Balakrishnan-Sekar
  _PBMH,     // Boyer-Moore-Horspool using Probabilities
  _FJS,      // Franek-Jennings-Smyth
  _BLOCK,    // 2-Block Boyer-Moore (missing)
  _HASH3,    // Wu-Manber for Single Pattern Matching (q=3)
  _HASH5,    // Wu-Manber for Single Pattern Matching (q=5)
  _HASH8,    // Wu-Manber for Single Pattern Matching (q=8)
  //_SM,    // Wu-Manber
  //_SWM,   // Simplified Wu-Manber
  _TSW,    // Two-Sliding-Window
  _BMH2,   // Boyer-Moore-Horspool with q-grams (missing)
  _BMH4,   // Boyer-Moore-Horspool with q-grams (missing)
  _GRASPm, // Genomic Rapid Algorithm for String Pattern-match
  _SSEF,   // SSEF (K=7)
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
  _BSOM,  // Backward Set Oracle Matching
  _SKIP2, // Skip Search with lookahead
  _SKIP3, // Skip Search with lookahead
  _SKIP4, // Skip Search with lookahead
  _SKIP5, // Skip Search with lookahead
  _SKIP6, // Skip Search with lookahead
  _SKIP7, // Skip Search with lookahead
  _SKIP8, // Skip Search with lookahead
  // Algorithms based on bit-parallelism
  _SO, // Shift-Or
  // _SOG,    // Parallel Shift-Or
  _SA,   // Shift-And
  _BNDM, // BNDM for Long patterns
  // BM_BNDM
  // Turbo_BNDM
  _BNDML,     // BNDM for Long patterns
  _SBNDM,     // Simplified BNDM
  _TNDM,      // Two-Way Nondeterministic DAWG Matching
  _TNDMa,     // Two-Way Nondeterministic DAWG Matching (version 2)
  _LBNDM,     // Long patterns BNDM
  _SVM0,      // Shift Vector Matching (version 0)
  _SVM1,      // Shift Vector Matching (version 1)
  _SVM2,      // Shift Vector Matching (version 2)
  _SVM3,      // Shift Vector Matching (version 3)
  _SVM4,      // Shift Vector Matching (version 4)
  _SBNDM2,    // Simplified BNDM with loop-unrolling
  _SBNDM_BMH, // SBNDM with Horspool Shift
  _BMH_SBNDM, // Horspool with SBNDM test
  _FNDM,      // Forward Nondeterministic DAWG Matching
  _BWW,       // Bit parallel Wide Window
  _FAOSO2,    // Fast Average Optimal Shift-Or (u=2)
  _FAOSO4,    // Fast Average Optimal Shift-Or (u=2)
  _FAOSO6,    // Fast Average Optimal Shift-Or (u=2)
  _AOSO2,     // Average Optimal Shift-Or (q=2)
  _AOSO4,     // Average Optimal Shift-Or (q=4)
  _AOSO6,     // Average Optimal Shift-Or (q=6)
  _BLIM,      // Bit-Parallel Length Invariant Matcher
  _FSBNDM,    // Forward Simplified BNDM
  // TODO HG, BG BNDM with q-grams, in Salmela et al. 2006
  _BNDMq2,  // BNDM with q-grams
  _BNDMq4,  // BNDM with q-grams
  _BNDMq6,  // BNDM with q-grams
  _SBNDMq2, // Simplified BNDM with q-grams
  _SBNDMq4, // Simplified BNDM with q-grams
  _SBNDMq6, // Simplified BNDM with q-grams
  _SBNDMq8, // Simplified BNDM with q-grams
  _UFNDMq2, // Shift-Or with q-grams
  _UFNDMq4, // Shift-Or with q-grams
  _UFNDMq6, // Shift-Or with q-grams
  _UFNDMq8, // Shift-Or with q-grams
  _SABP,    // Small Alphabet Bit Parallel
  _DBWW,    // DOUBLE BWW
  _DBWW2,   // DOUBLE BWW
  _KSA,     // Factorized SHIFT-AND
  _KBNDM,   // Factorized BNDM

  // new algorithms, yet uncategorized
  _BSDM,      // Backward SNR DAWG Matching
  _BSDM2,     // Backward SNR DAWG Matching (m>=2)
  _BSDM3,     // Backward SNR DAWG Matching (m>=3)
  _BSDM4,     // Backward SNR DAWG Matching (m>=4)
  _BSDM5,     // Backward SNR DAWG Matching (m>=5)
  _BSDM6,     // Backward SNR DAWG Matching (m>=6)
  _BSDM7,     // Backward SNR DAWG Matching (m>=7)
  _BSDM8,     // Backward SNR DAWG Matching (m>=8)
  _BXS,       // BXS
  _BXS1,      // BXS with q-grams limit
  _BXS2,      // BXS with q-grams limit
  _BXS3,      // BXS with q-grams limit
  _BXS4,      // BXS with q-grams limit
  _BXS6,      // BXS with q-grams limit
  _BXS8,      // BXS with q-grams limit
  _FS_W1,     // Multiple Sliding Windows
  _FS_W2,     // Multiple Sliding Windows
  _FS_W4,     // Multiple Sliding Windows
  _FS_W6,     // Multiple Sliding Windows
  _FS_W8,     // Multiple Sliding Windows
  _FSBNDM_W1, // Forward SBNDM with multiple sliding windows
  _FSBNDM_W2,
  _FSBNDM_W4,
  _FSBNDM_W6,
  _FSBNDM_W8,
  _FSBNDMQ20, // Forward SBNDM with q-grams and lookahead
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
  _IOM,   // Improved Occurrence Heuristic
  _JOM,   // Jumping Occurrence Matcher
  _LWFR2, // Weak Factor Recognizer, Linear Version
  _LWFR3, // Weak Factor Recognizer, Linear Version
  _LWFR4, // Weak Factor Recognizer, Linear Version
  _LWFR5, // Weak Factor Recognizer, Linear Version
  _LWFR6, // Weak Factor Recognizer, Linear Version
  _LWFR7, // Weak Factor Recognizer, Linear Version
  _LWFR8, // Weak Factor Recognizer, Linear Version
  _QF23,  // Q-gram Filtering q=2 s=3
  _QF24,  // Q-gram Filtering q=2 s=4
  _QF26,  // Q-gram Filtering q=2 s=6
  _QF28,  // Q-gram Filtering q=2 s=8
  _QF33,  // Q-gram Filtering q=3 s=3
  _QF34,  // Q-gram Filtering q=3 s=4
  _QF36,  // Q-gram Filtering q=3 s=6
  _QF42,  // Q-gram Filtering q=4 s=2
  _QF43,  // Q-gram Filtering q=4 s=3
  _QF44,  // Q-gram Filtering q=4 s=4
  _QF62,  // Q-gram Filtering q=6 s=2
  _QF63,  // Q-gram Filtering q=6 s=3
  _QLQS,  // Quantum Leap Quick Search
  _SBNDM_W2,
  _SBNDM_W4,
  _SBNDM_W6,
  _SSM,
  _TSA,     // word-wise popcount
  _TSA_Q2,  // word-wise popcount q=2
  _TSO5,    // optimized word-wise popcount
  _TUNEDBM, // Tuned Boyer-Moore
  _TVSBS_W2,
  _TVSBS_W4,
  _TVSBS_W6,
  _TVSBS_W8,
  _WFR, // Weak Factor Recognizer
  _WFR2,
  _WFR3,
  _WFR4,
  _WFR5,
  _WFR6,
  _WFR7,
  _WFR8,
  _WFRQ2, // Weak Factor Recognizer with q-grams
  _WFRQ3,
  _WFRQ4,
  _WFRQ5,
  _WFRQ6,
  _WFRQ7,
  _WFRQ8,
  _TWFR, // Tuned Weak Factor Recognizer
  _TWFR2,
  _TWFR3,
  _TWFR4,
  _TWFR5,
  _TWFR6,
  _TWFR7,
  _TWFR8,
  _TWFRQ2, // Tuned Weak Factor Recognizer qith q-grams
  _TWFRQ3,
  _TWFRQ4,
  _TWFRQ5,
  _TWFRQ6,
  _TWFRQ7,
  _TWFRQ8,
  _WC,  // Tailed Substring algorithm
  _WOM, // Worst Occurrence Matcher
  _DOUBLEHASH, // Bicer-Zhang 2019
  // state of the art:
  _FT3,   // Parallel Boyer-Moore with goodsuff, Lecroq 2024
  _HPBM,  // HP Parallel Boyer-Moore, Jeong et al 2015
  _SSECP, // SSE Crochemore-Perrin
  _EPSM,  // SSE4 Exact Packed String Matching
  _LIBC,  // strstr
  _LIBC1, // memmem
  _MUSL,  // musl strstr
  _MUSL1, // musl memmem
  _SIMDKR, // SIMD generic Rabin-Karp variants (SSE2, AVX2 and NEON)
} ALGO_POS;

struct algo {
  const enum algo_id id;
  const int execute; // e.g. broken
  const char *name;
  const char *desc;
  const int minlen;
  const int missing;
};

#define OK 1
#define FAIL 0
#define MISSING 1

// Note: This part may be edited by select
const struct algo ALGOS[] = {
    // clang-format off
  // Comparison based Algorithms
  [_BF] = {_BF, OK, "bf", "Brute Force", 0, 0},
  [_MP] = {_MP, OK, "mp", "Morris-Pratt", 0, 0},
  [_KMP] = {_KMP, OK, "kmp", "Knuth-Morris Pratt", 0, 0},
  [_BM] = {_BM, OK, "bm", "Boyer-Moore", 0, 0},
  [_HOR] = {_HOR, OK, "hor", "Hoorspool", 0, 0},
  [_GS] = {_GS, FAIL, "gs", "Galil Seiferas", 0, 0},
  [_AG] = {_AG, OK, "ag", "Apostolico-Giancarlo", 0, 0},
  [_KR] = {_KR, OK, "kr", "Karp-Rabin", 0, 0},
  [_ZT] = {_ZT, OK, "zt", "Zhu-Takaoka", 0, 0},
  [_AC] = {_AC, OK, "ac", "Apostolico-Crochemore", 0, 0},
  [_TW] = {_TW, OK, "tw", "Two-Way", 0, 0},
  [_OM] = {_OM, OK, "om", "Optimal Mismatch", 0, 0},
  [_MS] = {_MS, OK, "ms", "Maximal shift", 0, 0},
  [_QS] = {_QS, OK, "qs", "Quick-Search", 0, 0},
  [_TunBM] = {_TunBM, FAIL, "tunbm", "Tuned Boyer-Moore", 0, 0},
  [_NSN] = {_NSN, OK, "nsn", "not so naive", 2, 0},
  [_TBM] = {_TBM, OK, "tbm", "Turbo Boyer-Moore", 0, 0},
  [_Colussi] = {_Colussi, OK, "colussi", "Colussi", 0, 0},
  [_Smith] = {_Smith, OK, "smith", "Smith", 0, 0},
  [_GG] = {_GG, OK, "gg", "Galil-Giancarlo", 0, 0},
  [_Raita] = {_Raita, OK, "raita", "Raita", 0, 0},
  [_SMOA] = {_SMOA, OK, "smoa", "String Matching on Ordered Alphabets", 0, 0},
  [_RColussi] = {_RColussi, OK, "rcolussi", "Reverse Colussi", 0, 0},
  [_Skip] = {_Skip, OK, "skip", "Skip Search", 0, 0},
  [_KMPSkip] = {_KMPSkip, OK, "kmpskip", "KMP Skip Search", 0, 0},
  [_ASkip] = {_ASkip, OK, "askip", "Alpha Skip Search", 0, 0},
  [_BR] = {_BR, OK, "br", "Berry-Ravindran", 0, 0},
  [_AKC] = {_AKC, OK, "akc", "Ahmed-Kaykobad-Chowdhury", 0, 0},
  [_FS] = {_FS, OK, "fs", "Fast-Search", 0, 0},
  [_FFS] = {_FFS, OK, "ffs", "Forward-Fast-Search", 0, 0},
  [_BFS] = {_BFS, OK, "bfs", "Backward-Fast-Search, Fast Boyer-Moore", 0, 0},
  [_TS] = {_TS, OK, "ts", "Tailed-Substring", 0, 0},
  [_SSABS] = {_SSABS, OK, "ssabs", "Sheik-Sumit-Anindya-Balakrishnan-Sekar", 0, 0},
  [_TVSBS] = {_TVSBS, OK, "tvsbs", "Thathoo-Virmani-Sai-Balakrishnan-Sekar", 0, 0},
  [_PBMH] = {_PBMH, OK, "pbmh", "Boyer-Moore-Horspool using Probabilities", 0, 0},
  [_FJS] = {_FJS, OK, "fjs", "Franek-Jennings-Smyth", 0, 0},
  [_BLOCK] = {_BLOCK, FAIL, "block", "2-Block Boyer-Moore", 0, MISSING},
  [_HASH3] = {_HASH3, OK, "hash3", "Wu-Manber for Single Pattern Matching (q=3)", 0, 0},
  [_HASH5] = {_HASH5, OK, "hash5", "Wu-Manber for Single Pattern Matching (q=5)", 0, 0},
  [_HASH8] = {_HASH8, OK, "hash8", "Wu-Manber for Single Pattern Matching (q=8)", 0, 0},
  //[_WM] = {_WM, FAIL, "wm", "Wu-Manber", 0, MISSING},
  // S. Wu and U. Manber, Agrep - A Fast Approximate Pattern-Matching Tool, In
  // Proceedings of USENIX Technical Conference,(1992), pp. 153–162
  //[_SWM] = {_SWM, FAIL, "swm", "Simplified Wu-Manber", 0, MISSING},
  // L. Vespa and N. Weng, SWM: Simplified Wu-Manber for GPU-based Deep Packet
  // Inspection, in Proceedings of the 2012 International Conference on
  // Security and Management, 2012.
  [_TSW] = {_TSW, OK, "tsw", "Two-Sliding-Window", 0, 0},
  [_BMH2] = {_BMH2, FAIL, "bmh2", "Boyer-Moore-Horspool with q-grams (BMH2)", 2, MISSING},
  [_BMH4] = {_BMH4, FAIL, "bmh4", "Boyer-Moore-Horspool with q-grams (BMH4)", 4, MISSING},
  [_GRASPm] = {_GRASPm, OK, "graspm", "Genomic Rapid Algorithm for String Pattern-match", 0, 0},
  [_SSEF] = {_SSEF, X64_ONLY, "ssef", "SSEF (K=7)", 32, 0},
  // Algorithms based on automata
  [_AUT] = {_AUT, OK, "aut", "Automaton Matcher", 0, 0},
  [_RF] = {_RF, OK, "rf", "Reverse-Factor", 0, 0},
  [_TRF] = {_TRF, OK, "trf", "Turbo Reverse factor", 0, 0},
  [_Simon] = {_Simon, FAIL, "simon", "Simon", 0, 0},
  [_FDM] = {_FDM, OK, "fdm", "Forward-DAWG-Match", 0, 0},
  [_BOM] = {_BOM, OK, "bom", "BOM", 0, 0},
  [_BOM2] = {_BOM2, OK, "bom2", "BOM2", 0, 0},
  [_DFDM] = {_DFDM, OK, "dfdm", "Double Forward DAWG Matching", 0, 0},
  [_WW] = {_WW, OK, "ww", "Wide Window", 0, 0},
  [_LDM] = {_LDM, FAIL, "ldm", "Linear DAWG Matching", 0, 0},
  [_ILDM1] = {_ILDM1, OK, "ildm1", "ILDM1", 0, 0},
  [_ILDM2] = {_ILDM2, OK, "ildm2", "ILDM2", 0, 0},
  [_EBOM] = {_EBOM, OK, "ebom", "Extended Backward Oracle Matching", 0, 0},
  [_FBOM] = {_FBOM, OK, "fbom", "Forward Backward Oracle Matching", 0, 0},
  [_SEBOM] = {_SEBOM, OK, "sebom", "Simplified Extended Backward Oracle Matching", 0, 0},
  [_SFBOM] = {_SFBOM, OK, "sfbom", "Simplified Forward Backward Oracle Matching", 0, 0},
  [_SBDM] = {_SBDM, FAIL, "sbdm", "Succint Backward DAWG Matching", 0, MISSING},
  [_BSOM] = {_BSOM, FAIL, "bsom", "Backward Set Oracle Matching", 0, MISSING},
  [_SKIP2] = {_SKIP2, OK, "skip2", "Skip Search with lookahead", 2, 0},
  [_SKIP3] = {_SKIP3, OK, "skip3", "Skip Search with lookahead", 3, 0},
  [_SKIP4] = {_SKIP4, OK, "skip4", "Skip Search with lookahead", 4, 0},
  [_SKIP5] = {_SKIP5, OK, "skip5", "Skip Search with lookahead", 5, 0},
  [_SKIP6] = {_SKIP6, OK, "skip6", "Skip Search with lookahead", 6, 0},
  [_SKIP7] = {_SKIP7, OK, "skip7", "Skip Search with lookahead", 7, 0},
  [_SKIP8] = {_SKIP8, OK, "skip8", "Skip Search with lookahead", 8, 0},
  // Algorithms based on bit-parallelism
  [_SO] = {_SO, OK, "so", "Shift-Or", 0, 0},
  //[_SOG] = {_SOG, OK, "sog", "Parallel Shift-Or", 0, MISSING},
  [_SA] = {_SA, OK, "sa", "Shift-And", 0, 0},
  [_BNDM] = {_BNDM, OK, "bndm", "BNDM for Long patterns", 0, 0},
  [_BNDML] = {_BNDML, OK, "bndml", "BNDM for Long patterns", 0, 0},
  [_SBNDM] = {_SBNDM, OK, "sbndm", "Simplified BNDM", 0, 0},
  [_TNDM] = {_TNDM, OK, "tndm", "Two-Way Nondeterministic DAWG Matching", 0, 0},
  [_TNDMa] = {_TNDMa, OK, "tndma", "Two-Way Nondeterministic DAWG Matching (version 2)", 0, 0},
  [_LBNDM] = {_LBNDM, OK, "lbndm", "long patterns bndm", 0, 0},
  // hg BNDM with q-grams
  // bg BNDM with q-grams, and parallel search
  [_SVM0] = {_SVM0, OK, "svm0", "shift vector matching (version 0)", 0, 0},
  [_SVM1] = {_SVM1, OK, "svm1", "shift vector matching (version 1)", 0, 0},
  [_SVM2] = {_SVM2, OK, "svm2", "shift vector matching (version 2)", 0, 0},
  [_SVM3] = {_SVM3, OK, "svm3", "shift vector matching (version 3)", 0, 0},
  [_SVM4] = {_SVM4, OK, "svm4", "shift vector matching (version 4)", 0, 0},
  [_SBNDM2] = {_SBNDM2, OK, "sbndm2", "simplified bndm with loop-unrolling", 0, 0},
  [_SBNDM_BMH] = {_SBNDM_BMH, OK, "sbndm-bmh", "sbndm with horspool shift", 2, 0},
  [_BMH_SBNDM] = {_BMH_SBNDM, FAIL, "bmh-sbndm", "Horspool with SBNDM test", 0, 0},
  [_FNDM] = {_FNDM, OK, "fndm", "forward nondeterministic dawg matching", 0, 0},
  [_BWW] = {_BWW, OK, "bww", "bit parallel wide window", 0, 0},
  [_FAOSO2] = {_FAOSO2, OK, "faoso2", "fast average optimal shift-or (u=2,m>2)", 3, 0},
  [_FAOSO4] = {_FAOSO4, OK, "faoso4", "fast average optimal shift-or (u=2,m>4)", 5, 0},
  [_FAOSO6] = {_FAOSO6, FAIL, "faoso6", "fast average optimal shift-or (u=2,m>6)", 7, 0},
  [_AOSO2] = {_AOSO2, OK, "aoso2", "average optimal shift-or (q=2)", 2, 0},
  [_AOSO4] = {_AOSO4, OK, "aoso4", "average optimal shift-or (q=4)", 4, 0},
  [_AOSO6] = {_AOSO6, OK, "aoso6", "average optimal shift-or (q=6)", 6, 0},
  [_BLIM] = {_BLIM, FAIL, "blim", "bit-parallel length invariant matcher", 0, 0},
  [_FSBNDM] = {_FSBNDM, OK, "fsbndm", "forward sbndm", 0, 0},
  [_BNDMq2] = {_BNDMq2, OK, "bndmq2", "bndm with q-grams", 2, 0},
  [_BNDMq4] = {_BNDMq4, OK, "bndmq4", "bndm with q-grams", 4, 0},
  [_BNDMq6] = {_BNDMq6, OK, "bndmq6", "bndm with q-grams", 6, 0},
  [_SBNDMq2] = {_SBNDMq2, OK, "sbndmq2", "simplified bndm with q-grams", 2, 0},
  [_SBNDMq4] = {_SBNDMq4, OK, "sbndmq4", "simplified bndm with q-grams", 4, 0},
  [_SBNDMq6] = {_SBNDMq6, OK, "sbndmq6", "simplified bndm with q-grams", 6, 0},
  [_SBNDMq8] = {_SBNDMq8, OK, "sbndmq8", "simplified bndm with q-grams", 8, 0},
  [_UFNDMq2] = {_UFNDMq2, OK, "ufndmq2", "Upper bits fndm with q-grams", 2, 0},
  [_UFNDMq4] = {_UFNDMq4, OK, "ufndmq4", "Upper bits fndm with q-grams", 4, 0},
  [_UFNDMq6] = {_UFNDMq6, OK, "ufndmq6", "Upper bits fndm with q-grams", 6, 0},
  [_UFNDMq8] = {_UFNDMq8, OK, "ufndmq8", "Upper bits fndm with q-grams", 8, 0},
  [_SABP] = {_SABP, OK, "sabp", "Small Alphabet Bit Parallel", 0, 0},
  [_DBWW] = {_DBWW, OK, "dbww", "Double BWW", 0, 0},
  [_DBWW2] = {_DBWW2, OK, "dbww2", "Double BWW", 0, 0},
  [_KSA] = {_KSA, OK, "ksa", "Factorized Shift-And", 0, 0},
  [_KBNDM] = {_KBNDM, OK, "kbndm", "Factorized BNDM", 0, 0},
  // new algorithms
  [_BSDM] = {_BSDM, OK, "bsdm", "Backward SNR DAWG Matching", 0, 0},
  [_BSDM2] = {_BSDM2, OK, "bsdm2", "Backward SNR DAWG Matching (m>=2)", 2, 0},
  [_BSDM3] = {_BSDM3, OK, "bsdm3", "Backward SNR DAWG Matching (m>=3)", 3, 0},
  [_BSDM4] = {_BSDM4, OK, "bsdm4", "Backward SNR DAWG Matching (m>=4)", 4, 0},
  [_BSDM5] = {_BSDM5, OK, "bsdm5", "Backward SNR DAWG Matching (m>=5)", 5, 0},
  [_BSDM6] = {_BSDM6, OK, "bsdm6", "Backward SNR DAWG Matching (m>=6)", 6, 0},
  [_BSDM7] = {_BSDM7, OK, "bsdm7", "Backward SNR DAWG Matching (m>=7)", 7, 0},
  [_BSDM8] = {_BSDM8, OK, "bsdm8", "Backward SNR DAWG Matching (m>=8)", 8, 0},
  [_BXS] = {_BXS, OK, "bxs", "BXS", 0, 0}, // inexact>32
  [_BXS1] = {_BXS1, OK, "bxs1", "BXS with q-grams limit", 1, 0},
  [_BXS2] = {_BXS2, OK, "bxs2", "BXS with q-grams limit", 2, 0},
  [_BXS3] = {_BXS3, OK, "bxs3", "BXS with q-grams limit", 3, 0},
  [_BXS4] = {_BXS4, OK, "bxs4", "BXS with q-grams limit", 4, 0},
  [_BXS6] = {_BXS6, OK, "bxs6", "BXS with q-grams limit", 6, 0},
  [_BXS8] = {_BXS8, OK, "bxs8", "BXS with q-grams limit", 8, 0},
  [_FS_W1] = {_FS_W1, OK, "fs-w1", "Multiple Sliding Windows", 0, 0},
  [_FS_W2] = {_FS_W2, FAIL, "fs-w2", "Multiple Sliding Windows", 0, 0},
  [_FS_W4] = {_FS_W4, FAIL, "fs-w4", "Multiple Sliding Windows", 0, 0}, // n>=6
  [_FS_W6] = {_FS_W6, OK, "fs-w6", "Multiple Sliding Windows", 0, 0}, // n>=8
  [_FS_W8] = {_FS_W8, OK, "fs-w8", "Multiple Sliding Windows", 0, 0},
  [_FSBNDM_W1] =
        {_FSBNDM_W1, OK, "fsbndm-w1", "fsbndm with multiple sliding windows", 0, 0}, // inexact >31
  [_FSBNDM_W2] =
        {_FSBNDM_W2, OK, "fsbndm-w2", "fsbndm with multiple sliding windows", 0, 0}, // inexact >31
  [_FSBNDM_W4] =
        {_FSBNDM_W4, OK, "fsbndm-w4", "fsbndm with multiple sliding windows", 0, 0}, // inexact >31
  [_FSBNDM_W6] =
        {_FSBNDM_W6, OK, "fsbndm-w6", "fsbndm with multiple sliding windows", 0, 0}, // inexact >31
  [_FSBNDM_W8] =
        {_FSBNDM_W8, OK, "fsbndm-w8", "fsbndm with multiple sliding windows", 11, 0}, // inexact >31
  [_FSBNDMQ20] =
        {_FSBNDMQ20, OK, "fsbndmq20", "fsbndm with q-grams and lookahead", 2, 0}, // m: 2..32
  [_FSBNDMQ21] =
        {_FSBNDMQ21, OK, "fsbndmq21", "fsbndm with q-grams and lookahead", 2, 0}, // m: 2..31
  [_FSBNDMQ31] =
        {_FSBNDMQ31, OK, "fsbndmq31", "fsbndm with q-grams and lookahead", 3, 0}, // m: 3..31
  [_FSBNDMQ32] =
        {_FSBNDMQ32, FAIL, "fsbndmq32", "fsbndm with q-grams and lookahead", 3, 0}, // m: 3..30
  [_FSBNDMQ41] =
        {_FSBNDMQ41, OK, "fsbndmq41", "fsbndm with q-grams and lookahead", 4, 0}, // m: 4..31
  [_FSBNDMQ42] =
        {_FSBNDMQ42, FAIL, "fsbndmq42", "fsbndm with q-grams and lookahead", 4, 0}, // m: 4..30
  [_FSBNDMQ43] =
        {_FSBNDMQ43, FAIL, "fsbndmq43", "fsbndm with q-grams and lookahead", 4, 0}, // m: 4..29
  [_FSBNDMQ61] =
        {_FSBNDMQ61, OK, "fsbndmq61", "fsbndm with q-grams and lookahead", 6, 0}, // m: 6..31
  [_FSBNDMQ62] =
        {_FSBNDMQ62, FAIL, "fsbndmq62", "fsbndm with q-grams and lookahead", 6, 0}, // m: 6..30
  [_FSBNDMQ64] =
        {_FSBNDMQ64, FAIL, "fsbndmq64", "fsbndm with q-grams and lookahead", 6, 0}, // m: 6..28
  [_FSBNDMQ81] =
        {_FSBNDMQ81, OK, "fsbndmq81", "fsbndm with q-grams and lookahead", 8, 0}, // m: 8..31
  [_FSBNDMQ82] =
        {_FSBNDMQ82, FAIL, "fsbndmq82", "fsbndm with q-grams and lookahead", 8, 0}, // m: 8..30
  [_FSBNDMQ84] =
        {_FSBNDMQ84, FAIL, "fsbndmq84", "fsbndm with q-grams and lookahead", 8, 0}, // m: 8..28
  [_FSBNDMQ86] =
        {_FSBNDMQ86, FAIL, "fsbndmq86", "fsbndm with q-grams and lookahead", 8, 0}, // m: 8..26
  [_IOM] = {_IOM, OK, "iom", "Improved Occurrence Heuristic", 0, 0},
  [_JOM] = {_JOM, OK, "jom", "Jumping Occurrence Matcher", 2, 0},
  [_LWFR2] = {_LWFR2, OK, "lwfr2", "Weak Factor Recognizer, Linear Version", 2, 0},
  [_LWFR3] = {_LWFR3, OK, "lwfr3", "Weak Factor Recognizer, Linear Version", 3, 0},
  [_LWFR4] = {_LWFR4, OK, "lwfr4", "Weak Factor Recognizer, Linear Version", 4, 0},
  [_LWFR5] = {_LWFR5, OK, "lwfr5", "Weak Factor Recognizer, Linear Version", 5, 0},
  [_LWFR6] = {_LWFR6, OK, "lwfr6", "Weak Factor Recognizer, Linear Version", 6, 0},
  [_LWFR7] = {_LWFR7, OK, "lwfr7", "Weak Factor Recognizer, Linear Version", 7, 0},
  [_LWFR8] = {_LWFR8, OK, "lwfr8", "Weak Factor Recognizer, Linear Version", 8, 0},
  [_QF23] = {_QF23, OK, "qf23", "Q-gram Filtering q=2 s=3", 3, 0},
  [_QF24] = {_QF24, OK, "qf24", "Q-gram Filtering q=2 s=4", 3, 0},
  [_QF26] = {_QF26, OK, "qf26", "Q-gram Filtering q=2 s=6", 3, 0},
  [_QF28] = {_QF28, OK, "qf28", "Q-gram Filtering q=2 s=8", 3, 0},
  [_QF33] = {_QF33, OK, "qf33", "Q-gram Filtering q=3 s=3", 4, 0},
  [_QF34] = {_QF34, OK, "qf34", "Q-gram Filtering q=3 s=4", 4, 0},
  [_QF36] = {_QF36, OK, "qf36", "Q-gram Filtering q=3 s=6", 4, 0},
  [_QF42] = {_QF42, OK, "qf42", "Q-gram Filtering q=4 s=2", 5, 0},
  [_QF43] = {_QF43, OK, "qf43", "Q-gram Filtering q=4 s=3", 5, 0},
  [_QF44] = {_QF44, OK, "qf44", "Q-gram Filtering q=4 s=4", 5, 0},
  [_QF62] = {_QF62, OK, "qf62", "Q-gram Filtering q=6 s=2", 7, 0},
  [_QF63] = {_QF63, OK, "qf63", "Q-gram Filtering q=6 s=3", 7, 0},
  [_QLQS] = {_QLQS, OK, "qlqs", "Quantum Leap Quick Search", 0, 0},
  [_SBNDM_W2] = {_SBNDM_W2, OK, "sbndm-w2", "SBNDM with lookahead", 0, 0}, // inexact >32
  [_SBNDM_W4] = {_SBNDM_W4, OK, "sbndm-w4", "SBNDM with lookahead", 0, 0}, // inexact >32
  [_SBNDM_W6] = {_SBNDM_W6, OK, "sbndm-w6", "SBNDM with lookahead", 0, 0}, // inexact >32
  [_SSM] = {_SSM, OK, "ssm", "SSM", 0, 0},
  [_TSA] = {_TSA, FAIL, "tsa", "word-wise popcount", 0, 0}, // max 64
  [_TSA_Q2] = {_TSA_Q2, FAIL, "tsa-q2", "word-wise popcount q=2", 2, 0}, // max: 64
  [_TSO5] = {_TSO5, OK, "tso5", "optimized word-wise popcount", 0, 0}, // max: 64
  [_TUNEDBM] = {_TUNEDBM, OK, "tunedbm", "Tuned Boyer-Moore", 0, 0},
  [_TVSBS_W2] = {_TVSBS_W2, OK, "tvsbs-w2", "TVSBS", 0, 0},
  [_TVSBS_W4] = {_TVSBS_W4, FAIL, "tvsbs-w4", "TVSBS", 2, 0},
  [_TVSBS_W6] = {_TVSBS_W6, FAIL, "tvsbs-w6", "TVSBS", 2, 0},
  [_TVSBS_W8] = {_TVSBS_W8, FAIL, "tvsbs-w8", "TVSBS", 2, 0},
  [_WFR] = {_WFR, OK, "wfr", "Weak Factor Recognizer", 0, 0},
  [_WFR2] = {_WFR2, OK, "wfr2", "Weak Factor Recognizer (m>=2)", 2, 0},
  [_WFR3] = {_WFR3, OK, "wfr3", "Weak Factor Recognizer (m>=3)", 3, 0},
  [_WFR4] = {_WFR4, OK, "wfr4", "Weak Factor Recognizer (m>=4)", 4, 0},
  [_WFR5] = {_WFR5, OK, "wfr5", "Weak Factor Recognizer (m>=5)", 5, 0},
  [_WFR6] = {_WFR6, OK, "wfr6", "Weak Factor Recognizer (m>=6)", 6, 0},
  [_WFR7] = {_WFR7, OK, "wfr7", "Weak Factor Recognizer (m>=7)", 7, 0},
  [_WFR8] = {_WFR8, OK, "wfr8", "Weak Factor Recognizer (m>=8)", 8, 0},
  [_WFRQ2] = {_WFRQ2, OK, "wfrq2", "Weak Factor Recognizer with q-grams", 2, 0},
  [_WFRQ3] = {_WFRQ3, OK, "wfrq3", "Weak Factor Recognizer with q-grams", 3, 0},
  [_WFRQ4] = {_WFRQ4, OK, "wfrq4", "Weak Factor Recognizer with q-grams", 4, 0},
  [_WFRQ5] = {_WFRQ5, OK, "wfrq5", "Weak Factor Recognizer with q-grams", 5, 0},
  [_WFRQ6] = {_WFRQ6, OK, "wfrq6", "Weak Factor Recognizer with q-grams", 6, 0},
  [_WFRQ7] = {_WFRQ7, OK, "wfrq7", "Weak Factor Recognizer with q-grams", 7, 0},
  [_WFRQ8] = {_WFRQ8, OK, "wfrq8", "Weak Factor Recognizer with q-grams", 8, 0},
  [_TWFR] = {_TWFR, OK, "twfr", "Tuned Weak Factor Recognizer", 0, 0},
  [_TWFR2] = {_TWFR2, OK, "twfr2", "Tuned Weak Factor Recognizer", 0, 0},
  [_TWFR3] = {_TWFR3, OK, "twfr3", "Tuned Weak Factor Recognizer", 0, 0},
  [_TWFR4] = {_TWFR4, OK, "twfr4", "Tuned Weak Factor Recognizer", 0, 0},
  [_TWFR5] = {_TWFR5, OK, "twfr5", "Tuned Weak Factor Recognizer", 0, 0},
  [_TWFR6] = {_TWFR6, OK, "twfr6", "Tuned Weak Factor Recognizer", 0, 0},
  [_TWFR7] = {_TWFR7, OK, "twfr7", "Tuned Weak Factor Recognizer", 0, 0},
  [_TWFR8] = {_TWFR8, OK, "twfr8", "Tuned Weak Factor Recognizer", 0, 0},
  [_TWFRQ2] = {_TWFRQ2, OK, "twfrq2", "Tuned Weak Factor Recognizer with q-grams", 2, 0},
  [_TWFRQ3] = {_TWFRQ3, OK, "twfrq3", "Tuned Weak Factor Recognizer with q-grams", 3, 0},
  [_TWFRQ4] = {_TWFRQ4, OK, "twfrq4", "Tuned Weak Factor Recognizer with q-grams", 4, 0},
  [_TWFRQ5] = {_TWFRQ5, OK, "twfrq5", "Tuned Weak Factor Recognizer with q-grams", 5, 0},
  [_TWFRQ6] = {_TWFRQ6, OK, "twfrq6", "Tuned Weak Factor Recognizer with q-grams", 6, 0},
  [_TWFRQ7] = {_TWFRQ7, OK, "twfrq7", "Tuned Weak Factor Recognizer with q-grams", 7, 0},
  [_TWFRQ8] = {_TWFRQ8, OK, "twfrq8", "Tuned Weak Factor Recognizer with q-grams", 8, 0},
  [_WC] = {_WC, OK, "wc", "Tailed Substring algorithm", 0, 0},
  [_WOM] = {_WOM, OK, "wom", "Worst Occurrence Matcher", 0, 0},
  [_DOUBLEHASH] = {_DOUBLEHASH, OK, "doublehash", "Double-Hash hybrid", 0, 0},
  // state of the art:
  [_FT3] = {_FT3, OK, "ft3", "Parallel Boyer-Moore with goodsuff", 0, 0}, // Lecroq 2015
  [_HPBM] = {_HPBM, FAIL, "hpbm", "Parallel Boyer-Moore", 0, MISSING},
  [_SSECP] = {_SSECP, FAIL, "ssecp", "SSE Crochemore-Perrin", 0, 0}, // broken
  [_EPSM] = {_EPSM, X64_ONLY, "epsm", "SSE4 Exact Packed String Matching", 0, 0},
  [_LIBC] = {_LIBC, OK, "libc", "strstr", 0, 0},
  [_LIBC1] = {_LIBC1, OK, "libc1", "memmem", 0, 0},
  [_MUSL] = {_MUSL, OK, "musl", "musl strstr", 0, 0},
  [_MUSL1] = {_MUSL1, OK, "musl1", "musl memmem", 0, 0},
  [_SIMDKR] = {_SIMDKR, OK, "simdkr", "SIMD generic Rabin-Karp variants", 2, 0},
    // clang-format on
};

#undef OK
#undef FAIL

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define NumAlgo ARRAY_SIZE(ALGOS)

#endif // _ALGORITHMS_H
