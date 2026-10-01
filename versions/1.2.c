#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <getopt.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define VERSION "1.2"
#define BUILD_DATE __DATE__ " " __TIME__
#define BUFFER_SIZE 4096

typedef enum {
    LANG_EN,
    LANG_ES
} Language;

Language current_lang = LANG_EN;

void print_help(const char *prog_name) {
    printf("U2IP - URL to IP and Geolocation Tool v%s\n\n", VERSION);
    printf("Usage:\n");
    printf("  %s [options] <URL or Domain>\n", prog_name);
    printf("  %s -u <URL or Domain>\n\n", prog_name);
    printf("Options:\n");
    printf("  -h, --help            Show this help message and exit\n");
    printf("  -v, --version         Display program version\n");
    printf("  -b, --build           Display build timestamp and information\n");
    printf("  -l, --lang <en|es>    Select output language (Default: en)\n");
    printf("  -u, --url <URL>       Specify target URL or hostname\n\n");
    printf("Examples:\n");
    printf("  %s google.com\n", prog_name);
    printf("  %s -l es -u https://www.github.com\n", prog_name);
    printf("  %s --version\n", prog_name);
}

void print_version(void) {
    printf("U2IP version %s\n", VERSION);
}

void print_build(void) {
    printf("U2IP Build Info:\n");
    printf("  Version:    %s\n", VERSION);
    printf("  Build Date: %s\n", BUILD_DATE);
    printf("  Compiler:   GCC/Clang (POSIX C)\n");
}

void clean_url(const char *input_url, char *hostname) {
    const char *start = input_url;
    
    if (strncmp(start, "http://", 7) == 0) {
        start += 7;
    } else if (strncmp(start, "https://", 8) == 0) {
        start += 8;
    }
    
    int i = 0;
    while (start[i] != '\0' && start[i] != '/' && start[i] != ':' && start[i] != '?') {
        hostname[i] = start[i];
        i++;
    }
    hostname[i] = '\0';
}

int resolve_domain(const char *hostname, char *ip_str) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        return -1;
    }

    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, INET_ADDRSTRLEN);
    freeaddrinfo(res);
    return 0;
}

void parse_json_value(const char *json, const char *key, char *output, size_t max_len) {
    char search_key[128];
    snprintf(search_key, sizeof(search_key), "\"%s\":", key);
    
    char *pos = strstr(json, search_key);
    if (!pos) {
        strncpy(output, "N/A", max_len);
        return;
    }
    
    pos += strlen(search_key);
    while (*pos == ' ' || *pos == '"') pos++;
    
    size_t i = 0;
    while (*pos != '"' && *pos != ',' && *pos != '}' && *pos != '\0' && i < max_len - 1) {
        output[i++] = *pos++;
    }
    output[i] = '\0';
}

void get_geolocation(const char *ip) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE * 2] = {0};
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror((current_lang == LANG_ES) ? "Error al crear el socket" : "Socket creation error");
        return;
    }

    server = gethostbyname("ip-api.com");
    if (server == NULL) {
        fprintf(stderr, (current_lang == LANG_ES) ? 
            "[!] Error: No se pudo resolver el servicio de geolocalización.\n" : 
            "[!] Error: Could not resolve geolocation service host.\n");
        close(sockfd);
        return;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(80);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror((current_lang == LANG_ES) ? "[!] Error de conexión" : "[!] Connection failed");
        close(sockfd);
        return;
    }

    snprintf(request, sizeof(request),
             "GET /json/%s HTTP/1.1\r\n"
             "Host: ip-api.com\r\n"
             "User-Agent: U2IP/%s\r\n"
             "Connection: close\r\n\r\n", ip, VERSION);

    send(sockfd, request, strlen(request), 0);

    int bytes;
    while ((bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        strncat(response, buffer, sizeof(response) - strlen(response) - 1);
    }
    close(sockfd);

    char *json_body = strstr(response, "\r\n\r\n");
    if (json_body) {
        json_body += 4;
        
        char country[128], region[128], city[128], isp[128], org[128], country_code[16];
        
        parse_json_value(json_body, "country", country, sizeof(country));
        parse_json_value(json_body, "countryCode", country_code, sizeof(country_code));
        parse_json_value(json_body, "regionName", region, sizeof(region));
        parse_json_value(json_body, "city", city, sizeof(city));
        parse_json_value(json_body, "isp", isp, sizeof(isp));
        parse_json_value(json_body, "org", org, sizeof(org));

        if (current_lang == LANG_ES) {
            printf("\n========================================\n");
            printf("    U2IP v%s - GEOLOCALIZACIÓN           \n", VERSION);
            printf("========================================\n");
            printf(" País:          %s (%s)\n", country, country_code);
            printf(" Región/Estado: %s\n", region);
            printf(" Ciudad:        %s\n", city);
            printf(" Proveedor ISP: %s\n", isp);
            printf(" Organización:  %s\n", org);
            printf("========================================\n\n");
        } else {
            printf("\n========================================\n");
            printf("    U2IP v%s - GEOLOCATION DATA         \n", VERSION);
            printf("========================================\n");
            printf(" Country:       %s (%s)\n", country, country_code);
            printf(" Region/State:  %s\n", region);
            printf(" City:          %s\n", city);
            printf(" ISP:           %s\n", isp);
            printf(" Organization:  %s\n", org);
            printf("========================================\n\n");
        }
    } else {
        printf((current_lang == LANG_ES) ? 
            "[!] Error analizando respuesta JSON.\n" : 
            "[!] Could not parse geolocation response.\n");
    }
}

int main(int argc, char *argv[]) {
    char *input_url = NULL;

    static struct option long_options[] = {
        {"help",    no_argument,       0, 'h'},
        {"version", no_argument,       0, 'v'},
        {"build",   no_argument,       0, 'b'},
        {"lang",    required_argument, 0, 'l'},
        {"url",     required_argument, 0, 'u'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "hvbl:u:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                print_help(argv[0]);
                return 0;
            case 'v':
                print_version();
                return 0;
            case 'b':
                print_build();
                return 0;
            case 'l':
                if (strcmp(optarg, "es") == 0) {
                    current_lang = LANG_ES;
                } else if (strcmp(optarg, "en") == 0) {
                    current_lang = LANG_EN;
                } else {
                    fprintf(stderr, "Unknown language '%s'. Defaulting to English ('en').\n", optarg);
                    current_lang = LANG_EN;
                }
                break;
            case 'u':
                input_url = optarg;
                break;
            default:
                print_help(argv[0]);
                return 1;
        }
    }

    if (!input_url && optind < argc) {
        input_url = argv[optind];
    }

    if (!input_url) {
        fprintf(stderr, "Error: Missing target URL or hostname.\n\n");
        print_help(argv[0]);
        return 1;
    }

    char hostname[256];
    char ip[INET_ADDRSTRLEN];

    clean_url(input_url, hostname);

    if (current_lang == LANG_ES) {
        printf("[*] U2IP v%s\n", VERSION);
        printf("[*] Analizando URL: %s\n", input_url);
        printf("[*] Host extraído:  %s\n", hostname);
    } else {
        printf("[*] U2IP v%s\n", VERSION);
        printf("[*] Analyzing URL: %s\n", input_url);
        printf("[*] Extracted Host: %s\n", hostname);
    }

    if (resolve_domain(hostname, ip) == 0) {
        if (current_lang == LANG_ES) {
            printf("[+] Dirección IP:   %s\n", ip);
            printf("[*] Consultando geolocalización...\n");
        } else {
            printf("[+] Internet IP:   %s\n", ip);
            printf("[*] Fetching geolocation data...\n");
        }
        get_geolocation(ip);
    } else {
        if (current_lang == LANG_ES) {
            fprintf(stderr, "[-] Error: No se pudo resolver el dominio '%s'.\n", hostname);
        } else {
            fprintf(stderr, "[-] Error: Could not resolve IP for domain '%s'.\n", hostname);
        }
        return 1;
    }

    return 0;
}
