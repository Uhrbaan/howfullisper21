#include <M5Atom.h>
#include <sys/socket.h>

static int err;

/**
 * @brief These values are here to configure the IP address of the server the
 * chip will connect to, and what port to use.
 *
 * TODO: make configuration from build options.
 */
static const char *HOST_IP = "192.168.203.176";
static const int HOST_PORT = 12345;

static int sock;
static sockaddr_in dest_addr = {0};

static const int connection_attempts = 10;
static bool is_connected = false;

/**
 * @brief Creates a TCP scoket.
 *
 * @return ESP_OK if the socket could be created, ESP_FAIL if otherwise.
 */
int create_tcp_socket() {
    const char *TAG = "CREATE TCP SOCKET";

    // protocol number:
    // <http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml>
    sock = socket(AF_INET, SOCK_STREAM, 6);
    if (sock == -1) {
        ESP_LOGE(TAG, "Failed to create socket: %s", strerror(errno));
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Successfully created socket %d.", sock);
    return ESP_OK;
}

/**
 * @brief Initializes the TCP connection. It generates the TCP connection
 * configuration and creates a socket.
 *
 * @return ESP_OK if successful, ESP_FAIL if it failed to create the socket.
 */
int init_tcp() {
    const char *TAG = "INIT TCP";

    inet_pton(AF_INET, HOST_IP, &dest_addr.sin_addr);  // convert to binary
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(HOST_PORT);

    err = create_tcp_socket();
    return err;
}

/**
 * @brief Connects the socket to the server with the parameters defined in the
 * configuration (static).
 *
 * @return ESP_OK if the connection was made, ESP_FAIL if it could not create
 * the connection.
 */
int connect_tcp_socket() {
    const char *TAG = "CONNECT TCP SOCKET";

    // connect to provided ip address
    err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Error connecting to %s:%d. Errno: %s", HOST_IP,
                 HOST_PORT, strerror(errno));
        is_connected = false;
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Succesfully connected to %s:%d", HOST_IP, HOST_PORT);
    is_connected = true;
    return ESP_OK;
}

/**
 * @brief Sends some data (buffer) over TCP to the server. The function will
 * create a connection to the server if none has already been made.
 *
 * @param buffer The data that will be sent to the server.
 * @param size The size in bytes of the payload (the data).
 * @return int
 */
int send_tcp(const char *buffer, size_t size) {
    const char *TAG = "SEND TCP";

    // make attemps at connecting if not already connected
    if (!is_connected) {
        err = -1;
        for (int i = 0; i < connection_attempts && err != ESP_OK; i++) {
            err = connect_tcp_socket();
            delay(50);
        }
        if (err != ESP_OK) {
            return ESP_FAIL;
        }
    }

    int len = send(sock, buffer, size, 0);
    if (len == -1) {
        ESP_LOGE(TAG, "Failed to send data.");
        // check if conneciton is broken
        if (errno == EPIPE || errno == ECONNRESET) {
            ESP_LOGE(TAG, "Connection broken.");
            close(sock);
            sock = -1;  // Reset the socket descriptor
            is_connected = false;
        }
        return ESP_FAIL;
    }
    return ESP_OK;
}

/**
 * @brief Closes te tcp socket.
 *
 * @return ESP_OK if the socket got closed, ESP_FAIL otherwise (the socket was
 * probably invalid).
 */
int close_tcp_socket() { return (close(sock) == 0) ? ESP_OK : ESP_FAIL; }