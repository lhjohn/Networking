//
//  net_message.h
//  NetCommon
//
//  Created by Henrik John on 23/01/2021.
//

#pragma once

#include "net_common.h"

namespace lhj {
  namespace net {
    // Message header
    template <typename T>
    struct message_header {
      T id{};
      uint32_t size = 0;
    };

    // Message body
    template <typename T>
    struct message { 
      message_header<T> header{};
      std::vector<uint8_t> body;

      // Return size of entire message packet in bytes
      size_t size() const {
        // return sizeof(message_header<T>) + body.size();
        return body.size();
      }

      //Override for std::cout compatibility - produces friendly description of
      //message
      friend std::ostream& operator << (std::ostream& os, const message<T>& msg) {
        os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
        return os;
      }

      // Pushes any POD-like data into the message buffer by overriding "<<"
      template <typename DataType>
      friend message<T>& operator << (message<T>& msg, const DataType& data) {
        
        // Check that the type of the data being pushed is trivially copyable
        static_assert(std::is_standard_layout<DataType>::value,
                      "Data is too complex to be pushed into vector");
        
        // Cache current size of vector, as this will be the point we insert
        // the data
        size_t i = msg.body.size();

        // Resize the vector by the size of the data being pushed
        msg.body.resize(msg.body.size() + sizeof(DataType));

        // Physically copy the data into the newly allocated vector space
        std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

        // Recalculate the message size
        msg.header.size = msg.size();

        // Return the target message so it can be "chained"
        return msg;
      }


      template <typename DataType>
      friend message<T>& operator >> (message<T>& msg, DataType& data) {
      
        // Check that the type of the data being pushed is trivially copyable
        static_assert(std::is_standard_layout<DataType>::value,
                      "Data is too complex to be pushed into vector");
        
        // Cache the location towards the end of the vector where the pulled
        // data starts
        size_t i = msg.body.size() - sizeof(DataType);

        // Physically copy the data from the vector into the user variable
        std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

        // Shrink the vector to remove read bytes, and reset end position
        msg.body.resize(i);

        // Recalculate the message size
        msg.header.size = msg.size();

        // Return the target message so it can be "chained"
        return msg;
      }
    };

    // Forward declare the connection
    template <typename T>
    class connection;

    template <typename T>
    struct owned_message {
      std::shared_ptr<connection<T>> remote = nullptr;
      message<T> msg;

      // Again, a friendly string maker
      friend std::ostream& operator <<(std::ostream& os, const owned_message<T>& msg) {
        os << msg.msg;
        return os;
      }
    };
  }  
}
