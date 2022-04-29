/**
 * @file Timestamp.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 29/03/2022
 *
 * Definición e implementación del objeto de Timestamp
 *
 */

#ifndef TIMESTAMP_CLASS_H
#define TIMESTAMP_CLASS_H

#include <stdint.h>
#include <string>
#include <cmath>

#include "logging/debug.hh"

#define NANO_DIGITS 1000000000U

/**
 * Representación de un timestamp de un punto
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
     * @param utc UTC timestamp
     */
    Timestamp(uint8_t utc[8]) {
        uint64_t total = utc[0] + utc[1] * pow(2, 8) + utc[2] * pow(2, 16) + utc[3] * pow(2, 24) + utc[4] * pow(2, 32) +
                         utc[5] * pow(2, 40) + utc[6] * pow(2, 48) + utc[7] * pow(2, 56);
        seconds = total / NANO_DIGITS;
        nanoseconds = total % NANO_DIGITS;
    }

    /**
     * Constructor del objeto Timestamp
     * @param s Segundos
     * @param ns Nanosegundos
     */
    Timestamp(uint32_t s, uint32_t ns) : seconds(s), nanoseconds(ns) {}

    /**
     * Devuelve los segundos del timestamp
     * @return Segundos del timestamp
     */
    inline const uint32_t &getSeconds() const { return this->seconds; };

    /**
     * Devuelve los nanosegundos del timestamp
     * @return Negundos del timestamp
     */
    inline const uint32_t &getNanoseconds() const { return this->nanoseconds; };

    /**
     * Compara si el Timestamp actual es menor que otro
     * @param t Timestamp a comparar con el actual
     * @return true El Timestamp actual es menor
     * @return false El Timestamp actual es mayor o igual
     */
    bool operator<(const Timestamp &t) const {
        return this->seconds < t.seconds || (this->seconds == t.seconds && this->nanoseconds < t.nanoseconds);
    }

    /**
     * Compara si el Timestamp actual es mayor que otro
     * @param t Timestamp a comparar con el actual
     * @return true El Timestamp actual es mayor
     * @return false El Timestamp actual es menor o igual
     */
    bool operator>(const Timestamp &t) const {
        return this->seconds > t.seconds || (this->seconds == t.seconds && this->nanoseconds > t.nanoseconds);
    }

    /**
     * Compara si el Timestamp es igual que otro
     * @param t Timestamp a comparar con el actual
     * @return true El Timestamp actual es igual
     * @return false El Timestamp actual es mayor o menor
     */
    bool operator==(const Timestamp &t) const { return this->seconds == t.seconds && this->nanoseconds == t.nanoseconds; }

    /**
     * Suma nanosegundos al timestamp actual
     * @param ns Nanosegundos a sumar
     * @return Timestamp resultado
     */
    Timestamp operator+(const uint64_t ns) const {
        Timestamp t(this->seconds + ns / NANO_DIGITS, this->nanoseconds + ns % NANO_DIGITS);
        return t;
    }

    /**
     * Obtiene un string con los datos del timestamp
     * @return String con los datos del timestamp
     */
    inline const std::string string() const { return std::to_string(seconds) + "s " + std::to_string(nanoseconds) + "ns"; }

   private:
    uint32_t seconds;      ///< Segundos
    uint32_t nanoseconds;  ///< Nanosegundos
};

#endif  // TIMESTAMP_CLASS_H