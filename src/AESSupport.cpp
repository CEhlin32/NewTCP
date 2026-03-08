#ifndef ENCRYPT_DECRYPT
#include <AESSupport.h>

#include <Logger.h>
#include <PublishSubscribeLogOptions.h>

using namespace Devices_CE;
using namespace AESAccess;

AESAccessManagement * AESAccessManagement::theInstance = nullptr;
const CryptoBlockVector AESAccessManagement::emptyBlock;


AESAccessManagement::AESAccessManagement() : PublishSubscribe("AES_AccessManagement") //: Device("AES_AcessManagment")
{
    m_RequestedKeySize = AES_SIZES[AES_SIZE_128];
    m_RequestedIVSize = AES_SIZES[AES_SIZE_128];
}

AESAccessManagement * AESAccessManagement::Get()
{
    if( theInstance == nullptr)
    {
        theInstance = new AESAccessManagement();
    }
    return theInstance;
}

AESAccessManagement::~AESAccessManagement()
{
}

bool AESAccessManagement::OnEvent(const PublishData& pubEventData)
{
    bool result = true;
    time_t my_time = time(NULL);

    TheAppLogger.LogMsgWithTime(PubSubDispatchLogOption::Get(), "Subscriber::OnEvent %s for eventName(id) = %s(%d)\n",
            ctime(&my_time), pubEventData.pPublisher->GetEventName().c_str(),pubEventData.eventID);


    return result;

}


IVAndKeyValues AESAccessManagement::CreateIVAndKey(AES_BLOCK_SIZES keySize, AES_BLOCK_SIZES ivSize )
{
    IVAndKeyValues result;
    AddASEKeyAndIV(result);
    return result;

}


CryptoBlockVector AESAccessManagement::GetNextIV(AES_BLOCK_SIZES ivSize)
{

    CryptoBlockVector newIV = CryptoBlockVector();
    return newIV;
}

void AESAccessManagement::ReplaceIV(CryptoBlockVector oldIV, CryptoBlockVector newIV)
{
    CryptoBlockVector key = GetASEKey(oldIV);
    RemoveASEKey(oldIV);
    IVAndKeyValues newValues;
    newValues.IV = newIV;
    newValues.KEY = key;

    AddASEKeyAndIV(newValues);
}


void AESAccessManagement::AddASEKeyAndIV(IVAndKeyValues& keyAndIV)
{
    m_IVtoKeyMap.insert(std::pair<CryptoBlockVector,CryptoBlockVector>(keyAndIV.IV, keyAndIV.KEY) );

    // Notify that Access has been added
    PublishEvent();
}


void AESAccessManagement::RemoveASEKey(CryptoBlockVector& iv)
{
    IVToKeyMap::iterator it = m_IVtoKeyMap.find(iv);
    if( it != m_IVtoKeyMap.end())
    {
        m_IVtoKeyMap.erase(it);
    }

}

bool AESAccessManagement::IsIVValid(const CryptoBlockVector& iv)
{
    bool result = false;

    IVToKeyMap::iterator it = m_IVtoKeyMap.find(iv);
    if( it != m_IVtoKeyMap.end())
    {
        result = true;
    }

    return result;

}
CryptoBlockVector AESAccessManagement::GetASEKey(const CryptoBlockVector& iv)
{
    CryptoBlockVector result(false);

    IVToKeyMap::iterator it = m_IVtoKeyMap.find(iv);
    if( it != m_IVtoKeyMap.end())
    {
        result = it->second;
    }

    return result;
}



bool AESAccessManagement::Serialize(Serializer& serializer)
{
    bool result = true;
    int numberOfItems = m_IVtoKeyMap.size();
    if(true == serializer.SerializeInt(numberOfItems))
    {
        for(IVToKeyMap::iterator it = m_IVtoKeyMap.begin(); it != m_IVtoKeyMap.end(); ++it)
        {
            int ivSize = it->first.size();
            int keySize = it->second.size();
            if( false == serializer.SerializeInt(ivSize))
            {
                result = false;
                break;
            }
            if( false == serializer.SerializeInt(keySize))
            {
                result = false;
                break;
            }
            CryptoBlockVector firstBlockVector = it->first;
            if( false == serializer.SerializeStructure(firstBlockVector.GetBuf(), ivSize))
            {
                result = false;
                break;
            }
            if( false == serializer.SerializeStructure(it->second.GetBuf(), keySize))
            {
                result = false;
                break;
            }
        }
    }

    return result;
}

bool AESAccessManagement::DeSerialize(DeSerializer& deSerializer)
{
    bool result = true;
    int numberOfItems = 0;
    if(true == deSerializer.DeSerializeInt(&numberOfItems))
    {
        for(int n = 0; n < numberOfItems; n++)
        {
            int ivSize = 0;
            int keySize = 0;
            if( false == deSerializer.DeSerializeInt(&ivSize))
            {
                result = false;
                break;
            }
            if( false == deSerializer.DeSerializeInt(&keySize))
            {
                result = false;
                break;
            }
            
            uint8_t ivBuf[ivSize];
            uint8_t keyBuf[ivSize];

            if( false == deSerializer.DeSerializeStructure(ivBuf,ivSize))
            {
                result = false;
                break;
            }
            if( false == deSerializer.DeSerializeStructure(keyBuf, keySize))
            {
                result = false;
                break;
            }
            IVAndKeyValues ivAndKeyValues;
            ivAndKeyValues.IV.SetBuf(ivBuf, ivSize);
            ivAndKeyValues.KEY.SetBuf(keyBuf, keySize);
            AddASEKeyAndIV(ivAndKeyValues);
        }
    }
    return result;
}
#endif
