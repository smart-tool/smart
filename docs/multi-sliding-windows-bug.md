# Multiple Sliding Windows precondition

The Multiple Sliding Windows algorithms from Faro and Lecroq, *Multiple
Sliding Windows Algorithms for Searching Texts on Large Alphabets* (SEA 2012),
assume an even number of windows `k` and `m < n / k`, where `m` is pattern
length and `n` is text length.

The original SMART implementations did not enforce that precondition. Their
coordinated forward/backward scans can consequently address positions outside
the text or stop making progress when the windows overlap. The affected
families are `fs-w*`, `fsbndm-w*`, `sbndm-w*`, and `tvsbs-w*`.

The unsafe window implementations now route every invocation through the safe
fallback. The incomplete lockstep rewrites could not prove independent-window
progress at an exhausted boundary; retaining a fast path would leave an
input-dependent out-of-bounds read or non-terminating loop.

The safe fallback path uses `search_safe` to bypass macro-expanded output side
effects and timer lifecycle interactions that would otherwise disrupt lockstep
or coordinated wrapper scans.
