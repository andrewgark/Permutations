#include "treap.h"
#include <iostream>
#include <sstream>

enum ETestType
{
    ETT_RANDOM,
    ETT_NEXT_PERMUTATION
};

enum EQueryType
{
    EQT_SUBSEGMENT_SUM,
    EQT_INSERT,
    EQT_ASSIGN,
    EQT_NEXT_PERMUTATION
};

template<typename TypeValue>
std::string convertToString(TypeValue number)
{
   std::stringstream ss;
   ss << number;
   return ss.str();
}

struct Query
{
    EQueryType type;
    ui32 index, left, right;
    int value;
};

void generateRandomSubsegment(Query &query, ui32 arraySize)
{
    query.left = getRandomInt(0, arraySize - 1);
    query.right = getRandomInt(0, arraySize - 1);
    if (query.left > query.right)
        std::swap(query.left, query.right);
    query.right++;
}

void generateRandomValueAndIndex(Query &query, ui32 arraySize)
{
    query.value = getRandomInt();
    query.index = getRandomInt(0, arraySize - 1);
}

std::string convertQueryToString(Query query)
{
    switch (query.type)
    {
        case EQT_SUBSEGMENT_SUM:
            return "SUBSEGMENT_SUM " + convertToString(query.left) + " " + convertToString(query.right);
        case EQT_INSERT:
            return "INSERT " + convertToString(query.value) + " " + convertToString(query.index);
        case EQT_ASSIGN:
            return "ASSIGN " + convertToString(query.value) + " " + convertToString(query.index);
        case EQT_NEXT_PERMUTATION:
            return "NEXT_PERMUTATION " + convertToString(query.left) + " " + convertToString(query.right);
    }
}

template<ETestType TestType>
std::vector<Query> generateQueries(ui32 arraySize, ui32 queriesNumber);

template<>
std::vector<Query> generateQueries<ETT_NEXT_PERMUTATION>(ui32 arraySize, ui32 queriesNumber)
{
    std::vector<Query> queries;
    for (ui32 i = 0; i < arraySize; ++i)
    {
        Query currentQuery;
        currentQuery.type = EQT_INSERT;
        currentQuery.index = 0;
        currentQuery.value = getRandomInt();
        queries.push_back(currentQuery);
    }
    for (ui32 i = 0; i < queriesNumber; ++i)
    {
        Query currentQuery;
        currentQuery.type = EQT_NEXT_PERMUTATION;
        currentQuery.left = 0;
        currentQuery.right = arraySize;
        queries.push_back(currentQuery);
    }
    for (ui32 i = 0; i < arraySize; ++i)
    {
        Query currentQuery;
        currentQuery.type = EQT_SUBSEGMENT_SUM;
        currentQuery.left = i;
        currentQuery.right = i + 1;
        queries.push_back(currentQuery);
    }
    return queries;
}

template<>
std::vector<Query> generateQueries<ETT_RANDOM>(ui32 arraySize, ui32 queriesNumber)
{
    std::vector<Query> queries;
    Query firstQuery;
    firstQuery.type = EQT_INSERT;
    firstQuery.index = 0;
    firstQuery.value = getRandomInt();
    queries.push_back(firstQuery);
    ui32 currentArraySize = 1;
    for (ui32 i = 1; i < queriesNumber; ++i)
    {
        Query currentQuery;
        currentQuery.type = (EQueryType)getRandomInt(0, 3);
        switch (currentQuery.type)
        {
            case EQT_SUBSEGMENT_SUM:
            {
                generateRandomSubsegment(currentQuery, currentArraySize);
                break;
            }
            case EQT_INSERT:
            {
                generateRandomValueAndIndex(currentQuery, currentArraySize + 1);
                arraySize++;
                break;
            }
            case EQT_ASSIGN:
            {
                generateRandomValueAndIndex(currentQuery, currentArraySize);
                break;
            }
            case EQT_NEXT_PERMUTATION:
            {
                generateRandomSubsegment(currentQuery, currentArraySize);
                break;
            }
        }
        queries.push_back(currentQuery);
    }
    return queries;
}

long long resultQuery(IPermutableArray& permutableArray, Query query)
{
    switch (query.type)
    {
        case EQT_SUBSEGMENT_SUM:
        {
            return permutableArray.subsegmentSum(query.left, query.right);
        }
        case EQT_INSERT:
        {
            permutableArray.insert(query.value, query.index);
            return 1;
        }
        case EQT_ASSIGN:
        {
            permutableArray.assign(query.value, query.index);
            return 1;
        }
        case EQT_NEXT_PERMUTATION:
        {
            return permutableArray.nextPermutation(query.left, query.right);
        }
    }
}

std::vector<long long> resultsQueries(IPermutableArray &permutableArray, std::vector<Query> &queries)
{
    std::vector<long long> result;
    for (ui32 i = 0; i < queries.size(); ++i)
        result.push_back(resultQuery(permutableArray, queries[i]));
    return result;
}

class GlobalTestResult
{
    bool succeeds = true;

public:
    GlobalTestResult(){};

    bool isSucceeds()
    {
        return succeeds;
    }

    void execute(ui32 arraySize, ui32 queriesNumber)
    {
        test<ETT_RANDOM>(arraySize, queriesNumber);
        test<ETT_NEXT_PERMUTATION>(arraySize, queriesNumber);
    }

    template<ETestType testType>
    void test(ui32 arraySize, ui32 queriesNumber)
    {
        std::vector<Query> queries = generateQueries<testType>(arraySize, queriesNumber);
        Treap treap;
        SlowVector slowVector;
        std::vector<long long> treapResults = resultsQueries(treap, queries);
        std::vector<long long> slowVectorResults = resultsQueries(slowVector, queries);
        succeeds = (treapResults == slowVectorResults);
    }
};

