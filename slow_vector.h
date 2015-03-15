#include <vector>
#include <algorithm>

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
        long long result = 0;
        for (ui32 i = left; i < right; ++i)
            result += (long long) intVector[i];
        //std::cout << result;
        return result;
    }

    void insert(int value, ui32 index)
    {
        intVector.insert(intVector.begin() + index, value);
        /*intVector.push_back(0);
        for (ui32 i = intVector.size(); i > index; --i)
            intVector[i] = intVector[i - 1];
        intVector[index] = value;*/
    }

    void assign(int value, ui32 index)
    {
        intVector[index] = value;
    }

    bool nextPermutation(ui32 left, ui32 right)
    {
        return std::next_permutation(intVector.begin() + left, intVector.begin() + right);
    }
};

