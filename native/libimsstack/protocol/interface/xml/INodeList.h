#ifndef INTERFACE_NODE_LIST_H
#define INTERFACE_NODE_LIST_H

class INode;

/**
 * @brief This class provides the abstraction of an ordered collection of nodes,
 *        without defining or constraining how this collection is implemented.
 *
 * @see INode
 */
class INodeList
{
public:
    /**
     * @brief Returns the number of nodes in the list.
     *
     * The range of valid child node indices is 0 to (length - 1) inclusive.
     *
     * @return The number of nodes in this list.
     */
    virtual IMS_SINT32 GetLength() const = 0;

    /**
     * @brief Returns an item in the index of this list.
     *
     * If index is greater than or equal to the number of nodes in the list, this returns null.
     * The range of valid child node indices is 0 to (length - 1) inclusive.
     *
     * @param nIndex The index in this list
     * @return The node at the indexth position in the NodeList,
     *         or null if that is not a valid index.
     */
    virtual INode* Item(IN IMS_SINT32 nIndex) const = 0;
};

#endif
