/**
 * @file prop.h
 * @brief 战利品
 */

#ifndef SAWS_PROP_H
#define SAWS_PROP_H

typedef struct prop_s {
  struct prop_s *prev;
  struct prop_s *next;
  int id;
  int kind;
} prop_t;

#endif //SAWS_PROP_H
