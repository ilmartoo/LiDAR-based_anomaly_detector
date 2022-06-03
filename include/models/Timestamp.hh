/**
 * @file Timestamp.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 29/03/2022
 *
 * @brief Definición e implementación del objeto de Timestamp
 *
 */

#ifndef TIMESTAMP_CLASS_H
#define TIMESTAMP_CLASS_H

#include <stdint.h>
#include <string>
#include <cmath>

#define NANO_DIGITS 1000000000U

/**
 * @brief Marca temporal con una precisión de nanosegundos
 */
class Timestamp {
   public:
    /**
     * Constructor del objeto Timestamp
     * @param utc UTC timestamp
     */
    Timestamp(std::string utc) {
        uint64_t total = static_cast<uint64_t>(std::stoull(utc));
        seconds = total / NANO_DIGITS;
        nanoseconds = total % NANO_DIGITS;
    }
    /**
     * Constructor del objeto Timestamp
     * @param utc UTC timestamp en un array de bytes Little Endian
     */
    Timestamp(uint8_t utc[8]) {
        // Little Endian array
        uint64_t total = utc[0] +                      // 2^0
                         utc[1] * 256u +               // 2^8
                         utc[2] * 65536u +             // 2^16
                         utc[3] * 16777216u +          // 2^24
                         utc[4] * 4294967296u +        // 2^32
                         utc[5] * 1099511627776u +     // 2^40
                         utc[6] * 281474976710656u +   // 2^48
                         utc[7] * 72057594037927940u;  // 2^56
        seconds = total / NANO_DIGITS;
        nanoseconds = total % NANO_DIGITS;
    }
    /**
     * Constructor del objeto Timestamp
     * @param s Segundos
     * @param ns Nanosegundos
     */
    Timestamp(uint32_t s, uint32_t ns) : seconds(s + ns / NANO_DIGITS), nanoseconds(ns % NANO_DIGITS) {}

    ////// Getters
    /**
     * Devuelve los segundos del timestamp
     * @return Segundos del timestamp
     */
    uint32_t getSeconds() const { return this->seconds; }
    /**
     * Devuelve los nanosegundos del timestamp
     * @return Negundos del timestamp
     */
    uint32_t getNanoseconds() const { return this->nanoseconds; }

    ////// Formatting
    /**
     * Obtiene un string con los datos del timestamp
     * @return String con los datos del timestamp
     */
    std::string string() const { return std::to_string(seconds) + "s " + std::to_string(nanoseconds) + "ns"; }

    ////// Operators
    /**
     * Compara si el Timestamp actual es menor que otro
     * @param t Timestamp a comparar con el actual
     * @return true El Timestamp actual es menor
     */
    bool operator<(const Timestamp &t) const { return this->seconds < t.seconds || (this->seconds == t.seconds && this->nanoseconds < t.nanoseconds); }
    /**
     * Compara si el Timestamp actual es mayor que otro
     * @param t Timestamp a comparar con el actual
     * @return true El Timestamp actual es mayor
     */
    bool operator>(const Timestamp &t) const { return this->seconds > t.seconds || (this->seconds == t.seconds && this->nanoseconds > t.nanoseconds); }
    /**
     * Compara si el Timestamp es igual que otro
     * @param t Timestamp a comparar con el actual
     * @return true El Timestamp actual es igual
     */
    bool operator==(const Timestamp &t) const { return this->seconds == t.seconds && this->nanoseconds == t.nanoseconds; }
    /**
     * Compara si el Timestamp es desigual que otro
     * @param t Timestamp a comparar con el actual
     * @return true El Timestamp actual es igual
     */
    bool operator!=(const Timestamp &t) const { return !(*this == t); }
    /**
     * Suma nanosegundos al timestamp actual
     * @param ns Nanosegundos a sumar
     * @return Timestamp resultado
     */
    Timestamp operator+(const uint64_t ns) const { return {this->seconds + static_cast<uint32_t>(ns / NANO_DIGITS), this->nanoseconds + static_cast<uint32_t>(ns % NANO_DIGITS)}; }
    /**
     * Suma dos timestamps
     * @param t Timestamp a sumar
     * @return Timestamp resultado
     */
    Timestamp operator+(const Timestamp &t) const { return {this->seconds + t.seconds, this->nanoseconds + t.nanoseconds}; }

   private:
    uint32_t seconds;      ///< Segundos
    uint32_t nanoseconds;  ///< Nanosegundos
};

#endif  // TIMESTAMP_CLASS_H