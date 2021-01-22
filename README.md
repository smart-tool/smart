# SMART
**[SMART](http://www.dmi.unict.it/~faro/smart/index.php) (String Matching Algorithm Research Tool)** is a tool which provides a standard framework for researchers in string matching. It helps users to test, design, evaluate and understand existing solutions for the exact string matching problem. Moreover it provides the implementation of (almost) all string matching algorithms and a wide corpus of text buffers.

In the last **40 years of research** in computer science string matching was one of the most extensively studied problem, mainly due to its direct applications to such diverse areas as text, image and signal processing, speech analysis and recognition, data compression, information retrieval, computational biology and chemistry. Moreover String matching algorithms are also basic components used in implementations of practical softwares existing under most operating systems.

Since 1970 more than **80 string matching algorithms** have been proposed, and more than 50% of them in the last ten years. The smart tool provides a comprehensive collection of all string matching algorithms, implemented in C programming language, and helps researcher to perform experimental results and compare them from a practical point of view. Smart provides a practical and standard platform for testing string matching algorithms and sharing results with the community.

The smart tool provides also a corpus of **12 texts** on which the string matching algorithms can be tested. Texts in the corpus are of different types, including natural language texts, genome sequences, protein sequences, and random texts with a uniform distribution of characters.

## [Download SMART](https://github.com/smart-tool/smart/releases)
The release of smart will be available [here](https://github.com/smart-tool/smart/releases)

## How to use?
The documentation about smart is available [here](http://www.stringology.org/event/2016/p09.html)

## How to compile it from source
To compile the source just download (or clone) this repository and run the file makefile from terminal (with **./makefile**), it will compile the smart binaries and all the algorithms (the algorithms binaries will be created into source/bin/).

## Troubleshooting

Problem with **shared memory (shmget)**? see the [documentation](http://www.dmi.unict.it/~faro/smart/howto.php?id=03)

