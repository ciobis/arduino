#include <ArduinoUnit.h>
#include <ESPping.h>
#include <ESP8266WiFi.h>

void ensureWifi(String ssid, String password) {
  if (WiFi.status() != WL_CONNECTED || !Ping.ping(WiFi.gatewayIP(), 3)) {
    WiFi.disconnect();
    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
    }
  }
}

class StatusProvider {
  public:
    virtual bool getStatus() = 0;
};


class PingStatusProvider: public StatusProvider {
  public:
    PingStatusProvider(IPAddress ip, int repeatCount): StatusProvider() {
      this->ip = ip;
      this->repeatCount = repeatCount;
    }

    bool getStatus() {
      return Ping.ping(ip, repeatCount);
    }

    IPAddress ip;
    int repeatCount;
};

class Device {
  public:
    Device(StatusProvider* statusProvider, int minConsecutiveSuccess, int minConsecutiveFailures) {
      this->statusProvider = statusProvider;
      this->minConsecutiveSuccess = minConsecutiveSuccess;
      this->minConsecutiveFailures = minConsecutiveFailures;
      this->consecutiveSuccess = 0;
      this->consecutiveFailures = 0;
    }

    enum Status
    {
        ON,
        OFF,
        UNKNOWN
    };

    Status getStatus() {
      if (statusProvider->getStatus()) {
        if (consecutiveSuccess < minConsecutiveSuccess) { //no overflow
          consecutiveSuccess++;
        }
        
        consecutiveFailures = 0;
      } else {
        if (consecutiveFailures < minConsecutiveFailures) {
          consecutiveFailures++;
        }
        
        consecutiveSuccess = 0;
      }

      if (consecutiveSuccess >= minConsecutiveSuccess) {
        return Status::ON;
      }

      if (consecutiveFailures >= minConsecutiveFailures) {
        return Status::OFF;
      }

      return Status::UNKNOWN;
    }

    StatusProvider* statusProvider;
    int minConsecutiveSuccess;
    int minConsecutiveFailures;
    int consecutiveSuccess;
    int consecutiveFailures;
};

String ssid = "*******";
String password = "*******";
IPAddress pcIp = IPAddress(192,168,0,159);
IPAddress tvIp = IPAddress(192,168,0,158);
StatusProvider* pcStatusProvider = new PingStatusProvider(pcIp, 3);
StatusProvider* tvStatusProvider = new PingStatusProvider(tvIp, 3);
Device pc = Device(pcStatusProvider, 1, 5);
Device tv = Device(tvStatusProvider, 1, 5);

void setup()
{  
  // Serial.begin(9600); // for unit tests

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(5, OUTPUT);
}

void loop()
{
  // delay(2000); Test::run();  // for unit tests
  ensureWifi(ssid, password);

  Device::Status pcStatus = pc.getStatus();
  Device::Status tvStatus = tv.getStatus();

  if (pcStatus == Device::Status::UNKNOWN || tvStatus == Device::Status::UNKNOWN) {
    return;
  }

  int socketState = pcStatus == Device::Status::ON || tvStatus == Device::Status::ON;
  digitalWrite(5, socketState);
  digitalWrite(LED_BUILTIN, !socketState);

  delay(5000);
}

// -----------------------------------------------
//          UNIT TESTS
// -----------------------------------------------

class StubStatusProvider: public StatusProvider {
  public:
    bool getStatus() {
      return status;
    }

    void setStatus(bool status) {
      this->status = status;
    }

    bool status = false;
};

test(detects_status_on)
{
  StubStatusProvider* statusProvider = new StubStatusProvider();
  Device device = Device(statusProvider, 2, 3);

  statusProvider->setStatus(true);
  assertEqual(Device::Status::UNKNOWN, device.getStatus()); // neither on nor off - not enough consecutive statuses
  assertEqual(Device::Status::ON, device.getStatus()); // Enough for confirmation
  assertEqual(Device::Status::ON, device.getStatus());

  statusProvider->setStatus(false);
  assertEqual(Device::Status::UNKNOWN, device.getStatus()); // neither on nor off - not enough consecutive statuses
  statusProvider->setStatus(true);
  assertEqual(Device::Status::UNKNOWN, device.getStatus());
  assertEqual(Device::Status::ON, device.getStatus());
}

test(detects_status_on_with_first_retrieval)
{
  StubStatusProvider* statusProvider = new StubStatusProvider();
  Device device = Device(statusProvider, 1, 3);

  statusProvider->setStatus(true);
  assertEqual(Device::Status::ON, device.getStatus());
}

test(detects_status_off)
{
  StubStatusProvider* statusProvider = new StubStatusProvider();
  Device device = Device(statusProvider, 2, 3);

  statusProvider->setStatus(false);
  assertEqual(Device::Status::UNKNOWN, device.getStatus()); // neither on nor off - not enough consecutive statuses
  assertEqual(Device::Status::UNKNOWN, device.getStatus()); // neither on nor off - not enough consecutive statuses
  assertEqual(Device::Status::OFF, device.getStatus()); // Enough for confirmation
  assertEqual(Device::Status::OFF, device.getStatus());

  statusProvider->setStatus(true);
  assertEqual(Device::Status::UNKNOWN, device.getStatus());
  statusProvider->setStatus(false);
  assertEqual(Device::Status::UNKNOWN, device.getStatus());
  assertEqual(Device::Status::UNKNOWN, device.getStatus());
  assertEqual(Device::Status::OFF, device.getStatus());
}


