# Usage
1. Reed-Solomon Erasure Coding is implemented based on Vandermonde Matrix. For the w-bit word, w must be large enough so that n ≤ 2w+1 where n is the sum of data chunk number (k) and parity chunk number (m). The default setting of w is 8 that is enough for most cases, you can also change to 16, 32 if needed.

2. Delta coding could generate the new parity chunks by using delta between the new updated data chunks and the old data. As such, it can help to reduce the I/Os in some scenarios.

3. XOR Coding is also included, but the number of parity chunk is limited to 1 similar to RAID5. 

4. A simple example about how to use them to do data encoding, delta encoding, and data decoing is in the main.c file. 

./configure

make 

sudo ./main [a single file name]


# Install
Linux 64bit.

Jerasure: a library in C that supports erasure coding in storage applications.

GF-Complete: a Comprehensive Library for Galois Field Arithmetic.

[Link](http://jerasure.org/)

# Reference

1. Heng Zhang, Mingkai Dong, and Haibo Chen. Efficient and available in-memory KV-store with hybrid
erasure coding and replication (FAST'16).

2. James S. Plank, Jianqiang Luo, Catherine D. Schuman, Lihao Xu, and Zooko Wilcox-O’Hearn. A performance evaluation and examination of open-source erasure coding libraries for storage (FAST'09).
