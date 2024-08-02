#ifndef __ASTROS_MESSAGING_HPP
#define __ASTROS_MESSAGING_HPP

enum class AstrOsUiMessageType {
    UNKNOWN,
    SSID_DETECTED,
    WIFI_SCAN_COMPLETED,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_DISCONNECTED,
    MODAL_MESSAGE
};

typedef struct {
    AstrOsUiMessageType type;
    char * message;
} astros_ui_message_t;

enum class AstrOsWifiMessageType {
    UNKNOWN,
    SCAN_START,
    SCAN_COMPLETED,
    CONNECT,
    DISCONNECT
};

typedef struct {
    AstrOsWifiMessageType type;
    char * message;
} astros_wifi_message_t;

#endif  //__ASTROS_MESSAGING_HPP