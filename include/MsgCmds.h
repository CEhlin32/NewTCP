#ifndef MSGCMDS_H
#define MSGCMDS_H

#include <vector>
#include <string>

enum MsgCommands
{
    AvailableCmdInfoCmd = 0,
    EnableEncryptDecryptCmd,
    RequestKeyAndIVCmd,
    UpdateKeyAndIVCmd,
    AutherizationStartRequestCmd,
    AutherizationReadyCmd,
    AutherizationValidationCmd,
    AutherizationValidationResponseCmd,
    ValidateIVCmd,
    ValidateIVResultCmd,
    KeepAliveCmd,
    RemoteConnectionOkCmd,
    DebugQueryCmd,
    DebugResponseCmd
};

class MsgCmds
{
    public:
        virtual ~MsgCmds(){};
        virtual  bool GetCmdName(unsigned int cmdID, std::string& name );
        static   bool GetMsgCmdName(unsigned int cmdID, std::string& name);
        static std::vector<std::string> SystemCmdNames;
    protected:
        virtual MsgCmds * GetMsgCmd();
        static MsgCmds * theInstance;
};

//////////////////////////////////////////////////////////////////////////




#endif // MSGCMDS_H
