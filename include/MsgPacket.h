#ifndef MSG_PACKET_H
#define MSG_PACKET_H

#include <string>
#include <cstdint>
#include <vector>
#include <CommonDefinitions.h>

namespace CE::tcp
{

    /**
     * @brief Dual-representation data container maintaining synchronized string and byte formats
     * 
     * CompondData is a utility class that stores data simultaneously as both a string (DataAsStr)
     * and a byte vector (DataAsBytes), ensuring both representations remain synchronized. This
     * eliminates the need for manual conversions and prevents inconsistencies when data needs to
     * be accessed in different formats.
     * 
     * The class provides overloaded SetData() methods that accept either string or byte vector
     * input. When data is set in one format, the other format is automatically generated and
     * updated, maintaining perfect synchronization between the two representations.
     * 
     * Use Cases:
     * - Network protocols requiring both byte-level and string-level data access
     * - Data that needs to be processed as bytes but also logged/displayed as strings
     * - Interfacing between APIs that expect different data formats
     * 
     * Both data members (DataAsStr and DataAsBytes) are publicly accessible for direct read
     * access, but modifications should be done through SetData() to maintain synchronization.
     * 
     * @note Direct modification of DataAsStr or DataAsBytes will break synchronization between
     *       the two formats. Always use SetData() when updating the contained data.
     */
    class CompondData
    {
    public:

        // @brief Set the data from a string, which also updates the byte representation
        // @param strData The string data to set
        
        void SetData(std::string strData)
        {
            DataAsStr = strData;
            DataAsBytes = std::vector<uint8_t>(strData.begin(), strData.end());
        }
        // @brief Set the data from a byte vector, which also updates the string representation
        // @param byteData The byte vector data to set
        void SetData(std::vector<uint8_t> byteData)
        {
            DataAsBytes = byteData;
            DataAsStr = std::string(byteData.begin(), byteData.end());
        }   
        std::string DataAsStr;
        std::vector<uint8_t> DataAsBytes;

    };

    /**
     * @brief Network message packet container for TCP communication
     * 
     * MsgPacket encapsulates a complete network message with header and body components,
     * providing a structured way to transmit data over TCP connections. The class manages
     * both the packet metadata (prefix, message ID, body size, encryption status, postfix)
     * and the actual message payload.
     * 
     * Key Features:
     * - Packet Header: Contains prefix/postfix markers, message ID, body size, and encryption flag
     * - Dual Data Formats: Automatically maintains data in both string and byte vector formats
     * - Encryption Support: Tracks encryption status and manages encrypted/plain packet data
     * - Server Type Association: Links packets to specific TCP server types
     * - Connection Tracking: Associates packets with specific connection IDs
     * 
     * The class uses the CompondData helper to ensure data consistency between string and
     * byte representations. It separates plain body data (m_BodyData) from the packet data
     * (m_PacketData) that may be encrypted for transmission.
     * 
     * Typical Usage:
     * 1. Create a packet with a name and message ID
     * 2. Set body data using SetBodyDataFromStr() or SetBodyDataFromBytes()
     * 3. Configure encryption, server type, and connection ID as needed
     * 4. Use GetPacketDataAsBytes() for transmission after encryption (if required)
     * 5. On reception, populate packet data and extract body using GetBodyDataAsStr()
     */
    class MsgPacket
    {
    public:

        // @brief Constructor to initialize the message packet with a name and message ID
        // @param name The name of the message packet
        // @param msgID The message ID of the packet
        MsgPacket(std::string name = "Unknown");
        
        // @brief Destructor for the message packet
        virtual ~MsgPacket();
        // @brief Get the prefix value of the packet
        // @return The prefix value of the packet
        int GetPrefix() const;  
        // @brief Get the postfix value of the packet
        // @return The postfix value of the packet
        int GetPostfix() const;
        // @brief Set the message ID of the packet
        // @param msgID The message ID to set for the packet
        void SetMsgID(int msgID);
        // @brief Get the message ID of the packet
        // @return The message ID of the packet
        int GetMsgID() const;
        // @brief Set the server type for the packet
        // @param type The TCP server type to set for the packet
        void SetServerType(TCPServerTypes type);
        // @brief Get the server type of the packet
        // @return The TCP server type of the packet
        TCPServerTypes GetServerType() const;

        // @brief Set whether the packet is encrypted
        // @param isEncrypted True if the packet is encrypted, false otherwise
        void SetIsEncrypted(bool isEncrypted);
        // @brief Get whether the packet is encrypted
        // @return True if the packet is encrypted, false otherwise
        bool GetIsEncrypted() const;

        
        // @brief Set the connection ID for the packet
        // @param connectionID The connection ID to set for the packet
        void SetConnectionID(int connectionID)
        {
            m_ConnectionID = connectionID;
        }
        // @brief Get the connection ID of the packet
        // @return The connection ID of the packet
        int GetConnectionID() const
        {
            return m_ConnectionID;
        }

        // @brief Get the size of the packet header
        // @return The size of the packet header in bytes
        size_t GetMsgHdrSize() const
        {
            return sizeof(PacketHdrData);
        }
        // @brief  Get a pointer to the packet header data, which can be used for serialization or deserialization
        // @return Pointer to the packet header data
        uint8_t* GetMsgHdrDataPtr()
        {
            return reinterpret_cast<uint8_t*>(&m_Data);
        }      

        // @brief Set the size of the message body
        // @param size The size of the message body in bytes
        void SetMsgBodySize(size_t size)
        {
            if(size > 0)
            {
                m_Data.IsEncrypted = true; // Mark packet as encrypted if body size is greater than 0, can be overridden by derived classes
            }
            m_Data.MsgBodySize = static_cast<int>(size);
        }   
        // @brief Get the size of the message body
        // @return The size of the message body in bytes   
        size_t GetMsgBodySize() const
        {
            return m_Data.MsgBodySize;
        }
        
        // @brief Get the packet data as a encrypted or plain  data string
        // @return Reference to the packet data string
        std::string& GetPacketDataAsStr() 
        { 
            return m_PacketData.DataAsStr; 
        }
        // @brief Get the packet data as a encrypted or plain  data bytes
        // @return Reference to the packet data bytes
        std::vector<uint8_t>& GetPacketDataAsBytes() 
        { 
            return m_PacketData.DataAsBytes; 
        }

        // @brief  Set the packet data from a string, which also updates the byte representation
        // @param str The string data to set for the packet
        // @param data 
        void SetPacketDataFromBytes(std::vector<uint8_t>& data)
        {
            
            m_PacketData.SetData(data);
        }
    
        // @brief  Set the packet data from a string, which also updates the byte representation
        // @param str The string data to set for the packet
        void SetPacketDataFromStr(std::string& str)
        {
            m_PacketData.SetData(str);
        }

    ////////////////////////////
        // @brief Get the body data as a encrypted or plain  data string
        // @return Reference to the body data string
        std::string& GetBodyDataAsStr() 
        { 
            return m_BodyData.DataAsStr; 
        }
        // @brief Get the body data as a encrypted or plain  data bytes
        // @return Reference to the body data bytes
        std::vector<uint8_t>& GetBodyDataAsBytes() 
        { 
            return m_BodyData.DataAsBytes; 
        }

        // @brief  Set the body data from a string, which also updates the byte representation
        // @param str The string data to set for the body
        // @param data 
        void SetBodyDataFromBytes(std::vector<uint8_t>& data)
        {
            m_BodyData.SetData(data);
        }
    
        // @brief  Set the body data from a string, which also updates the byte representation
        // @param str The string data to set for the body
        void SetBodyDataFromStr(std::string& str)
        {
            m_BodyData.SetData(str);
        }



    private:
        struct PacketHdrData
        {
            // Define the structure of the packet data
            int Prefix;
            int MsgID;
            int MsgBodySize;
            bool IsEncrypted;
            uint8_t dummy1; // padding for bool isEncrypted to ensure consistent struct size across different compilers/platforms
            uint8_t dummy2; //
            uint8_t dummy3; //
            int Postfix;
        };
        PacketHdrData m_Data;
        int m_ConnectionID;
        TCPServerTypes ServerType;

        // @brief Holds the plain Msg body data
        CompondData m_BodyData;
        // @brief Holds the data to be sent or received over the network, either encrypted or plain
        CompondData m_PacketData;
    };
} // namespace CE::tcp


#endif // MSG_PACKET_H