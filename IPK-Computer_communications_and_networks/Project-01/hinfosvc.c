/**
 * Simple HTTP server, that provides information about the system.
 *     ___ _   __
 *    / _ \ | / /
 *   / // / |/ /
 *  /____/|___/
 *
 * @author Dominik Vagner, xvagne10
 */

#include <asm-generic/socket.h>
#include <bits/types.h>
#include <errno.h>
#include <math.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX 1024

/**
 * Executes command in shell (terminal) and returns the output of the command.
 *
 * @param String (char *) with the command we want to run.
 * @return String (char *) with the result/output of passed command.
 */
char *exec(const char *cmd) {
  char buffer[128] = {0};
  char *result = calloc(1024, sizeof(char));

  FILE *fp = popen(cmd, "r");
  if (!fp) {
    fprintf(stderr, "popen() error.\n");
    exit(1);
  }
  while (!feof(fp)) {
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
      strncat(result, buffer, strlen(buffer));
    }
  }

  pclose(fp);
  return result;
}

/**
 * Calculates current average cpu usage in percentage using info in /proc/stat.
 *
 * @param String (char *) into which the function outputs
 * the calculated percentage and a percent sign ('%').
 */
void load(char *func_ret) {
  char *result = calloc(16, sizeof(char));
  int prev[10], curr[10];

  sscanf(exec("cat /proc/stat | sed -n 1p | cut -d \" \" -f 3-"),
         "%d %d %d %d %d %d %d %d %d %d", &prev[0], &prev[1], &prev[2],
         &prev[3], &prev[4], &prev[5], &prev[6], &prev[7], &prev[8], &prev[9]);
  sleep(1);
  sscanf(exec("cat /proc/stat | sed -n 1p | cut -d \" \" -f 3-"),
         "%d %d %d %d %d %d %d %d %d %d", &curr[0], &curr[1], &curr[2],
         &curr[3], &curr[4], &curr[5], &curr[6], &curr[7], &curr[8], &curr[9]);

  int prev_idle = prev[3] + prev[4];
  int idle = curr[3] + curr[4];
  int prev_non_idle = prev[0] + prev[1] + prev[2] + prev[5] + prev[6] + prev[7];
  int non_idle = curr[0] + curr[1] + curr[2] + curr[5] + curr[6] + curr[7];
  double prev_total = prev_idle + prev_non_idle;
  double total = idle + non_idle;
  double total_diff = total - prev_total;
  double idle_diff = idle - prev_idle;
  double percentage = ((total_diff - idle_diff) / total_diff) * 100;
  sprintf(result, "%d%s", (int)round(percentage), "%");
  strcpy(func_ret, result);
  return;
}

/**
 * Using shell commands outputs cpu name of the system, from /proc/cpuinfo.
 *
 * @param String (char *) into which the function outputs
 * the cpu name it found.
 */
void cpu_name(char *func_ret) {
  strcpy(func_ret, exec("cat /proc/cpuinfo | cut -d : -f 2 | sed -n 5p | cut "
                        "-c 2- | tr -d '\n'"));
  return;
}

/**
 * Using shell commands outputs hostname of the system, from /proc/hostname.
 *
 * @param String (char *) into which the function outputs
 * the calculated percentage and a percent sign ('%').
 */
void hostname(char *func_ret) {
  strcpy(func_ret, exec("cat /proc/sys/kernel/hostname | tr -d '\n'"));
  return;
}

/**
 * Signal handler needed by the signal() function.
 * After it's called, function exits the program.
 *
 * @param Data type required by the signal() function.
 */
void signal_catcher(__sig_atomic_t s) { exit(1); }

int main(int argc, char *argv[]) {
  signal(SIGINT, signal_catcher);

  char http_good_header[1024] = "HTTP/1.1 200 OK\r\n"
                                "Content-Type: text/plain\r\n";

  char http_bad_header[1024] = "HTTP/1.1 400 Bad Request\r\n"
                               "Content-Type: text/plain\r\n"
                               "Content-Length: 11\r\n\r\nBad Request";

  /* Create an endpoint (File descriptor) for communication. */
  int socket_desc;
  if ((socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0) {
    fprintf(stderr, "Socket error.\n");
    exit(1);
  }

  /* Set socket option to 1 (On) at socket level.
   * SO_REUSEPORT | SO_REUSEADDR; This set of options allows for reusing
   * (repeated binding to) the same port and address.
   */
  int opt = 1;
  setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
             sizeof(opt));

  /* IP socket address, combination of IP Interface address and port number */
  int port = (int)strtol(argv[1], (char **)NULL, 10);
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(INADDR_ANY);

  /* Bind a name (address [sockaddr struct]) to file descriptor returned by
   * socket(). */
  if ((bind(socket_desc, (struct sockaddr *)&address, sizeof(address))) < 0) {
    fprintf(stderr, "Bind error.\n");
    close(socket_desc);
    exit(1);
  }

  /* Mark socket reffered to by "socket_desc" as a passive one
   * (The one that should be used to accept incoming requests) */
  if (listen(socket_desc, MAX) < 0) {
    fprintf(stderr, "Listen error.\n");
    exit(1);
  }

  int conn;
  char buffer[2048], response[2560] = {0};
  char func_return[1024] = {0};
  char return_len[128] = {0};

  /* In a infinite loop, check for incoming requests and respond accordingly. */
  while (1) {
    conn = accept(socket_desc, NULL, NULL); // Establish a new connection.
    bzero(func_return, strlen(func_return));
    bzero(return_len, strlen(return_len));
    bzero(response, strlen(response));

    recv(conn, buffer, sizeof(buffer), 0); // Read input from connection.

    if (strncmp(buffer, "GET /hostname", 13) == 0) {
      hostname(func_return);
      sprintf(return_len, "%s%ud%s",
              "Content-Length:", (int)strlen(func_return), "\r\n\r\n");
      sprintf(response, "%s%s%s", http_good_header, return_len, func_return);
    } else if (strncmp(buffer, "GET /cpu-name", 13) == 0) {
      cpu_name(func_return);
      sprintf(return_len, "%s%ud%s",
              "Content-Length:", (int)strlen(func_return), "\r\n\r\n");
      sprintf(response, "%s%s%s", http_good_header, return_len, func_return);
    } else if (strncmp(buffer, "GET /load", 9) == 0) {
      load(func_return);
      sprintf(return_len, "%s%ud%s",
              "Content-Length:", (int)strlen(func_return), "\r\n\r\n");
      sprintf(response, "%s%s%s", http_good_header, return_len, func_return);
    } else {
      sprintf(response, "%s", http_bad_header);
    }

    send(conn, response, sizeof(response), 0); // Send back appropriate output.
    close(conn); // Close connection made at the start of loop iteration.
  }

  close(socket_desc);
  return 0;
}
