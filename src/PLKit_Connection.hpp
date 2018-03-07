#pragma once

#include "Arduino.h"

#include "PLKit_Map.hpp"

#define BUFFER_SIZE 20
#define KEY_BUFFER_SIZE 20
#define STX 2
#define ETX 3

#define SEND_SEP "|"

enum PacketType {
  FLOAT = 0xf0,
  INT = 0xf1,
  BANG = 0xf2,
  STRING = 0xf3,
};

class Connection {
public:
  typedef void (*FloatFunc)(float) ;
  typedef void (*IntFunc)(int);
  typedef void (*BangFunc)();
  typedef void (*StringFunc)(const char *);

  struct Callback {
    union {
      BangFunc bangFunc;
      IntFunc intFunc;
      FloatFunc floatFunc;
      StringFunc stringFunc;
    } func;
    PacketType type;
  };

public:
  Connection():
  mReadState(State::IDLE) {
  }

  void open() {
    Serial.begin(115200);
  }

  void receive(const char *key, BangFunc func) {
    Callback cb;
    cb.type = BANG;
    cb.func.bangFunc = func;
    mCallbackMap.insert(key, cb);
  }

  void receive(const char *key, FloatFunc func) {
    Callback cb;
    cb.type = FLOAT;
    cb.func.floatFunc = func;
    mCallbackMap.insert(key, cb);
  }

  void receive(const char *key, IntFunc func) {
    Callback cb;
    cb.type = INT;
    cb.func.intFunc = func;
    mCallbackMap.insert(key, cb);
  }

  void receive(const char *key, StringFunc func) {
    Callback cb;
    cb.type = STRING;
    cb.func.stringFunc = func;
    mCallbackMap.insert(key, cb);
  }

  void send(const char *key) {
    Serial.print(SEND_SEP);
    Serial.print(key);
    Serial.print(",b,");
    Serial.print(SEND_SEP);
  }

  void send(const char *key, int value) {
    Serial.print(SEND_SEP);
    Serial.print(key);
    Serial.print(",i,");
    Serial.print(value);
    Serial.print(SEND_SEP);
  }

  void send(const char *key, float value) {
    Serial.print(SEND_SEP);
    Serial.print(key);
    Serial.print(",f,");
    Serial.print(value);
    Serial.print(SEND_SEP);
  }

  template <typename T>
  void log(T message) {
    Serial.print("{");
    Serial.print(message);
    Serial.print("}");
  }

  void update() {
    while (Serial.available() > 0) {
      if (mReadState == State::IDLE) {
        if (Serial.read() == STX) {
          mReadState = State::KEY;
        } else {
          mReadState = State::IDLE;
        }
      }
      else if (mReadState == State::KEY) {
        uint8_t keyLength = Serial.read();
        Serial.readBytes(&mKeyBuffer[0], keyLength);
        mKeyBuffer[keyLength] = '\0';
        mReadState = State::HEADER;
      }
      else if (mReadState == State::HEADER) {
        mPacketType = Serial.read();
        mReadState = State::PAYLOAD;
      }
      else if (mReadState == State::PAYLOAD) {
        uint8_t size = typeToSize(mPacketType);
        if (size > 0) {
          Serial.readBytes(&mDataBuffer[0], size);
          if (size < PacketType::INT) {
            mDataBuffer[size] = '\0';
            mPacketType == PacketType::STRING;
          }
        }
        mReadState = State::END;
      }
      else if (mReadState == State::END) {
        if (Serial.read() == ETX) {
          execute();
        }
        mReadState = State::IDLE;
      }
    }
  }

protected:
  void execute() {
    Callback cb;

    bool found = mCallbackMap.safeGet(&mKeyBuffer[0], &cb);

    if (found) {
      if (mPacketType == PacketType::FLOAT) {
        float f;
        memcpy(&f, &mDataBuffer[0], sizeof(float));
        cb.func.floatFunc(f);
      }
      else if (mPacketType == PacketType::INT) {
        int32_t i;
        memcpy(&i, &mDataBuffer[0], sizeof(int32_t));
        cb.func.intFunc(i);
      }
      else if (mPacketType == PacketType::BANG) {
        cb.func.bangFunc();
      }
      else if (mPacketType == PacketType::STRING) {
        cb.func.stringFunc(&mDataBuffer[0]);
      }
    }
    else {
      Serial.println("key not found");
    }

  }

protected:
  StringMap<Callback> mCallbackMap;

protected:
  enum class State { IDLE, KEY, HEADER, PAYLOAD, END } mReadState;
  byte mDataBuffer[BUFFER_SIZE];
  char mKeyBuffer[KEY_BUFFER_SIZE];
  PacketType mPacketType;

  uint8_t typeToSize(PacketType mt) {
    switch(mt) {
      case PacketType::INT:
      case PacketType::FLOAT:
        return 4;
      case PacketType::BANG:
        return 0;

      // for strings
      default:
        return static_cast<uint8_t>(mt);
    }
  }
};
