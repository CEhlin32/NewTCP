#include <TCPLogOptions.h>
#include <Logger.h>
#include <TCPServerManager.h>

namespace CE::tcp
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
    TCPEncryptLogOption::TCPEncryptLogOption() : LogOption("TCP Encrypted Data Cmds", TheAppLogger)
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
        
        m_CmdDefinitions.push_back(new EnableLogOptionCmd(&TCPInfoLogOption::instance(), "TCP Enable Info", 
                " Enables TCP Message Info Logging"));
        m_CmdDefinitions.push_back(new DisableLogOptionCmd(&TCPInfoLogOption::instance(), "TCP Disable Info",
            "Disables TCP Message Info Logging"));
        m_CmdDefinitions.push_back(new EnableLogOptionCmd(&TCPEncryptLogOption::instance(), "TCP Enable Encrypted", 
                " Enables Logging of Message Encrypted Data"));
        m_CmdDefinitions.push_back(new DisableLogOptionCmd(&TCPEncryptLogOption::instance(), "TCP Disable Encrypted",
            "Disables Logging of Message Encrypted Data"));
        m_CmdDefinitions.push_back(new EnableLogOptionCmd(&TCPDecryptLogOption::instance(), "TCP Enable Decrypted", 
                " Enables Logging of Message Decrypted Data"));
        m_CmdDefinitions.push_back(new DisableLogOptionCmd(&TCPDecryptLogOption::instance(), "TCP Disable Decrypted",
            "Disables Logging of Message Decrypted Data"));
        m_CmdDefinitions.push_back(new EnableLogOptionCmd(&TCPStatusChangeLogOption::instance(), "TCP Enable Status", 
                " Enables Logging of TCP Status"));
        m_CmdDefinitions.push_back(new DisableLogOptionCmd(&TCPStatusChangeLogOption::instance(), "TCP Disable Status",
            "Disables Logging of TCP Status")); 
    }

    TCPLogOptionGroup::TCPLogOptionGroup() : LogOptionGroup("TCP Commands", TheAppLogger)
    {
        // Add all LogOptions to this group
        m_LogOptions.push_back(&TCPInfoLogOption::instance());
        m_LogOptions.push_back(&TCPEncryptLogOption::instance());
        m_LogOptions.push_back(&TCPDecryptLogOption::instance());
        m_LogOptions.push_back(&TCPStatusChangeLogOption::instance());
        InitCmdDefinitions();

        TCPInfoLogOption::instance().Subscribe(this);
        TCPEncryptLogOption::instance().Subscribe(this);
        TCPDecryptLogOption::instance().Subscribe(this);
        TCPStatusChangeLogOption::instance().Subscribe(this);
    }
}
