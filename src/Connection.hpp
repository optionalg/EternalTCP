#ifndef __ETERNAL_TCP_CONNECTION__
#define __ETERNAL_TCP_CONNECTION__

#include "Headers.hpp"

#include "BackedReader.hpp"
#include "BackedWriter.hpp"
#include "SocketHandler.hpp"

namespace et {
class Connection {
 public:
  Connection(
      shared_ptr<SocketHandler> _socketHandler,
      const string& _id,
      const string& _key);

  virtual ~Connection();

  virtual bool readMessage(string* buf);
  virtual void writeMessage(const string& message);

  template <typename T>
  inline T readProto() {
    T t;
    string s;
    ssize_t readMessages = readMessage(&s);
    if (readMessages) {
      t.ParseFromString(s);
    }
    return t;
  }

  template <typename T>
  inline void writeProto(const T& t) {
    string s;
    t.SerializeToString(&s);
    writeMessage(s);
  }

  inline shared_ptr<BackedReader> getReader() { return reader; }
  inline shared_ptr<BackedWriter> getWriter() { return writer; }

  int getSocketFd() { return socketFd; }

  inline shared_ptr<SocketHandler> getSocketHandler() {
    return socketHandler;
  }

  bool isDisconnected() { return socketFd == -1; }

  string getId() { return id; }

  inline bool hasData() { return reader->hasData(); }

  virtual void closeSocket();

  void shutdown();

  inline bool isShuttingDown() {
    return shuttingDown;
  }

 protected:
  virtual ssize_t read(string* buf);
  virtual ssize_t write(const string& buf);
  bool recover(int newSocketFd);

  shared_ptr<SocketHandler> socketHandler;
  string id;
  string key;
  std::shared_ptr<BackedReader> reader;
  std::shared_ptr<BackedWriter> writer;
  int socketFd;
  bool shuttingDown;
  recursive_mutex connectionMutex;
};
}

#endif  // __ETERNAL_TCP_CONNECTION__
