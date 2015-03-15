#include "treap.h"
#include <iostream>
#include <sstream>


enum EQueryType
{
    SUBSEGMENT_SUM,
    INSERT,
    ASSIGN,
    NEXT_PERMUTATION
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
    void generateRandomSubsegment(ui32 arraySize)
    {
        left = getRandomInt(0, arraySize - 1);
        right = getRandomInt(left + 1, arraySize);
        //std::cout << arraySize << " " << left << " " << right<< std::endl;
    }
    void generateRandomValueAndIndex(ui32 arraySize)
    {
        value = getRandomInt(INT_MIN, INT_MAX);
        index = getRandomInt(0, arraySize - 1);
    }
};

std::string convertQueryToString(Query query)
{
    switch (query.type)
    {
        case SUBSEGMENT_SUM:
            return "SUBSEGMENT_SUM " + convertToString(query.left) + " " + convertToString(query.right);
        case INSERT:
            return "INSERT " + convertToString(query.value) + " " + convertToString(query.index);
        case ASSIGN:
            return "ASSIGN " + convertToString(query.value) + " " + convertToString(query.index);
        case NEXT_PERMUTATION:
            return "NEXT_PERMUTATION " + convertToString(query.left) + " " + convertToString(query.right);
    }
}

std::vector<Query> generateRandomQueries(ui32 queriesNumber)
{
    std::vector<Query> queries;
    Query firstQuery;
    firstQuery.type = INSERT;
    firstQuery.index = 0;
    firstQuery.value = getRandomInt(INT_MIN, INT_MAX);
    queries.push_back(firstQuery);
    ui32 arraySize = 1;
    for (ui32 i = 1; i < queriesNumber; ++i)
    {
        Query currentQuery;
        currentQuery.type = (EQueryType)getRandomInt(0, 3);
        switch (currentQuery.type)
        {
            case SUBSEGMENT_SUM:
            {
                currentQuery.generateRandomSubsegment(arraySize);
                break;
            }
            case INSERT:
            {
                currentQuery.generateRandomValueAndIndex(arraySize + 1);
                arraySize++;
                break;
            }
            case ASSIGN:
            {
                currentQuery.generateRandomValueAndIndex(arraySize);
                break;
            }
            case NEXT_PERMUTATION:
            {
                currentQuery.generateRandomSubsegment(arraySize);
                break;
            }
        }
        queries.push_back(currentQuery);
    }
    //for (ui32 i = 0; i < queriesNumber; ++i)
    //    std::cout << convertQueryToString(queries[i]) << "\n";
    return queries;
}

template<typename PermutableArrayType>
long long resultQuery(PermutableArrayType& permutableArray, Query query)
{
    switch (query.type)
    {
        case SUBSEGMENT_SUM:
        {
            return permutableArray.subsegmentSum(query.left, query.right);
        }
        case INSERT:
        {
            permutableArray.insert(query.value, query.index);
            return 1;
        }
        case ASSIGN:
        {
            permutableArray.assign(query.value, query.index);
            return 1;
        }
        case NEXT_PERMUTATION:
        {
            permutableArray.nextPermutation(query.left, query.right);
            return 1;
        }
    }
}

class GlobalTestResult
{
public:
    bool succeeds = true;
    double treapTime = 0, slowVectorTime = 0;
    GlobalTestResult(ui32 queriesNumber)
    {
        std::vector<Query> queries = generateRandomQueries(queriesNumber);
        Treap treap;
        SlowVector slowVector;
        ui32 arraySize = 0;
        for (ui32 i = 0; i < queriesNumber; ++i)
        {
            //std::cout << convertQueryToString(queries[i]) << "\n";
            if (queries[i].type == INSERT)
                ++arraySize;
            long long treapResult = resultQuery<Treap>(treap, queries[i]);
            //std::cout << "Treap ok" << "\n";
            long long slowVectorResult = resultQuery<SlowVector>(slowVector, queries[i]);
            //std::cout << "SlowV ok" << "\n";

                /*
            for (ui32 j = 0; j < arraySize; ++j)
                std::cout << slowVector.subsegmentSum(j, j + 1);
            std::cout << "\n";*/
            if (treapResult != slowVectorResult)
            {
                succeeds = false;
                /*for (ui32 j = 0; j < arraySize; ++j)
                    std::cout <<  treap.subsegmentSum(j, j + 1);
                std::cout << "\n";
                for (ui32 j = 0; j < arraySize; ++j)
                    std::cout <<  slowVector.subsegmentSum(j, j + 1);
                std::cout << "\n";
                std::cout << "\n";*/
                //std::cout << "Treap says: " << treapResult << "\n";
                //std::cout << "SlowV says: " << slowVectorResult << "\n";
            }
            //else
                //std::cout << "Reslt ok: " << treapResult << "\n";
        }
    }
};
