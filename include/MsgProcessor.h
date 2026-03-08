#ifndef NEW_MSG_FACTORY_H
#define NEW_MSG_FACTORY_H

#include <vector>
#include <Msg.h>
#include <MsgPacket.h>


namespace CE::tcp
{

    // @brief Interface for processing Msg instances created from MsgPacket data
    /*
     *
     * IMsgProcessor defines a pure virtual method ProcessMsg() that must be implemented by
     * any class that wants to handle Msg instances created from incoming MsgPacket data.
     * This allows for flexible and decoupled message handling logic, where different
     * processors can be registered to handle different types of messages based on their
     * content or associated metadata.
     *
     * Implementing classes will typically inspect the Msg instance, determine its type or
     * content, and execute appropriate business logic in response to the message.
     */
    class IMsgProcessor
    {
    public:
        virtual bool ProcessMsg(Msg& msg) = 0;
    };
    /* @brief Interface for creating Msg instances from MsgPacket data
     *
     * ICreateMsgFromPacket defines a pure virtual method CreateMsgFromPacketInternal() that
     * must be implemented by any class that wants to create Msg instances from incoming
     * MsgPacket data. This allows for flexible and decoupled message creation logic, where
     * different creators can be registered to handle different types of packets based on
     * their content or associated metadata.
     *
     * Implementing classes will typically inspect the MsgPacket, determine the appropriate
     * Msg type to create, and return a new instance of that Msg type populated with data
     * extracted from the packet.
     */
    class ICreateMsgFromPacket
    {
    public:
        virtual Msg* CreateMsg(MsgPacket& packet) = 0;
    };

    /**
     * @brief Message dispatch hub for creating and processing Msg instances from MsgPacket data
     *
     * MsgProcessor maintains static registries of handlers (IMsgProcessor) and creators
     * (ICreateMsgFromPacket) that are used to translate incoming packets into Msg objects
     * and then route them to appropriate processors.
     *
     * Typical flow:
     * - Register creators/handlers via AddMsgCreator() and AddMsgProcessor()
     * - Call ProcessMsgFromPacket() for an incoming packet
     * - A registered creator builds a Msg instance from the packet
     * - A registered processor handles the created message
     *
     * The class exposes static entry points for convenience while also defining virtual
     * hooks for derived implementations that provide specific creation/processing logic.
     */
    class MsgProcessor
    {
    public:
        // @brief Constructor and destructor for MsgProcessor
        MsgProcessor();
        // @brief Destructor for MsgProcessor
        ~MsgProcessor();
        // @brief Add a message processor to the static registry
        // @param msgProcessor Pointer to the message processor to add
        static void AddMsgProcessor (IMsgProcessor* msgProcessor);
        // @brief Remove a message processor from the static registry
        // @param msgProcessor Pointer to the message processor to remove
        static void RemoveMsgProcessor (IMsgProcessor* msgProcessor);

        // @brief Add a message creator to the static registry
        // @param msgCreator Pointer to the message creator to add
        static void AddMsgCreator (ICreateMsgFromPacket* msgCreator);
        // @brief Remove a message creator from the static registry
        // @param msgCreator Pointer to the message creator to remove
        static void RemoveMsgCreator (ICreateMsgFromPacket* msgCreator);

        // @brief Process a message from a packet using registered processors
        // @param packet The message packet to process
        // @return True if the message was successfully processed, false otherwise
        static bool ProcessMsgFromPacket(MsgPacket& packet);
        // @brief Create a message from a packet using registered creators
        // @param packet The message packet to create a message from
        // @return Pointer to the created message, or nullptr if creation failed
        static Msg* CreateMsgFromPacket(MsgPacket& packet);
protected:
        // @brief Process a message using the internal logic of the derived class
        // @param msg The message to process
        // @return True if the message was successfully processed, false otherwise
        virtual bool ProcessMsg(Msg& msg) = 0;
        // @brief Create a message from a packet using the internal logic of the derived class
        // @param packet The message packet to create a message from
        // @return Pointer to the created message, or nullptr if creation failed
        virtual Msg* CreateMsg(MsgPacket& packet) = 0;
        private:
        // @brief Static registries for message processors and creators
        static std::vector<IMsgProcessor*> m_MsgProcessors;
        // @brief Static registries for message processors and creators
        static std::vector<ICreateMsgFromPacket*> m_MsgCreators;

    };

} // namespace CE::tcp



#endif // NEW_MSG_FACTORY_H