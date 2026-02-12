#include <TCPLogOptions.h>
#include <Logger.h>
#include <TCPServerManager.h>

namespace newtcp
{
    /////////////////////////////////////////////////////////////////////////////////////////

    TCPInfoLogOption& TCPInfoLogOption::instance() 
    { 
        static TCPInfoLogOption inst; 
        return inst; 
    }    

    // Implement all pure virtual functions from LogOption
    void TCPInfoLogOption::InitCmdDefinitions()
    { 
        LogOption::InitCmdDefinitions();
        m_CmdDefinitions.push_back(new EnableLogOptionCmd( this, "TCP Enable Info", 
                " Enables TCP Message Info Logging"));

        m_CmdDefinitions.push_back(new DisableLogOptionCmd(this, "TCP Disable Info",
            "Disables TCP Message Info Logging"));
    }



    TCPInfoLogOption::TCPInfoLogOption() : LogOption("TCP Info Cmds", TheAppLogger)
    {
        InitCmdDefinitions();
    }


    /////////////////////////////////////////////////////////////////////////////////////////

    TCPEncryptLogOption& TCPEncryptLogOption::instance() 
    { 
        static TCPEncryptLogOption inst;
        return inst; 
    }    


    // Implement all pure virtual functions from LogOption
    void TCPEncryptLogOption::InitCmdDefinitions()
    { 
        LogOption::InitCmdDefinitions();
        // command definitions for LogMsgEncryptedData
        m_CmdDefinitions.push_back(new EnableLogOptionCmd(this, "TCP Enable Encrypted", 
                " Enables Logging of Message Encrypted Data"));

        m_CmdDefinitions.push_back(new DisableLogOptionCmd(this, "TCP Disable Encrypted",
            "Disables Logging of Message Encrypted Data"));  
    }
    TCPEncryptLogOption::TCPEncryptLogOption() : LogOption("TCPEncrypted Data Cmds", TheAppLogger)
    {
        InitCmdDefinitions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////

    TCPDecryptLogOption& TCPDecryptLogOption::instance() 
    { 
        static TCPDecryptLogOption inst; 
        return inst; 
    }    
        

    // Implement all pure virtual functions from LogOption
    void TCPDecryptLogOption::InitCmdDefinitions()
    { 
        LogOption::InitCmdDefinitions();
        // command definitions for LogMsgDecryptedData
        m_CmdDefinitions.push_back(new EnableLogOptionCmd(this,"TCP Enable Decrypted", 
                " Enables Logging of Message Decrypted Data"));    
        m_CmdDefinitions.push_back(new DisableLogOptionCmd(this, "TCP Disable Decrypted",
            "Disables Logging of Message Decrypted Data"));      
    }

    TCPDecryptLogOption::TCPDecryptLogOption() : LogOption("TCP Decrypted Data Cmds", TheAppLogger)
    {
        InitCmdDefinitions();
    }


    /////////////////////////////////////////////////////////////////////////////////////////

    TCPStatusChangeLogOption& TCPStatusChangeLogOption::instance() 
    { 
        static TCPStatusChangeLogOption inst; 
        return inst; 
    }    



    TCPStatusCmd::TCPStatusCmd() : CommandDefinition( "TCP Status", " Lists Current TCP Status")
    {

    }

    void TCPStatusCmd::Action() 
    {
        TCPServerManager::Get()->PrintCurrentServerStatus(Devices_CE::LogAlways::instance());
    }

    // Implement all pure virtual functions from LogOption
    void TCPStatusChangeLogOption::InitCmdDefinitions()
    { 
        LogOption::InitCmdDefinitions();
        // command definitions for LogTCPStatus
        m_CmdDefinitions.push_back(new EnableLogOptionCmd(this, "TCP Enable Status", 
                " Enables Logging of TCP Status"));        
        m_CmdDefinitions.push_back(new DisableLogOptionCmd(this, "TCP Disable Status",
            "Disables Logging of TCP Status"));
    }


    TCPStatusChangeLogOption::TCPStatusChangeLogOption() : LogOption("TCP Status Change Cmds", TheAppLogger)
    {
        InitCmdDefinitions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////

    TCPLogOptionGroup& TCPLogOptionGroup::instance() 
    { 
        static TCPLogOptionGroup inst; 
        return inst; 
    }    
    
    // Implement all pure virtual functions from LogOption
    void TCPLogOptionGroup::InitCmdDefinitions()
    { 
        LogOption::InitCmdDefinitions();
        // command definitions for TCPLogOptionGroup
        m_CmdDefinitions.push_back(new EnableLogOptionGroupCmd(this, "TCP Enable All", 
                " Enables Logging of all Message Data"));     

        m_CmdDefinitions.push_back(new DisableLogOptionGroupCmd(this, "TCP Disable All",        
            "Disables Logging of all Message Data"));

        m_CmdDefinitions.push_back(new TCPStatusCmd());    
    }

    TCPLogOptionGroup::TCPLogOptionGroup() : LogOptionGroup("TCP Commands", TheAppLogger)
    {
        InitCmdDefinitions();
        // Add all LogOptions to this group
        m_LogOptions.push_back(&TCPInfoLogOption::instance());
        m_LogOptions.push_back(&TCPEncryptLogOption::instance());
        m_LogOptions.push_back(&TCPDecryptLogOption::instance());
        m_LogOptions.push_back(&TCPStatusChangeLogOption::instance());

        TCPInfoLogOption::instance().Subscribe(this);
        TCPEncryptLogOption::instance().Subscribe(this);
        TCPDecryptLogOption::instance().Subscribe(this);
        TCPStatusChangeLogOption::instance().Subscribe(this);
    }
}
