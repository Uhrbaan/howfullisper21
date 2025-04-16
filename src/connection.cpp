#include <M5Atom.h>
#include <sys/socket.h>

static int err;

// technically, these values shouldn't change once the robot is running → static
static const char *HOST_IP = "192.168.203.176";
static const int HOST_PORT = 12345;

static int sock;
static sockaddr_in dest_addr = {0};

static const int connection_attempts = 10;
static bool is_connected = false;

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

int init_tcp() {
    const char *TAG = "INIT TCP";

    inet_pton(AF_INET, HOST_IP, &dest_addr.sin_addr);  // convert to binary
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(HOST_PORT);

    err = create_tcp_socket();
    return err;
}

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

int close_tcp_socket() { return (close(sock) == 0) ? ESP_OK : ESP_FAIL; }