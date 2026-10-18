#include "usb_cdc.h"

#include <iostream>
#include <string>

#include <thread> // Required for std::this_thread::sleep_for
#include <chrono> // Required for time units like std::chrono::milliseconds

int main()
{
    CDCUSB usb;

    std::cout << "====================================\n"
              << "       BBB USB CDC-ACM APP\n"
              << "====================================\n";

    if (!usb.openDevice())
        return 1;

    while (true) {
        std::cout << "\n"
                  << "1. Check connection\n"
                  << "2. Check info\n"
                  << "3. Write data and receive response\n"
                  << "0. Exit\n"
                  << "Select option: ";

        std::string choice;
        if (!std::getline(std::cin, choice))
            break;

        if (choice == "1")
            usb.checkConnection();
        else if (choice == "2")
            usb.checkInfo();
        else if (choice == "3")
            usb.writeReadOption();
        else if (choice == "0")
            break;
        else
            std::cout << "Invalid option.\n";
    }

    std::cout << "USB application closed.\n";
    return 0;



    // int loops = 10;
    // int passed = 0, failed = 0;
    // for (int i = 0; i < loops; ++i) {
    //     const std::string tx =
    //         "Chu and Han total war!!!!_" + std::to_string(i + 1);
    //     std::string rx;

    //     const std::string packet = tx + "\n";

    //     if (!usb.writeData(packet)) {
    //         ++failed;
    //         continue;
    //     }

    //     // Read immediately after write
    //     if (!usb.readData(rx, 3000)) {
    //         ++failed;
    //         continue;
    //     }

    //     if (rx == tx) {
    //         std::cout << "**RESULT: PASS**\n";
    //         ++passed;
    //     } else {
    //         std::cout << "**RESULT: FAIL**\n";
    //         ++failed;
    //     }

    //     std::cout << "===================\n";

    //     rx.clear();

    //     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // }
}
