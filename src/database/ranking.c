/**
 * @file ranking.c
 * @brief 排行榜相关 DAO
 * @author hans
 * @version 0.1
 */

#include <common.h>
#include <database/mysql.h>

#define MAX_QUERY_PATTERN_LENGTH 256
#define INSERT_RANKING_PATTERN "INSERT INTO rankings(uid, score, enroll_date, difficulty) VALUES (%d, %d, %d, %d)"
#define GET_RANKING_PATTERN "SELECT u.username, r.score, r.enroll_date, r.difficulty FROM users u, rankings r WHERE u.uid = r.uid ORDER BY score DESC"

void insert_ranking(int uid, int score, int enroll_date, int difficulty) {
  char buf[MAX_QUERY_PATTERN_LENGTH];
  sprintf(buf, INSERT_RANKING_PATTERN, uid, score, enroll_date, difficulty);
  MYSQL_RES *res = db_query(buf);
}

MYSQL_RES *get_ranking_json() {
  return db_query(GET_RANKING_PATTERN);
}