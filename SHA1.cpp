#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

class sha{
    private:
        // Initial hash values (in hex)
        uint32_t h0 = 0x67452301;
        uint32_t h1 = 0xEFCDAB89;
        uint32_t h2 = 0x98BADCFE;
        uint32_t h3 = 0x10325476;
        uint32_t h4 = 0xC3D2E1F0;
        
        // Round constants
        uint32_t k0 = 0x5A827999;  // Rounds 0-19
        uint32_t k1 = 0x6ED9EBA1;  // Rounds 20-39
        uint32_t k2 = 0x8F1BBCDC;  // Rounds 40-59
        uint32_t k3 = 0xCA62C1D6;  // Rounds 60-79

        //rotate left:
        uint32_t rotateLeft(uint32_t num, int shift){
            return (num >> shift | num << (32 - shift));
        }

        //bit manipulation functions

        uint32_t f1(uint32_t b, uint32_t c, uint32_t d){
            return (b & c) | (~b & d);
        }

        uint32_t f2(uint32_t b, uint32_t c, uint32_t d){
            return (b^c^d);
        }

        uint32_t f3(uint32_t b, uint32_t c, uint32_t d){
            return ((b & c) | (b & d) | (c & d));
        }

        vector<uint8_t> preprocessMessage(const string message){
            vector<uint8_t> paddedMsg;

            for(char c : message){
                paddedMsg.push_back(static_cast<uint32_t>(c));
            }

            uint64_t originalLength = message.length() * 8;
            //adding bit 1 to the padded message : 0x80
            paddedMsg.push_back(0x80);
            
            //adding 0 bits till length = 448 mod 512
            int length = (447 - (paddedMsg.size() - 1)) % 512;
            for(int i=0; i<length; i++){
                paddedMsg.push_back(0x00);
            }

            //Append original message length as 64-bit big-endian integer
            for(int i=0; i<8; i++){
                paddedMsg.push_back((originalLength >> ((7-i)*8)) & 0xFF);
            }

            return paddedMsg;
        }

        void processChunk(const vector<uint8_t>& paddedMsg, size_t size){
            uint8_t w[80];

            //converts bytes to 32 bits words
            for (int i = 0; i < 16; i++) {
                w[i] = (paddedMsg[size + i * 4] << 24) |
                    (paddedMsg[size + i * 4 + 1] << 16) |
                    (paddedMsg[size + i * 4 + 2] << 8) |
                    (paddedMsg[size + i * 4 + 3]);
            }

            for (int i = 16; i < 80; i++) {
                w[i] = rotateLeft(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
            }

            uint32_t a = h0;
            uint32_t b = h1;
            uint32_t c = h2;
            uint32_t d = h3;
            uint32_t e = h4;

             for (int i = 0; i < 80; i++) {
                uint32_t f, k;
                
                // Choose function and constant based on round
                if (i < 20) {
                    f = f1(b, c, d);
                    k = k0;
                } else if (i < 40) {
                    f = f2(b, c, d);
                    k = k1;
                } else if (i < 60) {
                    f = f3(b, c, d);
                    k = k2;
                } else {
                    f = f2(b, c, d);
                    k = k3;
                }
                
                // Perform round operation
                uint32_t temp = rotateLeft(a, 5) + f + e + k + w[i];
                e = d;
                d = c;
                c = rotateLeft(b, 30);
                b = a;
                a = temp;
            }
            
            // Step 2e: Add this chunk's hash to result so far
            h0 += a;
            h1 += b;
            h2 += c;
            h3 += d;
            h4 += e;
        }

        void processChunks(const vector<uint8_t> paddedMsg){
            for(size_t i=0; i<paddedMsg.size(); i+=64){
                processChunk(paddedMsg, i);
            }
        }

        string produceHash(){
            stringstream ss;

            ss << hex << setfill('0');
            ss << setw(8) << h0;
            ss << setw(8) << h1;
            ss << setw(8) << h2;
            ss << setw(8) << h3;
            ss << setw(8) << h4;
            
            return ss.str();
        }

    public:

        string hash(const string message){
            vector<uint8_t> paddedMsg = preprocessMessage(message);
            processChunks(paddedMsg);
            return produceHash();
        }
};


int main(){

    sha* sha1 = new sha();

    string message = "";

    cout<<"Enter your message: ";
    cin>>message;

    cout<<"HASHED: "<<sha1->hash(message)<<endl;

    return 0;
}