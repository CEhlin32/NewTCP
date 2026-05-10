#ifndef MSG_H
#define MSG_H

#include <string>
#include <MsgPacket.h>

namespace CE::tcp
{
    /**
     * @brief High-level message wrapper for building and transmitting TCP packets
     *
     * Msg provides an object-oriented layer over MsgPacket to describe an application-level
     * network message. It owns a MsgPacket instance and exposes a simple workflow:
     *
     * - Construct with a name and optional message ID (or an existing MsgPacket)
     * - Populate the message body in derived classes by overriding SerializeBody()
     * - Call Serialize() to build/update the underlying MsgPacket for transmission
     * - Access the resulting packet via GetMsgPacket()
     *
     * The base class assumes an empty body by default; derived message types implement
     * SerializeBody() and DeSerializeBody() to handle message-specific payload formats.
     *
     * Msg also tracks the connection ID and message ID through its MsgPacket, allowing
     * the transport layer to route and identify messages consistently.
     */
    class Msg
    {
    public:
        // @brief Constructor to initialize the message with a name and optional message ID
        // @param name The name of the message
        // @param msgID The message ID to set for the message packet (optional)
        Msg(std::string name, int msgID = -1);
        // @brief Constructor to initialize the message with a name and a pre-constructed message packet
        // @param name The name of the message
        // @param packet The message packet to associate with this message
        Msg(std::string name, MsgPacket& packet);
        // @brief Virtual destructor for the message
        virtual ~Msg();
        // @brief Serialize the message into a MsgPacket for transmission
        // @return Reference to the serialized MsgPacket
        MsgPacket& Serialize();
        // @brief Get the name of the message
        // @return The name of the message
        std::string GetName() const;
        // @brief Get the connection ID associated with the message
        // @return The connection ID associated with the message
        int GetConnectionID() const;
        // @brief Get the message ID of the message
        int GetMsgID() const;
        // @brief Get the MsgPacket associated with this message, which can be used for transmission
        // @return Reference to the MsgPacket associated with this message
        MsgPacket& GetMsgPacket(); 
        
        void SetNeverEncrypted()
        {
            m_MsgPacket.SetIsEncrypted(false);
            m_NeverEncrypted = true;
        }
        protected:
        // Virtual methods for serializing and deserializing the message body, which can be overridden by derived classes to handle specific message content
        virtual void SerializeBody(){/*Default is no Body to Serialize*/;}
        // Virtual method for deserializing the message body, which can be overridden by derived classes to handle specific message content
        virtual void DeSerializeBody() {/*Default is no Body to Deserialize*/;}

        // Add members and methods as needed
        std::string m_Name;
        MsgPacket m_MsgPacket;
        bool m_NeverEncrypted; // Flag to indicate if this message should never be encrypted, can be set by derived classes
    };
} // namespace CE::tcp



#endif // MSG_H