#ifndef AUTHERIZATION_H
#define AUTHERIZATION_H

#include <string>

#include <CommonDefinitions.h>
#include <CryptoKeyIV.h>

namespace CE::tcp
{

    class Autherization
    {
    public:
        static Autherization* Get();

        virtual ~Autherization();
        void StartAutherizationMode(const std::string& clientPinStr);
        void  StopAutherizationMode( );

        CryptoBlockVector GetKey();
        CryptoBlockVector GetIV();


    private:
        Autherization();
        static Autherization * theInstance;
        void DisplayPIN();
        void InitPin();
        std::string m_clientPinStr;
        std::string m_serverPinStr;
        CryptoBlockVector m_Key;
        CryptoBlockVector m_IV;


    };
} // namespace CE::tcp

#endif // AUTHERIZATION_H
