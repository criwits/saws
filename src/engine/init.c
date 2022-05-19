/**
 * @file init.c
 * @brief 初始化服务器
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <getopt.h>

#include <database/mysql.h>
#include <network/lws.h>

static int websocket_port = 1376;
static char *database_name = "saws_db";
static char *sql_username = "saws";
static char *sql_password = "mayitbe";
static char *config_filename = "config.json";

static int parse_args(int argc, const char *argv[]) {
  const struct option table[] = {
      {"port", required_argument, NULL, 'p'},
      {"database", required_argument, NULL, 'd'},
      {"username", required_argument, NULL, 'u'},
      {"password", required_argument, NULL, 'w'},
      {"config", required_argument, NULL, 'c'},
      {0, 0, NULL, 0}
  };

  int o;
  while ((o = getopt_long(argc, argv, "-p:d:u:w:c:", table, NULL)) != -1) {
    switch (o) {
      case 'p': sscanf(optarg, "%d", &websocket_port); break;
      case 'd': database_name = optarg; break;
      case 'u': sql_username = optarg; break;
      case 'w': sql_password = optarg; break;
      case 'c': config_filename = optarg; break;
      default:
        printf("Usage: %s [OPTION...]\n\n", argv[0]);
        printf("\t-p, --port=PORT            WebSocket server port\n");
        printf("\t-d, --database=DB_NAME     MySQL database name\n");
        printf("\t-u, --username=USERNAME    MySQL database username\n");
        printf("\t-w, --password=PASSWORD    MySQL database password\n");
        printf("\t-c, --config=CFG_FILE      Config filename\n");
        exit(0);
    }
  }
  saws_log("WebSocket port: %d", websocket_port);
  saws_log("Database: %s, username: %s", database_name, sql_username);
  saws_log("Config file: %s", config_filename);
  return 0;
}

void init_server(int argc, const char *argv[]) {
  parse_args(argc, argv);
  db_init(database_name, sql_username, sql_password);
  lws_init(websocket_port);
}