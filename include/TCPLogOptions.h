#ifndef TCP_LOG_OPTIONS_H
#define TCP_LOG_OPTIONS_H

#include <LogOptions.h>
#include <TCPLogOptions.h>
#include <CmdDefinition.h>

namespace newtcp
{
    class TCPStatusCmd : public CommandDefinition
    {
        public:
            TCPStatusCmd();

            virtual void Action() override;
    };
    class TCPInfoLogOption : public LogOption
    {
        public:
            static TCPInfoLogOption& instance();
        protected:

            // Implement all pure virtual functions from LogOption
            virtual void InitCmdDefinitions();
        private:
            TCPInfoLogOption();

    };

    class TCPEncryptLogOption : public LogOption
    {
        public:
            static TCPEncryptLogOption& instance();
        protected:

            // Implement all pure virtual functions from LogOptions
            virtual void InitCmdDefinitions();

            private:
            TCPEncryptLogOption();

    };

    class TCPDecryptLogOption : public LogOption
    {
        public:
            static TCPDecryptLogOption& instance();
        protected:

            // Implement all pure virtual functions from LogOption
            virtual void InitCmdDefinitions();
        private:
            TCPDecryptLogOption();

    };

    class TCPStatusChangeLogOption : public LogOption
    {
        public:
            static TCPStatusChangeLogOption& instance();
        protected:

            // Implement all pure virtual functions from LogOption
            virtual void InitCmdDefinitions();
        private:

            TCPStatusChangeLogOption();
    };


    class TCPLogOptionGroup : public LogOptionGroup
    {

        public:
            static TCPLogOptionGroup& instance();
        protected:

            // Implement all pure virtual functions from LogOption
            virtual void InitCmdDefinitions();

        private:
            TCPLogOptionGroup();

    };
}


#endif //TCP_LOG_OPTIONS_H