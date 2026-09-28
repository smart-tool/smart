# SMART
**SMART (String Matching Algorithm Research Tool)** is a tool which provides a standard framework for researchers in string matching. It helps users to test, design, evaluate and understand existing solutions for the exact string matching problem. Moreover it provides the implementation of (almost) all string matching algorithms and a wide corpus of text buffers.

In the last **40 years of research** in computer science string matching was one of the most extensively studied problem, mainly due to its direct applications to such diverse areas as text, image and signal processing, speech analysis and recognition, data compression, information retrieval, computational biology and chemistry. Moreover String matching algorithms are also basic components used in implementations of practical softwares existing under most operating systems.

Since 1970 more than **80 string matching algorithms** have been proposed, and more than 50% of them in the last ten years. The smart tool provides a comprehensive collection of all string matching algorithms (272 as of now), implemented in the C programming language, and helps researcher to perform experimental results and compare them from a practical point of view. Smart provides a practical and standard platform for testing string matching algorithms and sharing results with the community.

The smart tool provides also a corpus of **12 texts** on which the string matching algorithms can be tested. Texts in the corpus are of different types, including natural language texts, genome sequences, protein sequences, and random texts with a uniform distribution of characters.

## [Download SMART](https://github.com/rurban/smart/releases)
The release of smart will be available [here](https://github.com/rurban/smart/releases)

## How to use?
The documentation about smart is available [here](https://www.dmi.unict.it/faro/smart/howto.php)

## How to compile it from source
To compile the source just download (or clone) this repository and run the file build.sh from terminal (with **./build.sh**), it will compile the smart binaries and all the algorithms (the algorithms binaries will be created into bin/).

## Constraints
Several algorithms are constraint in a **minimal** or **maximal pattern length m**. Then fallbacks are used, which are skipped in the timing benchmarks.

The **NUL character** is allowed in the pattern and the text (i.e. the **memmem()** API). Only the new
strstr() reference algos libc and musl are skipped then.

The original tests ensured a **terminating NUL** (i.e. strstr(), not memmem()), but this was fixed, as
it was unrealistic.
Only this colussi implementation violated this, but the original paper had the missing check included.

Many algorithm implementations got constraint and memory-overflow fixes, some also added optimizations.

## Formal verification
Most algos could be formally verified by Reini with the model checker **[CBMC](http://www.cprover.org/cbmc/)**.
But since some algos use double-nested for loops, the depth is quadratic to the input length.
Thus formal verification is tuned to low m and n; 32 * 32 = --depth 1024, which would time out
within 10m.
We use max m 10 and max n (the text length) 36, i.e depth 360. A helper algocfg was added to
list all test properties and constraints, and use the best CBMC arguments.

## Tests
We test now with added assertions, using the sanitizers (address and UB), and are fuzzing with AFL+.
Lots of buffer overflow, memory leaks and undefined behavior errors were fixed.
Most allocating algos got optimizations with static stack buffers.
The cppcheck and clang-tidy linters were added and all violations fixed, resp. false positives noted.
github actions run all the tests, linters and some verifications on all changes.

## Summary

According to our experimental results in 2010 (until KBNDM), we conclude
that the following algorithms are the most efficient in the following situations.
MUSL1 and EPSM added later as the current best.

* SIMDKR: best stable overall, with x86_64 SSE4.2, AVX2, and ARM NEON paths.
* MUSL1 memmem(): short patterns.
* EPSM: Exact Packed String Matching with SSE4, AVX2, and ARM NEON paths.
* HC4 and FHC3: best for texts >= 8
* QF43: best for texts >= 16
* SA: very short patterns and very small alphabets.
* TVSBS: very short patterns and small alphabets, and long patterns and large alphabets.
* FJS: very short patterns and large and very large alphabets.
* EBOM: short patterns and large and very large alphabets.
* SBNDM-BMH and BMH-SBNDM: short patterns and very large alphabets.
* HASHq: short and large patterns and small alphabets.
* FSBNDM: long patterns and large and very larghe alphabets.
* SBNDMq: long pattern and small alphabets.
* LBNDM: very long patterns and very large alphabets.

- https://arxiv.org/pdf/1012.2547v1.pdf

However the old tests were done with temp. buffers on the heap, not static stack buffers.
And some implementations didn't free their temp. buffers.

## Benchmarks

* [Best](https://rurban.github.io/smart/results/best25/)
* [All](https://rurban.github.io/smart/results/all/)

## Reference

[Stringology Conference article url](https://www.stringology.org/event/2016/p09.html)

[Lecroq's EXACT STRING MATCHING ALGORITHMS](https://www-igm.univ-mlv.fr/~lecroq/string/)

If you work with SMART, please cite:
```BibTeX
@INPROCEEDINGS( PSC2016-9, 
 author = "Simone Faro and Thierry Lecroq and Stefano Borz\`i and Simone Di Mauro and Alessandro Maggio",
 title = "The String Matching Algorithms Research Tool",
 booktitle = "Proceedings of the Prague Stringology Conference 2016",
 address = "Czech Technical University in Prague, Czech Republic",
 editor = "Jan Holub and Jan {\v{Z}}{\v{d}}{\'{a}}rek",
 isbn = "978-80-01-05996-8",
 year = 2016,
 pages = "99--111",
)
```

## Troubleshooting

Problem with **shared memory (shmget)**? see the [documentation](https://www.dmi.unict.it/faro/smart/howto.php)
