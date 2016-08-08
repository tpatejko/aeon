#include <vector>
#include <algorithm>
#include <random>

#include "batch_iterator_shuffled.hpp"

using namespace std;

BatchIteratorShuffled::BatchIteratorShuffled(shared_ptr<BatchLoader> loader, uint seed)
    : _rand(seed), _loader(loader), _seed(seed), _epoch(0) {

    // fill indices with integers from  0 to _count.  indices can then be
    // shuffled and used to iterate randomly through the blocks.
    uint _count = _loader->blockCount();
    for(uint i = 0; i < _count; ++i) {
        _indices.push_back(i);
    }

    reset();
}

void BatchIteratorShuffled::shuffle() {
    std::shuffle(_indices.begin(), _indices.end(), _rand);
}

void BatchIteratorShuffled::read(buffer_in_array &dest) {
    _loader->loadBlock(dest, *_it);

    // shuffle the objects in BufferPair dest
    // seed the shuffle with the seed passed in the constructor + the _epoch
    // to ensure that the buffer shuffles are deterministic wrt the input seed.
    // HACK: pass the same seed to both shuffles to ensure that both buffers
    // are shuffled in the same order.

    for (auto d: dest) {
        d->shuffle(_seed + _epoch);
    }

    ++_it;

    if(_it == _indices.end()) {
        reset();
    }
}

void BatchIteratorShuffled::reset() {
    shuffle();

    _it = _indices.begin();

    ++_epoch;
}