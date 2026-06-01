#ifndef MSG_MANAGER_H
#define MSG_MANAGER_H
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <string_view>
#include <functional>
#include <vector>
#include <map>

#include <MsgPacket.h>
#include <Msg.h>

using namespace CE::tcp;

/**
 * @brief Singleton manager for message name/id registration, message creation,
 * and message processing.
 *
 * The manager supports three primary responsibilities:
 * 1. Registering message names and assigning an internal numeric id.
 * 2. Creating concrete Msg instances via registered creator callbacks.
 * 3. Dispatching Msg instances to registered processor callbacks.
 */
class MsgManager 
{
    /**
     * @brief Message processor callback.
     *
     * @param msg Message to process.
     * @return true if the processor handled the message, false otherwise.
     */
public:
    /**
     * @brief Returns the process-wide singleton instance.
     */
    static MsgManager& GetInstance();

    static constexpr int32_t IntHashOfStr(std::string_view str) noexcept 
    {
            uint32_t hash = 2166136261U;
            for (char c : str) 
            {
                hash ^= static_cast<uint32_t>(c);
                hash *= 16777619U;
            }
        return static_cast<int32_t>(hash);
    }

    /**
     * @brief Message processor callback.
     *
     * @param msg Message to process.
     * @return true if the processor handled the message, false otherwise.
     */
    using ProcessMsg = std::function<bool(Msg&)>;

    /**
     * @brief Message creator callback.
     *
     * @param msgId Message id extracted from an incoming packet.
     * @return A newly created Msg instance for the id, or nullptr if unsupported.
     */
    using CreateMsg = std::function<Msg*(MsgPacket&)>;

    /**
     * @brief Registers a single message name if not already registered.
     *
     * @param str Message name.
     */
    bool RegisterMsgName(const std::string& str); 

    /**
     * @brief Registers a list of message names.
     *
     * Duplicate names are ignored.
     *
     * @param msgNames Collection of message names.
     */
    bool RegisterMsgNames(const std::vector<std::string>& msgNames);

    /**
     * @brief Retrieves the id for a registered message name.
     *
     * @param str Message name.
     * @return Registered id, or 0 when the name is unknown.
     */
    int32_t GetMsgId(const std::string& str) const;

    /**
     * @brief Checks whether a numeric message id is currently registered.
     *
     * @param id Message id to test.
     * @return true when the id exists in the registry.
     */
    bool IsValidID(int32_t id) const;

    /**
     * @brief Retrieves the message name mapped to the specified id.
     *
     * @param id Message id.
     * @return Message name, or "Unknown" when no mapping exists.
     */
    std::string GetMsgNameFromId(int32_t id) const ;



    /**
     * @brief Creates a concrete Msg from a packet using registered creators.
     *
     * Creators are tried in registration order. The first non-null result is
     * returned.
     *
     * @param packet Input packet.
     * @return Created message, or nullptr when no creator supports the id.
     */
    virtual Msg* CreateMsgFromPacket(MsgPacket& packet);


    /**
     * @brief Adds a message processor callback.
     *
     * @param msgProcessor Processor callback.
     */
    void AddMsgProcessor(std::function<bool(Msg&)> msgProcessor);

    /**
     * @brief Adds a message creator callback.
     *
     * @param msgCreator Creator callback.
     */
    void AddMsgCreator(CreateMsg msgCreator);

    /**
     * @brief Dispatches a message to all registered processors.
     *
     * All processors are executed, and their results are OR-combined.
     *
     * @param msg Message to process.
     * @return true if at least one processor reported success.
     */
    virtual bool ProcessMsgs(Msg& msg); 
    std::unordered_map<std::string, int32_t> registryX;

private:
        MsgManager(); 
        std::map<std::string, int32_t> registry;
        std::vector<ProcessMsg> MsgProcessors;
        std::vector<CreateMsg> MsgCreators;
};

#endif // MSG_MANAGER_H

