#pragma once

#include <exception>
#include <string>

class NimonException : public std::exception {
    protected:
        std::string message;
    public:
        NimonException(const std::string& message) : message(message){}
        virtual const char* what() const noexcept override {
            return message.c_str();
        }
};

class InsufficientFundsException : public NimonException {
    public:
        InsufficientFundsException(int required, int available) 
            : NimonException("Uang tidak cukup. Dibutuhkan: M" + std::to_string(required) + ", Tersedia: M" + std::to_string(available)) {}
};

class CardSlotFullException : public NimonException {
public:
    CardSlotFullException()
        : NimonException("Slot kartu penuh. Maksimal 3 kartu di tangan.") {}
};

/**
 * Bisa menerima pesan bebas karena kasusnya banyak, contoh make : 
 *   throw InvalidActionException("Kartu kemampuan hanya bisa digunakan SEBELUM melempar dadu.");
 *   throw InvalidActionException("Petak ini sudah berstatus hotel, tidak dapat dibangun.");
 *   throw InvalidActionException("Pilihan tidak valid. Masukkan 1 atau 2.");
 * (nti dihapus aj)
**/
class InvalidActionException : public NimonException {
public:
    explicit InvalidActionException(const std::string& msg)
        : NimonException(msg) {}
};

class InvalidTileException : public NimonException {
public:
    explicit InvalidTileException(const std::string& code)
        : NimonException("Petak dengan kode '" + code + "' tidak ditemukan.") {}
};

class FileException : public NimonException {
public:
    explicit FileException(const std::string& msg)
        : NimonException(msg) {}
};