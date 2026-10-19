#include "usb_cdc.h"

#include <iostream>
#include <string>

#include <thread>
#include <chrono>

int main()
{
    CDCUSB usb;

    std::cout << "====================================\n"
              << "       BBB USB CDC-ACM APP\n"
              << "====================================\n";

    if (!usb.openDevice())
        return 1;

    // while (true) {
    //     std::cout << "\n"
    //               << "1. Check connection\n"
    //               << "2. Check info\n"
    //               << "3. Write data and receive response\n"
    //               << "0. Exit\n"
    //               << "Select option: ";

    //     std::string choice;
    //     if (!std::getline(std::cin, choice))
    //         break;

    //     if (choice == "1")
    //         usb.checkConnection();
    //     else if (choice == "2")
    //         usb.checkInfo();
    //     else if (choice == "3")
    //         usb.writeReadOption();
    //     else if (choice == "4")
    //         usb.ReadOption();
    //     else if (choice == "0")
    //         break;
    //     else
    //         std::cout << "Invalid option.\n";
    // }

    // std::cout << "USB application closed.\n";
    // return 0;



    constexpr int loops = 200;
    int passed = 0;
    int failed = 0;

    for (int i = 0; i < loops; ++i) {
        const std::string tx =
            "Chu and Han war1 + Chu and Han war2 + Chu and Han war3 + "
            "Chu and Han war4 + Chu and Han war5 + Chu and Han war6 + "
            "Chu and Han war7!!!!_ times: " + std::to_string(i + 1);
        const std::string packet = tx + "\n";
        std::string rx;

        std::cout << "[" << (i + 1) << "/" << loops << "] Sending command...\n";

        if (!usb.writeData(packet)) {
            ++failed;
            std::cout << "**RESULT: FAIL (write) **\n===================\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        if (!usb.readData(rx, 3000)) {
            ++failed;
            std::cout << "**RESULT: FAIL (read/timeout) **\n===================\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        if (rx == tx) {
            std::cout << "**RESULT: PASS**\n";
            ++passed;
        } else {
            std::cout << "**RESULT: FAIL (reply mismatch)**\n"
                      << "TX: " << tx << '\n'
                      << "RX: " << rx << '\n';
            ++failed;
        }

        std::cout << "===================\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    std::cout << "\n========== SUMMARY ==========\n"
              << "PASS: " << passed << '/' << loops << '\n'
              << "FAIL: " << failed << '/' << loops << '\n';

    return (failed == 0) ? 0 : 1;
}

