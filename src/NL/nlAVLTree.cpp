#include "NL/nlAVLTree.h"

/**
 * Offset/Address/Size: 0x0 | 0x802A8D80 | size: 0x360
 */
AVLTreeNode* AVLTreeUntemplated::AddAVLNode(
    AVLTreeNode** rootNode, void* key, void* value, AVLTreeNode** existingNode)
{
    AVLTreeNode** root;
    AVLTreeNode** existing;
    AVLTreeUntemplated* self;
    void* searchKey;
    void* entryValue;
    AVLTreeNode* balance;
    AVLTreeNode* next;
    AVLTreeNode* underBalance;
    AVLTreeNode* currSearch;
    AVLTreeNode* currBalance;
    AVLTreeNode* currAdjust;
    AVLTreeNode* newNode;
    unsigned int lessThanBalance;
    int comp;
    signed char* pathInfo;
    unsigned int stackTop;

    root = rootNode;
    existing = existingNode;
    self = this;
    searchKey = key;
    entryValue = value;

    *existing = 0;

    next = *root;
    if (next == 0)
    {
        newNode = self->AllocateEntry(searchKey, entryValue);
        *root = newNode;
        return newNode;
    }

    AVLTreeNode* father = 0;
    unsigned int balanceSpot;
    balance = next;
    currSearch = next;

    pathInfo = (signed char*)__alloca(32);

    stackTop = 0;
    balanceSpot = 0;

    while (true)
    {
        comp = self->CompareKey(searchKey, currSearch);
        pathInfo[stackTop] = comp;
        stackTop++;

        if (comp == 0)
        {
            *existing = currSearch;
            return currSearch;
        }

        if (comp > 0)
        {
            next = currSearch->right;
            if (next == 0)
            {
                newNode = self->AllocateEntry(searchKey, entryValue);
                currSearch->right = newNode;
                break;
            }
        }
        else
        {
            next = currSearch->left;
            if (next == 0)
            {
                newNode = self->AllocateEntry(searchKey, entryValue);
                currSearch->left = newNode;
                break;
            }
        }

        if (next->heavy != 0)
        {
            father = currSearch;
            balance = next;
            balanceSpot = stackTop;
        }

        currSearch = next;
    }

    comp = pathInfo[balanceSpot];
    lessThanBalance = ((unsigned int)comp) >> 31;

    if (lessThanBalance)
    {
        underBalance = balance->left;
    }
    else
    {
        underBalance = balance->right;
    }

    currAdjust = underBalance;
    while (currAdjust != newNode)
    {
        signed char path = pathInfo[++balanceSpot];

        if (path < 0)
        {
            currAdjust->heavy = -1;
            currAdjust = currAdjust->left;
        }
        else
        {
            currAdjust->heavy = 1;
            currAdjust = currAdjust->right;
        }
    }

    if (lessThanBalance)
    {
        if (balance->heavy == 0)
        {
            balance->heavy = -1;
            return newNode;
        }

        if (balance->heavy < 0)
        {
            if (underBalance->heavy < 0)
            {
                balance->left = underBalance->right;
                underBalance->right = balance;
                balance->heavy = 0;
                underBalance->heavy = 0;
                currBalance = underBalance;
            }
            else
            {
                currBalance = underBalance->right;
                underBalance->right = currBalance->left;
                currBalance->left = underBalance;
                balance->left = currBalance->right;
                currBalance->right = balance;

                if (currBalance->heavy == 0)
                {
                    balance->heavy = 0;
                    underBalance->heavy = 0;
                }
                else if (currBalance->heavy < 0)
                {
                    balance->heavy = 1;
                    underBalance->heavy = 0;
                }
                else
                {
                    balance->heavy = 0;
                    underBalance->heavy = -1;
                }

                currBalance->heavy = 0;
            }
        }
        else
        {
            balance->heavy = 0;
            return newNode;
        }
    }
    else
    {
        if (balance->heavy == 0)
        {
            balance->heavy = 1;
            return newNode;
        }

        if (balance->heavy > 0)
        {
            if (underBalance->heavy > 0)
            {
                balance->right = underBalance->left;
                underBalance->left = balance;
                balance->heavy = 0;
                underBalance->heavy = 0;
                currBalance = underBalance;
            }
            else
            {
                currBalance = underBalance->left;
                underBalance->left = currBalance->right;
                currBalance->right = underBalance;
                balance->right = currBalance->left;
                currBalance->left = balance;

                if (currBalance->heavy == 0)
                {
                    balance->heavy = 0;
                    underBalance->heavy = 0;
                }
                else if (currBalance->heavy > 0)
                {
                    balance->heavy = -1;
                    underBalance->heavy = 0;
                }
                else
                {
                    balance->heavy = 0;
                    underBalance->heavy = 1;
                }

                currBalance->heavy = 0;
            }
        }
        else
        {
            balance->heavy = 0;
            return newNode;
        }
    }

    if (father == 0)
    {
        *root = currBalance;
    }
    else if (balance == father->right)
    {
        father->right = currBalance;
    }
    else
    {
        father->left = currBalance;
    }

    return newNode;
}

/**
 * Offset/Address/Size: 0x360 | 0x802A90E0 | size: 0x458
 */
AVLTreeNode* AVLTreeUntemplated::RemoveAVLNode(AVLTreeNode** root, void* key)
{
    AVLTreeNode** stack;
    signed char* pathInfo;
    unsigned int stackTop;
    unsigned int savedStackTop;
    int comp;
    AVLTreeNode* deleted;
    AVLTreeNode* curr;
    AVLTreeNode* prev;
    AVLTreeNode* min;
    AVLTreeNode* kid;
    AVLTreeNode* grandkid;

    if (*root == 0)
    {
        return 0;
    }

    stack = (AVLTreeNode**)__alloca(32 * sizeof(AVLTreeNode*));
    pathInfo = (signed char*)__alloca(32);

    stackTop = 0;
    curr = *root;

    while (true)
    {
        comp = CompareKey(key, curr);
        if (comp < 0)
        {
            pathInfo[stackTop] = -1;
            stack[stackTop] = curr;
            stackTop++;
            curr = curr->left;
        }
        else if (comp > 0)
        {
            pathInfo[stackTop] = 1;
            stack[stackTop] = curr;
            stackTop++;
            curr = curr->right;
        }
        else
        {
            break;
        }
        if (curr == 0)
        {
            return 0;
        }
    }

    if (curr->left == 0)
    {
        if (stackTop == 0)
        {
            *root = curr->right;
        }
        else
        {
            prev = stack[stackTop - 1];
            if (curr == prev->left)
                prev->left = curr->right;
            else
                prev->right = curr->right;
        }
    }
    else if (curr->right == 0)
    {
        if (stackTop == 0)
        {
            *root = curr->left;
        }
        else
        {
            prev = stack[stackTop - 1];
            if (curr == prev->left)
                prev->left = curr->left;
            else
                prev->right = curr->left;
        }
    }
    else
    {
        savedStackTop = stackTop;
        pathInfo[stackTop] = 1;
        stack[stackTop] = curr;
        stackTop++;
        min = curr->right;

        do
        {
            pathInfo[stackTop] = -1;
            stack[stackTop] = min;
            stackTop++;
            min = min->left;
        } while (min != 0);

        stackTop--;
        min = stack[stackTop];

        if (savedStackTop == 0)
        {
            *root = min;
        }
        else
        {
            prev = stack[savedStackTop - 1];
            if (curr == prev->left)
                prev->left = min;
            else
                prev->right = min;
        }

        prev = stack[stackTop - 1];
        if (prev != curr)
        {
            prev->left = min->right;
            min->right = curr->right;
        }

        min->left = curr->left;
        min->heavy = curr->heavy;
        stack[savedStackTop] = min;
    }

    deleted = curr;

    while (stackTop > 0)
    {
        stackTop--;
        curr = stack[stackTop];

        if (curr->heavy == 0)
        {
            curr->heavy -= pathInfo[stackTop];
            break;
        }

        curr->heavy -= pathInfo[stackTop];
        if (curr->heavy == 0)
            continue;

        if (pathInfo[stackTop] == 1)
            kid = curr->left;
        else
            kid = curr->right;

        if (kid->heavy != pathInfo[stackTop])
        {
            if (pathInfo[stackTop] != 1)
            {
                curr->right = kid->left;
                kid->left = curr;
            }
            else
            {
                curr->left = kid->right;
                kid->right = curr;
            }

            if (stackTop != 0)
            {
                if (pathInfo[stackTop - 1] == 1)
                    stack[stackTop - 1]->right = kid;
                else
                    stack[stackTop - 1]->left = kid;
            }
            else
            {
                *root = kid;
            }

            if (kid->heavy == 0)
            {
                comp = -1;
                if (curr->heavy > 0)
                    comp = 1;

                curr->heavy = comp;
                kid->heavy = -curr->heavy;
                break;
            }

            kid->heavy = 0;
            curr->heavy = 0;
        }
        else
        {
            if (pathInfo[stackTop] != 1)
            {
                grandkid = kid->left;
                kid->left = grandkid->right;
                curr->right = grandkid->left;
                grandkid->right = kid;
                grandkid->left = curr;

                if (grandkid->heavy == 1)
                {
                    curr->heavy = -1;
                    kid->heavy = 0;
                }
                else if (grandkid->heavy == -1)
                {
                    curr->heavy = 0;
                    kid->heavy = 1;
                }
                else
                {
                    kid->heavy = 0;
                    curr->heavy = 0;
                }
            }
            else
            {
                grandkid = kid->right;
                kid->right = grandkid->left;
                curr->left = grandkid->right;
                grandkid->left = kid;
                grandkid->right = curr;

                if (grandkid->heavy == 1)
                {
                    curr->heavy = 0;
                    kid->heavy = -1;
                }
                else if (grandkid->heavy == -1)
                {
                    curr->heavy = 1;
                    kid->heavy = 0;
                }
                else
                {
                    curr->heavy = 0;
                    kid->heavy = 0;
                }
            }

            grandkid->heavy = 0;

            if (stackTop != 0)
            {
                if (pathInfo[stackTop - 1] == 1)
                    stack[stackTop - 1]->right = grandkid;
                else
                    stack[stackTop - 1]->left = grandkid;
            }
            else
            {
                *root = grandkid;
            }
        }
    }

    return deleted;
}
