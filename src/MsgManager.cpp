#include <MsgManager.h>
#include <SystemMsgs.h>

MsgManager* MsgManager::instance = nullptr;

MsgManager& MsgManager::GetInstance()
{
    if(instance == nullptr)
    {
        instance = new MsgManager();
        TCPSystemMsgs::GetInstance(); // Ensure SystemMsgs is initialized and its creators are registered
    }
    return *instance;
}


    /**
     * @brief Registers a single message name if not already registered.
     *
     * @param str Message name.
     */
    bool MsgManager::RegisterMsgName(const std::string& str) 
    {
        if(registry.find(str) != registry.end()) 
        {
            // ignore duplicate registration
            return false;
        }
        int32_t msgId = IntHashOfStr(str);
        registry[str] = msgId; // Use the hash of the string as the message id
        return true;
    }

    /**
     * @brief Registers a list of message names.
     *
     * Duplicate names are ignored.
     *
     * @param msgNames Collection of message names.
     */
    bool MsgManager::RegisterMsgNames(const std::vector<std::string>& msgNames)
    {
        bool allRegistered = true;
        for(const auto& name : msgNames)
        {
            allRegistered &= RegisterMsgName(name);
        }
        return allRegistered;
    }

    /**
     * @brief Retrieves the id for a registered message name.
     *
     * @param str Message name.
     * @return Registered id, or 0 when the name is unknown.
     */
    int32_t MsgManager::GetMsgId(const std::string& str) const 
    {
        auto it = registry.find(str);
        if (it != registry.end()) {
            return it->second;
        }
        return 0; // Return 0 if string is not registered
    }

    /**
     * @brief Checks whether a numeric message id is currently registered.
     *
     * @param id Message id to test.
     * @return true when the id exists in the registry.
     */
    bool MsgManager::IsValidID(int32_t id) const
    {
        for (const auto& pair : registry)
        {
            if (pair.second == id)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Retrieves the message name mapped to the specified id.
     *
     * @param id Message id.
     * @return Message name, or "Unknown" when no mapping exists.
     */
    std::string MsgManager::GetMsgNameFromId(int32_t id) const 
    {
        for (const auto& pair : registry) {
            if (pair.second == id) {
                return pair.first;
            }
        }
        return "Unknown"; // Return "Unknown" if hash is not found
    }



    /**
     * @brief Creates a concrete Msg from a packet using registered creators.
     *
     * Creators are tried in registration order. The first non-null result is
     * returned.
     *
     * @param packet Input packet.
     * @return Created message, or nullptr when no creator supports the id.
     */
    Msg* MsgManager::CreateMsgFromPacket(MsgPacket& packet)
    {

        for(const auto& creator : MsgCreators)
        {
            Msg* createdMsg = creator(packet);
            if(createdMsg)
            {
                return createdMsg;
            }
        }
        return nullptr;
    }


    /**
     * @brief Adds a message processor callback.
     *
     * @param msgProcessor Processor callback.
     */
    void MsgManager::AddMsgProcessor(std::function<bool(Msg&)> msgProcessor)
    {
        MsgProcessors.push_back(msgProcessor);
    }   

    /**
     * @brief Adds a message creator callback.
     *
     * @param msgCreator Creator callback.
     */
    void MsgManager::AddMsgCreator(CreateMsg msgCreator)
    {
        MsgCreators.push_back(msgCreator);
    }

    /**
     * @brief Dispatches a message to all registered processors.
     *
     * All processors are executed, and their results are OR-combined.
     *
     * @param msg Message to process.
     * @return true if at least one processor reported success.
     */
    bool MsgManager::ProcessMsgs(Msg& msg)
    {
        bool processed = false;
        for(const auto& processor : MsgProcessors)
        {
            processed |= processor(msg);
        }
        return processed;

    }


    MsgManager::MsgManager() 
    {
    }
