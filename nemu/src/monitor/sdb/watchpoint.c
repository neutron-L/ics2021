#include "sdb.h"
#include <string.h>

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
// the expression , assume its length does not exceed 32 bytes
  char str[32];
  // the tokens of str
  // struct Tokens tokens[32];
  // the value of expression
  word_t val;
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;


void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

/* The unused monitoring point with the lowest sequence number was found */
int new_wp(const char * str)
{
  assert(free_ != NULL);
  assert(str != NULL);
  WP* temp = free_;
  free_ = free_->next;
  temp->next = NULL;

  // insert it into head
  if (!head || head->NO > temp->NO)
  {
    temp->next = head;
    head = temp;
  }
  else
  {
    WP* p = head;
    while (p->next && p->next->NO < temp->NO)
      p = p->next;
    temp->next = p->next;
    p->next = temp;
  }
  memset(temp->str, 0, sizeof(temp->str));
  strncpy(temp->str, str, sizeof(temp->str) - 1);
  bool success;
  temp->val = expr(temp->str, &success);
  if (!success)
    assert(0);

  return 0;
}


int free_wp(int NO)
{
  WP * p, *wp;
  // find the wp with NO
  // remove it from head
  if (head->NO == NO)
  {
    wp = head;
    head = head->next;
  }
  else
  {
    p = head;
    while (p->next && p->next->NO != NO)
      p = p->next;
    // wp must be in head
    assert(p->next);
    wp = p->next;
    p->next = wp->next;
  }
// insert it into free_
  if (free_ == NULL || free_->NO > wp->NO)
  {
    wp->next = free_;
    free_ = wp;
  }
  else
  {
    p = free_;
    while (p->next && p->next->NO < wp->NO)
      p = p->next;
    wp->next = p->next;
    p->next = wp;
  } 

  return 0;
}

/* show watchpoints */
void show_watchpoints()
{
  WP *p = head;
  printf("Num\tWhat\n");
  while (p)
  {
      printf("%d\t%s\n", p->NO, p->str);
      p = p->next;
  }
}

/* check the watchpoints */
bool check_watchpoints()
{
  WP * p = head;
  bool success;
  bool update = false;
  while (p)
  {
    word_t val = expr(p->str, &success);
    if (!success)
      assert(0);
    if (val != p->val)
    {
      printf("Watch points %d: %s\n", p->NO, p->str);
      printf("Old value = %u\nNew value = %u\n\n", p->val, val);
      p->val = val;
      update = true;
    }


    p = p->next;
  }

  return update;
}