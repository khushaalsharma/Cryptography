#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

using namespace std;

class RSA{
    private:
        long long p, q, phi, n, e, d;

        bool isPrime(long long num){
            long long count = 0;

            for(long long i = 1; i <= sqrt(num); i++){
                if(n % i == 0){
                    count++;

                    if(num / i != i){
                        count++;
                    }
                }
            }

            return (count == 2 ? true : false);
        }

        long long generatePrime(long long min_val, long long max_val){
            random_device rd;
            mt19937 gen(rd());

            uniform_int_distribution<long long> dis(min_val, max_val);

            long long candidate;

            do{
                candidate = dis(gen);
                if(candidate % 2 == 0) candidate++;
            }while(!isPrime(candidate));

            return candidate;
        }

        long long eulerTotient(long long p, long long q){
            return (p-1)*(q-1);
        }

        long long gcd(long long a, long long b){
            while(b != 0){
                long long temp = b;
                b = a % b;
                a = temp;
            }

            return a;
        }

        // Extended Euclidean Algorithm
        long long extendedGCD(long long a, long long b, long long &x, long long &y) {
            if (a == 0) {
                x = 0;
                y = 1;
                return b;
            }
            
            long long x1, y1;
            long long gcd = extendedGCD(b % a, a, x1, y1);
            
            x = y1 - (b / a) * x1;
            y = x1;
            
            return gcd;
        }
        
        // Calculate modular inverse
        long long modInverse(long long a, long long m) {
            long long x, y;
            long long gcd = extendedGCD(a, m, x, y);
            
            if (gcd != 1) {
                throw runtime_error("Modular inverse does not exist");
            }
            
            return (x % m + m) % m;
        }
        
        // Fast modular exponentiation
        long long modPow(long long base, long long exp, long long mod) {
            long long result = 1;
            base = base % mod;
            
            while (exp > 0) {
                if (exp % 2 == 1) {
                    result = (result * base) % mod;
                }
                exp = exp >> 1;
                base = (base * base) % mod;
            }
            
            return result;
        }

    public:
        RSA(){
            p = generatePrime(100, 1000);
            do{
                q = generatePrime(100, 1000);
            }while(p == q);
            

            n = p*q;
            phi = eulerTotient(p, q);

            e = 65537;
            while(gcd(e, phi) != 1){
                e += 2;
            }

            d = modInverse(e, phi);
        }

        long long encrypt(long long message){
            if(message >= n){
                throw runtime_error("Message too large");
            }

            return modPow(message, e, n);
        }

        long long decrypt(long long ciphertext){
            return modPow(ciphertext, d, n);
        }

        vector<long long> encryptMessage(string message){
            vector<long long> encrypted;

            for(char ch : message){
                encrypted.push_back(encrypt(static_cast<long long>(ch)));
            }

            return encrypted;
        }

        string decryptMessage(vector<long long> encrypted){
            string decrypted = "";

            for(long long num : encrypted){
                decrypted += static_cast<char>(decrypt(num));
            }

            return decrypted;
        }

        void printKeys(){
            cout<<"Public Key: ("<<e<<", "<<n<<")"<<endl;
            cout<<"Private Key: ("<<d<<", "<<n<<")"<<endl;
        }

        bool verify(long long message, long long signature) {
            long long decrypted_sig = modPow(signature, e, n);
            return decrypted_sig == message;
        }
};

int main(){

    RSA* rsa = new RSA();

    rsa->printKeys();

    string message = "";

    cout<<"Enter Message: ";
    cin>>message;

    vector<long long> encrypted = rsa->encryptMessage(message);

    cout<<"Encrypted Message: ";
    for(long long num : encrypted){
        cout<<num<<" ";
    }
    cout<<endl;

    string decrypted = rsa->decryptMessage(encrypted);

    cout<<"Decrypted Message: "<<decrypted<<endl;

    return 0;
}