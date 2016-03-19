/** \file Exceptions.hpp
 *
 *  \brief Declare all the various exception types we use in operation.
 *
 */

#pragma once

#include <string>

class Exception {
  public:
    virtual std::string what();

    virtual ~Exception() = default;
};

namespace X {
    class NYI : public Exception {
      private:
        std::string feature;

      public:
        NYI(std::string whatthing = "this feature");

        std::string what();
    };

    class BadROM : public Exception {
      private:
        std::string reason;

      public:
        BadROM(std::string r);

        std::string what();
    };

    class BadIndex : public Exception {
      private:
        std::string whatidx;

      public:
        BadIndex(std::string wi);

        std::string what();
    };

    class NoConfig : public Exception {
      private:
        std::string trying;

      public:
        NoConfig(std::string t);

        std::string what();
    };

    namespace Config {
        class SyntaxError : public Exception {
          private:
            std::string explain;

          public:
            SyntaxError(std::string e);

            std::string what();
        };

        class BadCharacter : public SyntaxError {
          private:
            char problemchar;

          public:
            BadCharacter(char pc);

            // uses SyntaxError's what directly
        };

        class NotFound : public Exception {
          private:
            std::string type;
            std::string value;

          public:
            NotFound(std::string t, std::string v);

            std::string what();
        };

        class NoSuchKey : public NotFound {
          public:
            NoSuchKey(std::string key, bool toplevel = false);
        };

        class NoSuchValue : public NotFound {
          public:
            NoSuchValue(std::string val);
        };
    }

    namespace Yaz0 {
        class Decompress : public Exception {
          private:
            std::string reason;

          public:
            Decompress(std::string r);

            std::string what();
        };
    }
}