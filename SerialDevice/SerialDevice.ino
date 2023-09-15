#include <BLE2902.h>
#include <BLEDevice.h>

/***************************************************************************************/

#define GATT_DEVICE_NAME        "GATT Serial Device"

#define SERIAL_SERVICE_UUID     "6E40FEC1-B5A3-F393-E0A9-E50E24DCCA9E"
#define RX_CHARACTERISTIC_UUID  "6E40FEC2-B5A3-F393-E0A9-E50E24DCCA9E"
#define TX_CHARACTERISTIC_UUID  "6E40FEC3-B5A3-F393-E0A9-E50E24DCCA9E"

/***************************************************************************************/

class CServerCallback : public BLEServerCallbacks
{
public:
    void onConnect(BLEServer* pServer) override
    {
        Serial.println("Client connected");
    }
    
    void onDisconnect(BLEServer* pServer) override
    {
        Serial.println("Client disconnected. Restart advertising.");
        pServer->getAdvertising()->start();
    }
};

/***************************************************************************************/

class CTxCharCallbacks : public BLECharacteristicCallbacks
{
private:
    BLECharacteristic*  FRxChar;

public:
    CTxCharCallbacks(BLECharacteristic* RxChar)
        : BLECharacteristicCallbacks()
    {
        FRxChar = RxChar;
    }
    
    void onWrite(BLECharacteristic* pCharacteristic) override
    {
        Serial.println("Write request.");
        if (pCharacteristic->getLength() > 0 && pCharacteristic->getData() != NULL)
        {
            FRxChar->setValue(pCharacteristic->getData(), pCharacteristic->getLength());
            FRxChar->notify();
        }
    }
};

/***************************************************************************************/

void setup()
{
    Serial.begin(115200);
    delay(2000);

    Serial.println("Init BLE device");
    BLEDevice::init(GATT_DEVICE_NAME);

    Serial.println("Create server");
    BLEServer* Server = BLEDevice::createServer();
    Server->setCallbacks(new CServerCallback());

    Serial.println("Create service");
    BLEService* Service = Server->createService(SERIAL_SERVICE_UUID);
    
    Serial.println("Create RX characteristic");
    BLECharacteristic* RxChar = new BLECharacteristic(RX_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_NOTIFY);
    RxChar->addDescriptor(new BLE2902());

    Serial.println("Create TX characteristic");
    BLECharacteristic* TxChar = new BLECharacteristic(TX_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
    TxChar->setCallbacks(new CTxCharCallbacks(RxChar));

    Serial.println("Start GATT server");
    Service->addCharacteristic(RxChar);
    Service->addCharacteristic(TxChar);
    Service->start();

    Serial.println("Start advertising");
    BLEAdvertising* Advertising = BLEDevice::getAdvertising();
    Advertising->addServiceUUID(SERIAL_SERVICE_UUID);
    Server->getAdvertising()->start();
}

/***************************************************************************************/

void loop()
{

}