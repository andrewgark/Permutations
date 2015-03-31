#include <vector>
#include <algorithm>
#include <cassert>

typedef unsigned int ui32;

class IPermutableArray
{
public:
    virtual void insert(int value, ui32 index) = 0;
    virtual void assign(int value, ui32 index) = 0;
    virtual long long subsegmentSum(ui32 left, ui32 right) = 0;
    virtual bool nextPermutation(ui32 left, ui32 right) = 0;
    ~IPermutableArray(){};
};

class SlowVector: public IPermutableArray
{
    std::vector<int> intVector;
public:
    SlowVector(){}

    ~SlowVector(){}

    long long subsegmentSum(ui32 left, ui32 right)
    {
        assert(left < right && right <= intVector.size());
        long long result = 0;
        return std::accumulate(intVector.begin() + left, intVector.begin() + right, result);
    }

    void insert(int value, ui32 index)
    {
        assert(index <= intVector.size());
        intVector.insert(intVector.begin() + index, value);
    }

    void assign(int value, ui32 index)
    {
        assert(index < intVector.size());
        intVector[index] = value;
    }

    bool nextPermutation(ui32 left, ui32 right)
    {
        assert(left < right && right <= intVector.size());
        return std::next_permutation(intVector.begin() + left, intVector.begin() + right);
    }
};

