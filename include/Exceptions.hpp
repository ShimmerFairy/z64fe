/** \file Exceptions.hpp
 *
 *  \brief Declare all the various exception types we use in operation.
 *
 */

#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <initializer_list>

class Exception {
  public:
    virtual std::string what();

    virtual ~Exception() = default;
};

namespace X {
    class Multi : public Exception {
      private:
        std::vector<Exception *> elist;

      public:
        size_t size() const;

        void addException(Exception * e);

        std::string what() override;
    };

    class InternalError : public Exception {
      private:
        std::string specifics;

      public:
        InternalError(std::string sp);

        std::string what() override;
    };

    class NYI : public Exception {
      private:
        std::string feature;

      public:
        NYI(std::string whatthing = "this feature");

        std::string what() override;
    };

    class BadROM : public Exception {
      private:
        std::string reason;

      public:
        BadROM(std::string r);

        std::string what() override;
    };

    class BadIndex : public Exception {
      private:
        std::string whatidx;

      public:
        BadIndex(std::string wi);

        std::string what() override;
    };

    class NoConfig : public Exception {
      private:
        std::string trying;

      public:
        NoConfig(std::string t);

        std::string what() override;
    };

    namespace ROM {
        class NoMagic : public Exception {
          public:
            std::string what() override;
        };
    }

    namespace Config {
        class SyntaxError : public Exception {
          private:
            std::string explain;

          public:
            SyntaxError(std::string e);

            std::string what() override;
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

            std::string what() override;
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

            std::string what() override;
        };
    }

    namespace Text {
        class BadSequence : public Exception {
          private:
            std::vector<uint8_t> badseq;
            std::string optreason;

          public:
            BadSequence(std::initializer_list<uint8_t> bs, std::string optr = "");

            std::string what() override;
        };

        class HeaderError : public Exception {
          public:
            std::string what() override;
        };
    }
}