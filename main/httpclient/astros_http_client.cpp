#include "astros_http_client.hpp"
#include <esp_log.h>
#include <sys/param.h>
#include <esp_tls.h>
#include "../messaging/astros_messaging.hpp"
#include <astros_util.h>

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048

static const char *TAG = "AstrOsHttpClient";

esp_err_t httpSyncHandler(esp_http_client_event_t *evt);
esp_err_t httpScriptHandler(esp_http_client_event_t *evt);
void sendToUiQueue(AstrOsUiMessageType type, char *message);

QueueHandle_t AstrOsHttpClient::queue;

AstrOsHttpClient httpClient;

AstrOsHttpClient::AstrOsHttpClient()
{
}

AstrOsHttpClient::~AstrOsHttpClient()
{
}

void AstrOsHttpClient::Init(QueueHandle_t queue)
{
    AstrOsHttpClient::queue = queue;
}

void AstrOsHttpClient::SetApiKey(std::string apiKey)
{
    this->apiKey = apiKey;
}

void AstrOsHttpClient::SetHost(std::string host)
{
    this->host = host;
}

void AstrOsHttpClient::SendSyncRequest()
{
    char response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};

    esp_http_client_config_t config = {
        .host = host.c_str(),
        .path = "/api/remotecontrolsync",
        .disable_auto_redirect = true,
        .event_handler = httpSyncHandler,
        .user_data = response_buffer
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_header(client, "x-token", apiKey.c_str());

    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);
    
    if (logError(TAG, __func__, __LINE__, err))
    {
        sendToUiQueue(AstrOsUiMessageType::MODAL_MESSAGE, "Failed to send script command");
    }
}

void AstrOsHttpClient::SendScriptCommand(std::string scriptId)
{
    esp_http_client_config_t config = {
        .host = host.c_str(),
        .path = "/api/remotecontrol",
        .query = ("id=" + scriptId).c_str(),
        .disable_auto_redirect = true,
        .event_handler = httpScriptHandler};

    esp_http_client_handle_t client = esp_http_client_init(&config);
    
    esp_http_client_set_header(client, "x-token", apiKey.c_str());

    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);

    if (logError(TAG, __func__, __LINE__, err))
    {
        sendToUiQueue(AstrOsUiMessageType::MODAL_MESSAGE, "Failed to send script command");
    }
}

void AstrOsHttpClient::SendPanicStop()
{
    esp_http_client_config_t config = {
        .host = host.c_str(),
        .path = "/api/remotecontrol",
        .query = "id=panic",
        .disable_auto_redirect = true,
        .event_handler = httpScriptHandler};

    esp_http_client_handle_t client = esp_http_client_init(&config);
    
    esp_http_client_set_header(client, "x-token", apiKey.c_str());

    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);

    if (logError(TAG, __func__, __LINE__, err))
    {
        sendToUiQueue(AstrOsUiMessageType::MODAL_MESSAGE, "Failed to send panic stop command");
    }
}

// =======================================================================
// ========================= HTTP Event Handlers =========================
// =======================================================================

void sendToUiQueue(AstrOsUiMessageType type, char *message)
{
    astros_ui_message_t msg;
    msg.type = type;

    auto len = strlen(message);

    if (len > 0)
    {
        msg.message = (char *)malloc(len + 1);
        memcpy(msg.message, message, len);
        msg.message[len] = '\0';
    }
    else
    {
        msg.message = nullptr;
    }

    if (xQueueSend(AstrOsHttpClient::queue, &msg, pdMS_TO_TICKS(100)) != pdTRUE)
    {
        ESP_LOGE(TAG, "Failed to send message to UI update queue");
        free(msg.message);
    }
}

esp_err_t httpSyncHandler(esp_http_client_event_t *evt)
{
    static char *output_buffer; // Buffer to store response of http request from event handler
    static int output_len;      // Stores number of bytes read
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_DATA:
    {
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        // Clean the buffer in case of a new request
        if (output_len == 0 && evt->user_data)
        {
            // we are just starting to copy the output data into the use
            memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
        }
        /*
         *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
         *  However, event handler can also be used in case chunked encoding is used.
         */
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            // If user_data buffer is configured, copy the response into the buffer
            int copy_len = 0;
            if (evt->user_data)
            {
                // The last byte in evt->user_data is kept for the NULL character in case of out-of-bound access.
                copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                if (copy_len)
                {
                    memcpy(evt->user_data + output_len, evt->data, copy_len);
                }
            }
            else
            {
                int content_len = esp_http_client_get_content_length(evt->client);
                if (output_buffer == NULL)
                {
                    // We initialize output_buffer with 0 because it is used by strlen() and similar functions therefore should be null terminated.
                    output_buffer = (char *)calloc(content_len + 1, sizeof(char));
                    output_len = 0;
                    if (output_buffer == NULL)
                    {
                        ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                        return ESP_FAIL;
                    }
                }
                copy_len = MIN(evt->data_len, (content_len - output_len));
                if (copy_len)
                {
                    memcpy(output_buffer + output_len, evt->data, copy_len);
                }
            }
            output_len += copy_len;
        }

        break;
    }
    case HTTP_EVENT_ON_FINISH:
    {
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        if (output_buffer != NULL)
        {
            sendToUiQueue(AstrOsUiMessageType::SCRIPTS_RECEIVED, output_buffer);
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    }
    case HTTP_EVENT_DISCONNECTED:
    {
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        int mbedtls_err = 0;
        esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
        if (err != 0)
        {
            ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
            ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            sendToUiQueue(AstrOsUiMessageType::MODAL_MESSAGE, "Failed to sync scripts");
        }
        if (output_buffer != NULL)
        {
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    }
    default:
        break;
    }
    return ESP_OK;
}

esp_err_t httpScriptHandler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_FINISH:
    {
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        // Send a message to the queue
        sendToUiQueue(AstrOsUiMessageType::MODAL_MESSAGE, "Script command sent");
        break;
    }
    case HTTP_EVENT_DISCONNECTED:
    {
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        int mbedtls_err = 0;
        esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
        if (err != 0)
        {
            ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
            ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            sendToUiQueue(AstrOsUiMessageType::MODAL_MESSAGE, "Failed to send script command");
        }
        break;
    }
    default:
        break;
    }
    return ESP_OK;
}