#### 6.12

Since B = 4, we need 2 bits for block offsets;
Since we have 8 sets, we need 3 bits for set selection;
The remaiing 8 bits go to tags

#### 6.13

Address 0x0D53 = 0|1101|0101|0011 as address is 13-bit.
Modified for cache structure: 01101010|100|11, at set 4, tag value 0110|1010 = 0x6A, offset 3 (last byte)

We have a cache miss.

#### 6.14

Address 0x0CB4 = 0|1100|1011|0100
Modified for cache structure: 01100101|101|00, at set 0x5, tag value 0x65, offset 0x0

We have a cache miss

#### 6.17

```C
typedef int array[2][2];

void transpose1(array dst, array src) {
    int i, j;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            dst[j][i] = src[i][j];
        }
    }
}
```

Iteration 1:

Data layout:
src[0][0], src[0][1], src[1][0], src[1][1],
dst[0][0], dst[0][1], dst[1][0], dst[1][1]

Cache block size: 8 bytes in one line and two sets

*src read cache miss*

So we first load src[0][0] and src[0][1] into cache line 0;

*dst write cache miss* -> write allocate

So we then load dst[0][0] and dst[0][1] into cache line 0 because they map to the same line (because the lowest bit is block offset and the next one is set selection bit).


Iteration 2:

*src read cache hit*

src[0][1] can be fetched directly from cache.

*dst write cache miss*

dst[1][0] is a miss, dst[1][0] and dst[1][1] loaded into cache line 1

iteration 3:

*src read cache miss*

*dst write cache miss*

iteration 4:

*src read cache hit*

*dst write cache miss*

#### 6.18

Block size: 32 bytes
Set: 2048/32 = 64 sets

So we need 5 lowest bits to be block offset and next 6 bits to be set selection bits.

We also have integers to be 4-byte so the struct takes 8 bytes.

**Code to analyze**:
```C
for (i = 31; i >= 0; i--) {
    for (j = 31; j >= 0; j--) {
        total_x += grid[i][j].x;
    }
}

for (i = 31; i >= 0; i--) {
    for (j = 31; j >= 0; j--) {
        total_x += grid[i][j].y;
    }
}
```

First iteration tries to load grid[0][0].x -> cache miss -> loads [31][31], 24 bytes after into set 7;

Next 31 iterations all miss cache because it's looping backwards. For example the second iteration asks for [31][30] which is not in the cache, not only we have to load [31][30], [31][31] and 16 bytes after into the cache, we have to load them into the same set (each 4 contigous elements are mapped to the same cache set).

...

So after the first 32 iterations, all [0][j] are loaded into cache, 8 cache sets are occupied now. There are 8 misses.