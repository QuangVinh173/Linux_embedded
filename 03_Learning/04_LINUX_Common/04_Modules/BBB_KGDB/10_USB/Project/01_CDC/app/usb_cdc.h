#ifndef USB_CDC_H
#define USB_CDC_H
#include <string>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <sys/ioctl.h>

struct cdc_status {
    bool connected;
    bool full_speed;
};

struct cdc_info {
    __u16 bcdUSB;
    __u16 idVendor;
    __u16 idProduct;
    __u16 bcdDevice;
    __u8 iManufacturer;
    __u8 iProduct;
    __u8 iSerialNumber;
};
 
#define BBB_CDC_IOC_MAGIC 'U'
 
#define BBB_CDC_CONNECTION \
    _IOW(BBB_CDC_IOC_MAGIC, 1, struct cdc_info)

#define BBB_CDC_INFO \
    _IOW(BBB_CDC_IOC_MAGIC, 2, struct cdc_info)

class CDCUSB {
public:
    CDCUSB();
    ~CDCUSB();

    CDCUSB(const CDCUSB&) = delete;
    CDCUSB& operator=(const CDCUSB&) = delete;

    bool openDevice();
    void closeDevice();
    bool isOpen() const;

    void checkConnection() const;
    void checkInfo() const;
    bool writeData(const std::string& data);
    bool readData(std::string& response, int timeoutMs = 3000);

    void writeOption();
    void ReadOption();
    void writeReadOption();

private:
    static constexpr const char* DEVICE = "/dev/usb1";
    int fd_;
};

#endif // USB_CDC_H
