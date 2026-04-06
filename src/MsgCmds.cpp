
#include <CommonDefinitions.h>
#ifdef OLD_CODE
std::vector<std::string> MsgCmds::SystemCmdNames =
    {
    "AvailableCmdInfoCmd",
    "EnableEncryptDecryptCmd",
    "RequestKeyAndIVCmd",
    "UpdateKeyAndIVCmd",
    "AutherizationStartRequestCmd",
    "AutherizationReadyCmd",
    "AutherizationValidationCmd",
    "AutherizationValidationResponseCmd",
    "ValidateIVCmd",
    "ValidateIVResultCmd",
    "KeepAliveCmd",
    "RemoteConnectionOkCmd",
    "DebugQueryCmd",
    "DebugResponseCmd"
};

MsgCmds * MsgCmds::theInstance = nullptr;

MsgCmds * MsgCmds::GetMsgCmd()
{
    return theInstance;
}


bool MsgCmds::GetMsgCmdName(unsigned int cmdID, std::string& name)
{
    if( theInstance != nullptr)
        return theInstance->GetCmdName(cmdID, name);
    return false;
}

bool MsgCmds::GetCmdName(unsigned int cmdID, std::string & cmdName)
{
    bool result = true;
    if (cmdID <= RemoteConnectionOkCmd)
        cmdName = SystemCmdNames[cmdID];
    else
        result = false;
    return result;
}
#endif