#include "slow_vector.h"
#include <random>
#include <cassert>

std::default_random_engine generator;

int getRandomInt(int a, int b)
{
    std::uniform_int_distribution<int> distribution(a, b);
    return distribution(generator);
}

int getRandomInt()
{
    return getRandomInt(INT_MIN, INT_MAX);
}

class Treap: public IPermutableArray
{
    class Node
    {
        void updateSizeMax(ui32 &sizeMax, ui32 firstSizeMax, ui32 secondSizeMax, int mostFirstKey, int mostSecondKey, Node *first, Node *second)
        {
            sizeMax = 1;
            if (first)
            {
                sizeMax = firstSizeMax;
                if (first->size != sizeMax)
                    return;
                if (mostFirstKey <= key)
                    sizeMax++;
                else
                    return;
            }
            if (second && mostSecondKey >= key)
                sizeMax += secondSizeMax;
        }

        void updateSizeMaxSuffixAndPrefix()
        {
            updateSizeMax(sizeMaxDecreasingSuffix, (right ? right->getSizeMaxDecreasingSuffix() : 0), (left ? left->getSizeMaxDecreasingSuffix() : 0), (right ? right->getMostLeftKey() : 0), (left ? left->getMostRightKey() : 0), right, left);
            updateSizeMax(sizeMaxIncreasingPrefix, (left ? left->getSizeMaxIncreasingPrefix() : 0), (right ? right->getSizeMaxIncreasingPrefix() : 0), (left ? left->getMostRightKey() : 0), (right ? right->getMostLeftKey() : 0), left, right);
        }

        void relax()
        {
            if (reversed)
            {
                std::swap(left, right);
                std::swap(sizeMaxIncreasingPrefix, sizeMaxDecreasingSuffix);
                std::swap(mostLeftKey, mostRightKey);
                if (left)
                    left->makeReversed();
                if (right)
                    right->makeReversed();
                reversed = false;
            }
        }

        void updateSize()
        {
            ui32 leftSize = left ? left->size : 0;
            ui32 rightSize = right ? right->size : 0;
            size = leftSize + rightSize + 1;
        }

        void updateSum()
        {
            long long leftSum = left ? left->sum : 0;
            long long rightSum = right ? right->sum : 0;
            sum = leftSum + rightSum + (long long)key;
        }

        void updateMostLeftAndRightKeys()
        {
           mostLeftKey = left ? left->getMostLeftKey() : key;
           mostRightKey = right ? right->getMostRightKey() : key;
        }

    public:
        int key, priority, mostRightKey, mostLeftKey;
        long long sum;
        ui32 size, sizeMaxDecreasingSuffix, sizeMaxIncreasingPrefix;
        bool reversed;
        Node *left, *right;

        int getMostLeftKey() const
        {
            return reversed ? mostRightKey : mostLeftKey;
        }

        int getMostRightKey() const
        {
            return reversed ? mostLeftKey : mostRightKey;
        }

        ui32 getSizeMaxDecreasingSuffix() const
        {
            return reversed ? sizeMaxIncreasingPrefix : sizeMaxDecreasingSuffix;
        }

        ui32 getSizeMaxIncreasingPrefix() const
        {
            return reversed ? sizeMaxDecreasingSuffix : sizeMaxIncreasingPrefix;
        }

        explicit Node(int newKey)
            :key(newKey), priority(getRandomInt()), reversed(false), left(nullptr),
             right(nullptr), sum(key), size(1), sizeMaxDecreasingSuffix(1),
            sizeMaxIncreasingPrefix(1), mostRightKey(newKey), mostLeftKey(newKey)
        {}

        ~Node()
        {
            delete left;
            delete right;
        }

        void makeReversed()
        {
            reversed = reversed ? false : true;
        }

        void update()
        {
            relax();
            updateMostLeftAndRightKeys();
            updateSize();
            updateSizeMaxSuffixAndPrefix();
            updateSum();
        }
    };

    struct TwoNodes
    {
        Node *begin, *end;

        TwoNodes(Node *first, Node *second)
            : begin(first), end(second)
        {}

        TwoNodes()
        {}

        ~TwoNodes()
        {}
    };


    struct ThreeNodes
    {
        Node *begin, *middle, *end;

        ThreeNodes(Node *first, Node *second, Node *third)
            : begin(first), middle(second), end(third)
        {}

        ThreeNodes()
        {}

        ~ThreeNodes()
        {}
    };

    Node* root;

    Node* merge(Node* firstNode, Node* secondNode)
    {
        if (!firstNode)
            return secondNode;
        if (!secondNode)
            return firstNode;
        firstNode->update();
        secondNode->update();
        Node* newRoot;
        if (firstNode->priority >= secondNode->priority)
        {
            firstNode->right = merge(firstNode->right, secondNode);
            newRoot = firstNode;
        }
        else
        {
            secondNode->left = merge(firstNode, secondNode->left);
            newRoot = secondNode;
        }
        newRoot->update();
        return newRoot;
    }

    TwoNodes split(Node* node, ui32 index)// ind-й элемент включается в правое дерево
    {
        if (!node)
            return TwoNodes(nullptr, nullptr);
        node->update();
        ui32 leftSize = node->left ? node->left->size : 0;
        TwoNodes result;
        if (leftSize > index)
        {
            TwoNodes splittedLeft = split(node->left, index);
            node->left = splittedLeft.end;
            result = TwoNodes(splittedLeft.begin, node);
        }
        else if (leftSize < index)
        {
            TwoNodes splittedRight = split(node->right, index - leftSize - 1);
            node->right = splittedRight.begin;
            result = TwoNodes(node, splittedRight.end);
        }
        else
        {
            Node* leftNode = node->left;
            node->left = nullptr;
            result = TwoNodes(leftNode, node);
        }
        node->update();
        return result;
    }

    TwoNodes splitIncreasingByKey(Node* node, int key)// элементы с key включаются в левое дерево
    {
        if (!node)
            return TwoNodes(nullptr, nullptr);
        node->update();
        TwoNodes result;
        if (node->key > key)
        {
            TwoNodes splittedLeft = splitIncreasingByKey(node->left, key);
            node->left = splittedLeft.end;
            result = TwoNodes(splittedLeft.begin, node);
        }
        else
        {
            TwoNodes splittedRight = splitIncreasingByKey(node->right, key);
            node->right = splittedRight.begin;
            result = TwoNodes(node, splittedRight.end);
        }
        node->update();
        return result;
    }

    ThreeNodes split(Node* node, ui32 left, ui32 right)
    {
        assert(left < right);
        TwoNodes firstSplittedNodes = split(node, right);
        TwoNodes secondSplittedNodes = split(firstSplittedNodes.begin, left);
        return ThreeNodes(secondSplittedNodes.begin, secondSplittedNodes.end, firstSplittedNodes.end);
    }

    Node* merge(TwoNodes nodes)
    {
        return merge(nodes.begin, nodes.end);
    }

    Node* merge(ThreeNodes nodes)
    {
        return merge(merge(nodes.begin, nodes.middle), nodes.end);
    }

    bool nextPermutation(Node* node)
    {
        if (!node)
            return 0;
        if (node->sizeMaxDecreasingSuffix == node->size)
        {
            node->makeReversed();
            return 0;
        }
        else
        {
            ThreeNodes firstSplittedNodes = split(node, node->size - node->sizeMaxDecreasingSuffix - 1, node->size - node->sizeMaxDecreasingSuffix);
            if(firstSplittedNodes.end)
                firstSplittedNodes.end->makeReversed();
            TwoNodes secondSplittedNodes = splitIncreasingByKey(firstSplittedNodes.end, firstSplittedNodes.middle->key);
            TwoNodes thirdSplittedNodes = split(secondSplittedNodes.end, 1);
            node = merge(merge(firstSplittedNodes.begin, thirdSplittedNodes.begin), merge(secondSplittedNodes.begin, merge(firstSplittedNodes.middle, thirdSplittedNodes.end)));
            return 1;
        }
    }

public:
    Treap()
        :root(nullptr)
    {
    }

    ~Treap()
    {
        delete root;
    }

    long long subsegmentSum(ui32 left, ui32 right)
    {
        assert(left < right && (!root || right <= root->size));
        ThreeNodes splittedNodes = split(root, left, right);
        long long result = splittedNodes.middle->sum;
        root = merge(splittedNodes);
        return result;
    }

    void insert(int value, ui32 index)
    {
        assert(!root || index <= root->size);
        Node* newNode = new Node(value);
        newNode->update();
        if (!root || (index == root->size))
            root = merge(root, newNode);
        else
        {
            TwoNodes splittedNodes = split(root, index);
            root = merge(merge(splittedNodes.begin, newNode), splittedNodes.end);
        }
    }

    void assign(int value, ui32 index)
    {
        assert(!root || index < root->size);
        ThreeNodes splittedNodes = split(root, index, index + 1);
        splittedNodes.middle->key = value;
        splittedNodes.middle->update();
        root = merge(splittedNodes);
    }

    bool nextPermutation(ui32 left, ui32 right)
    {
        assert(left < right && (!root || right <= root->size));
        ThreeNodes splittedNodes = split(root, left, right);
        bool result = nextPermutation(splittedNodes.middle);
        root = merge(splittedNodes);
        return result;
    }
};
