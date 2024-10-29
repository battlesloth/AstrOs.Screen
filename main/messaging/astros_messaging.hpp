#ifndef __ASTROS_MESSAGING_HPP
#define __ASTROS_MESSAGING_HPP

enum class AstrOsUiMessageType {
    UNKNOWN,
    SSID_DETECTED,
    WIFI_SCAN_COMPLETED,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_DISCONNECTED,
    GET_WIFI_CREDS,
    MODAL_MESSAGE,
    UPDATE_BUTTON_NAMES,
    SCRIPTS_RECEIVED
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
    DISCONNECT,
    SYNC_SCRIPTS,
    SCRIPT_COMMAND,
    PANIC_STOP
};

typedef struct {
    AstrOsWifiMessageType type;
    char * message;
} astros_wifi_message_t;

#endif  //__ASTROS_MESSAGING_HPP