/** \file yaz0.cpp
 *
 *  \brief Implementing Yaz0 stuff
 *
 */

#include "yaz0.hpp"
#include "endian.hpp"
#include "Exceptions.hpp"

#include <string>
#include <iostream>

std::vector<uint8_t> yaz0_decompress(std::vector<uint8_t> ciphertext) {
    std::string magic(ciphertext.begin(), ciphertext.begin() + 4);

    if (magic != "Yaz0" && magic != "Yaz1") {
        throw X::Yaz0::Decompress("Not actually a compressed file.");
    }

    // we don't really need to use the size param, since we're using vectors,
    // but it does make for a cooler-looking implementation if we have a
    // pre-sized vector over using push_back.

    uint32_t dsize = be_u32(ciphertext.begin() + 4);

    std::vector<uint8_t> plaintext(dsize, 0);

    // index pointers, quite important
    auto cipher_R_pnt = ciphertext.begin() + 16; // last 8 bytes of header unused
    auto plain_R_pnt  = plaintext.begin();
    auto plain_W_pnt  = plaintext.begin();

    while (cipher_R_pnt != ciphertext.end() && plain_W_pnt != plaintext.end()) {
        uint8_t opset = *cipher_R_pnt++;

        // the weird choice for comparison is because an unsigned int see 0 - 1
        // == -1 as a positive number, of course, so the standard 0 <= i is
        // useless.
        for (uint8_t i = 7; i <= 7; i--) {
            // each iteration, if we're now at the end of the cipher stream (or
            // write buffer), we'll just stop; it's permissible (AFAIK) to not
            // pad your data to use all of the last opset.
            if (cipher_R_pnt == ciphertext.end() || plain_W_pnt == plaintext.end()) {
                break;
            }

            if (opset & (1 << i)) { // bit on: copy byte
                *plain_W_pnt++ = *cipher_R_pnt++;
            } else { // bit off: run data
                // readcnt is 16-bit in the event that we get a size of
                // e.g. 0xFF for it; then 0xFF + 0x12 = 0x111
                uint16_t readcnt = (*cipher_R_pnt) >> 4;
                uint16_t backamt = (*cipher_R_pnt++) & 0x0F;
                backamt = (backamt << 8) | *cipher_R_pnt++;
                backamt++; // plus 1 to stated back amount

                if (readcnt == 0) {
                    readcnt = *cipher_R_pnt++ + 0x10;
                }

                readcnt += 0x2;

                // now to set up the other read pointer
                plain_R_pnt = plain_W_pnt - backamt;

                // and now to copy! (the line in this loop is the
                // "cooler-looking implementation" mentioned earlier)
                for (uint16_t i = 0; i < readcnt; i++) {
                    *plain_W_pnt++ = *plain_R_pnt++;
                }
            }
        }
    }

    // now to do a quick check in the event that the write buffer got filled out
    // before we read the whole cipher buffer
    while (cipher_R_pnt != ciphertext.end()) {
        if (*cipher_R_pnt != 0) {
            std::cerr << "Warning: Not likely to be padding found after write buffer filled.\n";
        }

        cipher_R_pnt++;
    }

    if (plain_W_pnt != plaintext.end()) {
        throw X::Yaz0::Decompress("Some kind of problem with decompressing, didn't fill up plaintext exactly.");
    }

    return plaintext;
}