#include "slow_vector.h"
#include <random>

std::default_random_engine generator;

int getRandomInt(int a, int b)
{
    std::uniform_int_distribution<int> distribution(a, b);
    return distribution(generator);
}

class Treap: public IPermutableArray
{
    struct Node
    {
        int key, priority, mostRightKey, mostLeftKey;
        long long sum;
        ui32 size, sizeMaxDecreasingSuffix, sizeMaxIncreasingPrefix;
        bool reversed;
        Node *left, *right;

        explicit Node(int newKey)
            :key(newKey), priority(getRandomInt(INT_MIN, INT_MAX)), reversed(false), left(nullptr), right(nullptr), sum(key), size(1), sizeMaxDecreasingSuffix(1), mostRightKey(newKey), mostLeftKey(newKey)
        {}

        ~Node()
        {
            delete left;
            delete right;
        }

        void updateMostLeftAndRightKeys()
        {
           mostLeftKey = left ? left->getMostLeftKey() : key;
           mostRightKey = right ? right->getMostRightKey() : key;
        }

        void makeReversed()
        {
            reversed = reversed ? false : true;
        }

        int getMostLeftKey()
        {
            return reversed ? mostRightKey : mostLeftKey;
        }

        int getMostRightKey()
        {
            return reversed ? mostLeftKey : mostRightKey;
        }

        ui32 getSizeMaxDecreasingSuffix()
        {
            return reversed ? sizeMaxIncreasingPrefix : sizeMaxDecreasingSuffix;
        }

        ui32 getSizeMaxIncreasingPrefix()
        {
            return reversed ? sizeMaxDecreasingSuffix : sizeMaxIncreasingPrefix;
        }


        void updateSizeMaxSuffixAndPrefix()
        {
            if (!left && !right)
            {
                sizeMaxDecreasingSuffix = 1;
                sizeMaxIncreasingPrefix = 1;
            }
            else if (!left)
            {
                sizeMaxIncreasingPrefix = 1;
                sizeMaxDecreasingSuffix = right->getSizeMaxDecreasingSuffix();
                if (right->getMostLeftKey() >= key)
                    sizeMaxIncreasingPrefix += right->getSizeMaxIncreasingPrefix();
                if (right->getSizeMaxDecreasingSuffix() == right->size && right->getMostLeftKey() <= key)
                    sizeMaxDecreasingSuffix += 1;
            }
            else if (!right)
            {
                sizeMaxIncreasingPrefix = left->getSizeMaxIncreasingPrefix();
                sizeMaxDecreasingSuffix = 1;
                if (left->getMostRightKey() >= key)
                    sizeMaxDecreasingSuffix += left->getSizeMaxDecreasingSuffix();
                if (left->getSizeMaxIncreasingPrefix() == left->size && left->getMostRightKey() <= key)
                    sizeMaxIncreasingPrefix += 1;
            }
            else
            {

                if (right->getSizeMaxDecreasingSuffix() < right->size)
                    sizeMaxDecreasingSuffix = right->getSizeMaxDecreasingSuffix();
                else
                {
                    if (key >= right->getMostLeftKey())
                    {
                        sizeMaxDecreasingSuffix = right->size + 1;
                        if (key <= left->getMostRightKey())
                            sizeMaxDecreasingSuffix += left->getSizeMaxDecreasingSuffix();
                    }
                    else
                        sizeMaxDecreasingSuffix = right->getSizeMaxDecreasingSuffix();
                }

                if (left->getSizeMaxIncreasingPrefix() < left->size)
                    sizeMaxIncreasingPrefix = left->getSizeMaxIncreasingPrefix();
                else
                {
                    if (key >= left->getMostRightKey())
                    {
                        sizeMaxIncreasingPrefix = left->size + 1;
                        if (key <= right->getMostLeftKey())
                            sizeMaxIncreasingPrefix += right->getSizeMaxIncreasingPrefix();
                    }
                        else
                            sizeMaxIncreasingPrefix = left->getSizeMaxIncreasingPrefix();
                }
            }
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
        void update()
        {
            //std::cout << leftSum << " + " << rightSum << " + " << key << " = " << sum << "\n";
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

        ~TwoNodes()
        {
            //delete begin;
            //delete end;
        }
    };


    struct ThreeNodes
    {
        Node *begin, *middle, *end;

        ThreeNodes(Node *first, Node *second, Node *third)
            : begin(first), middle(second), end(third)
        {}

        ~ThreeNodes()
        {
            //delete begin;
            //delete middle;
            //delete end;
        }
    };

    Node* root;

    Node* merge(Node* firstNode, Node* secondNode)
    {
        if (!firstNode)
            return secondNode;
        if (!secondNode)
            return firstNode;
        firstNode->relax();
        secondNode->relax();
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
        node->relax();
        ui32 leftSize = node->left ? node->left->size : 0;
        if (leftSize > index)
        {
            TwoNodes splittedLeft = split(node->left, index);
            node->left = splittedLeft.end;
            node->update();
            return TwoNodes(splittedLeft.begin, node);
        }
        else if (leftSize < index)
        {
            TwoNodes splittedRight = split(node->right, index - leftSize - 1);
            node->right = splittedRight.begin;
            node->update();
            return TwoNodes(node, splittedRight.end);
        }
        else
        {
            Node* leftNode = node->left;
            node->left = nullptr;
            node->update();
            return TwoNodes(leftNode, node);
        }
    }

    TwoNodes splitIncreasingByKey(Node* node, int key)// элементы с key включаются в левое дерево
    {
        if (!node)
            return TwoNodes(nullptr, nullptr);
        node->relax();
        node->update();
        if (node->key > key)
        {
            TwoNodes splittedLeft = splitIncreasingByKey(node->left, key);
            node->left = splittedLeft.end;
            node->update();
            return TwoNodes(splittedLeft.begin, node);
        }
        else
        {
            TwoNodes splittedRight = splitIncreasingByKey(node->right, key);
            node->right = splittedRight.begin;
            node->update();
            return TwoNodes(node, splittedRight.end);
        }
    }

    ThreeNodes split(Node* node, ui32 left, ui32 right)
    {
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
            return 1;
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

    long long subsegmentSum(ui32 left, ui32 right)
    {
        ThreeNodes splittedNodes = split(root, left, right);
        long long result = splittedNodes.middle->sum;
        root = merge(splittedNodes);
        return result;
    }

    void insert(int value, ui32 index)
    {
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
        ThreeNodes splittedNodes = split(root, index, index + 1);
        splittedNodes.middle->key = value;
        splittedNodes.middle->update();
        root = merge(splittedNodes);
    }

    bool nextPermutation(ui32 left, ui32 right)
    {
        ThreeNodes splittedNodes = split(root, left, right);
        bool result = nextPermutation(splittedNodes.middle);
        root = merge(splittedNodes);
        return result;
    }
};
