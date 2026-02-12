#ifndef MSG_PACKET_H
#define MSG_PACKET_H

#include <string>
#include <vector>
#include <CommonDefinitions.h>

namespace newtcp
{
    class CompondData
    {
    public:

        void SetData(std::string strData)
        {
            DataAsStr = strData;
            DataAsBytes = std::vector<uint8_t>(strData.begin(), strData.end());
        }
        void SetData(std::vector<uint8_t> byteData)
        {
            DataAsBytes = byteData;
            DataAsStr = std::string(byteData.begin(), byteData.end());
        }   
        std::string DataAsStr;
        std::vector<uint8_t> DataAsBytes;

    };

    class MsgPacket
    {
    public:


        MsgPacket(std::string name = "", int msgID = -1);
        ~MsgPacket();
        // Add members and methods as needed

        std::string GetName() const { return m_Name; }
        int GetPrefix() const;  
        void SetMsgID(int msgID);
        int GetMsgID() const;
        void ChangeRelativeIDToAbsID();
        void ChangeAbsoluteIDToRelID();

        void SetServerType(TCPServerTypes type);
        TCPServerTypes GetServerType() const;

        void SetIsEncrypted(bool isEncrypted);
        bool GetIsEncrypted() const;
        int GetPostfix() const;

        void SetConnectionID(int connectionID)
        {
            m_ConnectionID = connectionID;
        }
        int GetConnectionID() const
        {
            return m_ConnectionID;
        }

        size_t GetMsgHdrSize() const
        {
            return sizeof(PacketHdrData);
        }
        uint8_t* GetMsgHdrDataPtr()
        {
            return reinterpret_cast<uint8_t*>(&m_Data);
        }      

        void SetMsgBodySize(size_t size)
        {
            m_Data.MsgBodySize = static_cast<int>(size);
        }   
        size_t GetMsgBodySize() const
        {
            return m_Data.MsgBodySize;
        }
        
        /// @brief Get the packet data as a encrypted or plain  data string
        /// @return Reference to the packet data string
        std::string& GetPacketDataAsStr() 
        { 
            return m_PacketData.DataAsStr; 
        }
        /// @brief Get the packet data as a encrypted or plain  data bytes
        /// @return Reference to the packet data bytes
        std::vector<uint8_t>& GetPacketDataAsBytes() 
        { 
            return m_PacketData.DataAsBytes; 
        }

        void SetPacketDataFromBytes(std::vector<uint8_t>& data)
        {
            m_PacketData.SetData(data);
        }
    
        void SetPacketDataFromStr(std::string& str)
        {
            m_PacketData.SetData(str);
        }

    ////////////////////////////
        /// @brief Get the body data as a encrypted or plain  data string
        /// @return Reference to the body data string
        std::string& GetBodyDataAsStr() 
        { 
            return m_BodyData.DataAsStr; 
        }
        /// @brief Get the body data as a encrypted or plain  data bytes
        /// @return Reference to the body data bytes
        std::vector<uint8_t>& GetBodyDataAsBytes() 
        { 
            return m_BodyData.DataAsBytes; 
        }

        void SetBodyDataFromBytes(std::vector<uint8_t>& data)
        {
            m_BodyData.SetData(data);
        }
    
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
            int Postfix;
        };
        PacketHdrData m_Data;
        std::string m_Name;
        int m_ConnectionID;
        TCPServerTypes ServerType;

        /// @brief Holds the plain Msg body data
        CompondData m_BodyData;
        /// @brief Holds the data to be sent or received over the network, either encrypted or plain
        CompondData m_PacketData;
        bool IsMsgIDAbsolute = false;
    };
} // namespace newtcp


#endif // MSG_PACKET_H