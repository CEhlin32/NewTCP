#ifndef AESSUPPORT_H
#define AESSUPPORT_H



#include <iostream>
#include <sstream>
#include <vector>
#include <map>

//#include <Device.h>
#include <PublishSubscribe.h>
#include <Serializer.h>
#include <DeSerializer.h>
#include <AESCryptor.h>

#include <netinet/in.h>
#include <CommonDefinitions.h>



using namespace std;
using namespace AESAccess;

typedef std::map<CryptoBlockVector, CryptoBlockVector> IVToKeyMap;

class AESAccessManagement : public PublishSubscribe//: public Device
{
    public:
        static AESAccessManagement * Get();
        virtual ~AESAccessManagement();

        IVAndKeyValues CreateIVAndKey(AES_BLOCK_SIZES keySize = AES_SIZE_128, AES_BLOCK_SIZES ivSize = AES_SIZE_128);
        CryptoBlockVector GetNextIV(AES_BLOCK_SIZES keySize);
        void ReplaceIV(CryptoBlockVector oldIV, CryptoBlockVector newIV);

        bool IsIVValid(const CryptoBlockVector& iv);
        CryptoBlockVector GetASEKey(const CryptoBlockVector& iv);
        bool Serialize(Serializer& serializer);
        bool DeSerialize(DeSerializer& deSerializer);

        // TODO: these should be set from configuration
        int m_RequestedKeySize;
        int m_RequestedIVSize;


    protected:
        void    AddASEKeyAndIV(IVAndKeyValues& keyAndIV);
        void    RemoveASEKey(CryptoBlockVector& iv);
        bool OnEvent(const PublishData& pubEventData) override ;

    private:
        AESAccessManagement() ;
        void AvoidSpecialChars(std::vector<uint8_t>& block);

        IVToKeyMap m_IVtoKeyMap;

        static const CryptoBlockVector emptyBlock;
        static AESAccessManagement * theInstance;
};
#endif // AESSUPPORT_H
