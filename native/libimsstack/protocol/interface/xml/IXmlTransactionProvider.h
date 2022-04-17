#ifndef INTERFACE_XML_TRANSACTION_PROVIDER_H_
#define INTERFACE_XML_TRANSACTION_PROVIDER_H_

#include "IXmlStateListener.h"
#include "IXmlTransaction.h"

/**
 * @brief This class provides an interface for parsing XML document asynchronously.
 *
 * @see IXmlStateListener, IXmlTransaction
 */
class IXmlTransactionProvider
{
public:
    /**
     * @brief Creates a transaction to parse an XML document.
     */
    virtual IXmlTransaction* CreateTransaction() = 0;

    /**
     * @brief Destroys the specified transaction.
     */
    virtual void DestroyTransaction(IN IXmlTransaction*& piTransaction) = 0;

    /**
     * @brief Pushes an XML transaction to be processed.
     *
     * @param piTransaction an XML transaction
     * @return If the transaction is successfully pushed, returns IMS_SUCCESS.
     *         Otherwise, returns IMS_FAILURE.
     */
    virtual IMS_RESULT Push(IN IXmlTransaction* piTransaction) = 0;

    /**
     * @brief Pops a topmost XML transaction.
     *
     * @return Pointer to IXmlTransaction.
     */
    virtual IXmlTransaction* Pop() = 0;

    /**
     * @brief Returns a state of XML transaction provider.
     *
     * @return The current state of this IXmlTransactionProvider.\n
     *         #STATE_IDLE\n
     *         #STATE_ATTACHING\n
     *         #STATE_ATTACHED\n
     *         #STATE_DETACHED\n
     */
    virtual IMS_SINT32 GetState() const = 0;

    /**
     * @brief Sets an XML state listener.
     *
     * @param piListener XML state listener
     */
    virtual void SetStateListener(IN IXmlStateListener* piListener) = 0;

public:
    enum
    {
        STATE_IDLE = 0,
        STATE_ATTACHING,
        STATE_ATTACHED,
        STATE_DETACHED
    };
};

#endif
