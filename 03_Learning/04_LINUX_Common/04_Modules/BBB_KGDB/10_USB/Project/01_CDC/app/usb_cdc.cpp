#include "usb_cdc.h"

#include <cerrno>
#include <chrono>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>       // For std::ostringstream

#include <iomanip>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

namespace {
constexpr std::size_t MAX_RESPONSE_SIZE = 4096;
}

std::string usbVersionToString(uint16_t bcdUSB)
{
    std::ostringstream oss;

    oss << ((bcdUSB >> 8) & 0xFF) << "."
        << std::hex << std::uppercase
        << ((bcdUSB >> 4) & 0x0F)
        << (bcdUSB & 0x0F);

    return oss.str();
}

CDCUSB::CDCUSB() : fd_(-1)
{
}

CDCUSB::~CDCUSB()
{
    closeDevice();
}

bool CDCUSB::isOpen() const
{
    return fd_ >= 0;
}

bool CDCUSB::openDevice()
{
    if (fd_ >= 0)
        return true;

    fd_ = open("/dev/usb1", O_RDWR);

    if (fd_ < 0) {
        std::cerr << "Failed to open /dev/usb1: "
                  << strerror(errno) << '\n';
        return false;
    }

    /*
     * Call the ioctl commands supported by your custom
     * USB kernel driver here.
     *
     * Example:
     *
     * if (ioctl(fd_, YOUR_USB_IOCTL_CMD, &argument) < 0) {
     *     std::cerr << "USB ioctl failed: "
     *               << strerror(errno) << '\n';
     *     closeDevice();
     *     return false;
     * }
     */

    std::cout << "Opened custom USB device: /dev/usb1\n";
    return true;
}

void CDCUSB::closeDevice()
{
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
}

void CDCUSB::checkConnection() const
{
    struct cdc_status status;

    if (ioctl(fd_, BBB_CDC_CONNECTION, &status) < 0) {
        perror("BBB_CDC_CONNECTION");
        close(fd_);
        return;
    }

    std::cout << "\n========================================\n";
    std::cout << "          USB DEVICE STATUS\n";
    std::cout << "========================================\n";

    if (status.connected){
        if (status.full_speed) std::cout << "Full-speed device connected\n";
        else std::cout << "Low-speed device connected\n";
    }
    else std::cout << "No device connected\n";
}

void CDCUSB::checkInfo() const
{
    if (!isOpen()) {
        std::cerr << "Device is not open.\n";
        return;
    }

    cdc_info info{};

    if (ioctl(fd_, BBB_CDC_INFO, &info) < 0) {
        std::cerr << "Failed to get USB information: "
                  << strerror(errno) << '\n';
        return;
    }

    if (info.bcdUSB == 0) {
        std::cerr << "Invalid USB information. Check the connection.\n";
        return;
    }

    std::cout << "\n========================================\n";
    std::cout << "          USB DEVICE INFORMATION\n";
    std::cout << "========================================\n";

    std::cout << "USB Specification: USB "
              << usbVersionToString(info.bcdUSB) << '\n';

    std::cout << "Vendor ID (VID)  : 0x"
              << std::uppercase << std::hex
              << std::setw(4) << std::setfill('0')
              << info.idVendor << '\n';

    std::cout << "Product ID (PID) : 0x"
              << std::setw(4) << std::setfill('0')
              << info.idProduct << '\n';

    std::cout << "Device Release   : 0x"
              << std::setw(4) << std::setfill('0')
              << info.bcdDevice << '\n';

    std::cout << std::dec << std::setfill(' ');

    std::cout << "\nDescriptor Indices:\n";
    std::cout << "  Manufacturer   : "
              << static_cast<unsigned>(info.iManufacturer) << '\n';

    std::cout << "  Product        : "
              << static_cast<unsigned>(info.iProduct) << '\n';

    std::cout << "  Serial Number  : "
              << static_cast<unsigned>(info.iSerialNumber) << '\n';

    std::cout << "========================================\n";
}

bool CDCUSB::writeData(const std::string& data)
{
    if (!isOpen()) {
        std::cerr << "Device is not open.\n";
        return false;
    }

    std::size_t total = 0;
    while (total < data.size()) {
        const ssize_t n = write(fd_, data.data() + total, data.size() - total);

        if (n == -2){
            std::cerr << "Device not connected yet \n";
            return false;
        }

        if (n < 0) {
            if (errno == EINTR)
                continue;

            std::cerr << "Write failed: " << std::strerror(errno) << '\n';
            return false;
        }

        if (n == 0) {
            std::cerr << "Write returned zero bytes.\n";
            return false;
        }

        total += static_cast<std::size_t>(n);
    }

    std::cout << "Sent " << total << " bytes\n";
    return true;
}

bool CDCUSB::readData(std::string& response, int timeoutMs)
{
    if (!isOpen()) {
        std::cerr << "Device is not open.\n";
        return false;
    }

    response.clear();
    const auto deadline = std::chrono::steady_clock::now()
                       + std::chrono::milliseconds(timeoutMs);

    while (response.size() < MAX_RESPONSE_SIZE) {
        const auto now = std::chrono::steady_clock::now();
        if (now >= deadline) {
            std::cerr << "Response timeout. Partial response: '" << response << "'\n";
            return false;
        }

        const auto remaining =
            std::chrono::duration_cast<std::chrono::milliseconds>(deadline - now).count();

        pollfd pfd{};
        pfd.fd = fd_;
        pfd.events = POLLIN;

        const int ret = poll(&pfd, 1, static_cast<int>(remaining));

        if (ret == 0) {
            std::cerr << "Response timeout. Partial response: '" << response << "'\n";
            return false;
        }

        if (ret < 0) {
            if (errno == EINTR)
                continue;
            std::cerr << "poll failed: " << std::strerror(errno) << '\n';
            return false;
        }

        if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            std::cerr << "Device disconnected or I/O error (poll flags: "
                      << pfd.revents << ").\n";
            return false;
        }

        if (pfd.revents & POLLIN) {
            char buffer[256];
            const ssize_t n = read(fd_, buffer, sizeof(buffer));

            if (n == -2){
                std::cerr << "Device not connected yet \n";
                return false;
            }

            if (n < 0) {
                if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
                    continue;

                std::cerr << "Read failed: " << std::strerror(errno) << '\n';
                return false;
            }

            if (n == 0) {
                // No bytes returned yet; continue until the overall timeout.
                continue;
            }

            response.append(buffer, static_cast<std::size_t>(n));

            // Treat newline as the end of one Arduino response.
            const auto newline = response.find('\n');
            if (newline != std::string::npos) {
                response.resize(newline);
                while (!response.empty() && response.back() == '\r')
                    response.pop_back();

                // std::cout << "sz = " << response.size() << "\n";
                // std::cout << "Received: '" << response << "'\n";

                printf("Received[%d]: ", response.size());
                std::cout << response << "'\n";
                return true;
            }
        }
    }

    std::cerr << "Response exceeded maximum size without a line terminator.\n";
    return false;
}

void CDCUSB::writeReadOption()
{
    if (!isOpen()) {
        std::cerr << "Device is not open.\n";
        return;
    }

    std::string command;
    std::string response;

    std::cout << "Enter message: ";
    if (!std::getline(std::cin, command))
        return;

    command += '\n';

    // Do not call tcflush() here: /dev/usb1 is a custom character device,
    // not necessarily a TTY, and flushing input may discard a pending reply.
    if (!writeData(command))
        return;

    std::cout << "Waiting for Arduino echo back ...\n";
    if (!readData(response, 3000))
        std::cout << "Transaction failed or timed out.\n";
}

