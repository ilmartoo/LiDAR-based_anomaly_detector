/**
 * @file string_format.h
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 02/04/2022
 *
 * Header compatible con C y C++ que recoge constantes de formatado de strings para dar color
 * o cambiar el estilo del texto
 *
 * Se definen una serie de constantes que recogen los distintos tipos de formatado disponibles según el estandar ANSI.
 * Toda constante tiene una macro asociada de forma que todo el texto introducido en el interior sera precedido de la
 * constante homónima y será proseguido por la constante de borrado de formato para evitar posibles errores no deseados
 * consecuencia de no introducirla cuando era necesario
 *
 * Existen unas macros especiales disponibles únicamente en C++ para operar con argumentos de tipo std::string cuyos
 * nombres finalizan con _s
 */

#ifndef STRING_FORMAT_H
#define STRING_FORMAT_H

/* RESET ALL FORMAT MODIFIERS STRING */
#define RESET             "\e[0m"  ///< Reset all formatting

/* STYLE FORMAT CONSTANTS */
#define BOLD              "\e[1m"  ///< BOLD style format
#define FAINT             "\e[2m"  ///< FAINT style format
#define UNDERLINE         "\e[3m"  ///< UNDERLINE style format
#define BLINK             "\e[5m"  ///< BLINK style format
#define REVERSED          "\e[7m"  ///< REVERSED style format
#define CONCEALED         "\e[8m"  ///< CONCEALED style format

/* COLOR FORMAT CONSTANTS */
#define WHITE             "\e[1;37m"  ///< WHITE color format
#define LIGHTGREY         "\e[37m"    ///< LIGHTGREY color format
#define DARKGREY          "\e[1;30m"  ///< DARKGREY color format
#define BLACK             "\e[30m"    ///< BLACK color format
#define RED               "\e[31m"    ///< RED color format
#define LIGHTRED          "\e[1;31m"  ///< LIGHTRED color format
#define YELLOW            "\e[1;33m"  ///< YELLOW color format
#define LIME              "\e[1;32m"  ///< LIME color format
#define GREEN             "\e[32m"    ///< GREEN color format
#define STEELBLUE         "\e[1;34m"  ///< STEELBLUE color format
#define SKYBLUE           "\e[1;36m"  ///< SKYBLUE color format
#define CYAN              "\e[36m"    ///< CYAN color format
#define BLUE              "\e[34m"    ///< BLUE color format
#define PURPLE            "\e[35m"    ///< PURPLE color format
#define MAGETA            "\e[1;35m"  ///< MAGETA color format
#define BROWN             "\e[33m"    ///< BROWN color format

/* BACKGROND COLOR FORMAT CONSTANTS */
#define B_WHITE           "\e[1;47m"  ///< WHITE background color format
#define B_LIGHTGREY       "\e[47m"    ///< LIGHTGREY background color format
#define B_DARKGREY        "\e[1;40m"  ///< DARKGREY background color format
#define B_BLACK           "\e[40m"    ///< BLACK background color format
#define B_RED             "\e[41m"    ///< RED background color format
#define B_LIGHTRED        "\e[1;41m"  ///< LIGHTRED background color format
#define B_YELLOW          "\e[1;43m"  ///< YELLOW background color format
#define B_LIME            "\e[1;42m"  ///< LIME background color format
#define B_GREEN           "\e[42m"    ///< GREEN background color format
#define B_STEELBLUE       "\e[1;44m"  ///< STEELBLUE background color format
#define B_SKYBLUE         "\e[1;46m"  ///< SKYBLUE background color format
#define B_CYAN            "\e[46m"    ///< CYAN background color format
#define B_BLUE            "\e[44m"    ///< BLUE background color format
#define B_PURPLE          "\e[45m"    ///< PURPLE background color format
#define B_MAGETA          "\e[1;45m"  ///< MAGETA background color format
#define B_BROWN           "\e[43m"    ///< BROWN background color format

/* TEXT FORMATTING MACROS */
#define bold(text)        BOLD text RESET       ///< BOLD text format macro
#define faint(text)       FAINT text RESET      ///< FAINT text format macro
#define underline(text)   UNDERLINE text RESET  ///< UNDERLINE text format macro
#define blink(text)       BLINK text RESET      ///< BLINK text format macro
#define reversed(text)    REVERSED text RESET   ///< REVERSED text format macro
#define concealed(text)   CONCEALED text RESET  ///< CONCEALED text format macro

#define white(text)       WHITE text RESET      ///< WHITE text format macro
#define lightgrey(text)   LIGHTGREY text RESET  ///< LIGHTGREY text format macro
#define darkgrey(text)    DARKGREY text RESET   ///< DARKGREY text format macro
#define black(text)       BLACK text RESET      ///< BLACK text format macro
#define red(text)         RED text RESET        ///< RED text format macro
#define lightred(text)    LIGHTRED text RESET   ///< LIGHTRED text format macro
#define yellow(text)      YELLOW text RESET     ///< YELLOW text format macro
#define lime(text)        LIME text RESET       ///< LIME text format macro
#define green(text)       GREEN text RESET      ///< GREEN text format macro
#define steelblue(text)   STEELBLUE text RESET  ///< STEELBLUE text format macro
#define skyblue(text)     SKYBLUE text RESET    ///< SKYBLUE text format macro
#define cyan(text)        CYAN text RESET       ///< CYAN text format macro
#define blue(text)        BLUE text RESET       ///< BLUE text format macro
#define purple(text)      PURPLE text RESET     ///< PURPLE text format macro
#define mageta(text)      MAGETA text RESET     ///< MAGETA text format macro
#define brown(text)       BROWN text RESET      ///< BROWN text format macro

#define b_white(text)     B_WHITE text RESET      ///< B_WHITE text format macro
#define b_lightgrey(text) B_LIGHTGREY text RESET  ///< B_LIGHTGREY text format macro
#define b_darkgrey(text)  B_DARKGREY text RESET   ///< B_DARKGREY text format macro
#define b_black(text)     B_BLACK text RESET      ///< B_BLACK text format macro
#define b_red(text)       B_RED text RESET        ///< B_RED text format macro
#define b_lightred(text)  B_LIGHTRED text RESET   ///< B_LIGHTRED text format macro
#define b_yellow(text)    B_YELLOW text RESET     ///< B_YELLOW text format macro
#define b_lime(text)      B_LIME text RESET       ///< B_LIME text format macro
#define b_green(text)     B_GREEN text RESET      ///< B_GREEN text format macro
#define b_steelblue(text) B_STEELBLUE text RESET  ///< B_STEELBLUE text format macro
#define b_skyblue(text)   B_SKYBLUE text RESET    ///< B_SKYBLUE text format macro
#define b_cyan(text)      B_CYAN text RESET       ///< B_CYAN text format macro
#define b_blue(text)      B_BLUE text RESET       ///< B_BLUE text format macro
#define b_purple(text)    B_PURPLE text RESET     ///< B_PURPLE text format macro
#define b_mageta(text)    B_MAGETA text RESET     ///< B_MAGETA text format macro
#define b_brown(text)     B_BROWN text RESET      ///< B_BROWN text format macro

#ifdef __cplusplus  // C++

#include <string>

/* TEXT FORMATTING MACROS FOR C++ STRINGS */
#define bold_s(text)        BOLD + text + RESET       ///< BOLD text format macro
#define faint_s(text)       FAINT + text + RESET      ///< FAINT text format macro
#define underline_s(text)   UNDERLINE + text + RESET  ///< UNDERLINE text format macro
#define blink_s(text)       BLINK + text + RESET      ///< BLINK text format macro
#define reversed_s(text)    REVERSED + text + RESET   ///< REVERSED text format macro
#define concealed_s(text)   CONCEALED + text + RESET  ///< CONCEALED text format macro

#define white_s(text)       WHITE + text + RESET      ///< WHITE text format macro
#define lightgrey_s(text)   LIGHTGREY + text + RESET  ///< LIGHTGREY text format macro
#define darkgrey_s(text)    DARKGREY + text + RESET   ///< DARKGREY text format macro
#define black_s(text)       BLACK + text + RESET      ///< BLACK text format macro
#define red_s(text)         RED + text + RESET        ///< RED text format macro
#define lightred_s(text)    LIGHTRED + text + RESET   ///< LIGHTRED text format macro
#define yellow_s(text)      YELLOW + text + RESET     ///< YELLOW text format macro
#define lime_s(text)        LIME + text + RESET       ///< LIME text format macro
#define green_s(text)       GREEN + text + RESET      ///< GREEN text format macro
#define steelblue_s(text)   STEELBLUE + text + RESET  ///< STEELBLUE text format macro
#define skyblue_s(text)     SKYBLUE + text + RESET    ///< SKYBLUE text format macro
#define cyan_s(text)        CYAN + text + RESET       ///< CYAN text format macro
#define blue_s(text)        BLUE + text + RESET       ///< BLUE text format macro
#define purple_s(text)      PURPLE + text + RESET     ///< PURPLE text format macro
#define mageta_s(text)      MAGETA + text + RESET     ///< MAGETA text format macro
#define brown_s(text)       BROWN + text + RESET      ///< BROWN text format macro

#define b_white_s(text)     B_WHITE + text + RESET      ///< B_WHITE text format macro
#define b_lightgrey_s(text) B_LIGHTGREY + text + RESET  ///< B_LIGHTGREY text format macro
#define b_darkgrey_s(text)  B_DARKGREY + text + RESET   ///< B_DARKGREY text format macro
#define b_black_s(text)     B_BLACK + text + RESET      ///< B_BLACK text format macro
#define b_red_s(text)       B_RED + text + RESET        ///< B_RED text format macro
#define b_lightred_s(text)  B_LIGHTRED + text + RESET   ///< B_LIGHTRED text format macro
#define b_yellow_s(text)    B_YELLOW + text + RESET     ///< B_YELLOW text format macro
#define b_lime_s(text)      B_LIME + text + RESET       ///< B_LIME text format macro
#define b_green_s(text)     B_GREEN + text + RESET      ///< B_GREEN text format macro
#define b_steelblue_s(text) B_STEELBLUE + text + RESET  ///< B_STEELBLUE text format macro
#define b_skyblue_s(text)   B_SKYBLUE + text + RESET    ///< B_SKYBLUE text format macro
#define b_cyan_s(text)      B_CYAN + text + RESET       ///< B_CYAN text format macro
#define b_blue_s(text)      B_BLUE + text + RESET       ///< B_BLUE text format macro
#define b_purple_s(text)    B_PURPLE + text + RESET     ///< B_PURPLE text format macro
#define b_mageta_s(text)    B_MAGETA + text + RESET     ///< B_MAGETA text format macro
#define b_brown_s(text)     B_BROWN + text + RESET      ///< B_BROWN text format macro

#endif  // __cplusplus

#endif  // STRING_FORMAT_H