#ifdef __cplusplus
    #ifndef SYSTEM_MSG_CONSTANTS_H
    #define SYSTEM_MSG_CONSTANTS_H

    #include <string>
    #include <vector>

    namespace SharedSysMsgConstants {


        inline static const int REMOTE_PORT = 51717;
        inline static const int LOCAL_PORT = 51718;
        inline static const int PAIRING_PORT = 51719;
        inline static const int DEBUG_PORT = 51720;

#ifdef OLD_CODE        
        enum SysMsgCommands
        {
            CommunicationFailedCmd = 0,
            RemoteConnectionTestCmd,
            ValidateIVCmd,
            ValidateIVResultCmd,
            AutherizationStartRequestCmd,
            AutherizationReadyCmd,
            AutherizationValidationCmd,
            AutherizationValidationResponseCmd,
            AutherizationEndCmd,
            AvailableCmdInfoCmd,
            AddToCmdInfoCmd,
            UpdateAvailableCmdInfoCmd,
            EnableEncryptDecryptCmd,
            RequestKeyAndIVCmd,
            UpdateKeyAndIVCmd,
            KeepAliveCmd,
            RemoteConnectionOkCmd,
            DebugQueryCmd,
            DebugResponseCmd
        };

    #endif // OLD_CODE

        inline static const std::vector<std::string> SystemCmdNames = {
            "CommunicationFailedCmd",
            "RemoteConnectionTestCmd",
            "ValidateIVCmd",
            "ValidateIVResultCmd",
            "AutherizationStartRequestCmd",
            "AutherizationReadyCmd",
            "AutherizationValidationCmd",
            "AutherizationValidationResponseCmd",
            "AutherizationEndCmd",
#ifdef OLD_CODE            
            "AvailableCmdInfoCmd",
            "AddToCmdInfoCmd",
            "UpdateAvailableCmdInfoCmd",
#endif
            "EnableEncryptDecryptCmd",
            "RequestKeyAndIVCmd",
            "UpdateKeyAndIVCmd",
            "KeepAliveCmd",
            "RemoteConnectionOkCmd",
            "DebugQueryCmd",
            "DebugResponseCmd"
        };

    } // namespace SharedSysMsgConstants

    #endif // SYSTEM_MSG_CONSTANTS_H
#else
    namespace SharedSysMsgConstants
    {
        public static class SysMsgConstants
        {
            public static readonly string[] SystemCmdNames =
            {
                "AvailableCmdInfoCmd",
                "AddToCmdInfoCmd",
                "UpdateAvailableCmdInfoCmd",
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

            public static readonly int REMOTE_PORT = 51717;
            public static readonly int LOCAL_PORT = 51718;
            public static readonly int PAIRING_PORT = 51719;
            public static readonly int DEBUG_PORT = 51720;

            public enum MsgCommands
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
            }
        }
    }
#endif
